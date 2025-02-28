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

// extern uint32_t __isr_vector_vma_start; // 中断向量表VMA起始地址

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    // 确保向量表指向 QSPI Flash
    SCB->VTOR = 0x90000000;
    // 初始化系统时钟
    SystemClock_Config();
    
    // 初始化串口
    USART1_Init();
    
    // LED 初始化
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    GPIOC->MODER &= ~(3U << (13 * 2));
    GPIOC->MODER |= (1U << (13 * 2));
    
    printf("\r\nApplication Started!\r\n");
    
    // LED 闪烁
    while(1) {
        GPIOC->ODR ^= (1 << 13);
        HAL_Delay(500);
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

// 添加一个简单的测试函数，放在固定偏移位置
__attribute__((section(".text.test_func")))
__attribute__((used))
__attribute__((aligned(4)))
static void test_function(void) __attribute__((naked));
static void test_function(void)
{
    asm volatile(
        ".thumb\n"            // 明确指定 Thumb 模式
        ".syntax unified\n"   // 使用统一语法
        ".align 2\n"         // 4 字节对齐
        "bx lr\n"            // 直接返回
        ".align 2\n"
    );
}
