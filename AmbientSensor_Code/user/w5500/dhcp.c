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

#include "w5500/dhcp.h"
#include "constants.h"

#define MAGIC_COOKIE_SIZE 4

static const TickType_t DHCP_INIT_FAIL_TIMEOUT = 3000; //!< failed initialization timeout
static const TickType_t DHCP_SOCKET_TIMEOUT    =  100; //!< socket timeout
static const TickType_t DHCP_SEND_TIMEOUT      =  100; //!< packet send timeout
static const TickType_t DHCP_RECV_TIMEOUT      =  100; //!< packet receive timeout

//! DHCP magic cookie
static const uint8_t MAGIC_COOKIE[MAGIC_COOKIE_SIZE] = {0x63, 0x82, 0x53, 0x63};

// private function prototypes
static inline size_t GetOptionPointer(dhcp_client_t* client, dhcp_option_t optionCode);
static inline const char* MessageTypeString(dhcp_msg_type_t e);
static inline const char* StateString(dhcp_state_t e);
static w5500_status_t DHCP_SendDISCOVER(dhcp_client_t* client);
static w5500_status_t DHCP_SendREQUEST(dhcp_client_t* client);
static w5500_status_t DHCP_Receive(dhcp_client_t* client, dhcp_msg_type_t* msgType);
static w5500_status_t DHCP_Initialize(dhcp_client_t* client);
static inline void DHCP_HandleINIT(dhcp_client_t* client);
static inline void DHCP_HandleSELECTING(dhcp_client_t* client);
static inline void DHCP_HandleREQUESTING(dhcp_client_t* client);
static inline void DHCP_HandleBOUND(dhcp_client_t* client);
static inline void DHCP_HandleRENEWING(dhcp_client_t* client);
static void DHCP_PrepareMessage(dhcp_client_t* client, size_t* segment);

/*!
* @brief   Gets the pointer to a desired option.
* @param   client - DHCP client
* @param   optionCode - option code to locate
* @returns Pointer to the first occurrence of the option, zero for no occurrence.
*/
inline size_t GetOptionPointer(dhcp_client_t* client, dhcp_option_t optionCode)
{
  // stat the pointer after the magic cookie
  size_t ptr = MAGIC_COOKIE_SIZE;
  uint8_t option;
  uint8_t optionLen;

  // search within the option field
  while (ptr < DHCP_OPT_SIZE)
  {
    // get option code
    option = client->msg.field.opt[ptr++];

    // handle single-octet options
    if (option == DHCP_OPTION_PAD)
    {
      continue;
    }
    else if (option == DHCP_OPTION_END)
    {
      return 0;
    }

    // found the option!
    if (option == optionCode)
    {
      return ptr;
    }
    // increment over the length of the option
    else
    {
      optionLen = client->msg.field.opt[ptr++];
      ptr += optionLen;
    }
  }

  return 0;
}

/*!
* @brief  enum to string conversion for DHCP message type.
* @param  e - message type enumeration value
* @return string for the corresponding enumeration
*/
inline const char* MessageTypeString(dhcp_msg_type_t e)
{
  switch (e)
  {
    case DHCP_DISCOVER:
      return "DHCP_DISCOVER";
    case DHCP_OFFER:
      return "DHCP_OFFER";
    case DHCP_REQUEST:
      return "DHCP_REQUEST";
    case DHCP_DECLINE:
      return "DHCP_DECLINE";
    case DHCP_ACK:
      return "DHCP_ACK";
    case DHCP_NAK:
      return "DHCP_NAK";
    case DHCP_RELEASE:
      return "DHCP_RELEASE";
    case DHCP_INFORM:
      return "DHCP_INFORM";
    default:
      return "UNKNOWN";
  }
}

/*!
* @brief  enum to string conversion for DHCP StateString.
* @param  e - state enumeration value
* @return string for the corresponding enumeration
*/
inline const char* StateString(dhcp_state_t e)
{
  switch (e)
  {
    case DHCP_INIT:
      return "DHCP_INIT";
    case DHCP_SELECTING:
      return "DHCP_SELECTING";
    case DHCP_REQUESTING:
      return "DHCP_REQUESTING";
    case DHCP_INITREBOOT:
      return "DHCP_INITREBOOT";
    case DHCP_REBOOTING:
      return "DHCP_REBOOTING";
    case DHCP_BOUND:
      return "DHCP_BOUND";
    case DHCP_RENEWING:
      return "DHCP_RENEWING";
    case DHCP_REBINDING:
      return "DHCP_REBINDING";
    default:
      return "UNKNOWN";
  }
}

