#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

double max(double_t a, double_t b);

double min(double_t a, double_t b);

double range(double_t x, double_t start, double_t end);

uint32_t RGBToHex(uint8_t red, uint8_t green, uint8_t blue);

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct RGBColor hexToRGB(uint32_t color);


/******************************** hack 解决 未定义的符号__aeabi_assert 报错 begin ******************************************/
__attribute__((weak, noreturn)) void __aeabi_assert(const char *expr, const char *file, int line);
__attribute__((weak)) void abort(void);
/******************************** hack 解决 未定义的符号__aeabi_assert 报错 end ********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H__ */