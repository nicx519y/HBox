#include "utils.h"
#include <stdio.h>
#include <math.h>

uint32_t RGBToHex(uint8_t red, uint8_t green, uint8_t blue)
{
  return ((green & 0xff) << 16 | (red & 0xff) << 8 | (blue & 0xff));
}

struct RGBColor hexToRGB(uint32_t color)
{
    struct RGBColor c = {
        .r = (uint8_t) (color >> 16 & 0xff),
        .g = (uint8_t) (color >> 8 & 0xff),
        .b = (uint8_t) (color & 0xff),
    };
    return c;
}


// 定义常量
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#define MU_0 (4 * M_PI * 1e-7) // 真空磁导率

// 定义方程组
void equations(double vars[], double B1, double B2, double L, double R, double d, double eqs[]) {
    double M = vars[0];
    double z1 = vars[1];

    double term1_1 = (L / 2 + z1) / sqrt(R * R + (L / 2 + z1) * (L / 2 + z1));
    double term1_2 = (L / 2 - z1) / sqrt(R * R + (L / 2 - z1) * (L / 2 - z1));
    double term2_1 = (L / 2 + (z1 + d)) / sqrt(R * R + (L / 2 + (z1 + d)) * (L / 2 + (z1 + d)));
    double term2_2 = (L / 2 - (z1 + d)) / sqrt(R * R + (L / 2 - (z1 + d)) * (L / 2 - (z1 + d)));

    eqs[0] = B1 - (MU_0 / 2) * M * (term1_1 - term1_2);
    eqs[1] = B2 - (MU_0 / 2) * M * (term2_1 - term2_2);
}

// 牛顿-拉夫森法求解方程组
void newton_raphson(double vars[], double B1, double B2, double L, double R, double d) {
    double tol = 1e-6; // 误差容限
    int max_iter = 100; // 最大迭代次数
    double eqs[2];
    double J[2][2]; // 雅可比矩阵
    double delta[2];
    int iter;

    for (iter = 0; iter < max_iter; iter++) {
        equations(vars, B1, B2, L, R, d, eqs);

        // 计算雅可比矩阵
        double h = 1e-6; // 微小增量
        double vars_temp[2];

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                vars_temp[j] = vars[j];
            }
            vars_temp[i] += h;
            double eqs_temp[2];
            equations(vars_temp, B1, B2, L, R, d, eqs_temp);
            J[0][i] = (eqs_temp[0] - eqs[0]) / h;
            J[1][i] = (eqs_temp[1] - eqs[1]) / h;
        }

        // 计算 delta = J^(-1) * eqs
        double det = J[0][0] * J[1][1] - J[0][1] * J[1][0];
        delta[0] = (J[1][1] * eqs[0] - J[0][1] * eqs[1]) / det;
        delta[1] = (-J[1][0] * eqs[0] + J[0][0] * eqs[1]) / det;

        // 更新变量
        vars[0] -= delta[0];
        vars[1] -= delta[1];

        // 检查收敛
        if (fabs(delta[0]) < tol && fabs(delta[1]) < tol) {
            break;
        }
    }
}

// 计算轴线上磁场强度的函数
double calculate_axial_magnetic_field(double L, double R, double M, double z) {
    double term1 = (L / 2 + z) / sqrt(R * R + (L / 2 + z) * (L / 2 + z));
    double term2 = (L / 2 - z) / sqrt(R * R + (L / 2 - z) * (L / 2 - z));
    double B_z = (MU_0 / 2) * M * (term1 - term2);
    return B_z;
}

#define TOLERANCE 10 // 误差容忍度

// 使用二分法反向求解轴线上距离
double find_distance_for_axial_field(double L, double R, double M, double B_target) {
    double low = 0.0;
    double high = 10.0; // 假设一个较大的初始上限
    double mid;

    while (high - low > TOLERANCE) {
        mid = (low + high) / 2.0;
        double B_mid = calculate_axial_magnetic_field(L, R, M, mid);

        if (B_mid > B_target) {
            low = mid;
        } else {
            high = mid;
        }
    }

    return (low + high) / 2.0;
}

/******************************** hack 解决 未定义的符号__aeabi_assert 报错 begin ******************************************/

__attribute__((weak, noreturn))
void __aeabi_assert(const char *expr, const char *file, int line)
{
    char str[12], *p;

    fputs("*** assertion failed: ", stderr);
    fputs(expr, stderr);
    fputs(", file ", stderr);
    fputs(file, stderr);
    fputs(", line ", stderr);

    p = str + sizeof(str);
    *--p = '\0';
    *--p = '\n';
    while (line > 0)
    {
        *--p = '0' + (line % 10);
        line /= 10;
    }
    fputs(p, stderr);

    abort();
}

__attribute__((weak))
void abort(void)
{
    for (;;)
        ;
}
/******************************** hack 解决 未定义的符号__aeabi_assert 报错 end ********************************************/