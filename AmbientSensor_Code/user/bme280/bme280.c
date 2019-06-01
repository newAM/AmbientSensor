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

#include "bme280.h"

// registers from Table 18: Memory Map
// static const uint8_t REG_HUM_LSB    = 0xFE;
// static const uint8_t REG_HUM_MSB    = 0xFB;
// static const uint8_t REG_TEMP_XLSB  = 0xFC;
// static const uint8_t REG_TEMP_LSB   = 0xFB;
// static const uint8_t REG_TEMP_MSB   = 0xFA;
// static const uint8_t REG_PRESS_XLSB = 0xF9;
// static const uint8_t REG_PRESS_LSB  = 0xF8;
static const uint8_t REG_PRESS_MSB  = 0xF7;
static const uint8_t REG_CONFIG     = 0xF5;
static const uint8_t REG_CTRL_MEAS  = 0xF4;
// static const uint8_t REG_STATUS     = 0xF3;
static const uint8_t REG_CTRL_HUM   = 0xF2;
// static const uint8_t REG_CALIB_41   = 0xF0;
// static const uint8_t REG_CALIB_40   = 0xEF;
// static const uint8_t REG_CALIB_39   = 0xEE;
// static const uint8_t REG_CALIB_38   = 0xED;
// static const uint8_t REG_CALIB_37   = 0xEC;
// static const uint8_t REG_CALIB_36   = 0xEB;
// static const uint8_t REG_CALIB_35   = 0xEA;
// static const uint8_t REG_CALIB_34   = 0xE9;
// static const uint8_t REG_CALIB_33   = 0xE8;
static const uint8_t REG_CALIB_32   = 0xE7;
// static const uint8_t REG_CALIB_31   = 0xE6;
// static const uint8_t REG_CALIB_30   = 0xE5;
// static const uint8_t REG_CALIB_29   = 0xE4;
// static const uint8_t REG_CALIB_28   = 0xE3;
// static const uint8_t REG_CALIB_27   = 0xE2;
static const uint8_t REG_CALIB_26   = 0xE1;
static const uint8_t REG_RESET      = 0xE0;
static const uint8_t REG_ID         = 0xD0;
static const uint8_t REG_CALIB_25   = 0xA1;
// static const uint8_t REG_CALIB_24   = 0xA0;
// static const uint8_t REG_CALIB_23   = 0x9F;
// static const uint8_t REG_CALIB_22   = 0x9E;
// static const uint8_t REG_CALIB_21   = 0x9D;
// static const uint8_t REG_CALIB_20   = 0x9C;
// static const uint8_t REG_CALIB_19   = 0x9B;
// static const uint8_t REG_CALIB_18   = 0x9A;
// static const uint8_t REG_CALIB_17   = 0x99;
// static const uint8_t REG_CALIB_16   = 0x98;
// static const uint8_t REG_CALIB_15   = 0x97;
// static const uint8_t REG_CALIB_14   = 0x96;
// static const uint8_t REG_CALIB_13   = 0x95;
// static const uint8_t REG_CALIB_12   = 0x94;
// static const uint8_t REG_CALIB_11   = 0x93;
// static const uint8_t REG_CALIB_10   = 0x92;
// static const uint8_t REG_CALIB_09   = 0x91;
// static const uint8_t REG_CALIB_08   = 0x90;
// static const uint8_t REG_CALIB_07   = 0x8F;
// static const uint8_t REG_CALIB_06   = 0x8E;
// static const uint8_t REG_CALIB_05   = 0x8D;
// static const uint8_t REG_CALIB_04   = 0x8C;
// static const uint8_t REG_CALIB_03   = 0x8B;
// static const uint8_t REG_CALIB_02   = 0x8A;
// static const uint8_t REG_CALIB_01   = 0x89;
static const uint8_t REG_CALIB_00   = 0x88;

// BME chip constants
static const uint8_t CID            = 0x60; //!< BME280 Chip ID
static const uint8_t T_STARTUP      =    3; //!< BME280 startup time in ms
static const uint8_t RESET_VAL      = 0xB6; //!< software reset key 

// other constants
static const uint32_t BME280_TIMEOUT = 1000; //!< I2C timeout in milliseconds

