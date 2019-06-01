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

#ifndef _W5500_LL_H_
#define _W5500_LL_H_

#include "stm32f0xx_hal.h"
#include "logging/logging.h"
#include "constants.h"
#include "w5500/w5500_common_regs.h"
#include "w5500/w5500_socket_regs.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#define W5500_NUM_SOCKETS 8 //!< number of sockets on the W5500
extern const uint8_t W5500_CHIP_VERSION; //!< chip version

//! helper macro to return on non-zero status codes
#define W5500_RETURN_NOT_OK(rc) \
  do                            \
  {                             \
    if ((rc) != W5500_OK)       \
    {                           \
      return (rc);              \
    }                           \
  } while (0)

//! W5500 socket interrupt events
typedef enum
{
  W5500_SN_EVENT_CON     = (1U << 0),
  W5500_SN_EVENT_DISCON  = (1U << 1),
  W5500_SN_EVENT_RECV    = (1U << 2),
  W5500_SN_EVENT_TIMEOUT = (1U << 3),
  W5500_SN_EVENT_SENDOK  = (1U << 4),
} w5500_sn_event_t;

//! W5500 device structure
typedef struct w5500_dev_t
{
  SPI_HandleTypeDef hspix;   //!< SPI port
  GPIO_TypeDef*      csPort;  //!< GPIO CS port
  uint16_t           csPin;   //!< GPIO CS pin
  GPIO_TypeDef*      rstPort; //!< GPIO rest port
  uint16_t           rstPin;  //!< GPIO rest pin
  GPIO_TypeDef*      intPort; //!< GPIO interrupt port
  uint16_t           intPin;  //!< GPIO interrupt pin
  w5500_sn_ir_t      snInt;   //!< socket N interrupt status
  uint8_t            mac[MAC_BYTES] __attribute__((aligned(16))); //!< MAC address
  EventGroupHandle_t snEvent[W5500_NUM_SOCKETS]; //! socket events
} w5500_dev_t;

//! W5500 device return codes
typedef enum 
{
  W5500_OK                  =  0U,
  W5500_SPI_ERROR           =  1U,
  W5500_SPI_BUSY            =  2U,
  W5500_SPI_TIMEOUT         =  3U,
  W5500_RD_ERROR            =  4U,
  W5500_LINK_ERROR          =  5U,
  W5500_TX_OVERFLOW         =  6U,
  W5500_RX_OVERFLOW         =  7U,
  W5500_STATUS_TIMEOUT      =  8U,
  W5500_CON_TIMEOUT         =  9U,
  W5500_SEND_TIMEOUT        = 10U,
  W5500_RECV_TIMEOUT        = 11U,
  W5500_DHCP_NO_COOKIE      = 12U,
  W5500_DHCP_BAD_OPCODE     = 13U,
  W5500_DHCP_CORRUPT_OPTION = 14U,
  W5500_DHCP_MISSING_TYPE   = 15U,
  W5500_OS_MEMORY_ERROR     = 16U,
  W5500_SOCKET_DISCONNECTED = 17U,
  W5500_MQTT_BAD_PACKET     = 18U,
  W5500_MQTT_CON_REFUSED    = 19U,
} w5500_status_t;

//! W5500 link status
typedef enum 
{
  W5500_LINK_DOWN    = 0U,
  W5500_LINK_UP      = 1U,
} w5500_link_status_t;

