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
pFunction JumpToApplication;

int8_t mmpResult = -1;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void checkInterruptVectorTable(uint32_t ram_addr, uint32_t ram_size);
static void MPU_Config(void);
void copyCodeFromQSPIToRAM(void);


extern uint32_t _QSPIFLASH_start;
extern uint32_t _QSPIFLASH_size;
extern uint32_t _QSPIFLASH_METADATA_start;
extern uint32_t _QSPIFLASH_METADATA_size;

const AppMetadata *metadata = (AppMetadata *)QSPI_METADATA_ADDRESS;  // 使用正确的元数据地址
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

    if(QSPI_W25Qxx_EnterMemoryMappedMode() != QSPI_W25Qxx_OK) {
        BOOT_DBG("Failed to enter memory mapped mode\r\n");
        return -1;
    }

    BOOT_DBG("\r\nEnter memory mapped mode\r\n");

    copyCodeFromQSPIToRAM();
    
    // 获取中断向量表地址
    const uint32_t vtor_addr = metadata->isr_vector.vma_start;
    BOOT_DBG("ISR Vector VMA: 0x%08X, LMA: 0x%08X\r\n", metadata->isr_vector.vma_start, metadata->isr_vector.lma_start);
    
    // 设置中断向量表
    SCB->VTOR = vtor_addr;
    
    // 清除所有中断
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // 关闭所有外设中断
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // 设置特权级别和禁用中断
    __set_CONTROL(0);
    __disable_irq();
    __set_PRIMASK(1);

    // 内存屏障
    __DSB();
    __ISB();

    // 验证向量表内容
    uint32_t *vector_table = (uint32_t *)vtor_addr;
    BOOT_DBG("Vector Table Contents:");
    for(int i = 0; i < 8; i++) {
        BOOT_DBG("  [%d]: 0x%08X", i, vector_table[i]);
    }

    // 获取应用程序入口点和堆栈指针
    uint32_t app_entry = *((uint32_t*)(vtor_addr + 4));  // 取地址中的值
    uint32_t app_stack = *((uint32_t*)vtor_addr);        // 取地址中的值

    BOOT_DBG("VTOR: 0x%08X\r\n", SCB->VTOR);
    BOOT_DBG("Application Stack: 0x%08X\r\n", app_stack);
    BOOT_DBG("Application Entry: 0x%08X\r\n", app_entry);

    if((app_stack & 0xFFF00000) == 0x30000000) {  // 检查堆栈指针是否在 RAM_D2 范围
        // 设置堆栈指针
        __set_MSP(app_stack);
        
        BOOT_DBG("Stack pointer set successfully\r\n");
        BOOT_DBG("Ready to jump to application at 0x%08X\r\n", app_entry);

        // 跳转到应用程序
        JumpToApplication = (pFunction)app_entry;
        JumpToApplication();
    } else {
        BOOT_ERR("Invalid stack pointer: 0x%08X\r\n", app_stack);
    }

    while (1)
    {
    }
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

