#ifndef _CONSTANT_H_
#define _CONSTANT_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#define FIRMWARE_VERSION                    (uint32_t)0x010000  //固件版本
#define CONFIG_VERSION                      (uint32_t)0x010000  //配置版本 三位版本号 0x aa bb cc

#define EX_ADDR                             0x90000000
#define FSDATA_ADDR                         0x0
#define CONFIG_ADDR                         0x00400000

#define EX_FSDATA_ADDR                      (EX_ADDR + FSDATA_ADDR)  // 0x90000000 ~ 0x903FFFFF      (4MB)
#define EX_CONFIG_ADDR                      (EX_ADDR + CONFIG_ADDR)  // 0x90400000 ~ 0x905FFFFF      (2MB)

#define NUM_PROFILES                        8
#define NUM_ADC1_BUTTONS                    8
#define NUM_ADC2_BUTTONS                    8
#define NUM_ADC_BUTTONS                     (NUM_ADC1_BUTTONS + NUM_ADC2_BUTTONS)
#define TIMES_ADC_CALIBRATION               100             // 单个按钮校准时的循环次数，必须100次连续稳定的值用于校准
#define DELAY_ADC_CALIBRATION               10              // 校准时间 TIMES_ADC_CALIBRATION * DELAY_ADC_CALIBRATION
#define ADC_VOLATILITY                      300             // ADC的浮动允许最大值

#define HAS_LED                                    //是否有LED
#define NUM_LEDs_PER_ADC_BUTTON     1              //每个按钮多少个LED
#define LEDS_BRIGHTNESS_RADIO       0.3             //默认led 亮度系数
#define LEDS_ANIMATION_CYCLE        6000            //LED 动画长度
#define LEDS_ANIMATION_STEP         80             //LED 动画步长，影响性能和效果

#define NUM_GPIO_BUTTONS            4               //GPIO按钮数量
#define GPIO_BUTTONS_DEBOUNCE       5             //去抖动延迟(ms) 

#define MAGNETIC_TICKNESS                   (double_t)10    // 磁芯长度 单位毫米
#define MAGNETIC_RADIUS                     (double_t)2     // 磁芯半径 单位毫米
#define MAGNETIC_DISTANCE                   (double_t)3.5   // 磁轴行程 单位毫米
#define MAGNETIC_DEFAULT_PRESS_ACCURACY     (double_t)0.1   // 默认按下精度 单位毫米
#define MAGNETIC_DEFAULT_RELEASE_ACCURACY   (double_t)0.1   // 默认回弹精度 单位毫米
#define MAGNETIC_DEFAULT_TOP_DEADZONE       (double_t)0.1   // 默认顶部死区 单位毫米
#define MAGNETIC_DEFAULT_BOTTOM_DEADZONE    (double_t)0.1   // 默认底部死区 单位毫米
#define MAGNETIC_BASE_VALUE                 (uint32_t)0    // 磁芯归零值

#define __RAM_Area__                __attribute__((section("._RAM_Area")))
#define __DTCMRAM_Area__            __attribute__((section("._DTCMRAM_Area")))

#ifdef __cplusplus
}
#endif

#endif // _CONSTANT_H_