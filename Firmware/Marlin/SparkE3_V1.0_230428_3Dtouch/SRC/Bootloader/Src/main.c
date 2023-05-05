/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "menu.h"
#include "common.h"
#include "flash_if.h"
#include "bsp_lcd.h"
#include "command.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define IAP_CHECK_TIME			1000
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t usart_recv_data;

extern pFunction JumpToApplication;
extern uint32_t JumpAddress;

const char DownloadFile[] = "GTM32Source.bin";
const char BackupFile[] = "GTM32Source.bk";

extern void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* sramHandle);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern void SerialDownload(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define TFT_LCD_ENABLE	
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_FSMC_Init();
    MX_SDIO_SD_Init();
    MX_FATFS_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
	Serial_PutString(BOOT_FW_VERSION);
    // SDCard update check first
    if(FR_OK == f_mount_status && f_open(&SDFile, DownloadFile, FA_READ) == FR_OK) {
        BSP_LCD_Init();
        FLASH_If_Init();
        LCD_DispString_EN(10, 280, BOOT_FW_VERSION, BLACK, GRAY, USB_FONT_24);
        LCD_DispString_EN(10, 100, "SDCard Upgrade ... ", BLACK, YELLOW, USB_FONT_24);
        Serial_PutString("\r\nSDCard Upgrade ...  \r\n");
        if(COMMAND_DOWNLOAD() != DOWNLOAD_OK) {
            LCD_DispString_EN(10, 130, "Flash download error", BLACK, RED, USB_FONT_24);
            Serial_PutString("Flash download error \r\n");
        } else {
            LCD_DispString_EN(10, 130, "Download done", BLACK, GREEN, USB_FONT_24);
            Serial_PutString("\r\nDownload done \r\n");
            f_close(&SDFile);
			#if 0
            if(f_rename(DownloadFile, BackupFile)) { //if(FR_OK == f_unlink(DownloadFile))
                HAL_Delay(200);
                HAL_NVIC_SystemReset();
            }
			#endif
        }
    } else { // Without SDCard update, check usart update.
        /* Clean the input path */
        __HAL_UART_FLUSH_DRREGISTER(&UartHandle);

        /* Receive key */
        if(HAL_OK == HAL_UART_Receive(&UartHandle, &usart_recv_data, 1, IAP_CHECK_TIME)) {
            if(usart_recv_data == 'c' || usart_recv_data == 'C') {
                BSP_LCD_Init();
                FLASH_If_Init();
                LCD_DispString_EN(10, 290, BOOT_FW_VERSION, BLACK, GRAY, USB_FONT_24);
                LCD_DispString_EN(10, 100, "USART Update ...", BLACK, YELLOW, USB_FONT_24);
				Serial_PutString("\r\nUSART Update ... \r\n");
                SerialDownload(); //Main_Menu ();
            }
        }
    }

    /*** Close all of interrupts before jump app */
    __disable_irq();

    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if(((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000) {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4);
        JumpToApplication = (pFunction) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
        JumpToApplication();
    }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
