#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"


uint32_t RGBToHex(uint8_t red, uint8_t green, uint8_t blue);

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct RGBColor hexToRGB(uint32_t color);

/**
 * 已知两个测试点的磁场强度以及这两个测试点的距离、磁芯半径、磁芯长度，求解磁化强度和第一个测试点的位置
 * 用牛顿-拉夫森法求解方程组
 * @param vars 磁化强度和第一个测试点的位置  vars[0] = M, vars[1] = z1
 * @param B1 第一个测试点的磁场强度
 * @param B2 第二个测试点的磁场强度
 * @param L 磁芯长度
 * @param R 磁芯半径
 * @param d 两个测试点的距离
 */
void newton_raphson(double vars[], double B1, double B2, double L, double R, double d);

/**
 * 已知磁芯长度、磁芯半径、磁化强度，求解磁芯轴向磁场为某个值时，磁芯轴向距离
 * @param L 磁芯长度
 * @param R 磁芯半径
 * @param M 磁化强度
 * @param B_target 目标磁场
 * @return 磁芯轴向距离
 */
double find_distance_for_axial_field(double L, double R, double M, double B_target);

/******************************** hack 解决 未定义的符号__aeabi_assert 报错 begin ******************************************/
__attribute__((weak, noreturn)) void __aeabi_assert(const char *expr, const char *file, int line);
__attribute__((weak)) void abort(void);
/******************************** hack 解决 未定义的符号__aeabi_assert 报错 end ********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H__ */