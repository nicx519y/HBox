#include "pwm-ws2812b.h"
#include "tim.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "constant.hpp"

/* WS2812B data protocol
|-------------------------------------------|
|T0H | 0.4us      | +-150ns
|T1H | 0.8us      | +-150ns
|T0L | 0.85us     | +-150ns
|T1L | 0.45us     | +-150ns
|RES | above 50us |
|-------------------------------------------
*/

#define NUM_LED	NUM_ADC_BUTTONS

#define DMA_BUFFER_LEN (((NUM_LED % 2 == 0) ? (NUM_LED + 4) : (NUM_LED + 5)) * 24) //RES >= 4 * 24 * 300 * 1/240 = 120us

#define HIGH_CCR_CODE 183 // 1/240 * 183 = 0.76us; 1/240 * (300 - 183) = 0.49us;

#define LOW_CCR_CODE 83 // 1/240 * 83 = 0.35us; 1/240 * (300 - 83) = 0.90us;

#define LED_DEFAULT_BRIGHTNESS 128

static WS2812B_StateTypeDef WS2812B_State = WS2812B_STOP;

static uint8_t LED_Colors[NUM_LED * 3];

static uint8_t LED_Brightness[NUM_LED];

__RAM_Area__ static uint32_t DMA_LED_Buffer[DMA_BUFFER_LEN];


void LEDDataToDMABuffer(const uint16_t start, const uint16_t length)
{
	if(start + length > NUM_LED) {
		return;
	}

	uint16_t i, j;
	uint16_t len = (start + length) * 3;
	
	for(j = start * 3; j < len; j += 3)
	{
		double_t brightness = (double_t)LED_Brightness[j / 3] / 255.0;
		uint32_t color = RGBToHex((uint8_t)round(LED_Colors[j] * brightness), (uint8_t)round(LED_Colors[j + 1] * brightness), (uint8_t)round(LED_Colors[j + 2] * brightness));
		for(i = 0; i < 24; i ++) {
			if(0x800000 & (color << i)) {
				DMA_LED_Buffer[j * 8 + i] = HIGH_CCR_CODE;
			} else {
				DMA_LED_Buffer[j * 8 + i] = LOW_CCR_CODE;
			}
		}
	}

	SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)DMA_LED_Buffer, sizeof(DMA_LED_Buffer));
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	// printf("PWM-WS2812B-PulseFinished...\r\n");

	uint16_t start = DMA_BUFFER_LEN / 2 / 24;

	uint16_t length = NUM_LED - start;

	if(length > 0) {
		LEDDataToDMABuffer(start, length);
	}

	// HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
	// printf("PWM-WS2812B-PulseFinishedHalfCplt...\r\n");

	uint16_t length = DMA_BUFFER_LEN / 2 / 24;

	(length < NUM_LED) ? LEDDataToDMABuffer(0, length): LEDDataToDMABuffer(0, NUM_LED);

}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
	printf("PWM-WS2812B-ErrorCallback...\r\n");
}



void WS2812B_Init(void)
{
	memset(&DMA_LED_Buffer, 0, sizeof(DMA_LED_Buffer)); 
	memset(&LED_Brightness, LED_DEFAULT_BRIGHTNESS, sizeof(LED_Brightness));

	LEDDataToDMABuffer(0, NUM_LED);

	if(HAL_TIM_Base_GetState(&htim4) != HAL_TIM_STATE_READY) {
		printf("WS2812B_Init..\n");
		MX_TIM4_Init();
	}
}

WS2812B_StateTypeDef WS2812B_Start()
{
	if(WS2812B_State != WS2812B_STOP) {
		return WS2812B_State;
	}

	HAL_StatusTypeDef state = HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t *)&DMA_LED_Buffer, DMA_BUFFER_LEN);

	if(state == HAL_OK) {
		WS2812B_State = WS2812B_RUNNING;
		printf("WS2812B_Start success\n");
	} else {
		WS2812B_State = WS2812B_ERROR;
		printf("WS2812B_Start failure\n");
	}
	return WS2812B_State;
}

WS2812B_StateTypeDef WS2812B_Stop()
{
    if(WS2812B_State != WS2812B_RUNNING) {
        return WS2812B_State;
    }

    HAL_StatusTypeDef state = HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);

    if(state == HAL_OK) {
		WS2812B_State = WS2812B_STOP;
    } else {
		WS2812B_State = WS2812B_ERROR;
    }
    return WS2812B_State;
}

void WS2812B_SetAllLEDBrightness(const uint8_t brightness)
{
    memset(&LED_Brightness, brightness, sizeof(LED_Brightness));
	SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)LED_Brightness, sizeof(LED_Brightness));
}

void WS2812B_SetAllLEDColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    int length = NUM_LED * 3;
    for(int i = 0; i < length; i += 3) {
        LED_Colors[i] = r;
        LED_Colors[i + 1] = g;
        LED_Colors[i + 2] = b;
    }
	SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)LED_Colors, sizeof(LED_Colors));
}

void WS2812B_SetLEDBrightness(const uint8_t brightness, const uint16_t index)
{
	if(index >= 0 && index < NUM_LED) {
		LED_Brightness[index] = brightness;
		SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)LED_Brightness[index], sizeof(LED_Brightness[0]));
	}
}

void WS2812B_SetLEDColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint16_t index)
{
	if(index >= 0 && index < NUM_LED) {
		uint16_t idx = index * 3;
		LED_Colors[idx] = r;
		LED_Colors[idx + 1] = g;
		LED_Colors[idx + 2] = b;
		SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)LED_Colors[idx], sizeof(LED_Colors[0]) * 3);
	}
}

void WS2812B_SetLEDBrightnessByMask(
  const uint8_t fontBrightness,
  const uint8_t backgroundBrightness,
  const uint32_t mask
)
{
	uint8_t len = NUM_LED > 32 ? 32 : NUM_LED;

	for(uint8_t i = 0; i < len; i ++) {
		if(mask >> i & 1 == 1) {
			LED_Brightness[i] = fontBrightness;
		} else {
			LED_Brightness[i] = backgroundBrightness;
		}
	}

	SCB_CleanInvalidateDCache_by_Addr ((uint8_t *)LED_Brightness, sizeof(LED_Brightness));
}

/**
 * @brief 根据mask设置frontColor和backgroundColor
 * example: mask = 100100010000100 
 * 从右侧开始 0是backgroundColor, 1是frontColor
 * mask 是一个 32位整型，也就是说 led 总数不能超过32个
 * @param frontColor 
 * @param backgroundColor 
 * @param mask 
 */
void WS2812B_SetLEDColorByMask(
	const struct RGBColor frontColor, 
    const struct RGBColor backgroundColor, 
	const uint32_t mask)
{

	uint8_t len = NUM_LED > 32 ? 32 : NUM_LED;
	uint16_t idx;

	for(uint8_t i = 0; i < len; i ++) {
		idx = i * 3;
		if(mask >> i & 1 == 1) {
			LED_Colors[idx] = frontColor.r;
			LED_Colors[idx + 1] = frontColor.g;
			LED_Colors[idx + 2] = frontColor.b;
		} else {
			LED_Colors[idx] = backgroundColor.r;
			LED_Colors[idx + 1] = backgroundColor.g;
			LED_Colors[idx + 2] = backgroundColor.b;
		}
	}

	SCB_CleanInvalidateDCache_by_Addr ((uint32_t *)LED_Colors, sizeof(LED_Colors));
}

WS2812B_StateTypeDef WS2812B_GetState()
{
	return WS2812B_State;
}