// private register RW functions
static inline bme280_status_t ReadRegister(bme280_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t num);
static inline bme280_status_t WriteRegister(bme280_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t num);
static bme280_status_t ReadCalibration(bme280_dev_t* dev);

/*!
* @brief  Initializes the BME280 on the given I2C port
* @param  dev - BME280 device structure
* @return BME280 status
*/
bme280_status_t BME280_Init(bme280_dev_t* dev, bme280_config_t* config, bme280_ctrl_meas_t* meas, bme280_ctrl_hum_t* hum)
{
  bme280_status_t rc;
  uint8_t chipID;

  // reset BME280
  rc = BME280_Reset(dev);
  if (rc != BME280_OK)
  {
    return rc;
  }

  // wait for startup
  vTaskDelay((configTICK_RATE_HZ / 1000) * T_STARTUP);

  // read chip ID register
  rc = ReadRegister(dev, REG_ID, &chipID, 1);
  if (rc != BME280_OK)
  {
    return rc;
  }

  // chip ID must match
  if (chipID != CID)
  {
    return BME280_BAD_CHIP_ID;
  }

  // read calibration into device structure
  rc = ReadCalibration(dev);
  if (rc != BME280_OK)
  {
    return rc;
  }

  // write default configuration
  rc = BME280_SetConfig(dev, config);
  if (rc != BME280_OK)
  {
    return rc;
  }

  // write default measurement settings
  return BME280_SetSampleSettings(dev, meas, hum);
}

/*!
* @brief  Read from num register(s) on the BME280
* @param  dev - BME280 device structure
* @param  reg - register address to read from
* @param  buf - buffer array for the read data
* @param  num - number of bytes to read
* @return BME280 status
*/
inline bme280_status_t ReadRegister(bme280_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t num)
{
  // Read example (BME280 Datasheet Figure 10: I2C multiple byte read)
  // +-------+---------------+----+------+------------------+------+
  // | Start | Slave Address | RW | ACKS | Register Address | ACKS |
  // +-------+---------------+----+------+------------------+------+
  // | S     | 111011x       |  0 |      | xxxxxxxx         |      |
  // +-------+---------------+----+------+------------------+------+
  //
  // +-------+---------------+----+------+---------------+------+---------------+--------+------+
  // | Start | Slave Address | RW | ACKS | Register Data | ACKM | Register Data | NOACKM | Stop |
  // +-------+---------------+----+------+---------------+------+---------------+--------+------+
  // | S     | 111011x       |  1 |      |      76543210 |      |      76543210 |        | P    |
  // +-------+---------------+----+------+---------------+------+---------------+--------+------+

  bme280_status_t rc;

  // take the mutex for the I2C bus
  xSemaphoreTake(dev->busMutex, portMAX_DELAY);

  // do I2C transfer in critical section
  taskENTER_CRITICAL();
  rc = HAL_I2C_Mem_Read(
    &dev->hi2cx,
    dev->addr,
    reg,
    I2C_MEMADD_SIZE_8BIT,
    buf,
    num,
    BME280_TIMEOUT
  );
  taskEXIT_CRITICAL();

  // release mutex
  xSemaphoreGive(dev->busMutex);

  return rc;
}

/*!
* @brief  Writes to num register(s) on the BME280
* @param  dev - BME280 device structure
* @param  reg - register address to read from
* @param  buf - array of values to write to the registers
* @param  num - number of bytes to write
* @return BME280 status
*/
inline bme280_status_t WriteRegister(bme280_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t num)
{
  // Write example (BME280 Datasheet Figure 9: I2C multiple byte write)
  // +-------+---------------+----+------+------------------+------+---------------+------+
  // | Start | Slave Address | RW | ACKS | Register Address | ACKS | Register Data | ACKS |
  // +-------+---------------+----+------+------------------+------+---------------+------+
  // | S     | 111011x       |  0 |      | xxxxxxxx         |      |      76543210 |      |
  // +-------+---------------+----+------+------------------+------+---------------+------+
  //
  //     +------------------+------+---------------+------+------+
  // ... | Register Address | ACKS | Register Data | ACKS | Stop |
  //     +------------------+------+---------------+------+------+
  // ... | xxxxxxxx         |      |       7654321 |      | P    |
  //     +------------------+------+---------------+------+------+

  bme280_status_t rc;

  // take the mutex for the I2C bus
  xSemaphoreTake(dev->busMutex, portMAX_DELAY);

  // do I2C transfer in critical section
  taskENTER_CRITICAL();
  rc = (bme280_status_t) HAL_I2C_Mem_Write(
    &dev->hi2cx,
    dev->addr,
    reg,
    I2C_MEMADD_SIZE_8BIT,
    buf,
    num,
    BME280_TIMEOUT
  );
  taskEXIT_CRITICAL();

  // release mutex
  xSemaphoreGive(dev->busMutex);

  return rc;
}

