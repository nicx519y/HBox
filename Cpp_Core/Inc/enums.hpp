#ifndef _ENUMS_H_
#define _ENUMS_H_  

#include "stdint.h"

typedef enum
{
	CONFIG_TYPE_WEB = 0,
	CONFIG_TYPE_SERIAL,
	CONFIG_TYPE_DISPLAY
} ConfigType;

enum InputMode
{
    INPUT_MODE_XINPUT,
    INPUT_MODE_SWITCH,
    INPUT_MODE_PS3,
    INPUT_MODE_KEYBOARD,
    INPUT_MODE_PS4,
    INPUT_MODE_CONFIG = 255,
};

enum DpadMode
{
    DPAD_MODE_DIGITAL,
    DPAD_MODE_LEFT_ANALOG,
    DPAD_MODE_RIGHT_ANALOG,
};

enum SOCDMode
{
    SOCD_MODE_UP_PRIORITY,
    SOCD_MODE_NEUTRAL,
    SOCD_MODE_SECOND_INPUT_PRIORITY,
    SOCD_MODE_FIRST_INPUT_PRIORITY,
    SOCD_MODE_BYPASS,
};

// typedef enum DpadDirection
// {
//     DIRECTION_NONE,
//     DIRECTION_UP,   
//     DIRECTION_DOWN,
//     DIRECTION_LEFT,
//     DIRECTION_RIGHT,
// } DpadDirection;  

enum GpioAction
{
    // the lowest value is the default, which should be NONE;
    // reserving some numbers in case we need more not-mapped type values
    NONE                         = -10,
    RESERVED                     = -5,
    ASSIGNED_TO_ADDON            = 0,
    BUTTON_PRESS_UP              = 1,
    BUTTON_PRESS_DOWN            = 2,
    BUTTON_PRESS_LEFT            = 3,
    BUTTON_PRESS_RIGHT           = 4,
    BUTTON_PRESS_B1              = 5,
    BUTTON_PRESS_B2              = 6,
    BUTTON_PRESS_B3              = 7,
    BUTTON_PRESS_B4              = 8,
    BUTTON_PRESS_L1              = 9,
    BUTTON_PRESS_R1              = 10,
    BUTTON_PRESS_L2              = 11,
    BUTTON_PRESS_R2              = 12,
    BUTTON_PRESS_S1              = 13,
    BUTTON_PRESS_S2              = 14,
    BUTTON_PRESS_A1              = 15,
    BUTTON_PRESS_A2              = 16,
    BUTTON_PRESS_L3              = 17,
    BUTTON_PRESS_R3              = 18,
    BUTTON_PRESS_FN              = 19,
    BUTTON_PRESS_DDI_UP          = 20,
    BUTTON_PRESS_DDI_DOWN        = 21,
    BUTTON_PRESS_DDI_LEFT        = 22,
    BUTTON_PRESS_DDI_RIGHT       = 23,
    SUSTAIN_DP_MODE_DP           = 24,
    SUSTAIN_DP_MODE_LS           = 25,
    SUSTAIN_DP_MODE_RS           = 26,
    SUSTAIN_SOCD_MODE_UP_PRIO    = 27,
    SUSTAIN_SOCD_MODE_NEUTRAL    = 28,
    SUSTAIN_SOCD_MODE_SECOND_WIN = 29,
    SUSTAIN_SOCD_MODE_FIRST_WIN  = 30,
    SUSTAIN_SOCD_MODE_BYPASS     = 31,
    BUTTON_PRESS_TURBO           = 32,
    BUTTON_PRESS_MACRO           = 33,
    BUTTON_PRESS_MACRO_1         = 34,
    BUTTON_PRESS_MACRO_2         = 35,
    BUTTON_PRESS_MACRO_3         = 36,
    BUTTON_PRESS_MACRO_4         = 37,
    BUTTON_PRESS_MACRO_5         = 38,
    BUTTON_PRESS_MACRO_6         = 39,
    CUSTOM_BUTTON_COMBO          = 40,
    BUTTON_PRESS_A3              = 41,
    BUTTON_PRESS_A4              = 42,
    BUTTON_PRESS_E1              = 43,
    BUTTON_PRESS_E2              = 44,
    BUTTON_PRESS_E3              = 45,
    BUTTON_PRESS_E4              = 46,
    BUTTON_PRESS_E5              = 47,
    BUTTON_PRESS_E6              = 48,
    BUTTON_PRESS_E7              = 49,
    BUTTON_PRESS_E8              = 50,
    BUTTON_PRESS_E9              = 51,
    BUTTON_PRESS_E10             = 52,
    BUTTON_PRESS_E11             = 53,
    BUTTON_PRESS_E12             = 54
};

enum GpioDirection
{
    GPIO_DIRECTION_INPUT         = 0,
    GPIO_DIRECTION_OUTPUT        = 1
};

enum GamepadHotkey
{
    HOTKEY_NONE,
    HOTKEY_DPAD_DIGITAL,
    HOTKEY_DPAD_LEFT_ANALOG,
    HOTKEY_DPAD_RIGHT_ANALOG,
    HOTKEY_HOME_BUTTON,
    HOTKEY_CAPTURE_BUTTON,
    HOTKEY_SOCD_UP_PRIORITY,
    HOTKEY_SOCD_NEUTRAL,
    HOTKEY_SOCD_LAST_INPUT,
    HOTKEY_INVERT_X_AXIS,
    HOTKEY_INVERT_Y_AXIS,
    HOTKEY_SOCD_FIRST_INPUT,
    HOTKEY_SOCD_BYPASS,
};

enum OnBoardLedMode
{
    BOARD_LED_OFF,
    MODE_INDICATOR,
    INPUT_TEST,
};

enum PLEDType
{
    PLED_TYPE_NONE = -1,
    PLED_TYPE_PWM = 0,
    PLED_TYPE_RGB = 1,
};




#endif /* _NET_DRIVER_H_  */