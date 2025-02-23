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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// USBD_HandleTypeDef USBD_Device;
// hack uart for debug

/* USER CODE END 0 */

extern uint32_t __isr_vector_vma_start; // 中断向量表VMA起始地址

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    // 先禁用所有中断
    __disable_irq();
    
    // 检查当前状态
    printf("\n\nApplication Entry Point!\n");
    printf("Current state:\n");
    printf("  MSP: 0x%08X\n", __get_MSP());
    printf("  VTOR: 0x%08X\n", SCB->VTOR);
    printf("  CONTROL: 0x%08X\n", __get_CONTROL());
    printf("  PRIMASK: 0x%08X\n", __get_PRIMASK());
    
    // 设置中断向量表
    SCB->VTOR = 0x30000000;
    
    // 初始化系统时钟
    SystemClock_Config();
    
    // 初始化串口
    USART1_Init();
    printf("\r\nApplication started!\r\n");
    
    // 启用 FPU
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
    
    // 启用缓存
    SCB_EnableICache();
    SCB_EnableDCache();
    
    // 启用中断
    __enable_irq();
    
    // 继续执行应用程序
    cpp_main();
    
    // 如果到这里说明 cpp_main 返回了
    printf("cpp_main returned!\r\n");
    
    while (1)
    {
        // 添加一些可见的指示
        printf(".");
        HAL_Delay(1000);
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  printf("Error_Handler\n");
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
