#ifndef _LEDS_MANAGER_H_
#define _LEDS_MANAGER_H_

#include "pwm-ws2812b.h"
#include "enums.hpp"
#include "types.hpp"
#include "constant.hpp"
#include "config.hpp"
#include "gradient_color.hpp"
#include "adc_btns_manager.hpp"

class LEDsManager {
    public:
        LEDsManager(LEDsManager const&) = delete;
        void operator=(LEDsManager const&) = delete;
        static LEDsManager& getInstance() {
            static LEDsManager instance;
            return instance;
        }

        void setup();
        void deinit();
        void runAnimate();
    private:
        LEDsManager();
        uint32_t t;
        GradientColor gtc;
        GamepadOptions& opts;
};

#endif // _LEDS_MANAGER_H_