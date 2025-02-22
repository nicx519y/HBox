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
#include "usart.h"
#include "qspi-w25q64.h"
#include <string.h>
#include "bootloader_config.h"
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
typedef void (*pFunction)(void);
pFunction JumpToApplication;

int8_t mmpResult = -1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void checkInterruptVectorTable(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint32_t _application_dst;    // RAM中应用程序的目标地址
extern uint32_t _application_size;    // QSPI Flash中应用程序的结束地址
extern uint32_t _application_length;    //RAM_D2的大小
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    SCB_EnableICache(); // 使能ICache
    SCB_EnableDCache(); // 使能DCache
    /* USER CODE END 1 */

    /* MPU Configuration--------------------------------------------------------*/
    MPU_Config();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    SystemClock_Config(); // 初始化时钟
    
    USART1_Init();      // 串口初始化
    BOOT_DBG("\r\nsystem start...\r\n");

    BOOT_DBG("\r\n***************************************\r\n");

    QSPI_W25Qxx_Init(); // 初始化W25Q64
    BOOT_DBG("\r\nW25Q64 init complete\r\n");

    SCB_DisableICache(); // 关闭ICache
    SCB_DisableDCache(); // 关闭Dcache
    BOOT_DBG("\r\nICache and Dcache disabled\r\n");

    HAL_MPU_Disable(); // 关闭MPU
    BOOT_DBG("\r\nMPU disabled\r\n");

    // const uint32_t ram_addr = 0x30000000;

    BOOT_DBG("Checking memory addresses:\r\n");
    BOOT_DBG("_application_dst: 0x%08X\r\n", (uint32_t) &_application_dst);    
    BOOT_DBG("_application_size: 0x%08X\r\n", (uint32_t) &_application_size);  

    // 拷贝应用程序到 从QSPI Flash 到 RAM_D2
    const uint32_t app_size = &_application_size; // 根据实际程序大小调整
    uint32_t ram_addr = &_application_dst; // 根据实际RAM地址调整

    BOOT_DBG("Copying application from QSPI to RAM...\r\n");
    
    // 进入XIP模式
    // if(QSPI_W25Qxx_EnterMemoryMappedMode() != QSPI_W25Qxx_OK) {
    //     BOOT_DBG("Failed to enter memory mapped mode\r\n");
    //     return -1;
    // }

    // // 拷贝应用程序 从QSPI Flash 到 RAM_D2
    // memcpy((void*)ram_addr, (void*)qspi_addr, app_size);

    // 拷贝应用程序 从QSPI Flash 到 RAM_D1
    if(QSPI_W25Qxx_ReadBuffer((uint8_t*)ram_addr, 0x00000000, app_size) != QSPI_W25Qxx_OK) {
        BOOT_DBG("Failed to read application from QSPI Flash to RAM_D1\r\n");
        return -1;
    }

    BOOT_DBG("Application copied to RAM_D1\r\n");

    // QSPI_W25Qxx_EnterMemoryMappedMode();
    

    SysTick->CTRL = 0; // 关闭SysTick
    SysTick->LOAD = 0; // 清零重载
    SysTick->VAL = 0;  // 清零计数
    BOOT_DBG("\r\nSysTick disabled\r\n");

    SCB->VTOR = (uint32_t)ram_addr; // 设置中断向量表地址
    // 检查中断向量表地址是否设置正确
    checkInterruptVectorTable();

    for (uint8_t i = 0; i < 8; i++)
    { // clear all NVIC Enable and Pending registers
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    __set_CONTROL(0); // priviage mode
    __disable_irq();  // disable interrupt
    __set_PRIMASK(1);

    
    

    BOOT_DBG("\r\nNVIC disabled\r\n");

    BOOT_DBG("ram_addr: 0x%08X\r\n", (uint32_t)ram_addr);

    JumpToApplication = (pFunction)(*(__IO uint32_t *)(ram_addr + 4)); // 设置起始地址
    __set_MSP(*(__IO uint32_t *)ram_addr);                             // 设置主堆栈指
    BOOT_DBG("Jump to W25Q64 user program RAM_D2>>>\r\n\r\n");

    JumpToApplication(); // 执行跳转

    print_app_metadata();

    while(1);
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 2;
    RCC_OscInitStruct.PLL.PLLN = 80;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN 0 */
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;     // 设置QSPI时钟
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK; // 选择HCLK(240MHz)作为QSPI内核时钟
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE END 0 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x0;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

// 定义中断向量表的大小
#define NVIC_NUM_INTERRUPTS 166 // 根据实际中断数量调整

void checkInterruptVectorTable(void)
{
    // 获取中断向量表的起始地址
    uint32_t *vectorTable = (uint32_t *)SCB->VTOR;
    BOOT_DBG("Checking interrupt vector table...\r\n");

    // 检查每个中断处理程序的地址
    for (uint32_t i = 0; i < NVIC_NUM_INTERRUPTS; i++)
    {
        uint32_t handlerAddress = vectorTable[i];

        // 检查是否指向 QSPI Flash (0x9xxxxxxx)
        if ((handlerAddress & 0xF0000000) == 0x90000000)
        {
            // 计算相对偏移量并重定向到 RAM_D2
            uint32_t offset = handlerAddress - 0x90000000;
            uint32_t new_address = (uint32_t)&_application_dst + offset;

            // 更新向量表
            // vectorTable[i] = new_address;

            BOOT_DBG("Fixed vector %d: 0x%08X -> 0x%08X", i, handlerAddress, new_address);
        } else {
            BOOT_DBG("Vector %d OK: 0x%08X", i, handlerAddress);
        }

        // // 检查修复后的地址是否在 RAM_D2 范围内
        // handlerAddress = vectorTable[i];
        // if (handlerAddress < (uint32_t)&_application_dst || 
        //     handlerAddress >= ((uint32_t)&_application_dst + 256 * 1024))
        // {
        //     BOOT_DBG("Warning: vector %d is not in RAM_D1: 0x%08X", i, handlerAddress);
        // }
        // else
        // {
        //     BOOT_DBG("Vector %d OK: 0x%08X\r\n", i, handlerAddress);
        // }
    }

    // 确保修改生效
    __DSB();
    __ISB();
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}


#ifdef USE_FULL_ASSERT
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

void print_app_metadata(void)
{
    AppMetadata *metadata = (AppMetadata *)0x30000000;
    
    if(metadata->magic == 0x4D544144) {
        BOOT_DBG("Application Metadata:\r\n");
        BOOT_DBG("Text size: %d bytes\r\n", metadata->text_size);
        BOOT_DBG("Data size: %d bytes\r\n", metadata->data_size);
        BOOT_DBG("BSS size: %d bytes\r\n", metadata->bss_size);
        BOOT_DBG("Total size: %d bytes\r\n", metadata->total_size);
    } else {
        BOOT_DBG("Invalid metadata magic number\r\n");
    }
}
