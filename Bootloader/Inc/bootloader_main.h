#ifndef __BOOTLOADER_MAIN_H
#define __BOOTLOADER_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

/* 导出变量 */
extern UART_HandleTypeDef huart1;
extern QSPI_HandleTypeDef hqspi;

/* 函数声明 */
void Error_Handler(void);
void SystemClock_Config(void);
void MX_QUADSPI_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOOTLOADER_MAIN_H */ 