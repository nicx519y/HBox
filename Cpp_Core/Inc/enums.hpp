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

enum ButtonLayout
{
    BUTTON_LAYOUT_STICK,
    BUTTON_LAYOUT_STICKLESS,
    BUTTON_LAYOUT_BUTTONS_ANGLED,
    BUTTON_LAYOUT_BUTTONS_BASIC,
    BUTTON_LAYOUT_KEYBOARD_ANGLED,
    BUTTON_LAYOUT_KEYBOARDA,
    BUTTON_LAYOUT_DANCEPADA,
    BUTTON_LAYOUT_TWINSTICKA,
    BUTTON_LAYOUT_BLANKA,
    BUTTON_LAYOUT_VLXA,
    BUTTON_LAYOUT_FIGHTBOARD_STICK,
    BUTTON_LAYOUT_FIGHTBOARD_MIRRORED,
    BUTTON_LAYOUT_CUSTOMA,
    BUTTON_LAYOUT_OPENCORE0WASDA,
    BUTTON_LAYOUT_STICKLESS_13,
    BUTTON_LAYOUT_STICKLESS_16,
    BUTTON_LAYOUT_STICKLESS_14,
    BUTTON_LAYOUT_STICKLESS_R16,        
};

enum ButtonLayoutRight
{
    BUTTON_LAYOUT_ARCADE,
    BUTTON_LAYOUT_STICKLESSB,
    BUTTON_LAYOUT_BUTTONS_ANGLEDB,
    BUTTON_LAYOUT_VEWLIX,
    BUTTON_LAYOUT_VEWLIX7,
    BUTTON_LAYOUT_CAPCOM,
    BUTTON_LAYOUT_CAPCOM6,
    BUTTON_LAYOUT_SEGA2P,
    BUTTON_LAYOUT_NOIR8,
    BUTTON_LAYOUT_KEYBOARDB,
    BUTTON_LAYOUT_DANCEPADB,
    BUTTON_LAYOUT_TWINSTICKB,
    BUTTON_LAYOUT_BLANKB,
    BUTTON_LAYOUT_VLXB,
    BUTTON_LAYOUT_FIGHTBOARD,
    BUTTON_LAYOUT_FIGHTBOARD_STICK_MIRRORED,
    BUTTON_LAYOUT_CUSTOMB,
    BUTTON_LAYOUT_KEYBOARD8B,
    BUTTON_LAYOUT_OPENCORE0WASDB,
    BUTTON_LAYOUT_STICKLESS_13B,
    BUTTON_LAYOUT_STICKLESS_16B,
    BUTTON_LAYOUT_STICKLESS_14B,
    BUTTON_LAYOUT_STICKLESS_R16B,
};

enum SplashMode
{
    STATICSPLASH,
    CLOSEIN,
    CLOSEINCUSTOM,
    NOSPLASH,
};

enum SplashChoice
{
    MAIN,
    X,
    Y,
    Z,
    CUSTOM,
    LEGACY
};

struct ButtonLayoutParams
{
    union {
        ButtonLayout layout;
        ButtonLayoutRight layoutRight;
    };
    int startX;
    int startY;
    int buttonRadius;
    int buttonPadding;
};

struct ButtonLayoutCustomOptions
{
    ButtonLayoutParams params;
    ButtonLayoutParams paramsRight;
};

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

