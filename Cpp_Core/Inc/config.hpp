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
    double_t    pressAccuracy;          // 按下精度 单位毫米
    double_t    releaseAccuracy;        // 回弹精度 单位毫米
    double_t    topDeadzone;            // 顶部死区 单位毫米
    double_t    bottomDeadzone;         // 底部死区 单位毫米
    double_t    magnettization;         // 磁化强度 单位特斯拉
    double_t    topPosition;            // 顶部位置 单位毫米
    double_t    bottomPosition;         // 底部位置 单位毫米
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
    uint32_t version;
    MainState mainState;
    uint8_t profileIndex;
    bool isCalibrateCompleted;
    ADCButton* ADCButtons[NUM_ADC_BUTTONS];
    GPIOButton* GPIOButtons[NUM_GPIO_BUTTONS];
    GamepadOptions* profiles[NUM_PROFILES];
} Config;

namespace ConfigUtils {
    bool load(Config& config);
    bool save(Config& config);
    bool reset(Config& config);
    
    void toJSON(char* buffer, Config& config);
    bool fromJSON(Config& config, char* data, size_t dataLen);
    bool fromStorage(Config& config);
    
};

#ifdef __cplusplus
}
#endif

#endif