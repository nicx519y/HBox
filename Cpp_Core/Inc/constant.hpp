#ifndef _CONSTANT_H_
#define _CONSTANT_H_ 

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_LABEL            "Version:"
#define CONFIG_VERSION          1

#define EX_FSDATA_ADDR          0x90000000  // 0x90000000 ~ 0x903FFFFF      (4MB)
#define EX_CONFIG_ADDR          0x90800000  // 0x90400000 ~ 0x905FFFFF      (2MB)

#define NUM_PROFILES            8
#define NUM_ADC_BUTTONS         16
#define NUM_GPIO_BUTTONS        4

#ifdef __cplusplus
}
#endif

#endif // _CONSTANT_H_