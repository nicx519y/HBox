#ifndef _CONSTANT_H_
#define _CONSTANT_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#define FIRMWARE_VERSION                    (uint32_t)0x010000  //固件版本
#define CONFIG_VERSION                      (uint32_t)0x000100  //配置版本 三位版本号 0x aa bb cc
#define ADC_MAPPING_VERSION                 (uint32_t)0x000002  //ADC值映射表版本

#define EX_ADDR                             0x90000000
#define FSDATA_ADDR                         0x0               // flash起始地址 webconfig 网页数据地址
#define ADC_VALUES_MAPPING_ADDR             0x00200000       //ADC值映射表地址
#define CONFIG_ADDR                         0x00400000       //配置数据地址

#define NUM_ADC_VALUES_MAPPING              8             // 最大8个映射 ADC按钮映射表用于值查找
#define MAX_ADC_VALUES_LENGTH               50             // 每个映射最大50个值 ADC按钮映射表用于值查找
#define MAX_NUM_MARKING_VALUE               100            // 每个step最大采集值个数
#define MIN_DIFF_ADC_VALUES_FIRST_AND_LAST  10000          // 最小值和最大值的差值，用于判断是否正确校准

#define NUM_PROFILES                        16
#define NUM_ADC1_BUTTONS                    9
#define NUM_ADC2_BUTTONS                    8
#define NUM_ADC_BUTTONS                     (NUM_ADC1_BUTTONS + NUM_ADC2_BUTTONS)
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
// #define NUM_LED                             21
#define NUM_LEDs_PER_ADC_BUTTON     1              //每个按钮多少个LED
#define LEDS_BRIGHTNESS_RADIO       0.3             //默认led 亮度系数
#define LEDS_ANIMATION_CYCLE        6000            //LED 动画长度
#define LEDS_ANIMATION_STEP         80             //LED 动画步长，影响性能和效果

#define NUM_GAMEPAD_HOTKEYS                 (uint8_t)11   // 快捷键数量

#define __RAM_Area__                __attribute__((section("._RAM_Area")))
#define __DTCMRAM_Area__            __attribute__((section("._DTCMRAM_Area")))
#define __RAM_D1_Area__             __attribute__((section("._RAM_D1_Area")))
#define __RAM_D2_Area__             __attribute__((section("._RAM_D2_Area")))

#ifdef __cplusplus
}
#endif

#endif // _CONSTANT_H_