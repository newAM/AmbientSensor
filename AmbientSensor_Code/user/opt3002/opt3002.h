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

#ifndef _OPT3002_H_
#define _OPT3002_H_

#include "stm32f0xx_hal.h"
#include "logging/logging.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define OPT3002_DEFAULT_ADDR    0x44 //!< address of the OPT3002 with address pin tied to GND
#define OPT3002_DEFAULT_CFG   0xC810 //!< default configuration
#define OPT3002_MIN_PERIOD       100 //!< minimum sample period for the OPT3002 in ms

//! structure for OPT3002 high limit, low limit, and result register
typedef union opt3002_num_t
{
  struct
  {
    uint16_t r : 12; // result bits
    uint16_t e :  4; // exponent bits
  } bits;
  uint16_t all;
} opt3002_num_t;

//! structure for OPT3002 configuration register
typedef union opt3002_cfg_t
{
  struct
  {
    uint16_t fc  : 2; // fault count
    uint16_t me  : 1; // mask exponent
    uint16_t pol : 1; // polarity
    uint16_t l   : 1; // latch
    uint16_t fl  : 1; // flag low
    uint16_t fh  : 1; // flag high
    uint16_t crf : 1; // conversion ready
    uint16_t ovf : 1; // overflow flag
    uint16_t m   : 2; // mode of conversion
    uint16_t ct  : 1; // conversion time
    uint16_t rn  : 3; // range number
  } bits;
  uint16_t all;
} opt3002_cfg_t;

//! structure for OPT3002 manufacturer ID register
typedef union opt3002_mfg_id_t
{
  struct
  {
    uint16_t id : 16; // manufacturer ID
  } bits;
  uint16_t all;
} opt3002_mfg_id_t;

//! OPT3002 device structure
typedef struct opt3002_dev_t
{
  uint8_t             addr;         //!< shifted 7-bit I2C address
  I2C_HandleTypeDef   hi2cx;        //!< I2C port
  SemaphoreHandle_t   busMutex;     //!< mutex for the I2C bus
} opt3002_dev_t;

//! OPT3002 return codes
typedef enum 
{
  OPT3002_OK           = 0x00U,
  OPT3002_I2C_ERROR    = 0x01U,
  OPT3002_I2C_BUSY     = 0x02U,
  OPT3002_I2C_TIMEOUT  = 0x03U,
  OPT3002_BAD_ID,
} opt3002_status_t;

//! modes
enum opt3002_modes_e
{
  OPT3002_MODE_SHUTDOWN = 0,
  OPT3002_MODE_SINGLE,
  OPT3002_MODE_CONTINUOUS
};

//! conversion times
enum opt3002_conv_e
{
  OPT3002_100_MS = 0,
  OPT3002_800_MS
};

// function prototypes
opt3002_status_t OPT3002_Init(opt3002_dev_t* optDev, opt3002_cfg_t* cfg);
opt3002_status_t OPT3002_Sample(opt3002_dev_t* optDev, float* lux);
const char* OPT3002_StatusString(opt3002_status_t status);

#endif // _OPT3002_H_