w5500_status_t W5500_GetMR(w5500_dev_t* dev, w5500_mr_t* mr);
w5500_status_t W5500_SetMR(w5500_dev_t* dev, w5500_mr_t* mr);
w5500_status_t W5500_GetGAR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_SetGAR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_GetSUBR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_SetSUBR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_GetSHAR(w5500_dev_t* dev, uint8_t* mac);
w5500_status_t W5500_SetSHAR(w5500_dev_t* dev, uint8_t* mac);
w5500_status_t W5500_GetSIPR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_SetSIPR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_GetINTLEVEL(w5500_dev_t* dev, uint16_t* intlevel);
w5500_status_t W5500_SetINTLEVEL(w5500_dev_t* dev, uint16_t* intlevel);
w5500_status_t W5500_GetIR(w5500_dev_t* dev, w5500_ir_t* ir);
w5500_status_t W5500_SetIR(w5500_dev_t* dev, w5500_ir_t* ir);
w5500_status_t W5500_GetIMR(w5500_dev_t* dev, w5500_ir_t* imr);
w5500_status_t W5500_SetIMR(w5500_dev_t* dev, w5500_ir_t* imr);
w5500_status_t W5500_GetSIR(w5500_dev_t* dev, uint8_t* sir);
w5500_status_t W5500_SetSIR(w5500_dev_t* dev, uint8_t* sir);
w5500_status_t W5500_GetSIMR(w5500_dev_t* dev, uint8_t* simr);
w5500_status_t W5500_SetSIMR(w5500_dev_t* dev, uint8_t* simr);
w5500_status_t W5500_GetRTR(w5500_dev_t* dev, uint16_t* rtr);
w5500_status_t W5500_SetRTR(w5500_dev_t* dev, uint16_t* rtr);
w5500_status_t W5500_GetRCR(w5500_dev_t* dev, uint8_t* rcr);
w5500_status_t W5500_SetRCR(w5500_dev_t* dev, uint8_t* rcr);
w5500_status_t W5500_GetPTIMER(w5500_dev_t* dev, uint8_t* ptimer);
w5500_status_t W5500_SetPTIMER(w5500_dev_t* dev, uint8_t* ptimer);
w5500_status_t W5500_GetPMAGIC(w5500_dev_t* dev, uint8_t* pmagic);
w5500_status_t W5500_SetPMAGIC(w5500_dev_t* dev, uint8_t* pmagic);
w5500_status_t W5500_GetPHAR(w5500_dev_t* dev, uint8_t* mac);
w5500_status_t W5500_SetPHAR(w5500_dev_t* dev, uint8_t* mac);
w5500_status_t W5500_GetPSID(w5500_dev_t* dev, uint16_t* psid);
w5500_status_t W5500_SetPSID(w5500_dev_t* dev, uint16_t* psid);
w5500_status_t W5500_GetPMRU(w5500_dev_t* dev, uint16_t* pmru);
w5500_status_t W5500_SetPMRU(w5500_dev_t* dev, uint16_t* pmru);
w5500_status_t W5500_GetUIPR(w5500_dev_t* dev, uint8_t* ip);
w5500_status_t W5500_GetUPORT(w5500_dev_t* dev, uint16_t* port);
w5500_status_t W5500_GetPHYCFGR(w5500_dev_t* dev, w5500_phycfgr_t* phycfgr);
w5500_status_t W5500_GetVERSIONR(w5500_dev_t* dev, uint8_t* ver);
w5500_status_t W5500_GetSnMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_mr_t* mr);
w5500_status_t W5500_SetSnMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_mr_t* mr);
w5500_status_t W5500_GetSnCR(w5500_dev_t* dev, uint8_t sn, uint8_t* cr);
w5500_status_t W5500_SetSnCR(w5500_dev_t* dev, uint8_t sn, uint8_t* cr);
w5500_status_t W5500_GetSnIR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir);
w5500_status_t W5500_SetSnIR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir);
w5500_status_t W5500_GetSnSR(w5500_dev_t* dev, uint8_t sn, uint8_t* sr);
w5500_status_t W5500_GetSnPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port);
w5500_status_t W5500_SetSnPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port);
w5500_status_t W5500_GetSnDHAR(w5500_dev_t* dev, uint8_t sn, uint8_t* mac);
w5500_status_t W5500_SetSnDHAR(w5500_dev_t* dev, uint8_t sn, uint8_t* mac);
w5500_status_t W5500_GetSnDIPR(w5500_dev_t* dev, uint8_t sn, uint8_t* ip);
w5500_status_t W5500_SetSnDIPR(w5500_dev_t* dev, uint8_t sn, uint8_t* ip);
w5500_status_t W5500_GetSnDPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port);
w5500_status_t W5500_SetSnDPORT(w5500_dev_t* dev, uint8_t sn, uint16_t* port);
w5500_status_t W5500_GetSnMSSR(w5500_dev_t* dev, uint8_t sn, uint16_t* mssr);
w5500_status_t W5500_SetSnMSSR(w5500_dev_t* dev, uint8_t sn, uint16_t* mssr);
w5500_status_t W5500_GetSnTOS(w5500_dev_t* dev, uint8_t sn, uint8_t* tos);
w5500_status_t W5500_SetSnTOS(w5500_dev_t* dev, uint8_t sn, uint8_t* tos);
w5500_status_t W5500_GetSnTTL(w5500_dev_t* dev, uint8_t sn, uint8_t* ttl);
w5500_status_t W5500_SetSnTTL(w5500_dev_t* dev, uint8_t sn, uint8_t* ttl);
w5500_status_t W5500_GetSnRxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size);
w5500_status_t W5500_SetSnRxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size);
w5500_status_t W5500_GetSnTxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size);
w5500_status_t W5500_SetSnTxBUFSIZE(w5500_dev_t* dev, uint8_t sn, uint8_t* size);
w5500_status_t W5500_GetSnTxFSR(w5500_dev_t* dev, uint8_t sn, uint16_t* size);
w5500_status_t W5500_GetSnTxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_GetSnTxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_SetSnTxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_GetSnRxRSR(w5500_dev_t* dev, uint8_t sn, uint16_t* size);
w5500_status_t W5500_GetSnRxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_SetSnRxRD(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_GetSnRxWR(w5500_dev_t* dev, uint8_t sn, uint16_t* ptr);
w5500_status_t W5500_GetSnIMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir);
w5500_status_t W5500_SetSnIMR(w5500_dev_t* dev, uint8_t sn, w5500_sn_ir_t* ir);
w5500_status_t W5500_GetSnFRAG(w5500_dev_t* dev, uint8_t sn, uint16_t* frag);
w5500_status_t W5500_SetSnFRAG(w5500_dev_t* dev, uint8_t sn, uint16_t* frag);
w5500_status_t W5500_GetSnKPALVTR(w5500_dev_t* dev, uint8_t sn, uint8_t* kpalvtr);
w5500_status_t W5500_SetSnKPALVTR(w5500_dev_t* dev, uint8_t sn, uint8_t* kpalvtr);

w5500_status_t W5500_SetSnTxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len);
w5500_status_t W5500_SetSnRxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len);
w5500_status_t W5500_GetSnTxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len);
w5500_status_t W5500_GetSnRxBuf(w5500_dev_t* dev, uint8_t sn, uint16_t ptr, uint8_t* data, uint16_t len);

void W5500_HardReset(w5500_dev_t* dev);
const char* W5500_StatusString(w5500_status_t status);

#endif // _W5500_LL_H_
