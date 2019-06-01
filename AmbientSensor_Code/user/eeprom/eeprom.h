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

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "stm32f0xx_hal.h"
#include "logging/logging.h"

#define EEPROM_MAC_ADDRESS_BYTES    6 //!< number of MAC address bytes (EUI-48)
#define EEPROM_MAC_MEMORY_START  0xFA //!< memory location of the EUI-48 MAC

//! 25AA02E48 EEPROM device structure
typedef struct eeprom_dev_t
{
  SPI_HandleTypeDef hspix;  //!< SPI port
  GPIO_TypeDef*     csPort; //!< GPIO CS port
  uint16_t          csPin;  //!< GPIO CS pin
} eeprom_dev_t;

//! 25AA02E48 EEPROM device return codes
typedef enum 
{
  EEPROM_OK           = 0x00U,
  EEPROM_SPI_ERROR    = 0x01U,
  EEPROM_SPI_BUSY     = 0x02U,
  EEPROM_SPI_TIMEOUT  = 0x03U,
  EEPROM_MAC_ALL_ONE  = 0x04u,
  EEPROM_MAC_ALL_ZERO = 0x05u,
  EEPROM_MAC_MIRROR   = 0x06u,
} eeprom_status_t;

//! EEPROM command codes
enum eeprom_cmd_e
{
  EEPROM_WRSR = 1, //!< write status register
  EEPROM_WRITE,    //!< write data
  EEPROM_READ,     //!< read data
  EEPROM_WRDI,     //!< reset the write enable latch (disable write operations)
  EEPROM_RDSR,     //!< read status register
  EEPROM_WREN      //!< set the write enable latch (enable write operations)
};

eeprom_status_t EEPROM_ReadMemory(eeprom_dev_t* dev, uint8_t addr, uint8_t* buf, uint16_t num);
eeprom_status_t EEPROM_ReadMAC(eeprom_dev_t* dev, uint8_t* mac);
const char* EEPROM_StatusString(eeprom_status_t status);

#endif // _EEPROM_H_
