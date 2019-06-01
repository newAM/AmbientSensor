/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include <stdbool.h>
#include <string.h>
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

#include "shared.h"
#include "constants.h"
#include "logging/logging.h"
#include "eeprom/eeprom.h"
#include "opt3002/opt3002.h"
#include "bme280/bme280.h"
#include "w5500/w5500.h"
#include "w5500/dhcp.h"
#include "w5500/mqtt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//! sample sources
typedef enum 
{
  TYPE_TEMPEARTURE,
  TYPE_HUMIDITY,
  TYPE_PRESSURE,
  TYPE_LUX,
  TYPE_LAST,
} sample_type_t;

//! sample source strings
static const char* SAMPLE_TYPE[TYPE_LAST] =
{
  "/home/bedroom/"DEVICE_NAME"/temperature",
  "/home/bedroom/"DEVICE_NAME"/humidity",
  "/home/bedroom/"DEVICE_NAME"/pressure",
  "/home/bedroom/"DEVICE_NAME"/luminosity",
};

//! sample object
typedef struct sample_t
{
  float         value;  //!< sample value
  sample_type_t type;   //!< sample type
} sample_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static const UBaseType_t SAMPLE_QUEUE_SIZE = 16;
QueueHandle_t sampleQueue;
SemaphoreHandle_t i2c1Mutex;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId luxTaskHandle;
osThreadId dhcpTaskHandle;
osThreadId mqttTaskHandle;
osThreadId bmeTaskHandle;
osThreadId wizTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // W5500 interrupt
  if (GPIO_Pin & WIZ_INT_Pin)
  {
    ASSERT(wizTaskHandle != NULL);
    vTaskNotifyGiveFromISR(wizTaskHandle, &xHigherPriorityTaskWoken);
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartLuxTask(void const * argument);
extern void DHCP_ClientTask(void const * argument);
void StartMqttTask(void const * argument);
void StartBmeTask(void const * argument);
void StartWizTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
  LOG_CRITICAL("OVERFLOW: %s", pcTaskName);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  i2c1Mutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  sampleQueue = xQueueCreate(SAMPLE_QUEUE_SIZE, sizeof(sample_t));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityRealtime, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of luxTask */
  osThreadDef(luxTask, StartLuxTask, osPriorityLow, 0, 256);
  luxTaskHandle = osThreadCreate(osThread(luxTask), NULL);

  /* definition and creation of dhcpTask */
  osThreadDef(dhcpTask, DHCP_ClientTask, osPriorityAboveNormal, 0, 256);
  dhcpTaskHandle = osThreadCreate(osThread(dhcpTask), (void*) &dhcp);

  /* definition and creation of mqttTask */
  osThreadDef(mqttTask, StartMqttTask, osPriorityNormal, 0, 256);
  mqttTaskHandle = osThreadCreate(osThread(mqttTask), NULL);

  /* definition and creation of bmeTask */
  osThreadDef(bmeTask, StartBmeTask, osPriorityLow, 0, 256);
  bmeTaskHandle = osThreadCreate(osThread(bmeTask), NULL);

  /* definition and creation of wizTask */
  osThreadDef(wizTask, StartWizTask, osPriorityHigh, 0, 256);
  wizTaskHandle = osThreadCreate(osThread(wizTask), (void*) &wiz);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  This function cannot be deleted in the CubeMX code generator.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  eeprom_status_t erc;
  w5500_status_t  wrc;

  // suspend tasks that require networking
  vTaskSuspend(dhcpTaskHandle);
  vTaskSuspend(mqttTaskHandle);

  // initialize UART logging
  Log_Init(&huart1);
  LOG_DEBUG("hello world!");

  // initialize shared device structures
  InitializeShared();

  // suspend these tasks when not bound
  dhcp.boundTask[0] = mqttTaskHandle;

  // read MAC from EEPROM
  do {
    erc = EEPROM_ReadMAC(&rom, wiz.mac);
    LOG_INFO("EEPROM Status: %s", EEPROM_StatusString(erc));
  } while (erc != EEPROM_OK);

  // log MAC
  LOG_INFO(
    "MAC %02X:%02X:%02X:%02X:%02X:%02X",
    wiz.mac[0],
    wiz.mac[1], 
    wiz.mac[2],
    wiz.mac[3],
    wiz.mac[4],
    wiz.mac[5]
  );

  // initialize W5500
  do {
    wrc = W5500_Initialize(&wiz);
    LOG_INFO("W5500 Status: %s", W5500_StatusString(wrc));
  } while (wrc != W5500_OK);

  // log PHY status
  W5500_LogPhyStatus(&wiz);

  // resume other threads
  vTaskResume(dhcpTaskHandle);

  // delete yourself
  vTaskDelete(NULL);

  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartLuxTask */
/**
* @brief Samples from an OPT3002 and places samples in a shared queue.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLuxTask */
void StartLuxTask(void const * argument)
{
  /* USER CODE BEGIN StartLuxTask */
  sample_t         optSample;     // OPT3002 sample
  opt3002_dev_t    optDev;        // OPT3002 device
  opt3002_cfg_t    optCfg;        // OPT3002 configuration
  opt3002_status_t rc;            // OPT3002 return codes
  bool             initialize;    // set to true to initialize OPT3002

  optSample.type  = TYPE_LUX;
  optDev.addr     = OPT3002_DEFAULT_ADDR << 1; // shifted 7-bit I2C address
  optDev.hi2cx    = hi2c1;                     // I2C port of the OPT3002
  optDev.busMutex = i2c1Mutex;                 // Mutex for the I2C bus
  optCfg.all      = OPT3002_DEFAULT_CFG;       // default configuration
  optCfg.bits.ct  = OPT3002_100_MS;            // conversion time
  optCfg.bits.m   = OPT3002_MODE_CONTINUOUS;   // continuous sample mode
  initialize      = true;

  while (1)
  {
    if (initialize)
    {
      LOG_DEBUG("Attempting OPT3002 initialization");
      rc = OPT3002_Init(&optDev, &optCfg);
      if (rc)
      {
        LOG_ERROR("OPT3002 initialization failed: %s", OPT3002_StatusString(rc));
      }
      else
      {
        LOG_INFO("OPT3002 initialized");
        initialize = false;
      }
    }
    else
    {
      // sample
      rc = OPT3002_Sample(&optDev, &optSample.value);
      if (rc)
      {
        LOG_ERROR("OPT3002 failed to sample: %s", OPT3002_StatusString(rc));
        initialize = true;
      }
      else
      {
        // enqueue samples for publishing
        xQueueSend(sampleQueue, (void *)&optSample, 0);
      }
    }

    // put this thread to sleep for the sample period
    vTaskDelay((configTICK_RATE_HZ * OPT3002_MIN_PERIOD) / 1000);
  }
  /* USER CODE END StartLuxTask */
}

/* USER CODE BEGIN Header_StartMqttTask */
/**
* @brief Pulls samples from the shared queue and publishes them.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMqttTask */
void StartMqttTask(void const * argument)
{
  /* USER CODE BEGIN StartMqttTask */

  // print buffer for samples
  static const size_t BUF_SIZE = 16;
  char printBuf[BUF_SIZE] __attribute__((aligned(16)));


  w5500_status_t rc;      // return code from client
  sample_t       sample;  // sample structure for fetching from queue
  int            printed; // characters printed by snprintf
  int32_t        whole;   // whole integer portion of floats for printing
  int32_t        decimal; // decimal portion of floats for printing

  while (1)
  {
    // initialize hardware and connect to MQTT server
    do {
      rc = MQTT_Initialize(&mqtt);
      if (rc != W5500_OK)
      {
        LOG_WARNING("MQTT_Initialize %s", W5500_StatusString(rc));
        continue;
      }

      rc = MQTT_Connect(&mqtt);
      if (rc != W5500_OK)
      {
        LOG_WARNING("MQTT_Connect %s", W5500_StatusString(rc));
      }
    } while (rc != W5500_OK);

    while (rc == W5500_OK)
    {
      // get sample from queue
      xQueueReceive(sampleQueue, (void*)&sample, portMAX_DELAY);

      // convert sample to string
      whole = (int32_t)sample.value;
      decimal = ((sample.value - (float)whole) * 1000);
      if (decimal < 0)
      {
        decimal *= -1;
      }
      printed = snprintf(printBuf, BUF_SIZE, "%01lu.%03lu", whole, decimal);

      // check for overflow
      if (printed > BUF_SIZE)
      {
        LOG_CRITICAL("BUFFER OVERFLOW %d vs %u", printed, BUF_SIZE);
        continue;
      }

      // publish sample
      rc = MQTT_Publish(
        &mqtt,                              // client
        SAMPLE_TYPE[sample.type],           // topic
        strlen(SAMPLE_TYPE[sample.type]),   // topic length
        printBuf,                           // payload
        (uint16_t)printed                   // payload length
      );
      if (rc != W5500_OK)
      {
        LOG_ERROR("MQTT_Publish failed %s", W5500_StatusString(rc));
      }
      else
      {
        LOG_INFO("MQTT_Publish %s %s", SAMPLE_TYPE[sample.type], printBuf);
      }
    }
  }
  /* USER CODE END StartMqttTask */
}

/* USER CODE BEGIN Header_StartBmeTask */
/**
* @brief Samples from an BME280 and places samples in a shared queue.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBmeTask */
void StartBmeTask(void const * argument)
{
  /* USER CODE BEGIN StartBmeTask */
  sample_t            temperatureSample;  // temperature sample structure
  sample_t            humiditySample;     // humidity sample structure
  sample_t            pressureSample;     // pressure sample structure
  bool                initialize;         // true when BME280 requires initialization
  bme280_status_t     rc;                 // BME280 return code
  bme280_dev_t        bmeDev;             // BME280 device
  bme280_ctrl_meas_t  ctrlMeas;           // BME280 measurement configuration
  bme280_ctrl_hum_t   ctrlHum;            // BME280 humidity configuration
  bme280_config_t     config;             // BME280 configuration
  TickType_t          standby;            // standby time in ticks

  bmeDev.addr        = BME280_DEFAULT_ADDR << 1; // shifted 7-bit I2C address
  bmeDev.hi2cx       = hi2c1;                    // I2C port of the BME280
  bmeDev.busMutex    = i2c1Mutex;                // Mutex for the I2C bus

  ctrlMeas.bits.osP  = BME280_OVERSAMPLE_08;    // pressure oversampling settings
  ctrlMeas.bits.osT  = BME280_OVERSAMPLE_08;    // temperature oversampling settings
  ctrlMeas.bits.mde  = BME280_MODE_NORMAL;      // BME280 mode
  ctrlHum.bits.osH   = BME280_OVERSAMPLE_08;    // humidity oversampling settings
  config.bits.filter = BME280_FILTER_16;        // filter settings
  config.bits.t_sb   = BME280_STANDBY_1000_ms;  // maximum standby to reduce self heating

  // initialize the BME280
  initialize = true;

  // get standby time in ticks
  standby = (BME280_GetStandbyTime(config.bits.t_sb) * configTICK_RATE_HZ) / 1000;

  // initialize sample structures
  temperatureSample.type = TYPE_TEMPEARTURE;
  humiditySample.type    = TYPE_HUMIDITY;
  pressureSample.type    = TYPE_PRESSURE;

  while (1)
  {
    if (initialize)
    {
      LOG_DEBUG("Attempting BME280 initialization");
      rc = BME280_Init(&bmeDev, &config, &ctrlMeas, &ctrlHum);
      if (rc)
      {
        LOG_ERROR("BME280 initialization failed: %s", BME280_StatusString(rc));
      }
      else
      {
        LOG_INFO("BME280 initialized");
        initialize = false;
      }
    }
    else
    {
      // sample
      rc = BME280_ReadEnvironment(
        &bmeDev,
        &temperatureSample.value,
        &pressureSample.value,
        &humiditySample.value
      );
      if (rc)
      {
        LOG_ERROR("BME280 failed to sample: %s", BME280_StatusString(rc));
        initialize = true;
      }
      else
      {
        // enqueue samples for publishing
        xQueueSend(sampleQueue, (void *)&temperatureSample, 0);
        xQueueSend(sampleQueue, (void *)&pressureSample,    0);
        xQueueSend(sampleQueue, (void *)&humiditySample,    0);
      }
    }

    // put this thread to sleep for the sample period
    vTaskDelay(standby);
  }
  /* USER CODE END StartBmeTask */
}

/* USER CODE BEGIN Header_StartWizTask */
/**
* @brief Handles W5500 interrupts, reads from the devices and sets the
         associated event groups if they exist.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWizTask */
void StartWizTask(void const * argument)
{
  /* USER CODE BEGIN StartWizTask */
  w5500_dev_t* dev = (w5500_dev_t*)argument;        // W5500 device from argument
  w5500_status_t rc;                                // W5500 return codes
  w5500_ir_t     ir   __attribute__((aligned(16))); // device interrupt register
  w5500_sn_ir_t  snir __attribute__((aligned(16))); // socket interrupt register
  uint8_t        sir  __attribute__((aligned(16))); // socket interrupt mask
  uint8_t        sn;                                // socket index

  while (1)
  {
    // wait for a notification from the interrupt handler
    ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

    // loop while the interrupt pin is in reset
    do
    {
      rc = W5500_GetIR(dev, &ir);
      if (rc != W5500_OK)
      {
        LOG_CRITICAL("W5500_GetIR FAILED %s", W5500_StatusString(rc));
        ir.all = 0;
      }

      // print out interrupts
      if (ir.all)
      {
        if (ir.bits.conflict)
        {
          LOG_WARNING("UNHANDLED EVENT: CONFLICT");
        }
        if (ir.bits.unreach)
        {
          LOG_WARNING("UNHANDLED EVENT: UNREACH");
        }
        if (ir.bits.ppoe)
        {
          LOG_WARNING("UNHANDLED EVENT: PPOE");
        }
        if (ir.bits.mp)
        {
          LOG_WARNING("UNHANDLED EVENT: MP");
        }

        // clear interrupts
        rc = W5500_SetIR(dev, &ir);
        if (rc != W5500_OK)
        {
          LOG_CRITICAL("W5500_SetIR FAILED %s", W5500_StatusString(rc));
        }
      }

      rc = W5500_GetSIR(dev, &sir);
      if (rc != W5500_OK)
      {
        LOG_CRITICAL("W5500_GetSIR FAILED %s", W5500_StatusString(rc));
        sir = 0;
      }

      for (sn = 0; sn < W5500_NUM_SOCKETS; sn++)
      {
        if (sir & (1U << sn))
        {
          rc = W5500_GetSnIR(dev, sn, &snir);
          if (rc != W5500_OK)
          {
            LOG_CRITICAL("W5500_GetSnIR FAILED %s", W5500_StatusString(rc));
            continue;
          }

          // set the event bits for other tasks
          if (dev->snEvent[sn] != NULL)
          {
            xEventGroupSetBits(dev->snEvent[sn], snir.all);
          }
          else
          {
            if (snir.bits.con)
            {
              LOG_WARNING("UNHANDLED EVENT: SOCKET %u CON", sn);
            }
            if (snir.bits.discon)
            {
              LOG_WARNING("UNHANDLED EVENT: SOCKET %u DISCON", sn);
            }
            if (snir.bits.recv)
            {
              LOG_WARNING("UNHANDLED EVENT: SOCKET %u RECV", sn);
            }
            if (snir.bits.timeout)
            {
              LOG_WARNING("UNHANDLED EVENT: SOCKET %u TIMEOUT", sn);
            }
            if (snir.bits.sendok)
            {
              LOG_WARNING("UNHANDLED EVENT: SOCKET %u SEND_OK", sn);
            }
          }

          rc = W5500_SetSnIR(dev, sn, &snir);
          if (rc != W5500_OK)
          {
            LOG_CRITICAL("W5500_SetSnIR FAILED %s", W5500_StatusString(rc));
          }
        }
      }
    } while (HAL_GPIO_ReadPin(dev->intPort, dev->intPin) == GPIO_PIN_RESET);
  }
  /* USER CODE END StartWizTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