/*!
* @brief  reads the calibration into the device structure
* @param  dev - BME280 device structure
* @return BME280 status
*/
bme280_status_t ReadCalibration(bme280_dev_t* dev)
{
  // calibration registers are located in two separate groups
  // calib00..25 at location 0x88..0xA1
  // calib26..41 at location 0xE1..0xF0

  bme280_status_t rc;

  rc = ReadRegister(dev, REG_CALIB_00, dev->cal.buf, REG_CALIB_25 - REG_CALIB_00 + 1);
  if (rc != BME280_OK)
  {
    return rc;
  }
  rc = ReadRegister(dev, REG_CALIB_26, dev->cal.buf + 26, REG_CALIB_32 - REG_CALIB_26 + 1);
  if (rc != BME280_OK)
  {
    return rc;
  }

  LOG_DEBUG("buf[00] = 0x%02X, buf[01] = 0x%02X, T1 = 0x%04X", dev->cal.buf[ 0], dev->cal.buf[ 1], dev->cal.dig.T1);
  LOG_DEBUG("buf[02] = 0x%02X, buf[03] = 0x%02X, T2 = 0x%04X", dev->cal.buf[ 2], dev->cal.buf[ 3], dev->cal.dig.T2);
  LOG_DEBUG("buf[04] = 0x%02X, buf[05] = 0x%02X, T3 = 0x%04X", dev->cal.buf[ 4], dev->cal.buf[ 5], dev->cal.dig.T3);

  LOG_DEBUG("buf[06] = 0x%02X, buf[07] = 0x%02X, P1 = 0x%04X", dev->cal.buf[ 6], dev->cal.buf[ 7], dev->cal.dig.P1);
  LOG_DEBUG("buf[08] = 0x%02X, buf[09] = 0x%02X, P2 = 0x%04X", dev->cal.buf[ 8], dev->cal.buf[ 9], dev->cal.dig.P2);
  LOG_DEBUG("buf[10] = 0x%02X, buf[11] = 0x%02X, P3 = 0x%04X", dev->cal.buf[10], dev->cal.buf[11], dev->cal.dig.P3);
  LOG_DEBUG("buf[12] = 0x%02X, buf[13] = 0x%02X, P4 = 0x%04X", dev->cal.buf[12], dev->cal.buf[13], dev->cal.dig.P4);
  LOG_DEBUG("buf[14] = 0x%02X, buf[15] = 0x%02X, P5 = 0x%04X", dev->cal.buf[14], dev->cal.buf[15], dev->cal.dig.P5);
  LOG_DEBUG("buf[16] = 0x%02X, buf[17] = 0x%02X, P6 = 0x%04X", dev->cal.buf[16], dev->cal.buf[17], dev->cal.dig.P6);
  LOG_DEBUG("buf[18] = 0x%02X, buf[19] = 0x%02X, P7 = 0x%04X", dev->cal.buf[18], dev->cal.buf[19], dev->cal.dig.P7);
  LOG_DEBUG("buf[20] = 0x%02X, buf[21] = 0x%02X, P8 = 0x%04X", dev->cal.buf[20], dev->cal.buf[21], dev->cal.dig.P8);
  LOG_DEBUG("buf[22] = 0x%02X, buf[23] = 0x%02X, P9 = 0x%04X", dev->cal.buf[22], dev->cal.buf[23], dev->cal.dig.P9);

  LOG_DEBUG("buf[24] = 0x%02X                  H1 = 0x%04X",   dev->cal.buf[24],                      dev->cal.dig.H1);
  LOG_DEBUG("buf[25] = 0x%02X, buf[26] = 0x%02X, H2 = 0x%04X", dev->cal.buf[25], dev->cal.buf[26], dev->cal.dig.H2);
  LOG_DEBUG("buf[27] = 0x%02X                  H3 = 0x%04X",   dev->cal.buf[27],                      dev->cal.dig.H3);
  LOG_DEBUG("buf[28] = 0x%02X, buf[29] = 0x%02X, H4 = 0x%04X", dev->cal.buf[28], dev->cal.buf[29], dev->cal.dig.H4);
  LOG_DEBUG("buf[30] = 0x%02X, buf[31] = 0x%02X, H5 = 0x%04X", dev->cal.buf[30], dev->cal.buf[31], dev->cal.dig.H5);
  LOG_DEBUG("buf[32] = 0x%02X                  H6 = 0x%04X",   dev->cal.buf[32],                      dev->cal.dig.H6);

  // these fields share some bits between a register read
  // see BME280 memory map for CALIB_xx for more details
  dev->cal.dig.H4 = (dev->cal.buf[29] << 4) | (dev->cal.buf[30] & 0xF);
  dev->cal.dig.H5 = ((dev->cal.buf[30] & 0xF0) >> 4) | (dev->cal.buf[31] << 4);
  dev->cal.dig.H6 = dev->cal.buf[32];

  return rc;
}

