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

#include "w5500/w5500_ll.h"

// extern definitions
const uint8_t W5500_CHIP_VERSION = 4;

// private constants
#define               SPI_FRAME_BYTES       3  //!< length of the frame header for a SPI transfer
static const uint32_t RST_HOLD         =   10; //!< duration to hold a hardware reset for in ms
static const uint32_t SPI_TIMEOUT      =   50; //!< SPI transfer timeout in ms
static const uint8_t  COMMON_BLOCK     = 0x00; //!< common block select bits
static const uint8_t  SOCKET_BLOCK     = 0x01; //!< socket block base address
static const uint8_t  SOCKET_TX_BUF    = 0x02; //!< socket TX buffer base address
static const uint8_t  SOCKET_RX_BUF    = 0x03; //!< socket RX buffer base address
static const uint8_t  SOCKET_SPACING   = 0x04; //!< per socket spacing between blocks

// registers from Table 3.1: Common Register Block
static const uint8_t REG_MR            = 0x00; //!< [1] [RW] mode
static const uint8_t REG_GAR           = 0x01; //!< [4] [RW] gateway address
static const uint8_t REG_SUBR          = 0x05; //!< [4] [RW] subnet mask address
static const uint8_t REG_SHAR          = 0x09; //!< [6] [RW] source hardware address
static const uint8_t REG_SIPR          = 0x0F; //!< [4] [RW] source IP address
static const uint8_t REG_INTLEVEL      = 0x13; //!< [2] [RW] interrupt low level timer
static const uint8_t REG_IR            = 0x15; //!< [1] [RW] interrupt
static const uint8_t REG_IMR           = 0x16; //!< [1] [RW] interrupt mask
static const uint8_t REG_SIR           = 0x17; //!< [1] [RW] socket interrupt
static const uint8_t REG_SIMR          = 0x18; //!< [1] [RW] socket interrupt mask
static const uint8_t REG_RTR           = 0x19; //!< [2] [RW] retry time
static const uint8_t REG_RCR           = 0x1B; //!< [1] [RW] retry count
static const uint8_t REG_PTIMER        = 0x1C; //!< [1] [RW] PPP LCP request timer
static const uint8_t REG_PMAGIC        = 0x1D; //!< [1] [RW] PPP LCP magic number
static const uint8_t REG_PHAR          = 0x1E; //!< [6] [RW] PPP destination MAC address
static const uint8_t REG_PSID          = 0x24; //!< [2] [RW] PPP session identification
static const uint8_t REG_PMRU          = 0x26; //!< [2] [RW] PPP maximum segment size
static const uint8_t REG_UIPR          = 0x28; //!< [4] [R]  unreachable IP address
static const uint8_t REG_UPORT         = 0x2C; //!< [2] [R]  unreachable port
static const uint8_t REG_PHYCFGR       = 0x2E; //!< [1] [RW] PHY configuration
static const uint8_t REG_VERSIONR      = 0x39; //!< [1] [R]  chip version

// registers from Table 3.2: Socket Register Block
static const uint8_t REG_SN_MR         = 0x00; //!< [1] [RW] mode
static const uint8_t REG_SN_CR         = 0x01; //!< [1] [RW] command
static const uint8_t REG_SN_IR         = 0x02; //!< [1] [R]  interrupt
static const uint8_t REG_SN_SR         = 0x03; //!< [1] [R]  status
static const uint8_t REG_SN_PORT       = 0x04; //!< [2] [RW] source port
static const uint8_t REG_SN_DHAR       = 0x06; //!< [6] [RW] destination HW address
static const uint8_t REG_SN_DIPR       = 0x0C; //!< [4] [RW] destination IP address
static const uint8_t REG_SN_DPORT      = 0x10; //!< [2] [RW] destination port
static const uint8_t REG_SN_MSSR       = 0x12; //!< [2] [RW] maximum segment size
static const uint8_t REG_SN_TOS        = 0x15; //!< [1] [RW] IP TOS
static const uint8_t REG_SN_TTL        = 0x16; //!< [1] [RW] IP TTL
static const uint8_t REG_SN_RXBUF_SIZE = 0x1E; //!< [1] [RW] RX buffer size
static const uint8_t REG_SN_TXBUF_SIZE = 0x1F; //!< [1] [RW] TX buffer size
static const uint8_t REG_SN_TX_FSR     = 0x20; //!< [2] [R]  TX free size
static const uint8_t REG_SN_TX_RD      = 0x22; //!< [2] [R]  TX read pointer
static const uint8_t REG_SN_TX_WR      = 0x24; //!< [2] [RW] TX write pointer
static const uint8_t REG_SN_RX_RSR     = 0x26; //!< [2] [RW] RX received size
static const uint8_t REG_SN_RX_RD      = 0x28; //!< [2] [R]  RX read pointer
static const uint8_t REG_SN_RX_WR      = 0x2A; //!< [2] [R]  RX write pointer
static const uint8_t REG_SN_IMR        = 0x2C; //!< [1] [RW] interrupt mask
static const uint8_t REG_SN_FRAG       = 0x2D; //!< [2] [RW] fragment offset in IP header
static const uint8_t REG_SN_KPALVTR    = 0x2F; //!< [1] [RW] keep alive timer

