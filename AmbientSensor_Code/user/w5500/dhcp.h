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

#ifndef _DHCP_H_
#define _DHCP_H_

#include "constants.h"
#include "w5500/w5500.h"
#include "logging/logging.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define DHCP_NUM_BOUND_TASKS    1 //!< Number of bound tasks
#define DHCP_SOURCE_PORT       68 //!< DHCP source port
#define DHCP_DESTINATION_PORT  67 //!< DHCP destination port
#define DHCP_CHADDR_SIZE       16 //!< client hardware address size
#define DHCP_SNAME_SIZE        64 //!< server name
#define DHCP_FILE_SIZE        128 //!< boot filename size
#define DHCP_OPT_SIZE         312 //!< option field size
#define DHCP_MSG_SIZE         548 //!< total DHCP message size

//! DHCP states
typedef enum 
{
  DHCP_INIT = 0x00U,
  DHCP_SELECTING,
  DHCP_REQUESTING,
  DHCP_INITREBOOT,
  DHCP_REBOOTING,
  DHCP_BOUND,
  DHCP_RENEWING,
  DHCP_REBINDING,
} dhcp_state_t;

//! DHCP message types (Option 53) (RFC 2132)
typedef enum
{
  DHCP_DISCOVER = 1,
  DHCP_OFFER    = 2,
  DHCP_REQUEST  = 3,
  DHCP_DECLINE  = 4,
  DHCP_ACK      = 5,
  DHCP_NAK      = 6,
  DHCP_RELEASE  = 7,
  DHCP_INFORM   = 8,
} dhcp_msg_type_t;

//! DHCP message op codes
typedef enum
{
  DHCP_BOOTREQUEST = 1,
  DHCP_BOOTREPLY   = 2,
} dhcp_op_t;

//! DHCP hardware types
typedef enum
{
  DHCP_HTYPE10MB  = 1,
  DHCP_HTYPE100MB = 2,
} dhcp_htype_t;

//! DHCP options (RFC 2132)
typedef enum
{
  DHCP_OPTION_PAD               =   0,
  DHCP_OPTION_SUBNET_MASK       =   1,
  DHCP_OPTION_TIME_OFFSET       =   2,
  DHCP_OPTION_ROUTER            =   3,
  DHCP_OPTION_TIME_SERVER       =   4,
  DHCP_OPTION_NAME_SERVER       =   5,
  DHCP_OPTION_DNS               =   6,
  DHCP_OPTION_HOSTNAME          =  12,
  DHCP_OPTION_REQUESTED_IP      =  50,
  DHCP_OPTION_LEASE_TIME        =  51,
  DHCP_OPTION_MESSAGE_TYPE      =  53,
  DHCP_OPTION_SERVER_ID         =  54,
  DHCP_OPTION_PARAMETER_REQUEST =  55,
  DHCP_OPTION_RENEWAL_TIME      =  58,
  DHCP_OPTION_REBINDING_TIME    =  59,
  DHCP_OPTION_CLIENT_ID         =  61,
  DHCP_OPTION_END               = 255,
} dhcp_option_t;

//! DHCP message
typedef union dhcp_msg_t
{
  struct
  {
    uint8_t  op;                         //!< operation code
    uint8_t  htype;                      //!< hardware type
    uint8_t  hlen;                       //!< hardware address length
    uint8_t  hops;                       //!< hops
    uint32_t xid;                        //!< transaction identifier
    uint16_t sec;                        //!< seconds
    uint16_t flags;                      //!< flags
    uint8_t  ciAddr[IPV4_BYTES];         //!< client IP address
    uint8_t  yiAddr[IPV4_BYTES];         //!< "your" IP address
    uint8_t  siAddr[IPV4_BYTES];         //!< server IP address
    uint8_t  giAddr[IPV4_BYTES];         //!< gateway IP address
    uint8_t  chAddr[DHCP_CHADDR_SIZE];   //!< client hardware address
    uint8_t  sname[DHCP_SNAME_SIZE];     //!< server name
    uint8_t  file[DHCP_FILE_SIZE];       //!< boot filename
    uint8_t  opt[DHCP_OPT_SIZE];         //!< options
  } __attribute__((packed)) field;
  uint8_t buf[DHCP_MSG_SIZE];
} dhcp_msg_t;

//! DHCP client
typedef struct dhcp_client_t
{
  w5500_dev_t* dev;                               //!< W5500 device to utilize 
  uint8_t      sn;                                //!< socket number
  char*        hostName;                          //!< device host name
  uint8_t      hostNameLen;                       //!< host name length
  uint8_t      clientIp[IPV4_BYTES];              //!< our IP address
  uint8_t      serverIp[IPV4_BYTES];              //!< DHCP server IP
  TickType_t   leaseTick;                         //!< DHCP lease time
  TickType_t   leaseDuration;                     //!< DHCP lease duration
  dhcp_state_t state;                             //!< DHCP state
  TaskHandle_t boundTask[DHCP_NUM_BOUND_TASKS];   //!< tasks to suspend when not bound
  dhcp_msg_t   msg __attribute__((aligned(16)));  //!< message buffer
} dhcp_client_t;

#endif // _DHCP_H_
