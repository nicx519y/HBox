/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "usb_otg_hs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
// void SystemClock_Config(void);
// void PeriphCommonClock_Config(void);
// static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// USBD_HandleTypeDef USBD_Device;
// hack uart for debug

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Enable FPU */
  // Enable CP10 and CP11 Full Access
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
  
  // Enable FPU and configure it
  FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;
  FPU->FPCAR = 0;
  FPU->FPDSCR = 0;
  
  // Clear floating-point status and control register
  __set_FPSCR(0);
  
  // Ensure all FPU instructions complete and flush pipeline
  __DSB();
  __ISB();
  __DMB();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  SystemClock_Config();
  
  // Initialize UART first for debug output
  USART1_Init();
  
  // Simple UART test
  printf("\r\n\r\n=== UART Test Start ===\r\n");
  printf("System Clock: %lu Hz\r\n", HAL_RCC_GetSysClockFreq());
  printf("HCLK: %lu Hz\r\n", HAL_RCC_GetHCLKFreq());
  printf("PCLK1: %lu Hz\r\n", HAL_RCC_GetPCLK1Freq());
  printf("PCLK2: %lu Hz\r\n", HAL_RCC_GetPCLK2Freq());
  printf("=== UART Test End ===\r\n\r\n");

  /* USER CODE BEGIN Init */

  // Enable the CPU Cache
  SCB_EnableICache(); /* Enable I-Cache */
  SCB_EnableDCache(); /* Enable D-Cache */

  cpp_main();
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
