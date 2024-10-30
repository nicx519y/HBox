#include "utils.h"
#include <stdio.h>

double max(double a, double b)
{
    if(a < b) {
        return b;
    } else {
        return a;
    }
}

double min(double a, double b)
{
    if(a > b) {
        return b;
    } else {
        return a;
    }
}

double range(double x, double start, double end)
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