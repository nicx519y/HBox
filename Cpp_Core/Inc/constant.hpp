#ifndef _CONSTANT_H_
#define _CONSTANT_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_LABEL                        "Version:"
#define CONFIG_VERSION                      1

#define EX_FSDATA_ADDR                      0x90000000  // 0x90000000 ~ 0x903FFFFF      (4MB)
#define EX_CONFIG_ADDR                      0x90800000  // 0x90400000 ~ 0x905FFFFF      (2MB)

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

#ifdef __cplusplus
}
#endif

#endif // _CONSTANT_H_