/*!
* @brief  Resets the BME280.
* @param  dev - BME280 device structure
* @return BME280 status
*/
bme280_status_t BME280_Reset(bme280_dev_t* dev)
{
  uint8_t buf = RESET_VAL;
  return WriteRegister(dev, REG_RESET, &buf, 1);
}

/*!
* @brief  Sets the standby and filter configuration from the device config structure
* @param  dev - BME280 device structure
* @param  config - BME280 configuration structure
* @return BME280 status
*/
bme280_status_t BME280_SetConfig(bme280_dev_t* dev, bme280_config_t* config)
{
  return WriteRegister(dev, REG_CONFIG, &config->all, 1);
}

/*!
* @brief  Sets the sampling settings and mode from the class ctrl_meas and
*         ctrl_hum structure.
* @param  dev - BME280 device structure
* @param  meas - measurement settings structure
* @param  hum - humidity temperature settings
* @return BME280 status
*/
bme280_status_t BME280_SetSampleSettings(bme280_dev_t* dev, bme280_ctrl_meas_t* meas, bme280_ctrl_hum_t* hum)
{
  // Note: A write to CTRL_HUM must be followed by a write to CTRL_MEAS for
  // changes to apply

  bme280_status_t rc;

  rc = WriteRegister(dev, REG_CTRL_HUM, &hum->all, 1);
  if (rc != BME280_OK)
  {
    return rc;
  }

  return WriteRegister(dev, REG_CTRL_MEAS, &meas->all, 1);
}

