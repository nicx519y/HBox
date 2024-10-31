#ifndef __WS2812B_H__
#define __WS2812B_H__

#include "stm32h7xx_hal.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  WS2812B Status structures definition
  */
typedef enum
{
  WS2812B_STOP         = 0x00,
  WS2812B_RUNNING      = 0x01,
  WS2812B_ERROR        = 0x02
} WS2812B_StateTypeDef;

void WS2812B_Init(void);

void WS2812B_SetAllLEDBrightness(const uint8_t brightness);

void WS2812B_SetAllLEDColor(const uint8_t r, const uint8_t g, const uint8_t b);

void WS2812B_SetLEDBrightness(const uint8_t brightness, const uint16_t index);

void WS2812B_SetLEDColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint16_t index);

void WS2812B_SetLEDColorByMask(
    const struct RGBColor frontColor, 
    const struct RGBColor backgroundColor, 
    const uint32_t mask);

WS2812B_StateTypeDef WS2812B_Start();

WS2812B_StateTypeDef WS2812B_Stop();

WS2812B_StateTypeDef WS2812B_GetState();

#ifdef __cplusplus
}
#endif

#endif /* __WS2812B_H__ */
