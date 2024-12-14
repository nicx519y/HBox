#include "config.hpp"
#include "qspi-w25q64.h"
#include "constant.hpp"
#include "cJSON.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char ConfigJSONBuffer[20*1024];

bool ConfigUtils::load(Config& config)
{
    bool fjResult;
    fjResult = fromStorage(config);

    if(fjResult == true && config.version == CONFIG_VERSION) { // 版本号一致
        uint32_t ver = config.version;
        printf("Config Version: %d.%d.%d\n", (ver>>16) & 0xff, (ver>>8) & 0xff, ver & 0xff);
        return true;
    } else {

        printf("init config, version: %d.%d.%d\n", (CONFIG_VERSION>>16) & 0xff, (CONFIG_VERSION>>8) & 0xff, CONFIG_VERSION & 0xff);
        // 设置基础配置
        config.version = CONFIG_VERSION;
        config.bootMode = BOOT_MODE_WEB_CONFIG;
        strcpy(config.defaultProfileId, "profile-0");
        config.isCalibrateCompleted = false;
        config.numProfilesMax = NUM_PROFILES;

        // 设置ADCButtons
        for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
            config.ADCButtons[i].virtualPin = i;
            config.ADCButtons[i].topPosition = 0.0f;
            config.ADCButtons[i].bottomPosition = 0.0f;
            config.ADCButtons[i].magnettization = -1.0f;
        }

        // 设置GPIOButtons
        for(uint8_t j = 0; j < NUM_GPIO_BUTTONS; j++) {
            config.GPIOButtons[j].virtualPin = j + NUM_ADC_BUTTONS;
        }
        
        // 设置profiles
        for(uint8_t k = 0; k < NUM_PROFILES; k++) {
            // 设置profile id, name, enabled
            sprintf(config.profiles[k].id, "profile-%d", k);
            sprintf(config.profiles[k].name, "Profile %d", k);
            config.profiles[k].enabled = (k == 0);  // 默认第一个profile为启用

            // 设置keysConfig
            config.profiles[k].keysConfig.inputMode = InputMode::INPUT_MODE_XINPUT;
            config.profiles[k].keysConfig.socdMode = SOCDMode::SOCD_MODE_NEUTRAL;
            config.profiles[k].keysConfig.fourWayMode = false;
            config.profiles[k].keysConfig.invertXAxis = false;
            config.profiles[k].keysConfig.invertYAxis = false;
            config.profiles[k].keysConfig.keyDpadUp = 1 << 0;
            config.profiles[k].keysConfig.keyDpadDown = 1 << 1;
            config.profiles[k].keysConfig.keyDpadLeft = 1 << 2;
            config.profiles[k].keysConfig.keyDpadRight = 1 << 3;
            config.profiles[k].keysConfig.keyButtonB1 = 1 << 4;
            config.profiles[k].keysConfig.keyButtonB2 = 1 << 5;
            config.profiles[k].keysConfig.keyButtonB3 = 1 << 6;
            config.profiles[k].keysConfig.keyButtonB4 = 1 << 7;
            config.profiles[k].keysConfig.keyButtonL1 = 1 << 8;
            config.profiles[k].keysConfig.keyButtonL2 = 1 << 9;
            config.profiles[k].keysConfig.keyButtonR1 = 1 << 10;
            config.profiles[k].keysConfig.keyButtonR2 = 1 << 11;
            config.profiles[k].keysConfig.keyButtonL3 = 1 << 12;
            config.profiles[k].keysConfig.keyButtonR3 = 1 << 13;
            config.profiles[k].keysConfig.keyButtonA1 = 1 << 14;
            config.profiles[k].keysConfig.keyButtonA2 = 1 << 15;
            config.profiles[k].keysConfig.keyButtonS1 = 1 << 17;
            config.profiles[k].keysConfig.keyButtonS2 = 1 << 18;
            config.profiles[k].keysConfig.keyButtonFn = 1 << 19;

            

            // 设置triggerConfigs 
            config.profiles[k].triggerConfigs.isAllBtnsConfiguring = true;

            for(uint8_t l = 0; l < NUM_ADC_BUTTONS; l++) {
                config.profiles[k].triggerConfigs.triggerConfigs[l] = {
                    .pressAccuracy = 0.1f,
                    .releaseAccuracy = 0.1f,
                    .topDeadzone = 0.2f,
                    .bottomDeadzone = 0.2f
                };
            }

            // 设置ledProfile
            config.profiles[k].ledsConfigs.ledEnabled = false;
            config.profiles[k].ledsConfigs.ledEffect = LEDEffect::STATIC;
            config.profiles[k].ledsConfigs.ledColor1 = 0x00ff00;
            config.profiles[k].ledsConfigs.ledColor2 = 0x0000ff;
            config.profiles[k].ledsConfigs.ledColor3 = 0x000000;
            config.profiles[k].ledsConfigs.ledBrightness = 100;
        }

        // 设置hotkeys
        for(uint8_t m = 0; m < NUM_GAMEPAD_HOTKEYS; m++) {
            if(m == 0) { // 第一个快捷键锁定, 锁定为webconfig
                config.hotkeys[m].isLocked = true;
                config.hotkeys[m].action = GamepadHotkey::HOTKEY_INPUT_MODE_WEBCONFIG;
                config.hotkeys[m].virtualPin = 0;
            } else { // 其他快捷键不锁定
                config.hotkeys[m].isLocked = false;
                config.hotkeys[m].action = GamepadHotkey::HOTKEY_NONE;
                config.hotkeys[m].virtualPin = -1;
            }
        }


        return save(config);
    } 
}

bool ConfigUtils::save(Config& config)
{
    printf("ConfigUtils::save begin\n");

    // 先擦除存储区域
    int8_t result = QSPI_W25Qxx_BlockErase_32K(CONFIG_ADDR);
    if(result != QSPI_W25Qxx_OK) {
        printf("ConfigUtils::save - block erase failure.\n");
        return false;
    }

    // 写入配置数据
    result = QSPI_W25Qxx_WriteBuffer((uint8_t*)&config, CONFIG_ADDR, sizeof(Config));
    if(result == QSPI_W25Qxx_OK) {
        printf("ConfigUtils::save - success.\n");
        return true;
    } else {
        printf("ConfigUtils::save - Write failure.\n");
        return false;
    }
}

/**
 * @brief 重置配置
 * 
 * @param config 
 * @return true 
 * @return false 
 */
bool ConfigUtils::reset(Config& config)
{
    int8_t result = QSPI_W25Qxx_BlockErase_32K(CONFIG_ADDR);
    if(result != QSPI_W25Qxx_OK) {
        printf("ConfigUtils::reset - block erase failure.\n");
        return false;
    }
    return ConfigUtils::load(config);
}

/**
 * @brief 从存储中读取配置
 * 
 * @param config 
 * @return true 
 * @return false 
 */
bool ConfigUtils::fromStorage(Config& config)
{
    int8_t result;
    printf("ConfigUtils::fromStorage begin.\n");
    
    result = QSPI_W25Qxx_ReadBuffer((uint8_t*)&config, CONFIG_ADDR, sizeof(Config));
    if(result == QSPI_W25Qxx_OK) {
        printf("ConfigUtils::fromStorage - success.\n");
        return true;
    } else {
        printf("ConfigUtils::fromStorage - Read failure.\n");
        return false;
    }
}