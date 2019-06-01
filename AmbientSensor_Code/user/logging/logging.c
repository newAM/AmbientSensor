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

#include "logging/logging.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define LOG_BUF_LEN 128
static char logBuf[LOG_BUF_LEN] __attribute__((aligned(16)));
static UART_HandleTypeDef logUart;
static SemaphoreHandle_t  logMutex;

/*!
* @brief  Sets the UART interface for STDOUT.
* @param  huart - a UART_HandleTypeDef object for ST
*/
void Log_Init(UART_HandleTypeDef* huart)
{
  logUart = *huart;
  logMutex = xSemaphoreCreateMutex();
  Log_printf("\n");
}

/*!
* @brief  Logs an assertion.
* @param  condition - condition string
* @param  file - file name
* @param  line - line number within the file
*/
void Log_Assert(char* condition, char* file, uint32_t line)
{
  Log_printf("[ASSERT  ] [%s:%lu] %s\n", file, line, condition);
}

/*!
* @brief  Writes a string to the log.  NOT THREAD SAFE!
* @param  msg - message buffer
* @param  len - length of the message
*/
void Log_putsn(char* msg, uint16_t len)
{
  // transmit data
  HAL_StatusTypeDef status = HAL_UART_Transmit(&logUart, (uint8_t*)msg, len, 1000);
  if (status != HAL_OK)
  {
    while (1);
  }
  return;
}

/*!
* @brief  Writes format string to the log.
* @param  fmt - message format
*/
void Log_printf(const char* fmt, ...)
{
  va_list argp;
  va_start(argp, fmt);
  xSemaphoreTake(logMutex, portMAX_DELAY);
  int len = vsnprintf(logBuf, LOG_BUF_LEN, fmt, argp);
  va_end(argp);

  if (len < 0)
  {
    logBuf[0] = 'E';
    logBuf[1] = 'R';
    logBuf[2] = 'R';
    logBuf[3] = '\n';
    logBuf[4] = '\0';
    len = 4;
  }
  else if (len > LOG_BUF_LEN)
  {
    len = LOG_BUF_LEN;
  }
  else if (len == 0) // prevents a HAL_ERROR
  {
    xSemaphoreGive(logMutex);
    return;
  }

  Log_putsn(logBuf, len);
  xSemaphoreGive(logMutex);
}