/*!
* @brief  Reads the temperature, humidity, and pressure into class variables.
* @param  dev - BME280 device structure
* @param  temperature - temperature output
* @param  pressure - pressure output
* @param  humidity - humidity output
* @return BME280 status
*/
bme280_status_t BME280_ReadEnvironment(bme280_dev_t* dev, float* temperature, float* pressure, float* humidity)
{
  int32_t t, h, p;          // temperature, humidity, pressure values
  int32_t t_fine;           // fine resolution temperature for pressure and humidity
  int64_t var1, var2, var3; // magic variables from BME280 data sheet
  bme280_meas_t meas;       // structure to make reading registers a bit more clear
  bme280_status_t rc;       // function return codes

  rc = ReadRegister(dev, REG_PRESS_MSB, meas.buf, BME280_NUM_MEAS_REG);
  if (rc != BME280_OK)
  {
    return rc;
  }

  // build signed 32-bit values out of register reads
  t = ((uint32_t)meas.reg.temp_msb   << 12) | // msb [7:0] = t[19:12]
      ((uint32_t)meas.reg.temp_lsb   <<  4) | // lsb [7:0] = t[11:4]
      ((uint32_t)meas.reg.temp_xlsb  >>  4) ; // xlsb[7:4] = t[3:0]
  p = ((uint32_t)meas.reg.press_msb  << 12) | // msb [7:0] = p[19:12]
      ((uint32_t)meas.reg.press_lsb  <<  4) | // lsb [7:0] = p[11:4]
      ((uint32_t)meas.reg.press_xlsb >>  4) ; // xlsb[7:4] = p[3:0]
  h = ((uint32_t)meas.reg.hum_msb    <<  8) | // msb [7:0] = h[15:8]
      ((uint32_t)meas.reg.hum_lsb         ) ; // lsb [7:0] = h[7:0]

  // output is held at reset value
  if (t == 0x80000000 || p == 0x80000000 || h == 0x8000)
  {
    return BME280_BAD_OUTPUT;
  }

  // convert temperature to a float with calibration values
  // magic formula provided by BME280 data sheet
  var1 = ((((t>>3) - ((int32_t)dev->cal.dig.T1<<1))) *
    ((int32_t)dev->cal.dig.T2)) >> 11;
  var2 = (((((t>>4) - ((int32_t)dev->cal.dig.T1)) *
    ((t>>4) - ((int32_t)dev->cal.dig.T1))) >> 12) *
    ((int32_t)dev->cal.dig.T3)) >> 14;
  t_fine = var1 + var2;
  *temperature = (t_fine * 5 + 128) >> 8;
  *temperature /= 100.0;

  // convert pressure to a float with calibration values
  // magic formula provided by BME280 data sheet
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dev->cal.dig.P6;
  var2 = var2 + ((var1 * (int64_t)dev->cal.dig.P5)<<17);
  var2 = var2 + (((int64_t)dev->cal.dig.P4)<<35);
  var1 = ((var1 * var1 * (int64_t)dev->cal.dig.P3)>>8) + ((var1 * (int64_t)dev->cal.dig.P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dev->cal.dig.P1)>>33;
  if (var1 == 0)
  {
    *pressure = 0; // avoid exception caused by division by zero
  }
  else
  {
    var3 = 1048576 - p;
    var3 = (((var3<<31) - var2)*3125)/var1;
    var1 = (((int64_t)dev->cal.dig.P9) * (var3>>13) * (var3>>13)) >> 25;
    var2 = (((int64_t)dev->cal.dig.P8) * var3) >> 19;

    var3 = ((var3 + var1 + var2) >> 8) + (((int64_t)dev->cal.dig.P7)<<4);
    *pressure = (float)var3 / 256.0;
  }

  // convert humidity to a float with calibration values
  // magic formula provided by BME280 data sheet
  var1 = (t_fine - ((int32_t)76800));
  var1 = (((((h << 14) - (((int32_t)dev->cal.dig.H4) << 20) -
    (((int32_t)dev->cal.dig.H5) * var1)) + ((int32_t)16384)) >> 15) * (((((((var1 *
    ((int32_t)dev->cal.dig.H6)) >> 10) * (((var1 * ((int32_t)dev->cal.dig.H3)) >> 11) + 
    ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
    ((int32_t)dev->cal.dig.H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)dev->cal.dig.H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);
  *humidity = (float)(var1>>12) / 1024.0;

  return rc;
}

/*!
* @brief  Returns the millisecond standby time of the BME280 device.
* @param  standby - standby time enumeration
* @return standby time in milliseconds
*/
uint32_t BME280_GetStandbyTime(bme280_standby_t standby)
{
  switch (standby)
  {
    case BME280_STANDBY_0_5_ms:
      return 1;
    case BME280_STANDBY_62_5_ms:
      return 63;
    case BME280_STANDBY_125_ms:
      return 125;
    case BME280_STANDBY_250_ms:
      return 250;
    case BME280_STANDBY_500_ms:
      return 500;
    case BME280_STANDBY_1000_ms:
      return 1000;
    case BME280_STANDBY_10_ms:
      return 10;
    case BME280_STANDBY_20_ms:
      return 20;
    default:
      ASSERT(0);
      return 1000;
  }
}

/*!
* @brief  enum to string conversion for BME280 status codes.
* @param  status - status enumeration value
* @return string for the corresponding enumeration
*/
const char* BME280_StatusString(bme280_status_t status)
{
  switch (status)
  {
    case BME280_OK:
      return "OK";
    case BME280_I2C_ERROR:
      return "I2C_ERROR";
    case BME280_I2C_BUSY:
      return "I2C_BUSY";
    case BME280_I2C_TIMEOUT:
      return "I2C_TIMEOUT";
    case BME280_BAD_CHIP_ID:
      return "BAD_CHIP_ID";
    case BME280_BAD_OUTPUT:
      return "BAD_OUTPUT";
    default:
      return "UNKNOWN";
  }
}
