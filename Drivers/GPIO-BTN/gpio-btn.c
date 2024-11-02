#include "gpio-btn.h"
#include "stm32h7xx_hal.h"

void GPIO_Btn_Init()
{
    // 时钟已在gpio.c种使能

    GPIO_InitTypeDef GPIO_Init;

    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_Init.Pull = GPIO_PULLDOWN;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);

}

inline uint8_t __attribute__((always_inline)) GPIO_Btn_IsPressed(uint8_t idx)
{
    uint8_t result;
    switch(idx) {
        case 0:
            result = GPIO_Btn_0;
            break;
        case 1:
            result = GPIO_Btn_1;
            break;
        case 2:
            result = GPIO_Btn_2;
            break;
        case 3:
            result = GPIO_Btn_3;
            break;
        default:
            break;
    }

    return result;
}