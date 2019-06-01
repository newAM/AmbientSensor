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

#include "shared.h"
#include "main.h"
#include "spi.h"

char*         hostName = DEVICE_NAME;
eeprom_dev_t  rom;
w5500_dev_t   wiz;
dhcp_client_t dhcp;
mqtt_client_t mqtt;

/*!
* @brief Initialized shared device structures.
*/
void InitializeShared(void)
{
  uint8_t sn;

  // W5500 Ethernet
  wiz.hspix   = hspi1;              // SPI port
  wiz.csPort  = WIZ_CS_GPIO_Port;   // chip select port
  wiz.csPin   = WIZ_CS_Pin;         // chip select pin
  wiz.rstPort = WIZ_RST_GPIO_Port;  // reset port
  wiz.rstPin  = WIZ_RST_Pin;        // reset pin
  wiz.intPort = WIZ_INT_GPIO_Port;  // interrupt port
  wiz.intPin  = WIZ_INT_Pin;        // interrupt pin

  for (sn = 0; sn < W5500_NUM_SOCKETS; sn++)
  {
    wiz.snEvent[sn] = NULL;
  }

  // DHCP client
  dhcp.dev         = &wiz;              // DHCP client device
  dhcp.sn          = 3;                 // DHCP socket number
  dhcp.hostName    = hostName;          // shared host name
  dhcp.hostNameLen = DEVICE_NAME_CHARS; // host name length
  dhcp.state       = DHCP_INIT;         // start DHCP FSM at DHCPINIT

  // MQTT client
  mqtt.dev             = &wiz;          // MQTT client device
  mqtt.sn              = 7;             // MQTT socket number
  mqtt.ip[0]           = 10;            // server IP
  mqtt.ip[1]           = 0;
  mqtt.ip[2]           = 0;
  mqtt.ip[3]           = 4;
  mqtt.destinationPort = 1883;          // destination port
  mqtt.sourcePort      = 33650;         // source port

  // EEPROM
  rom.hspix  = hspi2;               // SPI port
  rom.csPort = EEPROM_CS_GPIO_Port; // chip select port
  rom.csPin  = EEPROM_CS_Pin;       // chip select pin
}
