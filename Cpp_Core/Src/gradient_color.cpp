#include "gradient_color.hpp"
#include "utils.h"

GradientColor::GradientColor()
{}

void GradientColor::setup(
    const struct RGBColor color1, 
    const struct RGBColor color2, 
    const uint8_t brightness1,
    const uint8_t brightness2, 
    const uint32_t cycle
)
{
        cr = (double_t)color1.r;
        cg = (double_t)color1.g;
        cb = (double_t)color1.b;
        er = (double_t)color2.r;
        eg = (double_t)color2.g;
        eb = (double_t)color2.b;
        b1 = (double_t)brightness1;
        b2 = (double_t)brightness2;
        animationCycle = cycle;
}

inline void GradientColor::process()
{
    if(t == 0 || HAL_GetTick() - t >= 10 * animationCycle) {
        t = HAL_GetTick();
    }
}

struct RGBColor GradientColor::getCurrentRGB()
{
    uint32_t dt = HAL_GetTick() - t;
    double_t radio = (double_t) (dt % animationCycle) / (double_t) animationCycle;
    
    struct RGBColor color;

    if(radio < 1.0 && radio >= 0.5) {
        radio = 1.0 - radio;
    }

    color = {
        .r = (uint8_t)round(cr + (er - cr) * radio * 2),
        .g = (uint8_t)round(cg + (eg - cg) * radio * 2),
        .b = (uint8_t)round(cg + (eb - cb) * radio * 2),
    };
    
    return color;
}

uint8_t GradientColor::getCurrentBrightness()
{
    uint32_t dt = HAL_GetTick() - t;
    double_t radio = (double_t) (dt % animationCycle) / (double_t) animationCycle;
    
    if(radio < 1.0 && radio >= 0.5) {
        radio = 1.0 - radio;
    }

    return (uint8_t) round(b1 + (b2 - b1) * radio * 2);
}