// private function prototypes
static inline w5500_status_t W5500_Transfer(w5500_dev_t* dev, uint8_t* data, uint16_t len, uint16_t addr, uint8_t bsb, uint8_t access);

//! W5500 SPI access modes
typedef enum 
{
  W5500_SPI_RD = 0, //!< read access
  W5500_SPI_WR = 1, //!< write access
} w5500_spi_rw_t;

/*!
* @brief  Resets the W5500 via the hardware reset pin.
* @param  dev - W5500 device structure
*/
void W5500_HardReset(w5500_dev_t* dev)
{
  HAL_GPIO_WritePin(dev->rstPort, dev->rstPin, GPIO_PIN_RESET);
  vTaskDelay((configTICK_RATE_HZ * RST_HOLD) / 1000);
  HAL_GPIO_WritePin(dev->rstPort, dev->rstPin, GPIO_PIN_SET);
  vTaskDelay((configTICK_RATE_HZ * RST_HOLD) / 1000);
}

/*!
* @brief  enum to string conversion for W5500 status codes.
* @param  status - status enumeration value
* @return string for the corresponding enumeration
*/
const char* W5500_StatusString(w5500_status_t status)
{
  switch (status)
  {
    case W5500_OK:
      return "OK";
    case W5500_SPI_ERROR:
      return "SPI_ERROR";
    case W5500_SPI_BUSY:
      return "SPI_BUSY";
    case W5500_SPI_TIMEOUT:
      return "SPI_TIMEOUT";
    case W5500_RD_ERROR:
      return "RD_ERROR";
    case W5500_LINK_ERROR:
      return "LINK_ERROR";
    case W5500_TX_OVERFLOW:
      return "TX_OVERFLOW";
    case W5500_RX_OVERFLOW:
      return "RX_OVERFLOW";
    case W5500_STATUS_TIMEOUT:
      return "STATUS_TIMEOUT";
    case W5500_CON_TIMEOUT:
      return "CON_TIMEOUT";
    case W5500_SEND_TIMEOUT:
      return "SEND_TIMEOUT";
    case W5500_RECV_TIMEOUT:
      return "RECV_TIMEOUT";
    case W5500_DHCP_NO_COOKIE:
      return "DHCP_NO_COOKIE";
    case W5500_DHCP_BAD_OPCODE:
      return "DHCP_BAD_OPCODE";
    case W5500_DHCP_CORRUPT_OPTION:
      return "DHCP_CORRUPT_OPTION";
    case W5500_DHCP_MISSING_TYPE:
      return "DHCP_MISSING_TYPE";
    case W5500_OS_MEMORY_ERROR:
      return "OS_MEMORY_ERROR";
    case W5500_SOCKET_DISCONNECTED:
      return "SOCKET_DISCONNECTED";
    case W5500_MQTT_BAD_PACKET:
      return "MQTT_BAD_PACKET";
    case W5500_MQTT_CON_REFUSED:
      return "MQTT_CON_REFUSED";
    default:
      return "UNKNOWN";
  }
}

/******************************************************************************
* COMMON BLOCK PUBLIC FUNCTIONS
******************************************************************************/

