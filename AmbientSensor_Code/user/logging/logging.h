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

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdarg.h>
#include "stm32f0xx_hal.h"

//! set to 1 to use asserts
#define USE_ASSERT 1

//! logging level, set this to change logging verbosity
#define LOGGING_LEVEL 10

// logging verbosity levels
#define LOG_LEVEL_NOTSET    0 //!< default logging value
#define LOG_LEVEL_DEBUG    10 //!< debug logging value
#define LOG_LEVEL_INFO     20 //!< info logging value
#define LOG_LEVEL_WARNING  30 //!< warning logging value
#define LOG_LEVEL_ERROR    40 //!< error logging value
#define LOG_LEVEL_CRITICAL 50 //!< critical logging value

void Log_Init(UART_HandleTypeDef* huart);
void Log_Assert(char* condition, char* file, uint32_t line);
void Log_putsn(char* msg, uint16_t len);
void Log_printf(const char* fmt, ...);

#if USE_ASSERT
  #define ASSERT(x)                         \
    do                                      \
    {                                       \
      if (!(x))                             \
      {                                     \
        Log_Assert(#x, __FILE__, __LINE__); \
        while (1);                          \
      }                                     \
    } while (0)
#else // USE_ASSERT == 1
  #define ASSERT(x) ((void) 0U)
#endif // USE_ASSERT == 1

#define LOG_DEBUG(fmt, ...)                                                        \
  do                                                                               \
  {                                                                                \
    if (LOGGING_LEVEL <= LOG_LEVEL_DEBUG)                                          \
    {                                                                              \
      Log_printf("[DEBUG   ] [%s:%u] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }                                                                              \
  } while (0)

#define LOG_INFO(fmt, ...)                                                         \
  do                                                                               \
  {                                                                                \
    if (LOGGING_LEVEL <= LOG_LEVEL_INFO)                                           \
    {                                                                              \
      Log_printf("[INFO    ] [%s:%u] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }                                                                              \
  } while (0)

#define LOG_WARNING(fmt, ...)                                                      \
  do                                                                               \
  {                                                                                \
    if (LOGGING_LEVEL <= LOG_LEVEL_WARNING)                                        \
    {                                                                              \
      Log_printf("[WARNING ] [%s:%u] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }                                                                              \
  } while (0)

#define LOG_ERROR(fmt, ...)                                                        \
  do                                                                               \
  {                                                                                \
    if (LOGGING_LEVEL <= LOG_LEVEL_ERROR)                                          \
    {                                                                              \
      Log_printf("[ERROR   ] [%s:%u] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }                                                                              \
  } while (0)

#define LOG_CRITICAL(fmt, ...)                                                     \
  do                                                                               \
  {                                                                                \
    if (LOGGING_LEVEL <= LOG_LEVEL_CRITICAL)                                       \
    {                                                                              \
      Log_printf("[CRITICAL] [%s:%u] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    }                                                                              \
  } while (0)

#endif // _LOGGING_H_
