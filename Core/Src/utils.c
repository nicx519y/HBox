#include "utils.h"
#include <stdio.h>

uint16_t uint8ToUint16(uint8_t* n)
{
    return n[0] << 8 | n[1];
}

uint32_t uint8ToUint32(uint8_t * n)
{
    return n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
}

uint64_t uint8ToUint64(uint8_t * n)
{
    return n[0] << 56 | n[1] << 48 | n[2] << 40 | n[3] << 32 | n[4] << 24 | n[5] << 16 | n[6] << 8 | n[7];
}

void uint64ToUint8Array(uint8_t* r, uint64_t n)
{
    for(uint8_t i = 0; i < 8; i ++) {
        r[i] = n >> (8 * (8 - i - 1));
    }
}

void uint32ToUint8Array(uint8_t* r, uint32_t n)
{
    for(uint8_t i = 0; i < 4; i ++) {
        r[i] = n >> (8 * (4 - i - 1));
    }
}


double_t max(double a, double b)
{
    if(a < b) {
        return b;
    } else {
        return a;
    }
}

double_t min(double a, double b)
{
    if(a > b) {
        return b;
    } else {
        return a;
    }
}

double_t range(double x, double start, double end)
{
    if(start <= end) {
        if(x <= start) {
            return start;
        }
        if(x >= end) {
            return end;
        }
        return x;
    } else {
        if(x >= start) {
            return start;
        }
        if(x <= end) {
            return end;
        }
        return x;
    } 
}

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