/*!
* @brief  Initializes the W5500 hardware for DHCP.
* @param  client - DHCP client
* @return W5500 status
*/
w5500_status_t DHCP_Initialize(dhcp_client_t* client)
{
  w5500_status_t rc;
  uint8_t broadcast[IPV4_BYTES] __attribute__((aligned(16))) = {255,255,255,255};
  uint8_t zeroIp[IPV4_BYTES] __attribute__((aligned(16))) = {0,0,0,0};

  // zero out source address
  rc = W5500_SetSIPR(client->dev, zeroIp);
  W5500_RETURN_NOT_OK(rc);

  // zero out gateway address
  rc = W5500_SetGAR(client->dev, zeroIp);
  W5500_RETURN_NOT_OK(rc);

  // zero out subnet mask
  rc = W5500_SetSUBR(client->dev, zeroIp);
  W5500_RETURN_NOT_OK(rc);

  // set destination port and IP
  rc = W5500_SocketDestination(client->dev, client->sn, broadcast, DHCP_DESTINATION_PORT);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Prepares basic fields in a DHCP message.
* @param  client - DHCP client
* @param  optPtr - index for the option field
*/
void DHCP_PrepareMessage(dhcp_client_t* client, size_t* optPtr)
{
  size_t segment;
  client->msg.field.op    = DHCP_BOOTREQUEST;
  client->msg.field.htype = DHCP_HTYPE10MB;
  client->msg.field.hlen  = MAC_BYTES;
  client->msg.field.hops  = 0;
  client->msg.field.xid   = 1428032432; // arbitrary
  client->msg.field.sec   = 0;
  client->msg.field.flags = 0;

  // clear out addresses
  for (segment = 0; segment < IPV4_BYTES; segment++)
  {
    client->msg.field.ciAddr[segment] = 0;
    client->msg.field.yiAddr[segment] = 0;
    client->msg.field.siAddr[segment] = 0;
    client->msg.field.giAddr[segment] = 0;
  }

  // copy MAC address
  for (segment = 0; segment < MAC_BYTES; segment++)
  {
    client->msg.field.chAddr[segment] = client->dev->mac[segment];
  }

  // zero out other fields
  for (segment = 0; segment < DHCP_SNAME_SIZE; segment++)
  {
    client->msg.field.sname[segment] = 0;
  }
  for (segment = 0; segment < DHCP_FILE_SIZE; segment++)
  {
    client->msg.field.file[segment] = 0;
  }
  for (segment = 0; segment < DHCP_OPT_SIZE; segment++)
  {
    client->msg.field.opt[segment] = 0;
  }

  // magic cookie
  for (segment = 0; segment < MAGIC_COOKIE_SIZE; segment++)
  {
    client->msg.field.opt[segment] = MAGIC_COOKIE[segment];
  }
  *optPtr = segment;
}

/*!
* @brief  Sends a DHCPDISCOVER message.
* @param  client - DHCP client
* @return W5500 status
*/
w5500_status_t DHCP_SendDISCOVER(dhcp_client_t* client)
{
  w5500_status_t rc;
  size_t optPtr;
  size_t segment;

  // open socket for transmission
  rc = W5500_SocketOpen(client->dev, client->sn, W5500_SN_PROTO_UDP, DHCP_SOURCE_PORT, DHCP_SOCKET_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  // reset message fields
  DHCP_PrepareMessage(client, &optPtr);

  // message type
  client->msg.field.opt[optPtr++] = DHCP_OPTION_MESSAGE_TYPE;
  client->msg.field.opt[optPtr++] = 1; // length
  client->msg.field.opt[optPtr++] = DHCP_DISCOVER;

  // client identifier
  client->msg.field.opt[optPtr++] = DHCP_OPTION_CLIENT_ID;
  client->msg.field.opt[optPtr++] = 7;                    // length
  client->msg.field.opt[optPtr++] = DHCP_HTYPE10MB;       // hardware type
  client->msg.field.opt[optPtr++] = client->dev->mac[0];  // MAC address
  client->msg.field.opt[optPtr++] = client->dev->mac[1];
  client->msg.field.opt[optPtr++] = client->dev->mac[2];
  client->msg.field.opt[optPtr++] = client->dev->mac[3];
  client->msg.field.opt[optPtr++] = client->dev->mac[4];
  client->msg.field.opt[optPtr++] = client->dev->mac[5];

  // pass our hostname
  client->msg.field.opt[optPtr++] = DHCP_OPTION_HOSTNAME;
  client->msg.field.opt[optPtr++] = client->hostNameLen;
  for (segment = 0; segment < client->hostNameLen; segment++)
  {
    client->msg.field.opt[optPtr++] = client->hostName[segment];
  }

  // message done
  client->msg.field.opt[optPtr++] = DHCP_OPTION_END;

  // send data
  rc = W5500_SocketSend(client->dev, client->sn, client->msg.buf, DHCP_MSG_SIZE, DHCP_SEND_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Sends a DHCPREQUEST message.
* @param  client - DHCP client
* @return W5500 status
*/
w5500_status_t DHCP_SendREQUEST(dhcp_client_t* client)
{
  w5500_status_t rc;
  size_t optPtr;
  size_t segment;

  // open socket for transmission
  rc = W5500_SocketOpen(client->dev, client->sn, W5500_SN_PROTO_UDP, DHCP_SOURCE_PORT, DHCP_SOCKET_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  // reset message fields
  DHCP_PrepareMessage(client, &optPtr);

  // message type
  client->msg.field.opt[optPtr++] = DHCP_OPTION_MESSAGE_TYPE;
  client->msg.field.opt[optPtr++] = 1; // length
  client->msg.field.opt[optPtr++] = DHCP_REQUEST;

  // client identifier
  client->msg.field.opt[optPtr++] = DHCP_OPTION_CLIENT_ID;
  client->msg.field.opt[optPtr++] = 7;                    // length
  client->msg.field.opt[optPtr++] = DHCP_HTYPE10MB;       // hardware type
  client->msg.field.opt[optPtr++] = client->dev->mac[0];  // MAC address
  client->msg.field.opt[optPtr++] = client->dev->mac[1];
  client->msg.field.opt[optPtr++] = client->dev->mac[2];
  client->msg.field.opt[optPtr++] = client->dev->mac[3];
  client->msg.field.opt[optPtr++] = client->dev->mac[4];
  client->msg.field.opt[optPtr++] = client->dev->mac[5];

  // pass our hostname
  client->msg.field.opt[optPtr++] = DHCP_OPTION_HOSTNAME;
  client->msg.field.opt[optPtr++] = client->hostNameLen;
  for (segment = 0; segment < client->hostNameLen; segment++)
  {
    client->msg.field.opt[optPtr++] = client->hostName[segment];
  }

  // parameter request
  client->msg.field.opt[optPtr++] = DHCP_OPTION_PARAMETER_REQUEST;
  client->msg.field.opt[optPtr++] = 5; // length
  client->msg.field.opt[optPtr++] = DHCP_OPTION_SUBNET_MASK;
  client->msg.field.opt[optPtr++] = DHCP_OPTION_ROUTER;
  client->msg.field.opt[optPtr++] = DHCP_OPTION_DNS;
  client->msg.field.opt[optPtr++] = DHCP_OPTION_RENEWAL_TIME;
  client->msg.field.opt[optPtr++] = DHCP_OPTION_REBINDING_TIME;

  // request the IP
  client->msg.field.opt[optPtr++] = DHCP_OPTION_REQUESTED_IP;
  client->msg.field.opt[optPtr++] = 4; // length
  for (segment = 0; segment < IPV4_BYTES; segment++)
  {
    client->msg.field.opt[optPtr++] = client->clientIp[segment];
  }

  // message done
  client->msg.field.opt[optPtr++] = DHCP_OPTION_END;

  // send data
  rc = W5500_SocketSend(client->dev, client->sn, client->msg.buf, DHCP_MSG_SIZE, DHCP_SEND_TIMEOUT);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Reads a DHCP message into the buffer
* @param  client - DHCP client
* @param  msgType - message type
* @return W5500 status
*/
w5500_status_t DHCP_Receive(dhcp_client_t* client, dhcp_msg_type_t* msgType)
{
  w5500_status_t rc; // W5500 return codes
  size_t i; // general purpose array index

  // read from socket
  rc = W5500_SocketRecieveUDP(client->dev, client->sn, client->msg.buf, DHCP_MSG_SIZE, DHCP_RECV_TIMEOUT, client->serverIp, NULL);
  W5500_RETURN_NOT_OK(rc);

  // endian swaps
  client->msg.field.xid   = BYTE_SWAP_32(client->msg.field.xid);
  client->msg.field.sec   = BYTE_SWAP_16(client->msg.field.sec);
  client->msg.field.flags = BYTE_SWAP_16(client->msg.field.flags);

  // check that this is a reply
  if (client->msg.field.op != DHCP_BOOTREPLY)
  {
    return W5500_DHCP_BAD_OPCODE;
  }

  // expect magic cookie at the start
  for (i = 0; i < MAGIC_COOKIE_SIZE; i++)
  {
    if (client->msg.field.opt[i] != MAGIC_COOKIE[i])
    {
      return W5500_DHCP_NO_COOKIE;
    }
  }

  // get the message type
  i = GetOptionPointer(client, DHCP_OPTION_MESSAGE_TYPE);
  if (i == 0)
  {
    return W5500_DHCP_MISSING_TYPE;
  }
  else
  {
    // length must be one (RFC 2132)
    if (client->msg.field.opt[i++] != 1)
    {
      return W5500_DHCP_CORRUPT_OPTION;
    }
    *msgType = client->msg.field.opt[i];
  }

  return rc;
}

/*!
* @brief Runs the DHCP client.
* @param argument - pointer to DHCP client
*/
void DHCP_ClientTask(void const * argument)
{
  dhcp_client_t* client = (dhcp_client_t*)argument;

  // DHCP finite state machine
  while (1)
  {
    LOG_DEBUG("State: %s", StateString(client->state));
    switch (client->state)
    {
      case DHCP_INIT:
        DHCP_HandleINIT(client);
        break;
      case DHCP_SELECTING:
        DHCP_HandleSELECTING(client);
        break;
      case DHCP_REQUESTING:
        DHCP_HandleREQUESTING(client);
        break;
      case DHCP_BOUND:
        DHCP_HandleBOUND(client);
        break;
      case DHCP_RENEWING:
        DHCP_HandleRENEWING(client);
        break;
      default:
        LOG_CRITICAL("UNHANDLED DHCP STATE 0x%X", client->state);
        client->state = DHCP_INIT;
        break;
    }
  }
}

/*!
* @brief Called upon entering the INIT state.
* @param client - DHCP client
*/
inline void DHCP_HandleINIT(dhcp_client_t* client)
{
  w5500_status_t rc;

  // initialize the client hardware
  rc = DHCP_Initialize(client);
  if (rc != W5500_OK)
  {
    LOG_ERROR("DHCP_Initialize failed: 0x%X", rc);

    // prevent tight loops on repeat failures
    vTaskDelay(DHCP_INIT_FAIL_TIMEOUT);
    return;
  }

  // send DHCPDISCOVER
  rc = DHCP_SendDISCOVER(client);
  if (rc != W5500_OK)
  {
    LOG_ERROR("DHCP_SendDISCOVER failed: 0x%X", rc);

    // prevent tight loops on repeat failures
    vTaskDelay(DHCP_INIT_FAIL_TIMEOUT);
    return;
  }

  // success, update state
  client->state = DHCP_SELECTING;
}

/*!
* @brief Called upon entering the SELECTING state.
* @param client - DHCP client
*/
inline void DHCP_HandleSELECTING(dhcp_client_t* client)
{
  w5500_status_t rc;
  dhcp_msg_type_t msgType;
  size_t segment;

  // DHCPDISCOVER has been sent, wait for socket interrupt
  rc = DHCP_Receive(client, &msgType);
  if (rc == W5500_OK)
  {
    if (msgType == DHCP_OFFER)
    {
      // copy the client IP from the DHCPOFFER
      for (segment = 0; segment < IPV4_BYTES; segment++)
      {
        client->clientIp[segment] = client->msg.field.yiAddr[segment];
      }

      // send DHCPREQUEST
      rc = DHCP_SendREQUEST(client);
      if (rc != W5500_OK)
      {
        LOG_ERROR("DHCP_SendREQUEST failed: 0x%X", rc);
        client->state = DHCP_INIT;
        return;
      }

      // success, update state
      client->state = DHCP_REQUESTING;
    }
    else
    {
      LOG_ERROR("bad event for %s: %s", StateString(client->state), MessageTypeString(msgType));
      client->state = DHCP_INIT;
      return;
    }
  }
  else if (rc == W5500_RECV_TIMEOUT)
  {
    LOG_WARNING("DHCP_SELECTING TIMEOUT");
    client->state = DHCP_INIT;
    return;
  }
  else
  {
    LOG_ERROR("DHCP_Receive failed 0x%X", rc);
    client->state = DHCP_INIT;
    return;
  }
}

/*!
* @brief Called upon entering the REQUESTING state.
* @param client - DHCP client
*/
inline void DHCP_HandleREQUESTING(dhcp_client_t* client)
{
  w5500_status_t rc;
  dhcp_msg_type_t msgType;
  size_t  i;
  uint8_t ip[IPV4_BYTES] __attribute__((aligned(16)));
  uint32_t leaseDuration;
  uint8_t  optLen;
  size_t   optPtr;

  // DHCPREQUEST has been sent, wait for socket interrupt
  rc = DHCP_Receive(client, &msgType);
  if (rc == W5500_OK)
  {
    if (msgType == DHCP_ACK)
    {
      // set lease time
      client->leaseTick = xTaskGetTickCount();
      optPtr = GetOptionPointer(client, DHCP_OPTION_LEASE_TIME);
      if (optPtr == 0)
      {
        LOG_ERROR("missing lease time");
        client->state = DHCP_INIT;
        return;
      }
      else
      {
        optLen = client->msg.field.opt[optPtr++];
        if (optLen != 4)
        {
          LOG_ERROR("bad option length for lease time: %u", optLen);
          client->state = DHCP_INIT;
          return;
        }

        // get lease duration in seconds
        leaseDuration = 0;
        leaseDuration |= client->msg.field.opt[optPtr++] << 24;
        leaseDuration |= client->msg.field.opt[optPtr++] << 16;
        leaseDuration |= client->msg.field.opt[optPtr++] <<  8;
        leaseDuration |= client->msg.field.opt[optPtr++] <<  0;
        LOG_INFO("DHCP Lease Time: %lus", leaseDuration);

        // arbitrarily de-rate lease duration for timing inaccuracy over long periods
        leaseDuration -= (leaseDuration >> 3);

        // convert duration to ticks
        client->leaseDuration = leaseDuration * configTICK_RATE_HZ;

        // overflow detection
        if (leaseDuration != 0 && client->leaseDuration / configTICK_RATE_HZ != leaseDuration)
        {
          // set it to the highest delay we are capable of
          client->leaseDuration = portMAX_DELAY;
        }
      }

      // copy IP locally
      for (i = 0; i < IPV4_BYTES; i++)
      {
        ip[i] = client->clientIp[i];
      }

      // set device IP address
      rc = W5500_SetSIPR(client->dev, ip);
      if (rc != W5500_OK)
      {
        LOG_ERROR("W5500_SetSIPR failed");
        client->state = DHCP_INIT;
        return;
      }
      LOG_INFO("Bound IP: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

      // set device subnet mask
      optPtr = GetOptionPointer(client, DHCP_OPTION_SUBNET_MASK);
      if (optPtr == 0)
      {
        LOG_ERROR("missing subnet mask");
        client->state = DHCP_INIT;
        return;
      }
      else
      {
        optLen = client->msg.field.opt[optPtr++];
        if (optLen != IPV4_BYTES)
        {
          LOG_ERROR("bad option length for subnet: %u", optLen);
          client->state = DHCP_INIT;
          return;
        }

        // copy IP locally
        for (i = 0; i < IPV4_BYTES; i++)
        {
          ip[i] = client->msg.field.opt[optPtr++];
        }

        // set subnet mask
        rc = W5500_SetSUBR(client->dev, ip);
        if (rc != W5500_OK)
        {
          LOG_ERROR("W5500_SetSUBR failed");
          client->state = DHCP_INIT;
          return;
        }
        LOG_INFO("Subnet Mask: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
      }

      // set device gateway
      optPtr = GetOptionPointer(client, DHCP_OPTION_ROUTER);
      if (optPtr == 0)
      {
        LOG_ERROR("missing gateway address");
        client->state = DHCP_INIT;
        return;
      }
      else
      {
        optLen = client->msg.field.opt[optPtr++];
        if (optLen % IPV4_BYTES)
        {
          LOG_ERROR("bad option length for router: %u", optLen);
          client->state = DHCP_INIT;
          return;
        }

        // copy IP locally
        for (i = 0; i < IPV4_BYTES; i++)
        {
          ip[i] = client->msg.field.opt[optPtr++];
        }

        // set gateway address
        rc = W5500_SetGAR(client->dev, ip);
        if (rc != W5500_OK)
        {
          LOG_ERROR("W5500_SetGAR failed");
          client->state = DHCP_INIT;
          return;
        }
        LOG_INFO("Gateway: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
      }

      // update state
      client->state = DHCP_BOUND;
      return;
    }
    else if (msgType == DHCP_NAK)
    {
      client->state = DHCP_INIT;
      return;
    }
    else
    {
      LOG_ERROR("bad event for %s: %s", StateString(client->state), MessageTypeString(msgType));
      client->state = DHCP_INIT;
      return;
    }
  }
  else if (rc == W5500_RECV_TIMEOUT)
  {
    LOG_WARNING("DHCP_REQUESTING TIMEOUT");
    client->state = DHCP_INIT;
    return;
  }
  else
  {
    LOG_ERROR("DHCP_Receive failed 0x%X", rc);
    client->state = DHCP_INIT;
    return;
  }
}

/*!
* @brief Called upon entering the BOUND state.
* @param client - DHCP client
*/
inline void DHCP_HandleBOUND(dhcp_client_t* client)
{
  size_t i;
  w5500_status_t rc;
  TickType_t sleepDuration;

  // resume tasks that need a bound IP
  for (i = 0; i < DHCP_NUM_BOUND_TASKS; i++)
  {
    vTaskResume(client->boundTask[i]);
  }

  // sleep
  sleepDuration = client->leaseDuration - (xTaskGetTickCount() - client->leaseTick);
  LOG_DEBUG("Sleeping for %lus", sleepDuration / configTICK_RATE_HZ);
  vTaskDelay(sleepDuration);

  // attempt renewal
  rc = DHCP_SendREQUEST(client);
  if (rc != W5500_OK)
  {
    LOG_ERROR("DHCP_SendREQUEST failed 0x%X", rc);
    client->state = DHCP_INIT;
  }
  else
  {
    client->state = DHCP_RENEWING;
  }

  // suspend tasks that need a bound IP
  for (i = 0; i < DHCP_NUM_BOUND_TASKS; i++)
  {
    vTaskSuspend(client->boundTask[i]);
  }

  return;
}

/*!
* @brief Called upon entering the RENEWING state.
* @param client - DHCP client
*/
inline void DHCP_HandleRENEWING(dhcp_client_t* client)
{
  w5500_status_t rc;
  dhcp_msg_type_t msgType;
  uint32_t leaseDuration;
  uint8_t optLen;
  size_t  optPtr;

  // DHCPREQUEST has been sent, wait for socket interrupt
  rc = DHCP_Receive(client, &msgType);
  if (rc == W5500_OK)
  {
    if (msgType == DHCP_ACK)
    {
      // set lease time
      client->leaseTick = xTaskGetTickCount();
      optPtr = GetOptionPointer(client, DHCP_OPTION_LEASE_TIME);
      if (optPtr == 0)
      {
        LOG_ERROR("missing lease time");
        client->state = DHCP_INIT;
        return;
      }
      else
      {
        optLen = client->msg.field.opt[optPtr++];
        if (optLen != 4)
        {
          LOG_ERROR("bad option length for lease time: %u", optLen);
          client->state = DHCP_INIT;
          return;
        }

        // get lease duration in seconds
        leaseDuration = 0;
        leaseDuration |= client->msg.field.opt[optPtr++] << 24;
        leaseDuration |= client->msg.field.opt[optPtr++] << 16;
        leaseDuration |= client->msg.field.opt[optPtr++] <<  8;
        leaseDuration |= client->msg.field.opt[optPtr++] <<  0;
        LOG_INFO("DHCP Lease Time: %lus", leaseDuration);

        // arbitrarily de-rate lease duration for timing inaccuracy over long periods
        leaseDuration -= (leaseDuration >> 3);

        // convert duration to ticks
        client->leaseDuration = leaseDuration * configTICK_RATE_HZ;

        // overflow detection
        if (leaseDuration != 0 && client->leaseDuration / configTICK_RATE_HZ != leaseDuration)
        {
          // set it to the highest delay we are capable of
          client->leaseDuration = portMAX_DELAY;
        }
      }

      // update state
      client->state = DHCP_BOUND;
      return;
    }
    else if (msgType == DHCP_NAK)
    {
      client->state = DHCP_INIT;
      return;
    }
    else
    {
      LOG_ERROR("bad event for %s: %s", StateString(client->state), MessageTypeString(msgType));
      client->state = DHCP_INIT;
      return;
    }
  }
  else if (rc == W5500_RECV_TIMEOUT)
  {
    LOG_WARNING("DHCP_HandleRENEWING TIMEOUT");
    client->state = DHCP_INIT;
    return;
  }
  else
  {
    LOG_ERROR("DHCP_Receive failed 0x%X", rc);
    client->state = DHCP_INIT;
    return;
  }
}
