#ifndef _CONSTANT_H_
#define _CONSTANT_H_ 

#include <stdint.h>  // 添加这行来使用 uint8_t, uint32_t 等类型

#ifdef __cplusplus
extern "C" {
#endif

#define FIRMWARE_VERSION                    (uint32_t)0x010000  //固件版本
#define CONFIG_VERSION                      (uint32_t)0x000100  //配置版本 三位版本号 0x aa bb cc
#define ADC_MAPPING_VERSION                 (uint32_t)0x000001  //ADC值映射表版本

#define EX_ADDR                             0x90000000
#define FSDATA_ADDR                         0x0               // flash起始地址 webconfig 网页数据地址
#define ADC_VALUES_MAPPING_ADDR             0x00200000       //ADC值映射表地址
#define CONFIG_ADDR                         0x00400000       //配置数据地址

#define NUM_ADC_VALUES_MAPPING              8             // 最大8个映射 ADC按钮映射表用于值查找
#define MAX_ADC_VALUES_LENGTH               40             // 每个映射最大40个值 ADC按钮映射表用于值查找
#define MAX_NUM_MARKING_VALUE               100            // 每个step最大采集值个数
#define TIME_ADC_INIT                       1000           // ADC初始化时间，时间越长初始化越准确
#define NUM_WINDOW_SIZE                     8             // 校准滑动窗口大小

#define NUM_PROFILES                        16
#define NUM_ADC                             3             // 3个ADC
#define NUM_ADC1_BUTTONS                    6
#define NUM_ADC2_BUTTONS                    6
#define NUM_ADC3_BUTTONS                    5
#define NUM_ADC_BUTTONS                     (NUM_ADC1_BUTTONS + NUM_ADC2_BUTTONS + NUM_ADC3_BUTTONS)

#define ADC_BTNS_WORK_INTERVAL               50            // ADC按钮工作间隔 us

// C 代码块，使用 const
static const uint8_t ADC1_BUFFER_TO_KEY_INDEX[NUM_ADC1_BUTTONS] = {1, 8, 9, 6, 0, 5};
static const uint8_t ADC2_BUFFER_TO_KEY_INDEX[NUM_ADC2_BUTTONS] = {2, 3, 7, 4, 14, 11};
static const uint8_t ADC3_BUFFER_TO_KEY_INDEX[NUM_ADC3_BUTTONS] = {13, 15, 16, 10, 12};


#define TIMES_ADC_CALIBRATION               100             // 单个按钮校准时的循环次数，必须100次连续稳定的值用于校准
#define DELAY_ADC_CALIBRATION               10              // 校准时间 TIMES_ADC_CALIBRATION * DELAY_ADC_CALIBRATION
#define ADC_VOLATILITY                      300             // ADC的浮动允许最大值

#define NUM_GPIO_BUTTONS            4               //GPIO按钮数量
#define GPIO_BUTTONS_DEBOUNCE       5             //去抖动延迟(ms) 

#define LED_CALIBRATE_BRIGHTNESS            100             // 校准亮度
#define LED_CALIBRATE_COLOR_TOP             (uint32_t)0x0000FF  // 校准颜色
#define LED_CALIBRATE_COLOR_BOTTOM          (uint32_t)0x00FF00  // 校准颜色
#define LED_CALIBRATE_COLOR_COMPLETE        (uint32_t)0xFF0000  // 校准颜色

#define HAS_LED                                    //是否有LED
#define NUM_LED	                    (NUM_ADC_BUTTONS + NUM_GPIO_BUTTONS) //LED数量
// #define NUM_LED                             1

#define NUM_LEDs_PER_ADC_BUTTON     1              //每个按钮多少个LED
#define LEDS_BRIGHTNESS_RADIO       0.3             //默认led 亮度系数
#define LEDS_ANIMATION_CYCLE        6000            //LED 动画长度
#define LEDS_ANIMATION_STEP         80             //LED 动画步长，影响性能和效果

#define NUM_GAMEPAD_HOTKEYS                 (uint8_t)11   // 快捷键数量

// 调试开关
#define DEBUG_ADC 1

#ifdef DEBUG_ADC
    #define ADC_DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define ADC_DEBUG_PRINT(...)
#endif

#ifdef __cplusplus
}
#endif

#endif // _CONSTANT_H_