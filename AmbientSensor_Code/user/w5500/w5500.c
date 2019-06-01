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

#include "w5500/w5500.h"

/*!
* Initializes the W5500 device.
* The correct MAC address must already be set in the device structure.
* @param  dev - W5500 device structure
* @return W5500 status
*/
w5500_status_t W5500_Initialize(w5500_dev_t* dev)
{
  w5500_status_t rc;
  uint8_t reg __attribute__((aligned(16)));
  w5500_ir_t ir __attribute__((aligned(16)));
  w5500_phycfgr_t phycfg __attribute__((aligned(16)));

  W5500_HardReset(dev);

  // check that version register matches 
  rc = W5500_GetVERSIONR(dev, &reg);
  if (rc != W5500_OK)
  {
    return rc;
  }
  else if (reg != 0x04)
  {
    return W5500_RD_ERROR;
  }

  // wait for link up
  LOG_INFO("waiting for link up");
  do {
    rc = W5500_GetPHYCFGR(dev, &phycfg);
    W5500_RETURN_NOT_OK(rc);
  } while (phycfg.bits.lnk != W5500_LINK_UP);

  // source HW address
  rc = W5500_SetSHAR(dev, dev->mac); 
  W5500_RETURN_NOT_OK(rc);

  // enable socket interrupts
  reg = 0xFF;
  rc = W5500_SetSIMR(dev, &reg);
  W5500_RETURN_NOT_OK(rc);

  // enable generic interrupts
  ir.all = 0;
  ir.bits.conflict = 1;
  ir.bits.unreach = 1;
  rc = W5500_SetIMR(dev, &ir);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Logs the PHY status.
* @param  dev - W5500 device structure
* @return W5500 status
*/
w5500_status_t W5500_LogPhyStatus(w5500_dev_t* dev)
{
  w5500_phycfgr_t phycfg __attribute__((aligned(16)));
  w5500_status_t rc = W5500_GetPHYCFGR(dev, &phycfg);
  W5500_RETURN_NOT_OK(rc);

  LOG_INFO("PHY_CFG:   0x%02X",      phycfg.all);
  LOG_INFO("PHY RST:   %s in reset", phycfg.bits.rst ? "not" : "");
  LOG_INFO("PHY OPMD:  %cW",         phycfg.bits.opmd ? 'S' : 'H');
  LOG_INFO("PHY OPMDC: 0x%x",        phycfg.bits.opmdc);
  LOG_INFO("PHY DPX:   %s duplex",   phycfg.bits.dpx ? "Full" : "Half");
  LOG_INFO("PHY SPD:   %uMbps",      phycfg.bits.spd ? 100 : 10);
  LOG_INFO("PHY LNK:   Link %s",     phycfg.bits.lnk ? "Up" : "Down");
  return rc;
}

/*!
* @brief  Closes a socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketClose(w5500_dev_t* dev, uint8_t sn, TickType_t timeout)
{
  if (dev->snEvent[sn] != NULL)
  {
    vEventGroupDelete(dev->snEvent[sn]);
  }
  w5500_status_t rc = W5500_SocketCommand(dev, sn, W5500_SN_CMD_CLOSE);
  W5500_RETURN_NOT_OK(rc);
  return W5500_SocketStatusWait(dev, sn, W5500_SN_STATUS_CLOSED, timeout);
}

/*!
* @brief  Opens a socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  protocol - socket protocol
* @param  port - socket port to listen on
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketOpen(w5500_dev_t* dev, uint8_t sn, w5500_socket_proto_t protocol, uint16_t port, TickType_t timeout)
{
  w5500_status_t rc;
  w5500_sn_mr_t mode __attribute__((aligned(16)));
  w5500_sn_ir_t ir __attribute__((aligned(16)));

  // MACRAW can only be used on socket 0
  ASSERT(!(protocol == W5500_SN_PROTO_MACRAW && sn != 0));

  // close socket
  rc = W5500_SocketClose(dev, sn, timeout);
  W5500_RETURN_NOT_OK(rc);

  // clear socket interrupts
  rc = W5500_GetSnIR(dev, sn, &ir);
  W5500_RETURN_NOT_OK(rc);
  if (ir.all)
  {
    LOG_DEBUG("Clearing 0x%02X", ir.all);
    rc = W5500_SetSnIR(dev, sn, &ir);
    W5500_RETURN_NOT_OK(rc);
  }

  // create socket event group
  dev->snEvent[sn] = xEventGroupCreate();
  if (dev->snEvent[sn] == NULL)
  {
    return W5500_OS_MEMORY_ERROR;
  }

  // enable socket interrupts
  ir.all = 0;
  ir.bits.con     = 1;
  ir.bits.discon  = 1;
  ir.bits.timeout = 1;
  ir.bits.sendok  = 1;
  ir.bits.recv    = 1;
  rc = W5500_SetSnIMR(dev, sn, &ir);
  W5500_RETURN_NOT_OK(rc);

  // set socket mode
  mode.all = 0;
  mode.bits.proto = protocol;
  rc = W5500_SetSnMR(dev, sn, &mode);
  W5500_RETURN_NOT_OK(rc);

  // set socket source port
  rc = W5500_SetSnPORT(dev, sn, &port);
  W5500_RETURN_NOT_OK(rc);

  // open socket
  rc = W5500_SocketCommand(dev, sn, W5500_SN_CMD_OPEN);
  W5500_RETURN_NOT_OK(rc);

  // wait for open status
  if (protocol == W5500_SN_PROTO_TCP)
  {
    rc = W5500_SocketStatusWait(dev, sn, W5500_SN_STATUS_INIT, timeout);
  }
  else if (protocol == W5500_SN_PROTO_UDP)
  {
    rc = W5500_SocketStatusWait(dev, sn, W5500_SN_STATUS_UDP, timeout);
  }
  else if (protocol == W5500_SN_PROTO_MACRAW)
  {
    rc = W5500_SocketStatusWait(dev, sn, W5500_SN_STATUS_MACRAW, timeout);
  }

  return rc;
}

/*!
* @brief  Sets the destination address and port for the socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  ip - destination IP
* @param  port - destination port
* @return W5500 status
*/
w5500_status_t W5500_SocketDestination(w5500_dev_t* dev, uint8_t sn, uint8_t* ip, uint16_t port)
{
  w5500_status_t rc = W5500_SetSnDIPR(dev, sn, ip);
  W5500_RETURN_NOT_OK(rc);
  return W5500_SetSnDPORT(dev, sn, &port);
}

/*!
* @brief  Opens the connection for a TCP socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  ip - destination IP
* @param  port - destination port
* @return W5500 status
*/
w5500_status_t W5500_SocketConnect(w5500_dev_t* dev, uint8_t sn, uint8_t* ip, uint16_t port, TickType_t timeout)
{
  w5500_status_t rc;
  EventBits_t event;

  // set destination
  rc = W5500_SocketDestination(dev, sn, ip, port);
  W5500_RETURN_NOT_OK(rc);

  // send connection command
  rc = W5500_SocketCommand(dev, sn, W5500_SN_CMD_CONNECT);
  W5500_RETURN_NOT_OK(rc);

  // wait for CON event
  event = xEventGroupWaitBits(dev->snEvent[sn], W5500_SN_EVENT_CON, pdTRUE, pdFALSE, timeout);
  if (!(event & W5500_SN_EVENT_CON))
  {
    return W5500_CON_TIMEOUT;
  }

  return rc;
}

/*!
* @brief  Sends a command to a socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  cmd - socket command to send
* @return W5500 status
*/
w5500_status_t W5500_SocketCommand(w5500_dev_t* dev, uint8_t sn, uint8_t cmd)
{
  uint8_t cmdCopy __attribute__((aligned(16))) = cmd;
  w5500_status_t rc = W5500_SetSnCR(dev, sn, &cmdCopy);
  W5500_RETURN_NOT_OK(rc);

  // register will auto-clear when command has been accepted
  do {
    rc = W5500_GetSnCR(dev, sn, &cmdCopy);
    W5500_RETURN_NOT_OK(rc);
  } while (cmdCopy);
  return rc;
}

/*!
* @brief  Polls for a specific socket status.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  status - socket status to wait for
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketStatusWait(w5500_dev_t* dev, uint8_t sn, uint8_t status, TickType_t timeout)
{
  uint8_t currentStatus __attribute__((aligned(16)));
  w5500_status_t rc;
  TickType_t startTick = xTaskGetTickCount();
  do {
    rc = W5500_GetSnSR(dev, sn, &currentStatus);
    W5500_RETURN_NOT_OK(rc);
    if (xTaskGetTickCount() - startTick >= timeout)
    {
      return W5500_STATUS_TIMEOUT;
    }
  } while (currentStatus != status);
  return rc;
}

/*!
* @brief  Receives data from a TCP socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  data - buffer to receive data into
* @param  len - length of data buffer
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketRecieveTCP(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout)
{
  w5500_status_t rc;
  EventBits_t event;
  uint16_t ptr;
  uint16_t rsr;

  // wait for RECV event
  event = xEventGroupWaitBits(dev->snEvent[sn], W5500_SN_EVENT_RECV | W5500_SN_EVENT_DISCON, pdTRUE, pdFALSE, timeout);
  if (event & W5500_SN_EVENT_DISCON)
  {
    return W5500_SOCKET_DISCONNECTED;
  }
  else if (!(event & W5500_SN_EVENT_RECV))
  {
    return W5500_RECV_TIMEOUT;
  }

  rc = W5500_GetSnRxRSR(dev, sn, &rsr);
  W5500_RETURN_NOT_OK(rc);
  if (rsr > len)
  {
    return W5500_RX_OVERFLOW;
  }

  // get read pointer location
  rc = W5500_GetSnRxRD(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // read from W5500 into local buffer
  rc = W5500_GetSnRxBuf(dev, sn, ptr, data, rsr);
  W5500_RETURN_NOT_OK(rc);

  // increment pointer
  ptr += rsr;

  // set read pointer location
  rc = W5500_SetSnRxRD(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Receives data from a UDP socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  data - buffer to receive data into
* @param  len - length of data buffer
* @param  timeout - timeout duration in ticks
* @param  sourceIp - source address of the packet
* @param  sourcePort - source port of the packet
* @return W5500 status
*/
w5500_status_t W5500_SocketRecieveUDP(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout, uint8_t* sourceIp, uint16_t* sourcePort)
{
  w5500_packet_header_t header;
  w5500_status_t rc;
  EventBits_t event;
  size_t segment;
  uint16_t ptr;

  // wait for RECV event
  event = xEventGroupWaitBits(dev->snEvent[sn], W5500_SN_EVENT_RECV, pdTRUE, pdFALSE, timeout);
  if (!(event & W5500_SN_EVENT_RECV))
  {
    return W5500_RECV_TIMEOUT;
  }

  // get read pointer location
  rc = W5500_GetSnRxRD(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // read header
  rc = W5500_GetSnRxBuf(dev, sn, ptr, header.buf, W5500_PACKET_HEADER_SIZE);
  W5500_RETURN_NOT_OK(rc);
  header.field.size = BYTE_SWAP_16(header.field.size);

  // copy output arguments
  if (sourcePort != NULL)
  {
    *sourcePort = BYTE_SWAP_16(header.field.sourcePort);
  }
  if (sourceIp != NULL)
  {
    for (segment = 0; segment < IPV4_BYTES; segment++)
    {
      sourceIp[segment] = header.field.sourceIp[segment];
    }
  }

  // ensure local buffer can receive all data
  if (header.field.size > len)
  {
    return W5500_RX_OVERFLOW;
  }

  // increment pointer by header length
  ptr += W5500_PACKET_HEADER_SIZE;

  // read from W5500 into local buffer
  rc = W5500_GetSnRxBuf(dev, sn, ptr, data, header.field.size);
  W5500_RETURN_NOT_OK(rc);

  // increment pointer
  ptr += header.field.size;

  // set read pointer location
  rc = W5500_SetSnRxRD(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Sends data from a socket.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  data - data to write
* @param  len - length of data
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketSend(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout)
{
  w5500_status_t rc;
  EventBits_t event;
  uint16_t size;
  uint16_t ptr;

  event = xEventGroupGetBits(dev->snEvent[sn]);
  if (event & W5500_SN_EVENT_TIMEOUT || event & W5500_SN_EVENT_DISCON)
  {
    return W5500_SOCKET_DISCONNECTED;
  }

  if (len == 0)
  {
    return W5500_OK;
  }

  // get buffer free size
  rc = W5500_GetSnTxFSR(dev, sn, &size);
  W5500_RETURN_NOT_OK(rc);

  // message is larger than buffer free size
  if (len > size)
  {
    return W5500_TX_OVERFLOW;
  }

  // read the starting address for saving the transmitting data
  rc = W5500_GetSnTxWR(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // save the transmitting data at the starting address
  rc = W5500_SetSnTxBuf(dev, sn, ptr, data, len);
  W5500_RETURN_NOT_OK(rc);

  // increment pointer
  ptr += len;

  // send data in buffer
  rc = W5500_SocketSendBuffer(dev, sn, ptr, timeout);
  W5500_RETURN_NOT_OK(rc);

  return rc;
}

/*!
* @brief  Writes data to the socket incrementally.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  data - data to write
* @param  len - length of data
* @param  fsr - free size register value, must start as UINT32_MAX
* @param  ptr - position of the write pointer, must start as UINT32_MAX
* @return W5500 status
*/
w5500_status_t W5500_SocketWritePart(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, uint32_t* fsr, uint32_t* ptr)
{
  w5500_status_t rc;
  uint16_t _ptr;
  uint16_t _fsr;

  // initialize fsr
  if (*fsr == UINT32_MAX)
  {
    rc = W5500_GetSnTxFSR(dev, sn, &_fsr);
    W5500_RETURN_NOT_OK(rc);
  }
  else
  {
    _fsr = (uint16_t)(*fsr & UINT16_MAX);
  }

  // message is larger than buffer free size
  if (len > _fsr)
  {
    return W5500_TX_OVERFLOW;
  }

  // initialize ptr
  if (*ptr == UINT32_MAX)
  {
    rc = W5500_GetSnTxWR(dev, sn, &_ptr);
    W5500_RETURN_NOT_OK(rc);
  }
  else
  {
    _ptr = (uint16_t)(*ptr & UINT16_MAX);
  }

  if (len == 0)
  {
    rc = W5500_OK;
  }
  else
  {
    // save the transmitting data at the starting address
    rc = W5500_SetSnTxBuf(dev, sn, _ptr, data, len);
    W5500_RETURN_NOT_OK(rc);
  }

  // increment pointer
  _ptr += len;

  // decrement free size
  _fsr -= len;

  // store values
  *fsr = (uint32_t)_fsr;
  *ptr = (uint32_t)_ptr;

  return rc;
}

/*!
* @brief  Sends data currently in the buffer.
* @param  dev - W5500 device structure
* @param  sn - socket index
* @param  ptr - location of the write pointer
* @param  timeout - timeout duration in ticks
* @return W5500 status
*/
w5500_status_t W5500_SocketSendBuffer(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, TickType_t timeout)
{
  w5500_status_t rc;
  EventBits_t event;

  event = xEventGroupGetBits(dev->snEvent[sn]);
  if (event & W5500_SN_EVENT_TIMEOUT || event & W5500_SN_EVENT_DISCON)
  {
    return W5500_SOCKET_DISCONNECTED;
  }

  // set write pointer location
  rc = W5500_SetSnTxWR(dev, sn, &ptr);
  W5500_RETURN_NOT_OK(rc);

  // send data
  rc = W5500_SocketCommand(dev, sn, W5500_SN_CMD_SEND);
  W5500_RETURN_NOT_OK(rc);

  // wait for SENDOK event
  event = xEventGroupWaitBits(dev->snEvent[sn], W5500_SN_EVENT_SENDOK, pdTRUE, pdFALSE, timeout);
  if (!(event & W5500_SN_EVENT_SENDOK))
  {
    return W5500_SEND_TIMEOUT;
  }

  return rc;
}