void checkInterruptVectorTable(uint32_t ram_addr, uint32_t ram_size)
{
    // 定义中断向量表中的处理函数类型
    typedef void (*pFunction)(void);
    
    // 获取当前的中断向量表地址
    uint32_t *vectorTable = (uint32_t *)SCB->VTOR;
    
    BOOT_DBG("\r\nChecking Interrupt Vector Table at address: 0x%08X\r\n", (uint32_t)vectorTable);

    // 定义一些关键中断的名称
    const char* vector_names[] = {
        "Initial_SP",           // 0: Initial Stack Pointer
        "Reset_Handler",        // 1: Reset Handler
        "NMI_Handler",          // 2: NMI Handler
        "HardFault_Handler",    // 3: Hard Fault Handler
        "MemManage_Handler",    // 4: MPU Fault Handler
        "BusFault_Handler",     // 5: Bus Fault Handler
        "UsageFault_Handler",   // 6: Usage Fault Handler
        "Reserved",             // 7: Reserved
        "Reserved",             // 8: Reserved
        "Reserved",             // 9: Reserved
        "Reserved",             // 10: Reserved
        "SVC_Handler",          // 11: SVCall Handler
        "DebugMon_Handler",     // 12: Debug Monitor Handler
        "Reserved",             // 13: Reserved
        "PendSV_Handler",       // 14: PendSV Handler
        "SysTick_Handler"       // 15: SysTick Handler
    };

    // 打印前16个关键中断向量
    for (int i = 0; i < 16; i++) {
        uint32_t handler_addr = vectorTable[i];
        pFunction handler = (pFunction)handler_addr;
        
        BOOT_DBG("Vector %2d (%20s): 0x%08X", i, vector_names[i], handler_addr);
        
        // 检查地址的有效性
        if (handler_addr == 0 || handler_addr == 0xFFFFFFFF) {
            BOOT_DBG(" [INVALID]");
        } else {
            // 检查地址范围
            if ((handler_addr >= FLASH_BASE && handler_addr < (FLASH_BASE + FLASH_SIZE)) ||
                (handler_addr >= QSPI_APP_ADDRESS && handler_addr < (QSPI_APP_ADDRESS + QSPI_APP_SIZE)) ||
                (handler_addr >= ram_addr && handler_addr < (ram_addr + ram_size))) {
                BOOT_DBG(" [VALID]");
            } else {
                BOOT_DBG(" [OUT OF RANGE]");
            }
        }
        BOOT_DBG("\r\n");
    }

    // 检查向量表是否在正确的内存区域
    if ((uint32_t)vectorTable >= ram_addr && (uint32_t)vectorTable < (ram_addr + ram_size)) {
        BOOT_DBG("Vector Table is in RAM\r\n");
    } else if ((uint32_t)vectorTable >= FLASH_BASE && (uint32_t)vectorTable < (FLASH_BASE + FLASH_SIZE)) {
        BOOT_DBG("Vector Table is in FLASH\r\n");
    } else if ((uint32_t)vectorTable >= QSPI_APP_ADDRESS && (uint32_t)vectorTable < (QSPI_APP_ADDRESS + QSPI_APP_SIZE)) {
        BOOT_DBG("Vector Table is in QSPI FLASH\r\n");
    } else {
        BOOT_ERR("Vector Table is in unknown memory region!\r\n");
    }

    // 检查堆栈指针
    uint32_t sp = __get_MSP();
    BOOT_DBG("Current MSP: 0x%08X\r\n", sp);
    if (sp >= ram_addr && sp < (ram_addr + ram_size)) {
        BOOT_DBG("Stack pointer is in valid RAM region\r\n");
    } else {
        BOOT_ERR("Stack pointer is in invalid memory region!\r\n");
    }
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
    // 从正确的地址读取元数据
    AppMetadata *metadata = (AppMetadata *)QSPI_METADATA_ADDRESS;
    
    BOOT_DBG("Metadata address: 0x%08X", QSPI_METADATA_ADDRESS);
    BOOT_DBG("Metadata magic: 0x%08X", metadata->magic);

    if(metadata->magic == METADATA_MAGIC) {
        // 打印所有段的信息
        BOOT_DBG("Text section:");
        BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->text.vma_start, metadata->text.vma_end);
        BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->text.lma_start, metadata->text.lma_end);
        
        BOOT_DBG("Data section:");
        BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->data.vma_start, metadata->data.vma_end);
        BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->data.lma_start, metadata->data.lma_end);
        
        BOOT_DBG("BSS section:");
        BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->bss.vma_start, metadata->bss.vma_end);
        BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->bss.lma_start, metadata->bss.lma_end);
        
        BOOT_DBG("ROData section:");
        BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->rodata.vma_start, metadata->rodata.vma_end);
        BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->rodata.lma_start, metadata->rodata.lma_end);
        
        BOOT_DBG("ISR Vector section:");
        BOOT_DBG("  VMA: 0x%08X - 0x%08X", metadata->isr_vector.vma_start, metadata->isr_vector.vma_end);
        BOOT_DBG("  LMA: 0x%08X - 0x%08X", metadata->isr_vector.lma_start, metadata->isr_vector.lma_end);

        // 拷贝各个段
        // Text section
        uint32_t size = metadata->text.vma_end - metadata->text.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying text section");
            memcpy((void*)metadata->text.vma_start, (void*)metadata->text.lma_start, size);
        }

        // Data section
        size = metadata->data.vma_end - metadata->data.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying data section");
            memcpy((void*)metadata->data.vma_start, (void*)metadata->data.lma_start, size);
        }

        // BSS section
        size = metadata->bss.vma_end - metadata->bss.vma_start;
        if(size > 0) {
            BOOT_DBG("Zeroing BSS section");
            memset((void*)metadata->bss.vma_start, 0, size);
        }

        // ROData section
        size = metadata->rodata.vma_end - metadata->rodata.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying rodata section");
            memcpy((void*)metadata->rodata.vma_start, (void*)metadata->rodata.lma_start, size);
        }

        // ISR Vector section
        size = metadata->isr_vector.vma_end - metadata->isr_vector.vma_start;
        if(size > 0) {
            BOOT_DBG("Copying ISR vector section");
            
            // 打印源数据
            BOOT_DBG("Source data (LMA):");
            for(int i = 0; i < 16; i++) {
                BOOT_DBG("ISR Vector %2d: 0x%08X", 
                    i, *(uint32_t*)(metadata->isr_vector.lma_start + i * 4));
            }
            
            memcpy((void*)metadata->isr_vector.vma_start, 
                   (void*)metadata->isr_vector.lma_start, size);
            
            // 打印目标数据
            BOOT_DBG("Destination data (VMA):");
            for(int i = 0; i < 16; i++) {
                BOOT_DBG("ISR Vector %2d: 0x%08X", 
                    i, *(uint32_t*)(metadata->isr_vector.vma_start + i * 4));
            }
        }

        BOOT_DBG("All sections copied successfully");
    } else {
        BOOT_ERR("Invalid metadata magic number: 0x%08X", metadata->magic);
    }
}
