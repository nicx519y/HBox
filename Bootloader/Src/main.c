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
#include "main.h"
#include "usart.h"
#include "qspi-w25q64.h"
#include <string.h>
#include "bootloader_config.h"

typedef void (*pFunction)(void);
// pFunction JumpToApplication;

int8_t mmpResult = -1;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
void copyCodeFromQSPIToRAM(void);
void jump_to_application(uint32_t app_entry, uint32_t app_stack, uint32_t vtor_addr);


extern uint32_t _ApplicationMetadata;

const AppMetadata *metadata = (AppMetadata *)&_ApplicationMetadata;  // 使用正确的元数据地址
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    SCB_EnableICache();		// 使能ICache
	SCB_EnableDCache();		// 使能DCache
    /* USER CODE END 1 */

    /* MPU Configuration--------------------------------------------------------*/
    MPU_Config();

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    SystemClock_Config();                         //初始化时
    // LED_Init();                                //LED初始
    USART1_Init();                                //串口初始

    printf("\r\n***************************************\r\n");

    int8_t ret = QSPI_W25Qxx_Init();	  				                // 初始化W25Q64
    if(ret != 0) {
        BOOT_ERR("QSPI_W25Qxx_Init() failed");
        while(1);
    }
    BOOT_DBG("QSPI_W25Qxx_Init() success");

    SCB_DisableICache();		                      // 关闭ICache
    SCB_DisableDCache();		                      // 关闭Dcache

    HAL_MPU_Disable();                            // 关闭MPU

    uint32_t* buffer = (uint32_t*)malloc(1024);
    ret = QSPI_W25Qxx_ReadBuffer((uint8_t*)buffer, 0x00000000, 1024); 	// 配置QSPI为内存映射模
    if(ret != 0) {
        BOOT_ERR("QSPI_W25Qxx_ReadBuffer() failed");
        while(1);
    }
    BOOT_DBG("QSPI_W25Qxx_ReadBuffer() success");

    mmpResult = QSPI_W25Qxx_EnterMemoryMappedMode(); 	// 配置QSPI为内存映射模
    printf("QSPI_W25Qxx_EnterMemoryMappedMode() = %d\r\n", mmpResult);


    SysTick->CTRL = 0;		                        // 关闭SysTick
    SysTick->LOAD = 0;		                        // 清零重载
    SysTick->VAL = 0;			                        // 清零计数

    for(uint8_t i = 0; i < 8; i++) { //clear all NVIC Enable and Pending registers
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }
    __set_CONTROL(0); //priviage mode 
    __disable_irq(); //disable interrupt
    __set_PRIMASK(1);
    
    BOOT_DBG("MSP = 0x%08X", *(__IO uint32_t*) W25Qxx_Mem_Addr);
    BOOT_DBG("Entry point = 0x%08X", *(__IO uint32_t*) (W25Qxx_Mem_Addr + 4));
    // 验证向量表内容
	uint32_t *vector_table = (uint32_t*)W25Qxx_Mem_Addr;
	BOOT_DBG("Vector Table Contents:");
	BOOT_DBG("  Stack Pointer: 0x%08X", vector_table[0]);
	BOOT_DBG("  Reset Handler: 0x%08X", vector_table[1]);
	
	// 验证 Reset_Handler 的内容
	uint32_t *reset_handler = (uint32_t*)(vector_table[1] & ~1);  // 清除 Thumb 位
	BOOT_DBG("Reset Handler Memory:");
	for(int i = 0; i < 8; i++) {
		BOOT_DBG("  Word %d: 0x%08X", i, reset_handler[i]);
	}

    // 在跳转前打印关键信息
    BOOT_DBG("Vector Table Address: 0x%08X", W25Qxx_Mem_Addr);  // 打印向量表地址
    BOOT_DBG("Stack Pointer Value: 0x%08X", *(__IO uint32_t*)W25Qxx_Mem_Addr);  // 打印栈指针值
    BOOT_DBG("Reset Handler Value: 0x%08X", *(__IO uint32_t*)(W25Qxx_Mem_Addr + 4));  // 打印复位处理程序地址

    // 执行跳转
    JumpToApplication();

    // 不应该到达这里
    BOOT_ERR("Something went wrong!");
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
    /* USER CODE END 0 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};
    
    /* 禁用 MPU */
    HAL_MPU_Disable();

    /* 配置 RAM 区域 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x24000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* 配置 QSPI Flash 区域 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = 0x90000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;  // 允许执行
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* 启用 MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    /* 启用缓存 */
    SCB_EnableICache();
    SCB_EnableDCache();
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