struct AddonOptions
{
    uint8_t pinButtonTurbo;
    uint8_t pinButtonReverse;
    uint8_t pinSliderLS;
    uint8_t pinSliderRS;
    uint8_t pinSliderSOCDOne;
    uint8_t pinSliderSOCDTwo;
    uint8_t turboShotCount; // Turbo
    uint8_t pinTurboLED;    // Turbo LED
    uint8_t pinReverseLED;    // Reverse LED
    uint8_t reverseActionUp;
    uint8_t reverseActionDown;
    uint8_t reverseActionLeft;
    uint8_t reverseActionRight;
    uint8_t i2cAnalog1219SDAPin;
    uint8_t i2cAnalog1219SCLPin;
    int i2cAnalog1219Block;
    uint32_t i2cAnalog1219Speed;
    uint8_t i2cAnalog1219Address;
    uint8_t pinDualDirUp;    // Pins for Dual Directional Input
    uint8_t pinDualDirDown;
    uint8_t pinDualDirLeft;
    uint8_t pinDualDirRight;
    DpadMode dualDirDpadMode;    // LS/DP/RS
    uint8_t dualDirCombineMode; // Mix/Gamepad/Dual/None
    OnBoardLedMode onBoardLedMode;
    uint8_t analogAdcPinX;
    uint8_t analogAdcPinY;
    uint16_t bootselButtonMap;
    uint8_t extraButtonPin;
    uint32_t extraButtonMap;
    uint8_t buzzerPin;
    uint8_t buzzerVolume;
    uint8_t playerNumber;
    uint8_t shmupMode; // Turbo SHMUP Mode
    uint8_t shmupMixMode; // How we mix turbo and non-turbo buttons
    uint16_t shmupAlwaysOn1;
    uint16_t shmupAlwaysOn2;
    uint16_t shmupAlwaysOn3;
    uint16_t shmupAlwaysOn4;
    uint8_t pinShmupBtn1;
    uint8_t pinShmupBtn2;
    uint8_t pinShmupBtn3;
    uint8_t pinShmupBtn4;
    uint16_t shmupBtnMask1;
    uint16_t shmupBtnMask2;
    uint16_t shmupBtnMask3;
    uint16_t shmupBtnMask4;
    uint8_t pinShmupDial;
    SOCDMode sliderSOCDModeOne;
    SOCDMode sliderSOCDModeTwo;
    SOCDMode sliderSOCDModeDefault;
    uint8_t wiiExtensionSDAPin;
    uint8_t wiiExtensionSCLPin;
    int wiiExtensionBlock;
    uint32_t wiiExtensionSpeed;
    uint8_t AnalogInputEnabled;
    uint8_t BoardLedAddonEnabled;
    uint8_t BootselButtonAddonEnabled;
    uint8_t BuzzerSpeakerAddonEnabled;
    uint8_t DualDirectionalInputEnabled;
    uint8_t ExtraButtonAddonEnabled;
    uint8_t I2CAnalog1219InputEnabled;
    //bool I2CDisplayAddonEnabled; // I2C is special case
    uint8_t JSliderInputEnabled;
    //bool NeoPicoLEDAddonEnabled; // NeoPico is special case
    //bool PlayerLEDAddonEnabled; // PlayerLED is special case
    uint8_t PlayerNumAddonEnabled;
    uint8_t PS4ModeAddonEnabled;
    uint8_t ReverseInputEnabled;
    uint8_t TurboInputEnabled;
    uint8_t SliderSOCDInputEnabled;
    uint8_t WiiExtensionAddonEnabled;
    uint32_t checksum;
};

struct SplashImage
{
    uint8_t data[16*64];
    uint32_t checksum;
};

struct GamepadHotkeyEntry
{
    uint8_t dpadMask;
    GamepadHotkey action;
};

struct GamepadOptions
{
    InputMode inputMode {InputMode::INPUT_MODE_XINPUT}; 
    DpadMode dpadMode {DpadMode::DPAD_MODE_DIGITAL};
    SOCDMode socdMode {SOCDMode::SOCD_MODE_NEUTRAL};
    bool invertXAxis;
    bool invertYAxis;

    uint8_t keyDpadUp;
    uint8_t keyDpadDown;
    uint8_t keyDpadLeft;
    uint8_t keyDpadRight;
    uint8_t keyButtonB1;
    uint8_t keyButtonB2;
    uint8_t keyButtonB3;
    uint8_t keyButtonB4;
    uint8_t keyButtonL1;
    uint8_t keyButtonR1;
    uint8_t keyButtonL2;
    uint8_t keyButtonR2;
    uint8_t keyButtonS1;
    uint8_t keyButtonS2;
    uint8_t keyButtonL3;
    uint8_t keyButtonR3;
    uint8_t keyButtonA1;
    uint8_t keyButtonA2;

