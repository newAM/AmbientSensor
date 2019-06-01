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

#ifndef _W5500_SOCKET_REGS_H_
#define _W5500_SOCKET_REGS_H_

//! socket protocols
typedef enum
{
  W5500_SN_PROTO_CLOSED = 0,
  W5500_SN_PROTO_TCP    = 1,
  W5500_SN_PROTO_UDP    = 2,
  W5500_SN_PROTO_MACRAW = 3, // can only be used in socket 0
} w5500_socket_proto_t;

//! socket commands
typedef enum 
{
  // W5500_SOCKET_CMD_ACCEPTED  = 0x00, // clears to this state once accepted
  W5500_SN_CMD_OPEN      = 0x01,
  W5500_SN_CMD_LISTEN    = 0x02,
  W5500_SN_CMD_CONNECT   = 0x04,
  W5500_SN_CMD_DISCON    = 0x08,
  W5500_SN_CMD_CLOSE     = 0x10,
  W5500_SN_CMD_SEND      = 0x20,
  W5500_SN_CMD_SEND_MAC  = 0x21,
  W5500_SN_CMD_SEND_KEEP = 0x22,
  W5500_SN_CMD_RECV      = 0x40,
} w5500_socket_cmd_t;

//! socket status
typedef enum 
{
  W5500_SN_STATUS_CLOSED      = 0x00,
  W5500_SN_STATUS_INIT        = 0x13,
  W5500_SN_STATUS_LISTEN      = 0x14,
  W5500_SN_STATUS_SYNSENT     = 0x15,
  W5500_SN_STATUS_SYNRECV     = 0x16,
  W5500_SN_STATUS_ESTABLISHED = 0x17,
  W5500_SN_STATUS_FIN_WAIT    = 0x18,
  W5500_SN_STATUS_CLOSING     = 0x1A,
  W5500_SN_STATUS_TIME_WAIT   = 0x1B,
  W5500_SN_STATUS_CLOSE_WAIT  = 0x1C,
  W5500_SN_STATUS_LAST_ACK    = 0x1D,
  W5500_SN_STATUS_UDP         = 0x22,
  W5500_SN_STATUS_MACRAW      = 0x42,
} w5500_sn_status_t;

//! socket mode register
typedef union w5500_sn_mr_t
{
  struct
  {
    uint8_t proto  : 4; //!< protocol
    uint8_t ucastb : 1; //!< UNICAST blocking in UDP mode, IPv6 blocking in MACRAW mode
    uint8_t nd     : 1; //!< use no delayed ACK
    uint8_t bcastb : 1; //!< broadcast blocking in MACRAW and UDP mode
    uint8_t multi  : 1; //!< multi-casing in UDP mode
  } bits;
  uint8_t all;
} w5500_sn_mr_t;

//! socket interrupt register
typedef union w5500_sn_ir_t
{
  struct
  {
    uint8_t con     : 1; //!< connection to the peer is successful
    uint8_t discon  : 1; //!< FIN or FIN/ACK packet received from peer
    uint8_t recv    : 1; //!< data received from peer
    uint8_t timeout : 1; //!< ARP_TO or TCP_TO occurred
    uint8_t sendok  : 1; //!< SEND command is completed
    uint8_t rsvd    : 3; //!< reserved
  } bits;
  uint8_t all;
} w5500_sn_ir_t;

#endif // _W5500_SOCKET_REGS_H_