void copyCodeFromQSPIToRAM(void)
{
    if(metadata->magic == METADATA_MAGIC) {
        uint32_t size;

        // 1. 先拷贝中断向量表
        size = metadata->isr_vector.vma_end - metadata->isr_vector.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying ISR vector section");
            BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->isr_vector.vma_start, metadata->isr_vector.vma_end);
            BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->isr_vector.lma_start, metadata->isr_vector.lma_end);
            
            memcpy((void*)metadata->isr_vector.vma_start, 
                   (void*)metadata->isr_vector.lma_start, size);
            
            // 验证拷贝是否成功
            if (memcmp((void*)metadata->isr_vector.vma_start, 
                      (void*)metadata->isr_vector.lma_start, size) != 0) {
                BOOT_ERR("ISR vector verification failed");
                return;
            }
        }

        // 2. 拷贝代码段
        size = metadata->text.vma_end - metadata->text.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying text section");
            BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->text.vma_start, metadata->text.vma_end);
            BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->text.lma_start, metadata->text.lma_end);
            memcpy((void*)metadata->text.vma_start, 
                   (void*)metadata->text.lma_start, size);
        }

        // 3. 拷贝只读数据
        size = metadata->rodata.vma_end - metadata->rodata.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying rodata section");
            BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->rodata.vma_start, metadata->rodata.vma_end);
            BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->rodata.lma_start, metadata->rodata.lma_end);
            memcpy((void*)metadata->rodata.vma_start, 
                   (void*)metadata->rodata.lma_start, size);
        }

        // 4. 拷贝数据段
        size = metadata->data.vma_end - metadata->data.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying data section");
            BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->data.vma_start, metadata->data.vma_end);
            BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->data.lma_start, metadata->data.lma_end);
            memcpy((void*)metadata->data.vma_start, 
                   (void*)metadata->data.lma_start, size);
        }

        // 5. 清零 BSS 段
        size = metadata->bss.vma_end - metadata->bss.vma_start;
        if(size > 0) {
            BOOT_DBG("Zeroing BSS section");
            BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->bss.vma_start, metadata->bss.vma_end);
            BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->bss.lma_start, metadata->bss.lma_end);
            memset((void*)metadata->bss.vma_start, 0, size);
        }

        BOOT_DBG("All sections copied successfully");
    } else {
        BOOT_ERR("Invalid metadata magic number: 0x%08X", metadata->magic);
    }
}



void JumpToApplication(void)
{
    uint32_t jump_address = *(__IO uint32_t*)(W25Qxx_Mem_Addr + 4);
    uint32_t app_stack = *(__IO uint32_t*)W25Qxx_Mem_Addr;
    
    BOOT_DBG("App Stack: 0x%08X", app_stack);
    BOOT_DBG("Jump Address: 0x%08X", jump_address);

    // 验证栈指针和跳转地址
    if ((app_stack & 0xFF000000) != 0x24000000) {
        BOOT_ERR("Invalid stack pointer: 0x%08X", app_stack);
        return;
    }

    if ((jump_address & 0xFF000000) != 0x90000000) {
        BOOT_ERR("Invalid jump address: 0x%08X", jump_address);
        return;
    }

    // 先验证一下目标地址的内容
    uint16_t* code_ptr = (uint16_t*)(jump_address & ~1UL);
    BOOT_DBG("First instructions at target:");
    for(int i = 0; i < 4; i++) {
        BOOT_DBG("  Instruction %d: 0x%04X", i, code_ptr[i]);
    }

    // 关闭所有中断
    __disable_irq();
    BOOT_DBG("Interrupts disabled");

    // 清除所有中断
    for(int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    BOOT_DBG("NVIC cleared");

    // 设置向量表
    SCB->VTOR = W25Qxx_Mem_Addr;
    BOOT_DBG("VTOR set to: 0x%08X", SCB->VTOR);
    
    // 验证向量表设置是否生效
    BOOT_DBG("SCB->VTOR after set: 0x%08X", SCB->VTOR);
    BOOT_DBG("Stack Pointer from vector: 0x%08X", *(__IO uint32_t*)SCB->VTOR);
    BOOT_DBG("Reset Handler from vector: 0x%08X", *(__IO uint32_t*)(SCB->VTOR + 4));

    // 设置主堆栈指针
    __set_MSP(app_stack);
    BOOT_DBG("MSP set to: 0x%08X", __get_MSP());

    // 清除缓存
    SCB_CleanInvalidateDCache();
    SCB_InvalidateICache();
    BOOT_DBG("Cache cleared");

    // 内存屏障
    __DSB();
    __ISB();
    BOOT_DBG("Memory barriers executed");

    // 确保跳转地址是 Thumb 模式
    jump_address |= 0x1;
    BOOT_DBG("Final jump address (with Thumb bit): 0x%08X", jump_address);

    // 使用函数指针跳转
    typedef void (*pFunction)(void);
    pFunction app_reset_handler = (pFunction)jump_address;

    BOOT_DBG("About to jump...");
    BOOT_DBG("Last debug message before jump!");
    
    // 最后一次内存屏障
    __DSB();
    __ISB();
    
    // 跳转
    app_reset_handler();

    // 不应该到达这里
    BOOT_ERR("Jump failed!");
    while(1);
}
