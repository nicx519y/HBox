#include "main.h"

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/

// 只保留 Bootloader 需要的中断处理函数
void NMI_Handler(void) { while(1) {} }
void HardFault_Handler(void) { while(1) {} }
void MemManage_Handler(void) { while(1) {} }
void BusFault_Handler(void) { while(1) {} }
void UsageFault_Handler(void) { while(1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) { HAL_IncTick(); }

// 添加其他中断处理函数的弱定义
__weak void PVD_AVD_IRQHandler(void) {}
__weak void TAMP_STAMP_IRQHandler(void) {}
__weak void RTC_WKUP_IRQHandler(void) {}
__weak void FLASH_IRQHandler(void) {}
__weak void RCC_IRQHandler(void) {}
__weak void EXTI0_IRQHandler(void) {}
__weak void EXTI1_IRQHandler(void) {}
__weak void EXTI2_IRQHandler(void) {}
__weak void EXTI3_IRQHandler(void) {}
__weak void EXTI4_IRQHandler(void) {}
__weak void DMA1_Stream3_IRQHandler(void) {}
__weak void DMA1_Stream4_IRQHandler(void) {}
__weak void DMA1_Stream5_IRQHandler(void) {}
__weak void DMA1_Stream6_IRQHandler(void) {}
__weak void ADC_IRQHandler(void) {}
__weak void FDCAN1_IT0_IRQHandler(void) {}
__weak void FDCAN2_IT0_IRQHandler(void) {}
__weak void FDCAN1_IT1_IRQHandler(void) {}
__weak void FDCAN2_IT1_IRQHandler(void) {}
__weak void EXTI9_5_IRQHandler(void) {}
__weak void TIM1_BRK_IRQHandler(void) {}
__weak void TIM1_UP_IRQHandler(void) {}
__weak void TIM1_TRG_COM_IRQHandler(void) {}
__weak void TIM1_CC_IRQHandler(void) {}
__weak void TIM3_IRQHandler(void) {}
__weak void TIM4_IRQHandler(void) {}
__weak void I2C1_EV_IRQHandler(void) {}
__weak void I2C1_ER_IRQHandler(void) {}
__weak void I2C2_EV_IRQHandler(void) {}
__weak void I2C2_ER_IRQHandler(void) {}
__weak void SPI1_IRQHandler(void) {}
__weak void SPI2_IRQHandler(void) {}
__weak void USART1_IRQHandler(void) {}
__weak void USART2_IRQHandler(void) {}
__weak void USART3_IRQHandler(void) {}
__weak void EXTI15_10_IRQHandler(void) {}
__weak void RTC_Alarm_IRQHandler(void) {}

// 只保留需要的中断处理函数
void DMA1_Stream0_IRQHandler(void)
{
    // 空实现或基本处理
}

void DMA1_Stream1_IRQHandler(void)
{
    // 空实现或基本处理
}

void DMA1_Stream2_IRQHandler(void)
{
    // 空实现或基本处理
}

void TIM2_IRQHandler(void)
{
    // 空实现或基本处理
} 