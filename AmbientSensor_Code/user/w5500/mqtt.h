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

#ifndef _MQTT_H_
#define _MQTT_H_

#include "w5500/w5500.h"
#include "logging/logging.h"

// constants
#define MQTT_PROTO_LEN        4 //!< length of the protocol name
#define MQTT_PROTO_LEVEL      4 //!< protocol level [MQTT-3.1.2-2]
#define MQTT_CONNECT_BUF_LEN 14 //!< total length of MQTT CONNECT packet
#define MQTT_CONNACK_BUF_LEN  4 //!< total length of MQTT CONNACK packet
#define MQTT_PUBLISH_BUF_LEN  2 //!< total length of MQTT PUBLISH packet header
#define MQTT_CONNECT_LEN     12 //!< remaining length of the MQTT connect packet

//! MQTT control packets
typedef enum {
  MQTT_CONNECT     =  1,
  MQTT_CONNACK     =  2,
  MQTT_PUBLISH     =  3,
  MQTT_PUBACK      =  4,
  MQTT_PUBREC      =  5,
  MQTT_PUBREL      =  6,
  MQTT_PUBCOMP     =  7,
  MQTT_SUBSCRIBE   =  8,
  MQTT_SUBACK      =  9,
  MQTT_UNSUBSCRIBE = 10,
  MQTT_UNSUBACK    = 11,
  MQTT_PINGREQ     = 12,
  MQTT_PINGRESP    = 13,
  MQTT_DISCONNECT  = 14,
} mqtt_ctrl_packet_type_t;

//! MQTT connection return codes 
typedef enum {
  MQTT_CON_ACCEPT      = 0, //!< Connection Accepted
  MQTT_CON_BAD_PROTO   = 1, //!< Connection Refused, unacceptable protocol version
  MQTT_CON_BAD_ID      = 2, //!< Connection Refused, identifier rejected
  MQTT_CON_UNAVALIABLE = 3, //!< Connection Refused, Server unavailable
  MQTT_CON_BAD_CREDS   = 4, //!< Connection Refused, bad user name or password
  MQTT_CON_NOT_AUTH    = 5, //!< Connection Refused, not authorized
} mqtt_connect_rc_t;

//! MQTT CONNECT packet
typedef union mqtt_connect_t {
  struct {
    uint8_t  rsvd : 4;              //!< reserved
    uint8_t  type : 4;              //!< control packet type
    uint8_t  msgLen;                //!< length of everything after this
    uint16_t protoLen;              //!< length of protocol name
    char     proto[MQTT_PROTO_LEN]; //!< protocol name
    uint8_t  protoLevel;            //!< protocol level (0x4 for v3.1.1)
    struct {
      uint8_t rsvd         : 1;     //!< reserved
      uint8_t cleanSession : 1;     //!< see [MQTT-3.1.2-4]
      uint8_t willFlag     : 1;     //!< will message must be stored on the server
      uint8_t willQos      : 2;     //!< QoS when publishing will message
      uint8_t willRetain   : 1;     //!< publish the will message as a retained message
      uint8_t passwordFlag : 1;     //!< password present in payload
      uint8_t userName     : 1;     //!< username present in payload
    } __attribute__((packed)) flag; //!< flags to specify behavior of connection
    uint16_t keepAlive;             //!< keep alive interval in seconds
    uint16_t clientIdLen;           //!< length of the client ID, if zero, MUST set CleanSession = 1
  } __attribute__((packed)) field;
  uint8_t buf[MQTT_CONNECT_BUF_LEN];
} mqtt_connect_t;

//! MQTT CONNACK packet
typedef union mqtt_connack_t {
  struct {
    uint8_t  rsvd : 4;              //!< reserved
    uint8_t  type : 4;              //!< control packet type
    uint8_t  len;                   //!< length of everything after this
    uint8_t  ackFlag;               //!< connect acknowledgment flags
    uint8_t  rc;                    //!< connect return code
  } __attribute__((packed)) field;
  uint8_t buf[MQTT_CONNACK_BUF_LEN];
} mqtt_connack_t;

//! MQTT PUBLISH packet fixed header
typedef union mqtt_publish_t {
  struct {
    uint8_t retain : 1; //!< store the message if set
    uint8_t qos    : 2; //!< QoS level
    uint8_t dup    : 1; //!< message may be a redelivery
    uint8_t type   : 4; //!< control packet type
    uint8_t length;     //!< total length of the packet
  } __attribute__((packed)) field;
  uint8_t buf[MQTT_PUBLISH_BUF_LEN];
} mqtt_publish_t;

//! MQTT client
typedef struct mqtt_client_t
{
  w5500_dev_t* dev;                                         //!< W5500 device to utilize 
  uint8_t      sn;                                          //!< socket number
  uint8_t      ip[IPV4_BYTES] __attribute__((aligned(16))); //!< server IP
  uint16_t     destinationPort;                             //!< server port
  uint16_t     sourcePort;                                  //!< our port
} mqtt_client_t;

// function prototypes
w5500_status_t MQTT_Initialize(mqtt_client_t* client);
w5500_status_t MQTT_Connect(mqtt_client_t* client);
w5500_status_t MQTT_Publish(mqtt_client_t* client, const char* topic, uint16_t topicLen, const char* payload, uint16_t payloadLen);
#endif // _MQTT_H_
