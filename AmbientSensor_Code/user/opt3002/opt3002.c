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

#include "opt3002.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include <math.h>

// register map
static const uint8_t REG_RESULT  = 0x00;
static const uint8_t REG_CFG     = 0x01;
// static const uint8_t REG_LOW_LIM = 0x02;
// static const uint8_t REG_HI_LIM  = 0x03;
static const uint8_t REG_MFG_ID  = 0x7E;

// private constants
static const uint16_t MFG_ID   = 0x5449; //!< manufacturer ID (TI in ASCII)
static const uint32_t TIMEOUT  =   1000; //!< I2C timeout in milliseconds
static const uint8_t  REG_SIZE =      2; //!< register size in bytes

/*!
* conversion factor to lux for the OPT3002
* output for the OPT3002 is given in nW / cm^2
* result /= 1000000000 [  W / cm^2 ]
* result *=         93 [ lm / cm^2 ] (sunlight luminous efficacy)
* result *=      10000 [ lm /  m^2 ]
*/
static const float CONV = 0.001116;

// private register RW functions
static opt3002_status_t ReadRegister(opt3002_dev_t* dev, const uint8_t reg, uint16_t* buf);
static opt3002_status_t WriteRegister(opt3002_dev_t* dev, const uint8_t reg, uint16_t* buf);

/*!
* @brief  Initializes the OPT3002 on the given I2C port
* @param  dev - OPT3002 device structure
* @param  optCfg - OPT3002 device configuration
* @return OPT3002 device status
*/
opt3002_status_t OPT3002_Init(opt3002_dev_t* dev, opt3002_cfg_t* optCfg)
{
  opt3002_status_t rc;
  opt3002_mfg_id_t mfgId;

  // read manufacturer ID register
  rc = ReadRegister(dev, REG_MFG_ID, &mfgId.all);
  if (rc)
  {
    return rc;
  }

  // manufacturer ID must match
  if (mfgId.bits.id != MFG_ID)
  {
    return OPT3002_BAD_ID;
  }

  // write configuration
  return WriteRegister(dev, REG_CFG, &optCfg->all);
}

/*!
* @brief  Samples the OPT3002
* @param[in]  dev - OPT3002 device structure
* @param[out] lux - luminosity reading
* @return OPT3002 device status
*/
opt3002_status_t OPT3002_Sample(opt3002_dev_t* dev, float* lux)
{
  opt3002_status_t rc;
  opt3002_num_t raw;

  // read result register
  rc = ReadRegister(dev, REG_RESULT, &raw.all);
  if (rc)
  {
    return rc;
  }

  *lux = CONV * (float)pow(2, (float)raw.bits.e) * (float) raw.bits.r;

  return rc;
}

/*!
* @brief  Read from a register on the OPT3002
* @param  dev - BME280 device structure
* @param  reg - register address to read from
* @param  buf - buffer for the read data
* @return OPT3002 device status
*/
opt3002_status_t ReadRegister(opt3002_dev_t* dev, const uint8_t reg, uint16_t* buf)
{
  opt3002_status_t rc;

  // take the mutex for the I2C bus
  xSemaphoreTake(dev->busMutex, portMAX_DELAY);

  // do I2C transfer in critical section
  taskENTER_CRITICAL();
  rc = (opt3002_status_t) HAL_I2C_Mem_Read(
    &dev->hi2cx,
    dev->addr,
    reg,
    I2C_MEMADD_SIZE_8BIT,
    (uint8_t*)buf,
    REG_SIZE,
    TIMEOUT
  );
  taskEXIT_CRITICAL();

  // release mutex
  xSemaphoreGive(dev->busMutex);

  // endian swap
  *buf = (*buf << 8) | (*buf >> 8);

  return rc;
}

/*!
* @brief  Writes to a register on the OPT3002
* @param  dev - BME280 device structure
* @param  reg - register address to read from
* @param  buf - buffer containing data to write
* @return OPT3002 device status
*/
opt3002_status_t WriteRegister(opt3002_dev_t* dev, const uint8_t reg, uint16_t* buf)
{
  opt3002_status_t rc;

  // endian swap
  *buf = (*buf << 8) | (*buf >> 8);

  // take the mutex for the I2C bus
  xSemaphoreTake(dev->busMutex, portMAX_DELAY);

  // do I2C transfer in critical section
  taskENTER_CRITICAL();
  rc = (opt3002_status_t) HAL_I2C_Mem_Write(
    &dev->hi2cx,
    dev->addr,
    reg,
    I2C_MEMADD_SIZE_8BIT,
    (uint8_t*)buf,
    REG_SIZE,
    TIMEOUT
  );
  taskEXIT_CRITICAL();

  // release mutex
  xSemaphoreGive(dev->busMutex);

  return rc;
}

/*!
* @brief  enum to string conversion for BME280 status codes.
* @param  status - status enumeration value
* @return string for the corresponding enumeration
*/
const char* OPT3002_StatusString(opt3002_status_t status)
{
  switch (status)
  {
    case OPT3002_OK:
      return "OK";
    case OPT3002_I2C_ERROR:
      return "I2C_ERROR";
    case OPT3002_I2C_BUSY:
      return "I2C_BUSY";
    case OPT3002_I2C_TIMEOUT:
      return "I2C_TIMEOUT";
    case OPT3002_BAD_ID:
      return "BAD_ID";
    default:
      return "UNKNOWN";
  }
}
