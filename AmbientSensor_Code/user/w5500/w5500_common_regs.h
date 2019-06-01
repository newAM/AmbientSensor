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

#ifndef _W5500_COMMON_REGS_H_
#define _W5500_COMMON_REGS_H_

//! mode register
typedef union w5500_mr_t
{
  struct
  {
    uint8_t rsvd0 : 1; //!< reserved
    uint8_t farp  : 1; //!< force ARP
    uint8_t rsvd2 : 1; //!< reserved
    uint8_t pppoe : 1; //!< PPPoE mode
    uint8_t pb    : 1; //!< ping block mode
    uint8_t wol   : 1; //!< wake on lan
    uint8_t rsvd6 : 1; //!< reserved
    uint8_t rst   : 1; //!< software reset
  } bits;
  uint8_t all;
} w5500_mr_t;

//! interrupt register
typedef union w5500_ir_t
{
  struct
  {
    uint8_t rsvd_0   : 4; //!< reserved
    uint8_t mp       : 1; //!< magic packet
    uint8_t ppoe     : 1; //!< PPPoE close
    uint8_t unreach  : 1; //!< destination unreachable
    uint8_t conflict : 1; //!< IP conflict
  } bits;
  uint8_t all;
} w5500_ir_t;

//! PHY configuration register
typedef union w5500_phycfgr_t
{
  struct
  {
    uint8_t lnk   : 1; //!< link status: 0=up, 1=down
    uint8_t spd   : 1; //!< speed: 0=10Mbps, 1=100Mbps
    uint8_t dpx   : 1; //!< duplex: 0=half, 1=full
    uint8_t opmdc : 3; //!< operation mode, 0x7 = all capable, auto-negotiation enabled.
    uint8_t opmd  : 1; //!< 0 = configure OPMDC with PMODE HW pins, 1 = configure with PHYCFGR
    uint8_t rst   : 1; //!< 0=reset
  } bits;
  uint8_t all;
} w5500_phycfgr_t;

#endif // _W5500_COMMON_REGS_H_
