#ifndef _GPIO_BTNS_MANAGER_
#define _GPIO_BTNS_MANAGER_

#include "types.hpp"
#include "config.hpp"

class GPIOBtnsManager {
    public:
        GPIOBtnsManager(GPIOBtnsManager const&) = delete;
        void operator=(GPIOBtnsManager const&) = delete;
        static GPIOBtnsManager& getInstance() {
            static GPIOBtnsManager instance;
            return instance;
        }
        
        void setup();
        Mask_t read();
    private:

        Mask_t virtualPinMask = 0x0;
        GPIOButton* (&btns)[NUM_GPIO_BUTTONS];
        GPIOBtnsManager();

};

#endif // _GPIO_BTNS_MANAGER_
