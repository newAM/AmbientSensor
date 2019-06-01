/******************************************************************************
* Copyright 2019 Alex M.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/

#include "w5500/mqtt.h"

static const TickType_t MQTT_ACK_TIMEOUT    = 1000; //!< server acknowledgment timeout
static const TickType_t MQTT_CON_TIMEOUT    =  500; //!< connection timeout
static const TickType_t MQTT_SEND_TIMEOUT   =  100; //!< packet send timeout

/*!
* @brief  Initializes the W5500 hardware for MQTT.
* @param  client - MQTT client
* @return W5500 status
*/
w5500_status_t MQTT_Initialize(mqtt_client_t* client)
{
  w5500_status_t rc;

  // open TCP socket
  rc = W5500_SocketOpen(client->dev, client->sn, W5500_SN_PROTO_TCP, client->sourcePort, MQTT_CON_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  // connect
  rc = W5500_SocketConnect(client->dev, client->sn, client->ip, client->destinationPort, MQTT_CON_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Connects to the MQTT server.
* @param  client - MQTT client
* @return W5500 status
*/
w5500_status_t MQTT_Connect(mqtt_client_t* client)
{
  w5500_status_t rc;
  mqtt_connect_t connect __attribute__((aligned(16)));
  mqtt_connack_t connack __attribute__((aligned(16)));

  connect.field.rsvd              = 0;                // zero out reserved bits
  connect.field.type              = MQTT_CONNECT;     // connect packet
  connect.field.msgLen            = MQTT_CONNECT_LEN; // fixed length
  connect.field.protoLen          = MQTT_PROTO_LEN;   // fixed protocol length
  connect.field.proto[0]          = 'M';              // protocol name
  connect.field.proto[1]          = 'Q';
  connect.field.proto[2]          = 'T';
  connect.field.proto[3]          = 'T';
  connect.field.protoLevel        = MQTT_PROTO_LEVEL; // fixed protocol level
  connect.field.flag.rsvd         = 0;                // zero out reserved flag   
  connect.field.flag.cleanSession = 1;                // do not store session state
  connect.field.flag.willFlag     = 0;                // do not store will message
  connect.field.flag.willQos      = 0;                // must be zero with willFlag=0
  connect.field.flag.willRetain   = 0;                // must be zero with willFlag=0
  connect.field.flag.passwordFlag = 0;                // no password
  connect.field.flag.userName     = 0;                // no username
  connect.field.keepAlive         = 3600;             // 3600 second keep alive
  connect.field.clientIdLen       = 0;                // no client ID

  // byte swap the 16-bit fields
  connect.field.protoLen    = BYTE_SWAP_16(connect.field.protoLen);
  connect.field.keepAlive   = BYTE_SWAP_16(connect.field.keepAlive);
  connect.field.clientIdLen = BYTE_SWAP_16(connect.field.clientIdLen);

  // send CONNECT
  rc = W5500_SocketSend(client->dev, client->sn, connect.buf, MQTT_CONNECT_BUF_LEN, MQTT_SEND_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  // wait for CONNACK
  rc = W5500_SocketRecieveTCP(client->dev, client->sn, connack.buf, MQTT_CONNACK_BUF_LEN, MQTT_ACK_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  // check for correct packet
  if (connack.field.type != MQTT_CONNACK || connack.field.len != 2)
  {
    return W5500_MQTT_BAD_PACKET;
  }

  // check that connection was accepted
  if (connack.field.rc != MQTT_CON_ACCEPT)
  {
    return W5500_MQTT_CON_REFUSED;
  }

  return rc;
}

/*!
* @brief  Publishes a message to the server.
* @param  client - MQTT client
* @param  topic - topic to publish to
* @param  topicLen - length of the topic
* @param  payload - payload to publish
* @param  payloadLen - payload length
* @return W5500 status
*/
w5500_status_t MQTT_Publish(mqtt_client_t* client, const char* topic, uint16_t topicLen, const char* payload, uint16_t payloadLen)
{
  static const uint16_t TOPIC_LEN_BYTES = 2;
  w5500_status_t rc;
  mqtt_publish_t header __attribute__((aligned(16)));
  uint32_t fsr = UINT32_MAX;
  uint32_t ptr = UINT32_MAX;

  header.field.retain = 0;
  header.field.qos    = 0;
  header.field.dup    = 0;
  header.field.type   = MQTT_PUBLISH;
  header.field.length = topicLen + payloadLen + TOPIC_LEN_BYTES;

  // write header
  rc = W5500_SocketWritePart(client->dev, client->sn, header.buf, MQTT_PUBLISH_BUF_LEN, &fsr, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // write topic length
  topicLen = BYTE_SWAP_16(topicLen);
  rc = W5500_SocketWritePart(client->dev, client->sn, (uint8_t*)&topicLen, TOPIC_LEN_BYTES, &fsr, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // write topic
  topicLen = BYTE_SWAP_16(topicLen);
  rc = W5500_SocketWritePart(client->dev, client->sn, (uint8_t*)topic, topicLen, &fsr, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // write payload
  rc = W5500_SocketWritePart(client->dev, client->sn, (uint8_t*)payload, payloadLen, &fsr, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // send data
  rc = W5500_SocketSendBuffer(client->dev, client->sn, (uint16_t)ptr, MQTT_SEND_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}