w5500_status_t W5500_GetMR(w5500_dev_t* dev, w5500_mr_t* mr)
{
  return W5500_Transfer(dev, &mr->all, 1, REG_MR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetMR(w5500_dev_t* dev, w5500_mr_t* mr)
{
  return W5500_Transfer(dev, &mr->all, 1, REG_MR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetGAR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_GAR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetGAR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_GAR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetSUBR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SUBR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetSUBR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SUBR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetSHAR(w5500_dev_t* dev, uint8_t* mac)
{
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_SHAR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetSHAR(w5500_dev_t* dev, uint8_t* mac)
{
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_SHAR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetSIPR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SIPR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetSIPR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SIPR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetINTLEVEL(w5500_dev_t* dev, uint16_t* intlevel)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_INTLEVEL, COMMON_BLOCK, W5500_SPI_RD);
  *intlevel = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetINTLEVEL(w5500_dev_t* dev, uint16_t* intlevel)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*intlevel & 0xFF00) >> 8;
  buf[1] = (*intlevel & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_INTLEVEL, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetIR(w5500_dev_t* dev, w5500_ir_t* ir)
{
  return W5500_Transfer(dev, &ir->all, 1, REG_IR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetIR(w5500_dev_t* dev, w5500_ir_t* ir)
{
  return W5500_Transfer(dev, &ir->all, 1, REG_IR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetIMR(w5500_dev_t* dev, w5500_ir_t* imr)
{
  return W5500_Transfer(dev, &imr->all, 1, REG_IMR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetIMR(w5500_dev_t* dev, w5500_ir_t* imr)
{
  return W5500_Transfer(dev, &imr->all, 1, REG_IMR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetSIR(w5500_dev_t* dev, uint8_t* sir)
{
  return W5500_Transfer(dev, sir, 1, REG_SIR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetSIR(w5500_dev_t* dev, uint8_t* sir)
{
  return W5500_Transfer(dev, sir, 1, REG_SIR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetSIMR(w5500_dev_t* dev, uint8_t* simr)
{
  return W5500_Transfer(dev, simr, 1, REG_SIMR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetSIMR(w5500_dev_t* dev, uint8_t* simr)
{
  return W5500_Transfer(dev, simr, 1, REG_SIMR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetRTR(w5500_dev_t* dev, uint16_t* rtr)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_RTR, COMMON_BLOCK, W5500_SPI_RD);
  *rtr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetRTR(w5500_dev_t* dev, uint16_t* rtr)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*rtr & 0xFF00) >> 8;
  buf[1] = (*rtr & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_RTR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetRCR(w5500_dev_t* dev, uint8_t* rcr)
{
  return W5500_Transfer(dev, rcr, 1, REG_RCR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetRCR(w5500_dev_t* dev, uint8_t* rcr)
{
  return W5500_Transfer(dev, rcr, 1, REG_RCR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetPTIMER(w5500_dev_t* dev, uint8_t* ptimer)
{
  return W5500_Transfer(dev, ptimer, 1, REG_PTIMER, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetPTIMER(w5500_dev_t* dev, uint8_t* ptimer)
{
  return W5500_Transfer(dev, ptimer, 1, REG_PTIMER, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetPMAGIC(w5500_dev_t* dev, uint8_t* pmagic)
{
  return W5500_Transfer(dev, pmagic, 1, REG_PMAGIC, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetPMAGIC(w5500_dev_t* dev, uint8_t* pmagic)
{
  return W5500_Transfer(dev, pmagic, 1, REG_PMAGIC, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetPHAR(w5500_dev_t* dev, uint8_t* mac)
{
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_PHAR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_SetPHAR(w5500_dev_t* dev, uint8_t* mac)
{
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_PHAR, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetPSID(w5500_dev_t* dev, uint16_t* psid)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_PSID, COMMON_BLOCK, W5500_SPI_RD);
  *psid = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetPSID(w5500_dev_t* dev, uint16_t* psid)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*psid & 0xFF00) >> 8;
  buf[1] = (*psid & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_PSID, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetPMRU(w5500_dev_t* dev, uint16_t* pmru)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_PMRU, COMMON_BLOCK, W5500_SPI_RD);
  *pmru = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetPMRU(w5500_dev_t* dev, uint16_t* pmru)
{
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*pmru & 0xFF00) >> 8;
  buf[1] = (*pmru & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_PMRU, COMMON_BLOCK, W5500_SPI_WR);
}
w5500_status_t W5500_GetUIPR(w5500_dev_t* dev, uint8_t* ip)
{
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_UIPR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_GetUPORT(w5500_dev_t* dev, uint16_t* port)
{
  uint8_t buf[PORT_BYTES] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, PORT_BYTES, REG_UPORT, COMMON_BLOCK, W5500_SPI_RD);
  *port = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_GetPHYCFGR(w5500_dev_t* dev, w5500_phycfgr_t* phycfgr)
{
  return W5500_Transfer(dev, &phycfgr->all, 1, REG_PHYCFGR, COMMON_BLOCK, W5500_SPI_RD);
}
w5500_status_t W5500_GetVERSIONR(w5500_dev_t* dev, uint8_t* ver)
{
  return W5500_Transfer(dev, ver, 1, REG_VERSIONR, COMMON_BLOCK, W5500_SPI_RD);
}

/******************************************************************************
* SOCKET REGISTER PUBLIC FUNCTIONS
******************************************************************************/

w5500_status_t W5500_GetSnMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_mr_t* mr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &mr->all, 1, REG_SN_MR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_mr_t* mr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &mr->all, 1, REG_SN_MR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnCR(w5500_dev_t* dev, uint8_t sn, uint8_t* cr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, cr, 1, REG_SN_CR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnCR(w5500_dev_t* dev, uint8_t sn, uint8_t* cr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, cr, 1, REG_SN_CR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnIR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &ir->all, 1, REG_SN_IR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnIR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &ir->all, 1, REG_SN_IR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnSR(w5500_dev_t* dev, uint8_t sn, uint8_t* sr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, sr, 1, REG_SN_SR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_GetSnPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[PORT_BYTES] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, PORT_BYTES, REG_SN_PORT, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *port = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[PORT_BYTES] __attribute__((aligned(16)));
  buf[0] = (*port & 0xFF00) >> 8;
  buf[1] = (*port & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, PORT_BYTES, REG_SN_PORT, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnDHAR(w5500_dev_t* dev, uint8_t sn, uint8_t* mac)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_SN_DHAR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnDHAR(w5500_dev_t* dev, uint8_t sn, uint8_t* mac)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, mac, MAC_BYTES, REG_SN_DHAR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnDIPR(w5500_dev_t* dev, uint8_t sn, uint8_t* ip)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SN_DIPR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnDIPR(w5500_dev_t* dev, uint8_t sn, uint8_t* ip)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, ip, IPV4_BYTES, REG_SN_DIPR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnDPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[PORT_BYTES] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, PORT_BYTES, REG_SN_DPORT, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *port = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnDPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[PORT_BYTES] __attribute__((aligned(16)));
  buf[0] = (*port & 0xFF00) >> 8;
  buf[1] = (*port & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, PORT_BYTES, REG_SN_DPORT, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnMSSR(w5500_dev_t* dev, uint8_t sn, uint16_t* mssr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_MSSR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *mssr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnMSSR(w5500_dev_t* dev, uint8_t sn, uint16_t* mssr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*mssr & 0xFF00) >> 8;
  buf[1] = (*mssr & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_SN_MSSR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnTOS(w5500_dev_t* dev, uint8_t sn, uint8_t* tos)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, tos, 1, REG_SN_TOS, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnTOS(w5500_dev_t* dev, uint8_t sn, uint8_t* tos)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, tos, 1, REG_SN_TOS, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnTTL(w5500_dev_t* dev, uint8_t sn, uint8_t* ttl)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, ttl, 1, REG_SN_TTL, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnTTL(w5500_dev_t* dev, uint8_t sn, uint8_t* ttl)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, ttl, 1, REG_SN_TTL, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnRxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, size, 1, REG_SN_RXBUF_SIZE, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnRxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, size, 1, REG_SN_RXBUF_SIZE, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnTxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, size, 1, REG_SN_TXBUF_SIZE, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnTxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, size, 1, REG_SN_TXBUF_SIZE, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnTxFSR(w5500_dev_t* dev, uint8_t sn, uint16_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_TX_FSR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *size = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_GetSnTxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_TX_RD, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *ptr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_GetSnTxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_TX_WR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *ptr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnTxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*ptr & 0xFF00) >> 8;
  buf[1] = (*ptr & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_SN_TX_WR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnRxRSR(w5500_dev_t* dev, uint8_t sn, uint16_t* size)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_RX_RSR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *size = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_GetSnRxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_RX_RD, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *ptr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnRxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*ptr & 0xFF00) >> 8;
  buf[1] = (*ptr & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_SN_RX_RD, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnRxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_RX_WR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *ptr = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_GetSnIMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &ir->all, 1, REG_SN_IMR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnIMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, &ir->all, 1, REG_SN_IMR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnFRAG(w5500_dev_t* dev, uint8_t sn, uint16_t* frag)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  w5500_status_t rc = W5500_Transfer(dev, buf, 2, REG_SN_FRAG, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
  *frag = (buf[0] << 8) | buf[1];  
  return rc;
}
w5500_status_t W5500_SetSnFRAG(w5500_dev_t* dev, uint8_t sn, uint16_t* frag)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  uint8_t buf[2] __attribute__((aligned(16)));
  buf[0] = (*frag & 0xFF00) >> 8;
  buf[1] = (*frag & 0x00FF) >> 0;
  return W5500_Transfer(dev, buf, 2, REG_SN_FRAG, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnKPALVTR(w5500_dev_t* dev, uint8_t sn, uint8_t* kpalvtr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, kpalvtr, 1, REG_SN_KPALVTR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_SetSnKPALVTR(w5500_dev_t* dev, uint8_t sn, uint8_t* kpalvtr)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, kpalvtr, 1, REG_SN_KPALVTR, SOCKET_BLOCK + sn * SOCKET_SPACING, W5500_SPI_WR);
}

/******************************************************************************
* SOCKET BUFFER PUBLIC FUNCTIONS
******************************************************************************/

w5500_status_t W5500_SetSnTxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, data, len, ptr, SOCKET_TX_BUF + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_SetSnRxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, data, len, ptr, SOCKET_RX_BUF + sn * SOCKET_SPACING, W5500_SPI_WR);
}
w5500_status_t W5500_GetSnTxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, data, len, ptr, SOCKET_TX_BUF + sn * SOCKET_SPACING, W5500_SPI_RD);
}
w5500_status_t W5500_GetSnRxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len)
{
  ASSERT(sn < W5500_NUM_SOCKETS);
  return W5500_Transfer(dev, data, len, ptr, SOCKET_RX_BUF + sn * SOCKET_SPACING, W5500_SPI_RD);
}

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

//! SPI frame header
typedef union
{
  struct
  {
    uint16_t addr;     //!< 16-bit offset address
    struct
    {
      uint8_t om  : 2; //!< output mode (0 = variable data length)
      uint8_t rw  : 1; //!< 0=read 1=write
      uint8_t bsb : 5; //!< block select bits (register, socket register, socket buffer)
    } __attribute__((packed)) ctrl;
  } __attribute__((packed)) field;
  uint8_t buf[SPI_FRAME_BYTES];
} w5500_spi_header_t;

//! W5500 SPI output modes
typedef enum
{
  W5500_SPI_VDM   = 0, //!< variable data length mode
  W5500_SPI_FDM_1 = 1, //!< 1-byte fixed data length mode
  W5500_SPI_FDM_2 = 2, //!< 2-byte fixed data length mode
  W5500_SPI_FDM_4 = 3, //!< 4-byte fixed data length mode
} w5500_spi_op_mode_t;

/*!
* @brief  Transfers data to and from the W5500
* @param  dev    - W5500 device structure
* @param  data   - read or write buffer
* @param  len    - length of data
* @param  addr   - 16-bit offset address for the transfer
* @param  bsb    - block selection bits
* @param  access - access mode, either read or write
* @return HAL status
*/
inline w5500_status_t W5500_Transfer(
  w5500_dev_t* dev,
  uint8_t*     data,
  uint16_t     len,
  uint16_t     addr,
  uint8_t      bsb,
  uint8_t      access
)
{
  HAL_StatusTypeDef rc;
  w5500_spi_header_t header __attribute__((aligned(16))) = {
    .field.addr     = BYTE_SWAP_16(addr),
    .field.ctrl.om  = W5500_SPI_VDM,
    .field.ctrl.rw  = access,
    .field.ctrl.bsb = bsb,
  };

  // check for correct alignment
  ASSERT(IS_SPI_16BIT_ALIGNED_ADDRESS(data));

  taskENTER_CRITICAL();
  HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_RESET);

  // send header
  rc = HAL_SPI_Transmit(&dev->hspix, header.buf, SPI_FRAME_BYTES, SPI_TIMEOUT);
  if (rc != HAL_OK)
  {
    goto cleanup; // forgive me
  }

  // do the data transfer
  if (access == W5500_SPI_RD)
  {
    rc = HAL_SPI_Receive(&dev->hspix, data, len, SPI_TIMEOUT);
  }
  else if (access == W5500_SPI_WR)
  {
    rc = HAL_SPI_Transmit(&dev->hspix, data, len, SPI_TIMEOUT);
  }

cleanup:
  HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_SET);
  taskEXIT_CRITICAL();

  return (w5500_status_t)rc;
}
