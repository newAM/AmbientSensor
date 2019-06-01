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

#ifndef _BME280_H_
#define _BME280_H_

#include "stm32f0xx_hal.h"
#include "shared.h"
#include "logging/logging.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define BME280_DEFAULT_ADDR    0x76 //!< default BME280 address (ADR to GND)
#define BME280_NUM_CALIB_REG     33 //!< number of calibration registers
#define BME280_NUM_MEAS_REG       8 //!< number of measurement registers

//! structure for compensation parameters
typedef union bme280_dig_t
{
  struct
  {
    uint16_t T1; // 0x88..0x89 buf[00:01]
    int16_t  T2; // 0x8A..0x8B buf[02:03]
    int16_t  T3; // 0x8C..0x8D buf[04:05]
    uint16_t P1; // 0x8E..0x8F buf[06:07]
    int16_t  P2; // 0x90..0x91 buf[08:09]
    int16_t  P3; // 0x92..0x93 buf[10:11]
    int16_t  P4; // 0x94..0x95 buf[12:13]
    int16_t  P5; // 0x96..0x97 buf[14:15]
    int16_t  P6; // 0x98..0x99 buf[16:17]
    int16_t  P7; // 0x9A..0x9B buf[18:19]
    int16_t  P8; // 0x9C..0x9D buf[20:21]
    int16_t  P9; // 0x9E..0x9F buf[22:23]
    uint8_t  H1; // 0xA1       buf[25]
    int16_t  H2; // 0xE1..0xE2 buf[26:27]
    uint8_t  H3; // 0xE3       buf[28]
    int16_t  H4; // 0xE4..0xE5[3:0] = H4 [11:4]..[3:0]
    int16_t  H5; // 0xE5[7:4]..0xE6 = H5 [3:0]..[11:4]
    int8_t   H6; // 0xE7       buf[32]
  } dig __attribute__((packed));
  uint8_t buf[BME280_NUM_CALIB_REG];
} bme280_dig_t;

//! structure for BME280 ctrl_meas register
typedef union bme280_ctrl_meas_t
{
  struct
  {
    uint8_t mde : 2; // mode
    uint8_t osP : 3; // pressure oversample
    uint8_t osT : 3; // temperature oversample
  } bits;
  uint8_t all;
} bme280_ctrl_meas_t;

//! structure for BME280 ctrl_hum register
typedef union bme280_ctrl_hum_t
{
  struct
  {
    uint8_t osH  : 3; // humidity oversample
    uint8_t rsvd : 5; // reserved
  } bits;
  uint8_t all;
} bme280_ctrl_hum_t;

//! structure for BME280 configuration register
typedef union bme280_config_t
{
  struct
  {
    uint8_t spi3w_en : 1; // enables 3-wire SPI
    uint8_t rsvd     : 1; // reserved
    uint8_t filter   : 3; // filter coefficient
    uint8_t t_sb     : 3; // standby time
  } bits;
  uint8_t all;
} bme280_config_t;

//! structure for BME280 environment reads
typedef union bme280_meas_t
{
  struct
  {
    uint8_t press_msb;
    uint8_t press_lsb;
    uint8_t press_xlsb;
    uint8_t temp_msb;
    uint8_t temp_lsb;
    uint8_t temp_xlsb;
    uint8_t hum_msb;
    uint8_t hum_lsb;
  } reg;
  uint8_t buf[BME280_NUM_MEAS_REG];
} bme280_meas_t;

//! BME280 device structure
typedef struct bme280_dev_t
{
  uint8_t             addr;         //!< shifted 7-bit I2C address
  I2C_HandleTypeDef   hi2cx;        //!< I2C port
  SemaphoreHandle_t   busMutex;     //!< mutex for the I2C bus
  bme280_dig_t        cal;          //!< calibration from the chip
} bme280_dev_t;

//! standby times
typedef enum
{
  BME280_STANDBY_0_5_ms = 0,
  BME280_STANDBY_62_5_ms,
  BME280_STANDBY_125_ms,
  BME280_STANDBY_250_ms,
  BME280_STANDBY_500_ms,
  BME280_STANDBY_1000_ms,
  BME280_STANDBY_10_ms,
  BME280_STANDBY_20_ms
} bme280_standby_t;

//! filter settings
typedef enum
{
  BME280_FILTER_OFF = 0,
  BME280_FILTER_02,
  BME280_FILTER_04,
  BME280_FILTER_08,
  BME280_FILTER_16
} bme280_filter_t;

//! oversample settings
typedef enum
{
  BME280_OVERSAMPLE_01 = 1,
  BME280_OVERSAMPLE_02,
  BME280_OVERSAMPLE_04,
  BME280_OVERSAMPLE_08,
  BME280_OVERSAMPLE_16
} bme280_oversample_t;

//! modes
typedef enum
{
  BME280_MODE_SLEEP = 0,
  BME280_MODE_FORCED,
  BME280_MODE_NORMAL = 3
} bme280_modes_t;

//! BME280 return codes
typedef enum 
{
  BME280_OK           = 0x00U,
  BME280_I2C_ERROR    = 0x01U,
  BME280_I2C_BUSY     = 0x02U,
  BME280_I2C_TIMEOUT  = 0x03U,
  BME280_BAD_CHIP_ID,
  BME280_BAD_OUTPUT,
} bme280_status_t;

// public functions
bme280_status_t BME280_Init(bme280_dev_t* dev, bme280_config_t* config, bme280_ctrl_meas_t* meas, bme280_ctrl_hum_t* hum);
bme280_status_t BME280_Reset(bme280_dev_t* dev);
bme280_status_t BME280_SetSampleSettings(bme280_dev_t* dev, bme280_ctrl_meas_t* meas, bme280_ctrl_hum_t* hum);
bme280_status_t BME280_SetConfig(bme280_dev_t* dev, bme280_config_t* config);
bme280_status_t BME280_ReadEnvironment(bme280_dev_t* dev, float* temperature, float* pressure, float* humidity);
uint32_t BME280_GetStandbyTime(bme280_standby_t standby);
const char* BME280_StatusString(bme280_status_t status);

#endif // _BME280_H_
