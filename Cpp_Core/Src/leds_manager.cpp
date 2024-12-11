#include "leds_manager.hpp"
#include "storagemanager.hpp"
#include "config.hpp"
#include "utils.h"
#include <stdio.h>

LEDsManager::LEDsManager():
    opts((&Storage::getInstance().config)->profiles[Storage::getInstance().config.profileIndex])
{};

void LEDsManager::setup()
{
    // opts = Storage::getInstance().getGamepadOptions();
    if(opts->ledEnabled == false) return;

    ADCButtonManagerState state = ADCBtnsManager::getInstance().getState();

    if(WS2812B_GetState() == WS2812B_StateTypeDef::WS2812B_STOP 
        && WS2812B_Start() != WS2812B_StateTypeDef::WS2812B_RUNNING) {

        printf("LEDsManager::setup WS2812B_Start failure.\n");

    } else if(state == ADCButtonManagerState::WORKING) {
        
        struct RGBColor color2 = hexToRGB(opts->ledColor2);        //background color  1
        struct RGBColor color3 = hexToRGB(opts->ledColor3);        //background color  2
        uint8_t brightness = (uint8_t)round(opts->ledBrightness * LEDS_BRIGHTNESS_RADIO);

        switch(opts->ledEffect) {
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

    } else {    // ADCButtonManagerState::CALIBRATING
        
        struct RGBColor color = hexToRGB(opts->ledColorCalibrateTop);
        WS2812B_SetAllLEDBrightness(opts->ledBrightnesssCalibrate);
        WS2812B_SetAllLEDColor(color.r, color.g, color.b);

    }
}

void LEDsManager::deinit()
{
    WS2812B_SetAllLEDBrightness(0);
    HAL_Delay(50);
    WS2812B_Stop();
}

void LEDsManager::runAnimate()
{
    if(opts->ledEnabled == false) return;

    if(t == 0) t = HAL_GetTick();

    uint32_t tt = HAL_GetTick();
    if(tt - t < LEDS_ANIMATION_STEP) return;

    t = tt;

    ADCButtonManagerState state = ADCBtnsManager::getInstance().getState();

    if(state == ADCButtonManagerState::WORKING) {
        
        Mask_t virtualPinMask = ADCBtnsManager::getInstance().getButtonIsPressed();

        struct RGBColor frontColor = hexToRGB(opts->ledColor1);
        struct RGBColor currentColor = gtc.getCurrentRGB();
        uint8_t currentBrightness = gtc.getCurrentBrightness();
        WS2812B_SetLEDColorByMask(frontColor, currentColor, (uint32_t) virtualPinMask);
        WS2812B_SetLEDBrightnessByMask((uint8_t)round((double_t)opts->ledBrightness*LEDS_BRIGHTNESS_RADIO), 
            currentBrightness, (uint32_t) virtualPinMask);

    } else { // ADCButtonManagerState::CALIBRATING

        ADCButtonState* btnStates = ADCBtnsManager::getInstance().getButtonStates();
        struct RGBColor topColor = hexToRGB(opts->ledColorCalibrateTop);
        struct RGBColor bottomColor = hexToRGB(opts->ledColorCalibrateBottom);
        struct RGBColor completeColor = hexToRGB(opts->ledColorCalibrateComplete);

        for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
            switch(btnStates[i]) {
                case ADCButtonState::NOT_READY:
                case ADCButtonState::CALIBRATING_TOP:
                    WS2812B_SetLEDColor(topColor.r, topColor.g, topColor.b, i);
                    break;
                case ADCButtonState::CALIBRATING_BOTTOM:
                    WS2812B_SetLEDColor(bottomColor.r, bottomColor.g, bottomColor.b, i);
                    break;
                default:    // ADCButtonState::READY
                    WS2812B_SetLEDColor(completeColor.r, completeColor.g, completeColor.b, i);
                    break;
            }
        }

    }
}
