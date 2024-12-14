#include "constant.hpp"
#include "gpio_btns_manager.hpp"
#include "storagemanager.hpp"
#include "gpio-btn.h"

__attribute__((section("._DTCMRAM_Area"))) static uint8_t GPIO_lastActionValues[NUM_GPIO_BUTTONS];
__attribute__((section("._DTCMRAM_Area"))) static uint32_t GPIO_debounce_t[NUM_GPIO_BUTTONS];

// 定义静态成员
GPIOButton* GPIOBtnsManager::buttonPtrs[NUM_GPIO_BUTTONS];

GPIOBtnsManager::GPIOBtnsManager() : btns(getButtonPtrs()) {
    for(uint8_t i = 0; i < NUM_GPIO_BUTTONS; i++) {
        buttonPtrs[i] = &Storage::getInstance().config.GPIOButtons[i];
    }
}

void GPIOBtnsManager::setup()
{
    memset(&GPIO_lastActionValues[0], 0, sizeof(GPIO_lastActionValues));
    memset(&GPIO_debounce_t[0], 0, sizeof(GPIO_debounce_t));
    this->virtualPinMask = 0x0;
}

void GPIOBtnsManager::read()
{
    for(uint8_t i = 0; i < NUM_GPIO_BUTTONS; i ++) {
        uint8_t r = GPIO_Btn_IsPressed(i);
        if(GPIO_lastActionValues[i] != r) {
            if(GPIO_debounce_t[i] == 0) {
                GPIO_debounce_t[i] = HAL_GetTick();
            } else if(HAL_GetTick() - GPIO_debounce_t[i] >= GPIO_BUTTONS_DEBOUNCE) {   //触发
                GPIO_lastActionValues[i] = r;
                (r == 1) ? this->virtualPinMask |= btns[i]->virtualPin : this->virtualPinMask &= ~ btns[i]->virtualPin;
            }
        } else {
            GPIO_debounce_t[i] = 0;
        }
    }

}

