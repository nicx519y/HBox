#ifndef _LEDS_MANAGER_H_
#define _LEDS_MANAGER_H_

#include "pwm-ws2812b.h"
#include "enums.hpp"
#include "types.hpp"
#include "constant.hpp"
#include "config.hpp"
#include "gradient_color.hpp"

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
        void process(const Mask_t virtualPinMask);
        void setEnabled(const bool enabled);
        void setLedEffect(const LEDEffect effect);
        void setColors(uint32_t color1, uint32_t color2, uint32_t color3);
        void setBrightness(const uint8_t brightness);
    private:
        LEDsManager();
        uint32_t t;
        GradientColor gtc;
        GamepadOptions& opts;
};

#endif // _LEDS_MANAGER_H_