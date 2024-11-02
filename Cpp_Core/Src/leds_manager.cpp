#include "leds_manager.hpp"
#include "storagemanager.hpp"
#include "config.hpp"
#include "adc_btns_manager.hpp"
#include "utils.h"
#include <stdio.h>

void LEDsManager::setup()
{
    opts = Storage::getInstance().getGamepadOptions();

    color1 = hexToRGB(opts.ledColor1);        //front color
    color2 = hexToRGB(opts.ledColor2);        //background color  1
    color3 = hexToRGB(opts.ledColor3);        //background color  2
    brightness = (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO);


    if(WS2812B_Start() != WS2812B_StateTypeDef::WS2812B_RUNNING) {
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

inline void __attribute__((always_inline)) LEDsManager::process(const Mask_t virtualPinMask)
{
    if(opts.ledEnabled == false) return;

    if(t == 0) {
        t = HAL_GetTick();
    }

    uint32_t tt = HAL_GetTick();
    if(tt - t >= LEDS_ANIMATION_STEP) {
        struct RGBColor currentColor = gtc.getCurrentRGB();
        uint8_t currentBrightness = gtc.getCurrentBrightness();
        Mask_t mask = ADCBtnsManager::getInstance().getButtonIsPressed();
        WS2812B_SetLEDColorByMask(color1, currentColor, (uint32_t) mask);
        WS2812B_SetAllLEDBrightness(currentBrightness);
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
    
    switch(opts.ledEffect) {
        case LEDEffect::BREATHE:
            gtc.setup(hexToRGB(color2), hexToRGB(color3), 
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                0,
                LEDS_ANIMATION_CYCLE);
            break;
        default:
            gtc.setup(hexToRGB(color2), hexToRGB(color3), 
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                LEDS_ANIMATION_CYCLE);
            break;
    }
}

void LEDsManager::setBrightness(const uint8_t brightness)
{
    if(brightness == opts.ledBrightness) return;

    opts.ledBrightness = brightness;
    
    switch(opts.ledEffect) {
        case LEDEffect::BREATHE:
            gtc.setup(color2, color3, 
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                0,
                LEDS_ANIMATION_CYCLE);
            break;
        default:
            gtc.setup(color2, color3, 
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                (uint8_t)round(opts.ledBrightness * LEDS_BRIGHTNESS_RADIO),
                LEDS_ANIMATION_CYCLE);
            break;
    }
}
