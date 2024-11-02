#ifndef __GPIO_BTN_H__
#define __GPIO_BTN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define GPIO_Btn_0   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)
#define GPIO_Btn_1   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)
#define GPIO_Btn_2   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)
#define GPIO_Btn_3   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)

void GPIO_Btn_Init(void);
uint8_t GPIO_Btn_IsPressed(uint8_t idx);

#ifdef __cplusplus
}
#endif

#endif // !__GPIO_BTN_H__
