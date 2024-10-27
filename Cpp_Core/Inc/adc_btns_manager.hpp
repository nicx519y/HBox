#ifndef _ADC_BTNS_MANAGER_H_
#define _ADC_BTNS_MANAGER_H_

#include "stm32h7xx.h"
#include "types.hpp"
#include "config.hpp"
#include "enums.hpp"

/**
 * @brief ADC按钮管理
 * 初始化                         setup()
 * 停止并且清空数据                deinit()
 * 循环处理                       process() 
 * 切换状态(工作状态和标定状态)     setState(const ADCButtonManagerState state)
 * 获取哪些按钮按下                getButtonIsPressed()
 * 获取按钮状态（NOT_READY、READY、CALIBRATING_TOP、CALIBRATING_BOTTOM）    getButtonStates()
 */

class ADCBtnsManager {
    public:
        ADCBtnsManager(ADCBtnsManager const&) = delete;
        void operator=(ADCBtnsManager const&) = delete;
        static ADCBtnsManager& getInstance() {
            static ADCBtnsManager instance;
            return instance;
        }

        void setup();
        void deinit();
        void process();
        void setState(const ADCButtonManagerState state);
        ADCButtonManagerState getState();
        ADCButtonState* getButtonStates();
        Mask_t getButtonIsPressed();
    private:

        __attribute__((section("._ADC_DMA_Area"))) static uint32_t ADC_Values[NUM_ADC_BUTTONS];
        __attribute__((section("._ADC_DMA_Area"))) static ADCButtonState ADC_btnStates[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_topValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_bottomValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_pressAccValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_releaseAccValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_topDeadZoneValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_bottomDeadZoneValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_virtualPinMasks[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static int32_t ADC_lastTriggerValues[NUM_ADC_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static uint8_t ADC_lastActionValues[NUM_ADC_BUTTONS];
        __attribute__((section("._ADC_DMA_Area"))) static uint8_t ADC_timesOfCalibrate[NUM_ADC_BUTTONS];
        __attribute__((section("._ADC_DMA_Area"))) static int32_t ADC_tmp[NUM_ADC_BUTTONS];
        Mask_t virtualPinMask = 0x0;

        ADCBtnsManager();
        void btnsConfigSave();
        ADCButtonManagerState state = ADCButtonManagerState::WORKING;
        ADCButton* (&btns)[NUM_ADC_BUTTONS];
        
};

#endif //  _ADC_BTNS_MANAGER_H_