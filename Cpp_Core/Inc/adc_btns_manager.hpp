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
        Mask_t read();
        void calibrate();
        void setState(const ADCButtonManagerState state);

        ADCButtonState* getButtonStates();
        inline ADCButtonManagerState getState() { return this->state; }
        inline Mask_t getButtonIsPressed() { return this->virtualPinMask; }
        

    private:
        
        Mask_t virtualPinMask = 0x0;
        uint32_t calibrate_t = 0;

        ADCBtnsManager();
        void btnsConfigSave();
        ADCButtonManagerState state = ADCButtonManagerState::WORKING;
        ADCButton* (&btns)[NUM_ADC_BUTTONS];
        
};

#endif //  _ADC_BTNS_MANAGER_H_