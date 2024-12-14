#ifndef _GPIO_BTNS_MANAGER_H_
#define _GPIO_BTNS_MANAGER_H_

#include "stm32h7xx.h"
#include "types.hpp"
#include "config.hpp"
#include "enums.hpp"
#include "constant.hpp"

class GPIOBtnsManager {
    public:
        GPIOBtnsManager(GPIOBtnsManager const&) = delete;
        void operator=(GPIOBtnsManager const&) = delete;
        static GPIOBtnsManager& getInstance() {
            static GPIOBtnsManager instance;
            return instance;
        }
        
        void setup();
        void read();
        inline Mask_t getButtonIsPressed() { return this->virtualPinMask; }

    private:
        GPIOBtnsManager();
        Mask_t virtualPinMask = 0x0;
        GPIOButton* (&btns)[NUM_GPIO_BUTTONS];
        static inline GPIOButton* (&getButtonPtrs())[NUM_GPIO_BUTTONS] { return buttonPtrs; }
        static GPIOButton* buttonPtrs[NUM_GPIO_BUTTONS];
};

#endif // _GPIO_BTNS_MANAGER_H_
