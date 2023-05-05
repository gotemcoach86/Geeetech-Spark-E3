/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "usart.h"
#include "flash_if.h"
#include "gpio.h"
#include "common.h"
#include "menu.h"
#include "ff.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define BOOT_FW_VERSION			"Bootloader Version: 1.0.9"			

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define UartHandle  huart1

#define FLASH_SIZE                         ((uint32_t)0x80000)  /* 128 KBytes */
#define IAP_SIZE                           ((uint32_t)0x7000)   /* 32Kbytes as IAP size */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SDIO_CD_Pin GPIO_PIN_12
#define SDIO_CD_GPIO_Port GPIOA
#define FSMC_LIGHT_Pin GPIO_PIN_13
#define FSMC_LIGHT_GPIO_Port GPIOD

#define HEATER_0_Pin GPIO_PIN_9
#define HEATER_0_GPIO_Port GPIOB

#define HEATER_BED_Pin GPIO_PIN_0
#define HEATER_BED_GPIO_Port GPIOE

#define FAN_Pin			GPIO_PIN_1
#define FAN_GPIO_Port	GPIOE

#define FAN1_24V_Pin		GPIO_PIN_3
#define FAN1_24V_GPIO_Port	GPIOD

#define FAN2_24V_Pin		GPIO_PIN_6
#define FAN2_24V_GPIO_Port	GPIOD

/* USER CODE BEGIN Private defines */

extern const char DownloadFile[];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
