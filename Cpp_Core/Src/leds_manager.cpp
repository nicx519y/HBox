#include "leds_manager.hpp"
#include "storagemanager.hpp"
#include "config.hpp"
#include "utils.h"
#include <stdio.h>

LEDsManager::LEDsManager():
    opts(Storage::getInstance().getGamepadOptions())
{};

void LEDsManager::setup()
{
    // opts = Storage::getInstance().getGamepadOptions();

    struct RGBColor color1 = hexToRGB(opts.ledColor1);        //front color
    struct RGBColor color2 = hexToRGB(opts.ledColor2);        //background color  1
    struct RGBColor color3 = hexToRGB(opts.ledColor3);        //background color  2
    uint8_t brightness = (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO);


    if(WS2812B_GetState() == WS2812B_StateTypeDef::WS2812B_STOP 
        && WS2812B_Start() != WS2812B_StateTypeDef::WS2812B_RUNNING) {
        printf("LEDsManager::setup WS2812B_Start failure.\n");
    } else {
        switch(opts.ledEffect) {
            case LEDEffect::BREATHE:
                WS2812B_SetAllLEDBrightness(brightness);
                WS2812B_SetAllLEDColor(color2.r, color2.g, color2.b);
                gtc.setup(color2, color3, brightness, 0, LEDS_ANIMATION_CYCLE);
                break;
            default:    //STATIC
                WS2812B_SetAllLEDBrightness(brightness);
                WS2812B_SetAllLEDColor(color2.r, color2.g, color2.b);
                gtc.setup(color2, color3, brightness, brightness, LEDS_ANIMATION_CYCLE);
                break;
        }
    }
}

void LEDsManager::deinit()
{
    WS2812B_SetAllLEDBrightness(0);
    WS2812B_Stop();
}

void LEDsManager::process(const Mask_t virtualPinMask)
{
    if(opts.ledEnabled == false) return;

    if(t == 0) {
        t = HAL_GetTick();
    }

    uint32_t tt = HAL_GetTick();
    if(tt - t >= LEDS_ANIMATION_STEP) {
        struct RGBColor fontColor = hexToRGB(opts.ledColor1);
        struct RGBColor currentColor = gtc.getCurrentRGB();
        uint8_t currentBrightness = gtc.getCurrentBrightness();
        // printf("currentColor: %d, %d, %d\n", currentColor.r, currentColor.g, currentColor.b);
        WS2812B_SetLEDColorByMask(fontColor, currentColor, (uint32_t) virtualPinMask);
        WS2812B_SetLEDBrightnessByMask((uint8_t)round((double_t)opts.ledBrightness*LEDS_BRIGHTNESS_RADIO), 
            currentBrightness, (uint32_t) virtualPinMask);

        // printf("fontBrightness: %d, currentBrightness: %d\n", (uint8_t)round((double_t)opts.ledBrightness*LEDS_BRIGHTNESS_RADIO), currentBrightness);
        t = tt;
    }
}

void LEDsManager::setEnabled(const bool enabled)
{
    if(enabled == opts.ledEnabled) return;

    opts.ledEnabled = enabled;

    if(opts.ledEnabled == true) {
        setup();
    } else {
        deinit();
    }
}

void LEDsManager::setLedEffect(const LEDEffect effect)
{
    if(opts.ledEffect == effect) return;
    
    opts.ledEffect = effect;

    deinit();
    setup();
}

void LEDsManager::setColors(uint32_t color1, uint32_t color2, uint32_t color3)
{
    opts.ledColor1 = color1;
    opts.ledColor2 = color2;
    opts.ledColor3 = color3;
    
    // switch(opts.ledEffect) {
    //     case LEDEffect::BREATHE:
    //         gtc.setup(hexToRGB(color2), hexToRGB(color3), 
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             0,
    //             LEDS_ANIMATION_CYCLE);
    //         break;
    //     default:
    //         gtc.setup(hexToRGB(color2), hexToRGB(color3), 
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             LEDS_ANIMATION_CYCLE);
    //         break;
    // }
}

void LEDsManager::setBrightness(const uint8_t brightness)
{
    if(brightness == opts.ledBrightness) return;

    opts.ledBrightness = brightness;
    
    // switch(opts.ledEffect) {
    //     case LEDEffect::BREATHE:
    //         gtc.setup(hexToRGB(opts.ledColor2), hexToRGB(opts.ledColor3), 
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             0,
    //             LEDS_ANIMATION_CYCLE);
    //         break;
    //     default:
    //         gtc.setup(hexToRGB(opts.ledColor2), hexToRGB(opts.ledColor3), 
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
    //             LEDS_ANIMATION_CYCLE);
    //         break;
    // }
}
