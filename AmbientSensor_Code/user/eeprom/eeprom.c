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

#include "eeprom/eeprom.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

static const uint32_t EEPROM_TIMEOUT = 1000; //!< SPI timeout in milliseconds

/*!
* @brief  Read from memory on the 25AA02E48 EEPROM chip
* @param  dev  - 25AA02E48 device structure
* @param  addr - starting address of the EEPROM to read from
* @param  buf  - buffer array for the read data
* @param  num  - number of bytes to read
* @return EEPROM status
*/
eeprom_status_t EEPROM_ReadMemory(eeprom_dev_t* dev, uint8_t addr, uint8_t* buf, uint16_t num)
{
  HAL_StatusTypeDef rc; // return code

  // elevate priority during transfer
  UBaseType_t originalPriority = uxTaskPriorityGet(NULL);
  vTaskPrioritySet(NULL, osPriorityRealtime);

  // enable CS
  HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_RESET);

  // send read command followed by address
  uint8_t txData[] = {EEPROM_READ, addr};
  rc = HAL_SPI_Transmit(&dev->hspix, txData, 2, EEPROM_TIMEOUT);

  // check for failed transmission
  if (rc != HAL_OK)
  {
    goto cleanup; // forgive me
  }

  // grab all the data bytes
  rc = HAL_SPI_Receive(&dev->hspix, buf, num, EEPROM_TIMEOUT);

cleanup:
  // disable CS
  HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_SET);

  // restore original priority
  vTaskPrioritySet(NULL, originalPriority);

  return (eeprom_status_t) rc;
}

/*!
* @brief  Read the MAC address from memory
* @param  dev - 25AA02E48 device structure
* @param  mac - buffer to read the MAC address into **MUST BE 16-BYTE ALIGNED**
* @return EEPROM status
*/
eeprom_status_t EEPROM_ReadMAC(eeprom_dev_t* dev, uint8_t* mac)
{
  eeprom_status_t rc; // return code
  size_t i; // loop counter

  // check for alignment
  ASSERT(IS_SPI_16BIT_ALIGNED_ADDRESS(mac));

  // read from EEPROM
  rc = EEPROM_ReadMemory(dev, EEPROM_MAC_MEMORY_START, mac, EEPROM_MAC_ADDRESS_BYTES);
  if (rc)
  {
    return rc;
  }

  // check for known modes of failure with the EEPROM read
  for (i = 0; i < EEPROM_MAC_ADDRESS_BYTES; i++)
  {
    if (mac[i] != 0xFF)
    {
      break;
    }
    if (i + 1 == EEPROM_MAC_ADDRESS_BYTES)
    {
      return EEPROM_MAC_ALL_ONE;
    }
  }
  for (i = 0; i < EEPROM_MAC_ADDRESS_BYTES; i++)
  {
    if (mac[i] != 0x00)
    {
      break;
    }
    if (i + 1 == EEPROM_MAC_ADDRESS_BYTES)
    {
      return EEPROM_MAC_ALL_ONE;
    }
  }
  for (i = 0; i < EEPROM_MAC_ADDRESS_BYTES; i++)
  {
    if (mac[i] != EEPROM_MAC_MEMORY_START + i)
    {
      break;
    }
    if (i + 1 == EEPROM_MAC_ADDRESS_BYTES)
    {
      return EEPROM_MAC_ALL_ONE;
    }
  }

  return rc;
}

/*!
* @brief  enum to string conversion for BME280 status codes.
* @param  status - status enumeration value
* @return string for the corresponding enumeration
*/
const char* EEPROM_StatusString(eeprom_status_t status)
{
  switch (status)
  {
    case EEPROM_OK:
      return "OK";
    case EEPROM_SPI_ERROR:
      return "SPI_ERROR";
    case EEPROM_SPI_BUSY:
      return "SPI_BUSY";
    case EEPROM_SPI_TIMEOUT:
      return "SPI_TIMEOUT";
    case EEPROM_MAC_ALL_ONE:
      return "MAC_ALL_ONE";
    case EEPROM_MAC_ALL_ZERO:
      return "MAC_ALL_ZERO";
    case EEPROM_MAC_MIRROR:
      return "MAC_MIRROR";
    default:
      return "UNKNOWN";
  }
}
