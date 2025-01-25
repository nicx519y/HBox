#ifndef GPIO_BTNS_WORKER_HPP
#define GPIO_BTNS_WORKER_HPP

#include "stm32h7xx.h"
#include "types.hpp"
#include "config.hpp"
#include "enums.hpp"
#include "constant.hpp"

class GPIOBtnsWorker {
    public:
        GPIOBtnsWorker(GPIOBtnsWorker const&) = delete;
        void operator=(GPIOBtnsWorker const&) = delete;
        static GPIOBtnsWorker& getInstance() {
            static GPIOBtnsWorker instance;
            return instance;
        }
            
        void setup();
        void process();
        inline Mask_t getButtonIsPressed() { return this->virtualPinMask; }

    private:
        GPIOBtnsWorker();

        static __attribute__((section("._DTCMRAM_Area"))) uint8_t GPIO_lastActionValues[NUM_GPIO_BUTTONS];
        static __attribute__((section("._DTCMRAM_Area"))) uint32_t GPIO_debounce_t[NUM_GPIO_BUTTONS];

        Mask_t virtualPinMask = 0x0;
        GPIOButton* (&btns)[NUM_GPIO_BUTTONS];
        static inline GPIOButton* (&getButtonPtrs())[NUM_GPIO_BUTTONS] { return buttonPtrs; }
        static GPIOButton* buttonPtrs[NUM_GPIO_BUTTONS];
};

#define GPIO_BTNS_WORKER GPIOBtnsWorker::getInstance()

#endif // GPIO_BTNS_WORKER_HPP
