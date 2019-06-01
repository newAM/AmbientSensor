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

#ifndef _SHARED_H_
#define _SHARED_H_

#include "eeprom/eeprom.h"
#include "w5500/w5500.h"
#include "w5500/dhcp.h"
#include "w5500/mqtt.h"

#define DEVICE_NAME       "ambient1"                //!< device name used for MQTT client ID and host name
#define DEVICE_NAME_CHARS (sizeof(DEVICE_NAME) - 1) //!< characters in the device name

extern char*         hostName;   //!< device hostname
extern eeprom_dev_t  rom;        //!< EEPROM device structure
extern w5500_dev_t   wiz;        //!< W5500 device structure
extern dhcp_client_t dhcp;       //!< DHCP client
extern mqtt_client_t mqtt;       //!< MQTT client

void InitializeShared(void);

#endif // _SHARED_H_