    GamepadHotkeyEntry hotkeyF1Up;
    GamepadHotkeyEntry hotkeyF1Down;
    GamepadHotkeyEntry hotkeyF1Left;
    GamepadHotkeyEntry hotkeyF1Right;
    GamepadHotkeyEntry hotkeyF2Up;
    GamepadHotkeyEntry hotkeyF2Down;
    GamepadHotkeyEntry hotkeyF2Left;
    GamepadHotkeyEntry hotkeyF2Right;

    uint32_t checksum;
};

struct __attribute__ ((__packed__)) AnimationOptions
{
    uint32_t checksum;
    uint8_t baseAnimationIndex;
    uint8_t brightness;
    uint8_t staticColorIndex;
    uint8_t buttonColorIndex;
    int16_t chaseCycleTime;
    int16_t rainbowCycleTime;
    uint8_t themeIndex;
    bool hasCustomTheme;
    uint32_t customThemeUp;
    uint32_t customThemeDown;
    uint32_t customThemeLeft;
    uint32_t customThemeRight;
    uint32_t customThemeB1;
    uint32_t customThemeB2;
    uint32_t customThemeB3;
    uint32_t customThemeB4;
    uint32_t customThemeL1;
    uint32_t customThemeR1;
    uint32_t customThemeL2;
    uint32_t customThemeR2;
    uint32_t customThemeS1;
    uint32_t customThemeS2;
    uint32_t customThemeL3;
    uint32_t customThemeR3;
    uint32_t customThemeA1;
    uint32_t customThemeA2;
    uint32_t customThemeUpPressed;
    uint32_t customThemeDownPressed;
    uint32_t customThemeLeftPressed;
    uint32_t customThemeRightPressed;
    uint32_t customThemeB1Pressed;
    uint32_t customThemeB2Pressed;
    uint32_t customThemeB3Pressed;
    uint32_t customThemeB4Pressed;
    uint32_t customThemeL1Pressed;
    uint32_t customThemeR1Pressed;
    uint32_t customThemeL2Pressed;
    uint32_t customThemeR2Pressed;
    uint32_t customThemeS1Pressed;
    uint32_t customThemeS2Pressed;
    uint32_t customThemeL3Pressed;
    uint32_t customThemeR3Pressed;
    uint32_t customThemeA1Pressed;
    uint32_t customThemeA2Pressed;
};

struct BoardOptions
{
    bool hasBoardOptions;
    uint8_t pinDpadUp;
    uint8_t pinDpadDown;
    uint8_t pinDpadLeft;
    uint8_t pinDpadRight;
    uint8_t pinButtonB1;
    uint8_t pinButtonB2;
    uint8_t pinButtonB3;
    uint8_t pinButtonB4;
    uint8_t pinButtonL1;
    uint8_t pinButtonR1;
    uint8_t pinButtonL2;
    uint8_t pinButtonR2;
    uint8_t pinButtonS1;
    uint8_t pinButtonS2;
    uint8_t pinButtonL3;
    uint8_t pinButtonR3;
    uint8_t pinButtonA1;
    uint8_t pinButtonA2;
    ButtonLayout buttonLayout;
    ButtonLayoutRight buttonLayoutRight;
    SplashMode splashMode;
    SplashChoice splashChoice;
    int splashDuration; // -1 = Always on
    uint8_t i2cSDAPin;
    uint8_t i2cSCLPin;
    int i2cBlock;
    uint32_t i2cSpeed;
    bool hasI2CDisplay;
    int displayI2CAddress;
    uint8_t displaySize;
    uint8_t displayFlip;
    bool displayInvert;
    int displaySaverTimeout;
    ButtonLayoutCustomOptions buttonLayoutCustomOptions;
    char boardVersion[32]; // 32-char limit to board name
    uint32_t checksum;
};


#endif /* _NET_DRIVER_H_  */