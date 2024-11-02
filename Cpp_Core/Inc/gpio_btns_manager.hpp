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
        void deinit();
        inline void __attribute__((always_inline)) process();
        inline Mask_t __attribute__((always_inline)) getButtonIsPressed();
    private:

        __attribute__((section("._DTCMRAM_Area"))) static uint8_t GPIO_lastActionValues[NUM_GPIO_BUTTONS];
        __attribute__((section("._DTCMRAM_Area"))) static uint32_t GPIO_debounce_t[NUM_GPIO_BUTTONS];

        Mask_t virtualPinMask = 0x0;
        inline bool __attribute__((always_inline)) readBtn(uint8_t idx);
        GPIOButton* (&btns)[NUM_GPIO_BUTTONS];
        GPIOBtnsManager();

};

#endif // _GPIO_BTNS_MANAGER_
