#include "gpio_btns_manager.hpp"
#include "storagemanager.hpp"
#include "constant.hpp"
#include "gpio-btn.h"

GPIOBtnsManager::GPIOBtnsManager():
    btns(Storage::getInstance().getGPIOButtonOptions())
{}

void GPIOBtnsManager::setup()
{
    
}

void GPIOBtnsManager::deinit()
{
    memset(&GPIO_lastActionValues[0], 0, sizeof(GPIO_lastActionValues));
    memset(&GPIO_debounce_t[0], 0, sizeof(GPIO_debounce_t));
    this->virtualPinMask = 0x0;
}

void GPIOBtnsManager::process()
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

