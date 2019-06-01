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

#ifndef _W5500_H_
#define _W5500_H_

#include "stm32f0xx_hal.h"
#include "constants.h"
#include "w5500/w5500_ll.h"

#define W5500_PACKET_HEADER_SIZE 8 //!< length of a packet header

//! W5500 packet header
typedef union w5500_packet_header_t
{
  struct
  {
    uint8_t  sourceIp[IPV4_BYTES];
    uint16_t sourcePort;
    uint16_t size;
  } field __attribute__((packed));
  uint8_t buf[W5500_PACKET_HEADER_SIZE] __attribute__((aligned(16)));
} w5500_packet_header_t;

w5500_status_t W5500_Initialize(w5500_dev_t* dev);
w5500_status_t W5500_LogPhyStatus(w5500_dev_t* dev);
w5500_status_t W5500_SocketClose(w5500_dev_t* dev, uint8_t sn, TickType_t timeout);
w5500_status_t W5500_SocketOpen(w5500_dev_t* dev, uint8_t sn, w5500_socket_proto_t protocol, uint16_t port, TickType_t timeout);
w5500_status_t W5500_SocketDestination(w5500_dev_t* dev, uint8_t sn, uint8_t* ip, uint16_t port);
w5500_status_t W5500_SocketConnect(w5500_dev_t* dev, uint8_t sn, uint8_t* ip, uint16_t port, TickType_t timeout);
w5500_status_t W5500_SocketCommand(w5500_dev_t* dev, uint8_t sn, uint8_t cmd);
w5500_status_t W5500_SocketStatusWait(w5500_dev_t* dev, uint8_t sn, uint8_t status, TickType_t timeout);
w5500_status_t W5500_SocketRecieveTCP(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout);
w5500_status_t W5500_SocketRecieveUDP(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout, uint8_t* sourceIp, uint16_t* sourcePort);
w5500_status_t W5500_SocketSend(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, TickType_t timeout);
w5500_status_t W5500_SocketWritePart(w5500_dev_t* dev, uint8_t sn, uint8_t* data, uint16_t len, uint32_t* fsr, uint32_t* ptr);
w5500_status_t W5500_SocketSendBuffer(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, TickType_t timeout);

#endif // _W5500_H_
