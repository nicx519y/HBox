#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>
#include "enums.hpp"
#include "constant.hpp"
#include "stm32h750xx.h"
#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


// struct LEDOptions
// {
//     bool useUserDefinedLEDs;
//     int dataPin;
//     LEDFormat ledFormat;
//     ButtonLayout ledLayout;
//     uint8_t ledsPerButton;
//     uint8_t brightnessMaximum;
//     uint8_t brightnessSteps;
//     int indexUp;
//     int indexDown;
//     int indexLeft;
//     int indexRight;
//     int indexB1;
//     int indexB2;
//     int indexB3;
//     int indexB4;
//     int indexL1;
//     int indexR1;
//     int indexL2;
//     int indexR2;
//     int indexS1;
//     int indexS2;
//     int indexL3;
//     int indexR3;
//     int indexA1;
//     int indexA2;
//     int pledType;
//     int pledPin1;
//     int pledPin2;
//     int pledPin3;
//     int pledPin4;
//     RGB pledColor;
//     uint32_t checksum;
// };

// struct PS4Options
// {
//     uint8_t serial[16];
//     uint8_t signature[256];
//     mbedtls_mpi_uint rsa_n[64];
//     mbedtls_mpi_uint rsa_e[1];
//     mbedtls_mpi_uint rsa_d[64];
//     mbedtls_mpi_uint rsa_p[32];
//     mbedtls_mpi_uint rsa_q[32];
//     mbedtls_mpi_uint rsa_dp[32];
//     mbedtls_mpi_uint rsa_dq[32];
//     mbedtls_mpi_uint rsa_qp[32];
//     mbedtls_mpi_uint rsa_rn[64];
//     uint32_t checksum;
// };

typedef struct 
{
    uint8_t data[16*64];
    uint32_t checksum;
} SplashImage;

typedef struct
{
    uint8_t dpadMask;
    GamepadHotkey action;
} GamepadHotkeyEntry;

typedef struct
{
    uint32_t    virtualPin;             // 虚拟pin 实质上是在所有buttons之内的序号
    uint16_t    pressAccuracy;          // 按下精度 单位微米
    uint16_t    releaseAccuracy;        // 回弹精度 单位微米
    uint16_t    topDeadzone;            // 顶部死区 单位微米
    uint16_t    bottomDeadzone;         // 底部死区 单位微米
    uint16_t    keyTravel;              // 物理键程 单位微米
    uint16_t    topValue;               // 顶部模拟值
    uint16_t    bottomValue;            // 底部模拟值
    bool        ready;                  // 是否校准过
} ADCButton;

typedef struct
{
    uint32_t        virtualPin;
} GPIOButton;

typedef struct
{
    InputMode inputMode {InputMode::INPUT_MODE_XINPUT}; 
    DpadMode dpadMode {DpadMode::DPAD_MODE_DIGITAL};
    SOCDMode socdMode {SOCDMode::SOCD_MODE_NEUTRAL};
    bool invertXAxis;                                                                                                                                                                                 
    bool invertYAxis;
    bool fourWayMode;

    uint32_t keyDpadUp;             // example: 0001 0000 0010 0000 0000 0000 0000 0000  说明 keyDpadUp 这个功能按键和物理按键pin的关系
    uint32_t keyDpadDown;
    uint32_t keyDpadLeft;
    uint32_t keyDpadRight;
    uint32_t keyButtonB1;
    uint32_t keyButtonB2;
    uint32_t keyButtonB3;
    uint32_t keyButtonB4;
    uint32_t keyButtonL1;
    uint32_t keyButtonR1;
    uint32_t keyButtonL2;
    uint32_t keyButtonR2;
    uint32_t keyButtonS1;
    uint32_t keyButtonS2;
    uint32_t keyButtonL3;
    uint32_t keyButtonR3;
    uint32_t keyButtonA1;
    uint32_t keyButtonA2;
    uint32_t keyButtonFn;

    bool ledEnabled;
    LEDEffect ledEffect;
    bool isDoubleColor;
    uint32_t ledColor1;
    uint32_t ledColor2;
    uint32_t ledColor3;
    uint8_t ledBrightness;
    uint32_t ledColorCalibrateTop;
    uint32_t ledColorCalibrateBottom;
    uint32_t ledColorCalibrateComplete;
    uint8_t ledBrightnesssCalibrate;
    

} GamepadOptions;

typedef struct
{
    /* data */
    uint8_t profileIndex;
    ADCButton * ADCButtons[NUM_ADC_BUTTONS];
    GPIOButton * GPIOButtons[NUM_GPIO_BUTTONS];
    GamepadOptions * profiles[NUM_PROFILES];
} Config;

namespace ConfigUtils {
    bool load(Config& config);
    bool save(Config& config);
    
    char * toJSON(const Config& config);
    bool fromJSON(Config& config, const char* data, size_t dataLen);
    bool fromStorage(Config& config);
};

#ifdef __cplusplus
}
#endif

#endif