#ifndef _GRADIENT_COLOR_
#define _GRADIENT_COLOR_

#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"

class GradientColor {
    public:
        GradientColor();
        void setup(
            const struct RGBColor color1, 
            const struct RGBColor color2, 
            const uint8_t brightness1,
            const uint8_t brightness2, 
            const uint32_t duration
        );
        inline void process();
        struct RGBColor getCurrentRGB();
        uint8_t getCurrentBrightness();
    private:
        double_t cr;
        double_t cg;
        double_t cb;
        double_t er;
        double_t eg;
        double_t eb;
        double_t b1;
        double_t b2;
        uint32_t t;
        uint32_t animationCycle; // 动画周期
};

#endif // !_GRADIENT_COLOR_
