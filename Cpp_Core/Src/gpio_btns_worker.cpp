#include "constant.hpp"
#include "gpio_btns_worker.hpp"
#include "storagemanager.hpp"
#include "gpio-btn.h"
#include "message_center.hpp"



GPIOBtnsWorker::GPIOBtnsWorker() : btns(getButtonPtrs()) {
    for(uint8_t i = 0; i < NUM_GPIO_BUTTONS; i++) {
        buttonPtrs[i] = &Storage::getInstance().config.GPIOButtons[i];
    }
}

void GPIOBtnsWorker::setup()
{
    memset(&GPIO_lastActionValues[0], 1, sizeof(GPIO_lastActionValues)); // 1 表示松开 0 表示按下
    memset(&GPIO_debounce_t[0], 0, sizeof(GPIO_debounce_t));
    this->virtualPinMask = 0x0;
}

void GPIOBtnsWorker::process()
{
    bool changed = false;
    for(uint8_t i = 0; i < NUM_GPIO_BUTTONS; i ++) {
        uint8_t r = GPIO_Btn_IsPressed(i);
        if(GPIO_lastActionValues[i] != r) {
            if(GPIO_debounce_t[i] == 0) {
                GPIO_debounce_t[i] = HAL_GetTick();
            } else if(HAL_GetTick() - GPIO_debounce_t[i] >= GPIO_BUTTONS_DEBOUNCE) {   //触发
                GPIO_lastActionValues[i] = r;
                (r == 0) ? this->virtualPinMask |= btns[i]->virtualPin : this->virtualPinMask &= ~ btns[i]->virtualPin; // 0 表示按下 1 表示松开
                changed = true;
                // 如果状态发生变化，则发送消息
                if(r == 0) {
                    MC.publish(MessageId::GPIO_BTNS_PRESSED, &btns[i]->virtualPin);
                } else {
                    MC.publish(MessageId::GPIO_BTNS_RELEASED, &btns[i]->virtualPin);
                }
            }
        } else {
            GPIO_debounce_t[i] = 0;
        }
    }

    // 如果状态发生变化，则发送消息
    if(changed) {
        MC.publish(MessageId::GPIO_BTNS_STATE_CHANGED, &this->virtualPinMask);
    }

}

