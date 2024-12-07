#include "config.hpp"
#include "qspi-w25q64.h"
#include "constant.hpp"
#include "cJSON.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define config_version      CONFIG_VERSION

__RAM_Area__ static char ConfigJSONBuffer[20*1024];

/**
 * @brief 默认的profile配置
 */
static GamepadOptions defaultProfile = {
    .name           = "Default Profile",
    .enabled        = false,
    .inputMode      = INPUT_MODE_XINPUT,
    .dpadMode       = DPAD_MODE_DIGITAL,
    .socdMode       = SOCD_MODE_NEUTRAL,
    .invertXAxis    = false,
    .invertYAxis    = false,
    .fourWayMode    = false,
    .keyDpadUp      = (uint32_t) 1 << 0,
    .keyDpadDown    = (uint32_t) 1 << 1,
    .keyDpadLeft    = (uint32_t) 1 << 2,
    .keyDpadRight   = (uint32_t) 1 << 3,
    .keyButtonB1    = (uint32_t) 1 << 4,
    .keyButtonB2    = (uint32_t) 1 << 5,
    .keyButtonB3    = (uint32_t) 1 << 6,
    .keyButtonB4    = (uint32_t) 1 << 7,
    .keyButtonL1    = (uint32_t) 1 << 8,
    .keyButtonR1    = (uint32_t) 1 << 9,
    .keyButtonL2    = (uint32_t) 1 << 10,
    .keyButtonR2    = (uint32_t) 1 << 11,
    .keyButtonS1    = (uint32_t) 1 << 12,
    .keyButtonS2    = (uint32_t) 1 << 13,
    .keyButtonL3    = (uint32_t) 1 << 14,
    .keyButtonR3    = (uint32_t) 1 << 15,
    .keyButtonA1    = (uint32_t) 1 << 16,
    .keyButtonA2    = (uint32_t) 1 << 17,
    .keyButtonFn    = (uint32_t) 1 << 18,

    .RTProfiles = {},

    .ledEnabled      = true,
    .ledEffect       = LEDEffect::STATIC,
    .isDoubleColor   = true,
    .ledColor1       = 0xff0000,
    .ledColor2       = 0x00ff00,
    .ledColor3       = 0x0000ff,
    .ledBrightness   = 0xff,
    .ledColorCalibrateTop       = 0xff0000,
    .ledColorCalibrateBottom    = 0x0000ff,
    .ledColorCalibrateComplete  = 0x00ff00,
    .ledBrightnesssCalibrate    = 0xff
};

/**
 * @brief 默认的配置
 */
static Config defaultConfig = 
{
    .version        = CONFIG_VERSION,
    .mainState      = MainState::MAIN_STATE_WEB_CONFIG,
    .profileIndex   = 0,
    .isCalibrateCompleted = false,
    .ADCButtons     = {},
    .GPIOButtons    = {},
    .profiles       = {}
};

/**
 * @brief 默认的RT触发器配置
 */
static RipidTriggerProfile defaultRTProfiles = {
    .pressAccuracy = MAGNETIC_DEFAULT_PRESS_ACCURACY,
    .releaseAccuracy = MAGNETIC_DEFAULT_RELEASE_ACCURACY,
    .topDeadzone = MAGNETIC_DEFAULT_TOP_DEADZONE,
    .bottomDeadzone = MAGNETIC_DEFAULT_BOTTOM_DEADZONE
};

bool ConfigUtils::load(Config& config)
{
    bool fjResult;

    fjResult = fromStorage(config);

    if(fjResult == true) {
        uint32_t ver = config.version;
        printf("Config Version: %d.%d.%d\n", (ver>>16) & 0xff, (ver>>8) & 0xff, ver & 0xff);

        return true;
    } else {
        
        config.version = defaultConfig.version;
        config.mainState = defaultConfig.mainState;
        config.profileIndex = defaultConfig.profileIndex;
        config.isCalibrateCompleted = defaultConfig.isCalibrateCompleted;
        
        for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
            free(config.ADCButtons[i]);
            ADCButton* ptr = (ADCButton*) malloc(sizeof(ADCButton));
            if(ptr != NULL) {
                ptr->virtualPin = i;
                ptr->topPosition = 0;
                ptr->bottomPosition = 0;
                ptr->magnettization = -1;
                config.ADCButtons[i] = ptr;
            } else {
                printf("ConfigUtils::load - malloc falure.\n");
            }
        }

        for(uint8_t j = 0; j < NUM_GPIO_BUTTONS; j ++) {
            free(config.GPIOButtons[j]);
            GPIOButton* ptr = (GPIOButton*) malloc(sizeof(GPIOButton));
            if(ptr != NULL) {
                ptr->virtualPin = j;
                config.GPIOButtons[j] = ptr;
            } else {
                printf("ConfigUtils::load - malloc falure.\n");
            }
        }

        for(uint8_t k = 0; k < NUM_PROFILES; k ++) {
            free(config.profiles[k]);
            GamepadOptions* ptr = (GamepadOptions*) malloc(sizeof(GamepadOptions));
            if(ptr != NULL) {
                memcpy(ptr, &defaultProfile, sizeof(defaultProfile));
                if(k != 0) {
                    strcpy(ptr->name, "Profile ");
                    ptr->name[7] = '0' + k;
                    ptr->enabled = false;
                } else {
                    strcpy(ptr->name, defaultProfile.name);
                    ptr->enabled = true;
                }
                for(uint8_t l = 0; l < NUM_ADC_BUTTONS; l ++) {
                    ptr->RTProfiles[l] = (RipidTriggerProfile*) malloc(sizeof(defaultRTProfiles));
                    memcpy(ptr->RTProfiles[l], &defaultRTProfiles, sizeof(defaultRTProfiles));
                }
                config.profiles[k] = ptr;
            } else {
                printf("ConfigUtils::load - malloc falure.\n");
            }
        }

        return save(config);
    } 
}

bool ConfigUtils::save(Config& config)
{
    printf("ConfigUtils::save begin\n");
    memset(ConfigJSONBuffer, '\0', sizeof(ConfigJSONBuffer));
    toJSON(ConfigJSONBuffer, config);

    if(strchr(ConfigJSONBuffer, '\0') == NULL || strcmp(ConfigJSONBuffer, "") == 0) {
        printf("ConfigUtils::save. To JSON failure.\n");
        return false;
    } else {
        printf("begin write config: %s\n", ConfigJSONBuffer);
    }
    QSPI_W25Qxx_BlockErase_32K(CONFIG_ADDR);
    int8_t result = QSPI_W25Qxx_WriteString(ConfigJSONBuffer, CONFIG_ADDR);
    if(result == QSPI_W25Qxx_OK) {
        printf("ConfigUtils::save. success.\n");
        return true;
    } else {
        printf("ConfigUtils::save. Write failure.\n");
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
 * @brief 将配置转换为JSON字符串
 * 
 * @param buffer 
 * @param config 
 */
void ConfigUtils::toJSON(char* buffer, Config& config)
{
    cJSON* pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "version", config.version);
    cJSON_AddNumberToObject(pRoot, "mainState", config.mainState);
    cJSON_AddNumberToObject(pRoot, "profileIndex", config.profileIndex);
    cJSON_AddBoolToObject(pRoot, "isCalibrateCompleted", config.isCalibrateCompleted);

    cJSON* pADCButtons = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "ADCButtons", pADCButtons);

    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
        cJSON* pADCButton = cJSON_CreateObject();
        cJSON_AddNumberToObject(pADCButton, "virtualPin", config.ADCButtons[i]->virtualPin);
        cJSON_AddNumberToObject(pADCButton, "magnettization", config.ADCButtons[i]->magnettization);
        cJSON_AddNumberToObject(pADCButton, "topPosition", config.ADCButtons[i]->topPosition);
        cJSON_AddNumberToObject(pADCButton, "bottomPosition", config.ADCButtons[i]->bottomPosition);
        cJSON_AddItemToArray(pADCButtons, pADCButton);
    }

    cJSON* pGPIOButtons = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "GPIOButtons", pGPIOButtons);

    for(uint8_t j = 0; j < NUM_GPIO_BUTTONS; j ++) {
        cJSON* pGPIOButton = cJSON_CreateObject();
        cJSON_AddNumberToObject(pGPIOButton, "virtualPin", config.GPIOButtons[j]->virtualPin);
        cJSON_AddItemToArray(pGPIOButtons, pGPIOButton);
    }

    cJSON* pProfiles = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "profiles", pProfiles);

    for(uint8_t k = 0; k < NUM_PROFILES; k ++) {
        cJSON* pOptions = cJSON_CreateObject();
        cJSON_AddItemToArray(pProfiles, pOptions);

        cJSON_AddStringToObject(pOptions, "name", config.profiles[k]->name);
        cJSON_AddBoolToObject(pOptions, "enabled", config.profiles[k]->enabled);
        cJSON_AddNumberToObject(pOptions, "inputMode", config.profiles[k]->inputMode);
        cJSON_AddNumberToObject(pOptions, "dpadMode", config.profiles[k]->dpadMode);
        cJSON_AddNumberToObject(pOptions, "socdMode", config.profiles[k]->socdMode);
        cJSON_AddBoolToObject(pOptions, "invertXAxis", config.profiles[k]->invertXAxis);
        cJSON_AddBoolToObject(pOptions, "invertYAxis", config.profiles[k]->invertYAxis);
        cJSON_AddBoolToObject(pOptions, "fourWayMode", config.profiles[k]->fourWayMode);

        cJSON_AddNumberToObject(pOptions, "keyDpadUp", config.profiles[k]->keyDpadUp);
        cJSON_AddNumberToObject(pOptions, "keyDpadDown", config.profiles[k]->keyDpadDown);
        cJSON_AddNumberToObject(pOptions, "keyDpadLeft", config.profiles[k]->keyDpadLeft);
        cJSON_AddNumberToObject(pOptions, "keyDpadRight", config.profiles[k]->keyDpadRight);
        cJSON_AddNumberToObject(pOptions, "keyButtonB1", config.profiles[k]->keyButtonB1);
        cJSON_AddNumberToObject(pOptions, "keyButtonB2", config.profiles[k]->keyButtonB2);
        cJSON_AddNumberToObject(pOptions, "keyButtonB3", config.profiles[k]->keyButtonB3);
        cJSON_AddNumberToObject(pOptions, "keyButtonB4", config.profiles[k]->keyButtonB4);
        cJSON_AddNumberToObject(pOptions, "keyButtonL1", config.profiles[k]->keyButtonL1);
        cJSON_AddNumberToObject(pOptions, "keyButtonL2", config.profiles[k]->keyButtonL2);
        cJSON_AddNumberToObject(pOptions, "keyButtonR1", config.profiles[k]->keyButtonR1);
        cJSON_AddNumberToObject(pOptions, "keyButtonR2", config.profiles[k]->keyButtonR2);
        cJSON_AddNumberToObject(pOptions, "keyButtonS1", config.profiles[k]->keyButtonS1);
        cJSON_AddNumberToObject(pOptions, "keyButtonS2", config.profiles[k]->keyButtonS2);
        cJSON_AddNumberToObject(pOptions, "keyButtonL3", config.profiles[k]->keyButtonL3);
        cJSON_AddNumberToObject(pOptions, "keyButtonR3", config.profiles[k]->keyButtonR3);
        cJSON_AddNumberToObject(pOptions, "keyButtonA1", config.profiles[k]->keyButtonA1);
        cJSON_AddNumberToObject(pOptions, "keyButtonA2", config.profiles[k]->keyButtonA2);
        cJSON_AddNumberToObject(pOptions, "keyButtonFn", config.profiles[k]->keyButtonFn);

        cJSON_AddBoolToObject(pOptions, "ledEnabled", config.profiles[k]->ledEnabled);
        cJSON_AddNumberToObject(pOptions, "ledEffect", config.profiles[k]->ledEffect);
        cJSON_AddBoolToObject(pOptions, "isDoubleColor", config.profiles[k]->isDoubleColor);
        cJSON_AddNumberToObject(pOptions, "ledColor1", config.profiles[k]->ledColor1);
        cJSON_AddNumberToObject(pOptions, "ledColor2", config.profiles[k]->ledColor2);
        cJSON_AddNumberToObject(pOptions, "ledColor3", config.profiles[k]->ledColor3);
        cJSON_AddNumberToObject(pOptions, "ledBrightness", config.profiles[k]->ledBrightness);
        cJSON_AddNumberToObject(pOptions, "ledColorCalibrateTop", config.profiles[k]->ledColorCalibrateTop);
        cJSON_AddNumberToObject(pOptions, "ledColorCalibrateBottom", config.profiles[k]->ledColorCalibrateBottom);
        cJSON_AddNumberToObject(pOptions, "ledColorCalibrateComplete", config.profiles[k]->ledColorCalibrateComplete);
        cJSON_AddNumberToObject(pOptions, "ledBrightnesssCalibrate", config.profiles[k]->ledBrightnesssCalibrate);

        for(uint8_t l = 0; l < NUM_ADC_BUTTONS; l ++) {
            cJSON* pRTProfiles = cJSON_CreateObject();
            cJSON_AddItemToObject(pOptions, "RTProfiles", pRTProfiles);

            cJSON_AddNumberToObject(pRTProfiles, "pressAccuracy", config.profiles[k]->RTProfiles[l]->pressAccuracy);
            cJSON_AddNumberToObject(pRTProfiles, "releaseAccuracy", config.profiles[k]->RTProfiles[l]->releaseAccuracy);
            cJSON_AddNumberToObject(pRTProfiles, "topDeadzone", config.profiles[k]->RTProfiles[l]->topDeadzone);
            cJSON_AddNumberToObject(pRTProfiles, "bottomDeadzone", config.profiles[k]->RTProfiles[l]->bottomDeadzone);

        }  
    }
    
    strcpy(buffer, cJSON_PrintUnformatted(pRoot));
    cJSON_Delete(pRoot);
}

/**
 * @brief 将JSON字符串转换为配置
 * 
 * @param config 
 * @param data 
 * @param dataLen 
 * @return true 
 * @return false 
 */
bool ConfigUtils::fromJSON(Config& config, char* data, size_t dataLen)
{
    char str[dataLen + 1];
    strcpy(str, data);

    printf("ConfigUtils::fromJSON begin.\n");
    // printf("%s\n", str);

    cJSON* root = cJSON_Parse(str);

    if(!root) {
        printf("ConfigUtils::fromJSON failure.\n");
        return false;
    }

    config.version = (uint32_t) (cJSON_GetObjectItem(root, "version")->valueint);
    config.mainState = (MainState) (cJSON_GetObjectItem(root, "mainState")->valueint);
    config.profileIndex = (uint8_t) (cJSON_GetObjectItem(root, "profileIndex")->valueint);
    config.isCalibrateCompleted = cJSON_GetObjectItem(root, "isCalibrateCompleted")->type == cJSON_True? true: false;


    cJSON* ADCButtonsArray = cJSON_GetObjectItem(root, "ADCButtons");
    uint8_t ADCButtonsSize = (uint8_t) cJSON_GetArraySize(ADCButtonsArray);

    for(uint8_t i = 0; i < ADCButtonsSize; i ++) {
        cJSON* ADCButtonObj = cJSON_GetArrayItem(ADCButtonsArray, i);
        free(config.ADCButtons[i]);
        ADCButton* ptr = (ADCButton*)malloc(sizeof(ADCButton));
        
        ptr->virtualPin = (uint32_t) cJSON_GetObjectItem(ADCButtonObj, "virtualPin")->valueint;
        ptr->magnettization = (double_t) cJSON_GetObjectItem(ADCButtonObj, "magnettization")->valuedouble;
        ptr->topPosition = (double_t) cJSON_GetObjectItem(ADCButtonObj, "topPosition")->valuedouble;
        ptr->bottomPosition = (double_t) cJSON_GetObjectItem(ADCButtonObj, "bottomPosition")->valuedouble;

        config.ADCButtons[i] = ptr;
    }
    
    cJSON* GPIOButtonsArray = cJSON_GetObjectItem(root, "GPIOButtons");
    uint8_t GPIOButtonsSize = (uint8_t) cJSON_GetArraySize(GPIOButtonsArray);

    for(uint8_t j = 0; j < GPIOButtonsSize; j ++) {
        free(config.GPIOButtons[j]);
        cJSON* GPIOButtonObj = cJSON_GetArrayItem(GPIOButtonsArray, j);
        GPIOButton* ptr = (GPIOButton*)malloc(sizeof(GPIOButton));
        ptr->virtualPin = (uint32_t) cJSON_GetObjectItem(GPIOButtonObj, "virtualPin")->valueint;
        config.GPIOButtons[j] = ptr;
    }

    cJSON* profilesArray = cJSON_GetObjectItem(root, "profiles");
    uint8_t profilesSize = cJSON_GetArraySize(profilesArray);

    for(uint8_t k = 0; k < profilesSize; k ++) {
        cJSON* profileObj = cJSON_GetArrayItem(profilesArray, k);
        free(config.profiles[k]);
        GamepadOptions* ptr = (GamepadOptions*)malloc(sizeof(GamepadOptions));

        strcpy(ptr->name, cJSON_GetObjectItem(profileObj, "name")->valuestring);
        ptr->enabled = cJSON_GetObjectItem(profileObj, "enabled")->type == cJSON_True ? true: false;
        ptr->inputMode = (InputMode) cJSON_GetObjectItem(profileObj, "inputMode")->valueint;
        ptr->dpadMode = (DpadMode) cJSON_GetObjectItem(profileObj, "dpadMode")->valueint;
        ptr->socdMode = (SOCDMode) cJSON_GetObjectItem(profileObj, "socdMode")->valueint;
        ptr->invertXAxis = cJSON_GetObjectItem(profileObj, "invertXAxis")->type == cJSON_True ? true: false;
        ptr->invertYAxis = cJSON_GetObjectItem(profileObj, "invertYAxis")->type == cJSON_True ? true: false;
        ptr->fourWayMode = cJSON_GetObjectItem(profileObj, "fourWayMode")->type == cJSON_True ? true: false;
        
        ptr->keyDpadUp = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadUp")->valueint;
        ptr->keyDpadDown = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadDown")->valueint;
        ptr->keyDpadLeft = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadLeft")->valueint;
        ptr->keyDpadRight = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadRight")->valueint;
        ptr->keyButtonB1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB1")->valueint;
        ptr->keyButtonB2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB2")->valueint;
        ptr->keyButtonB3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB3")->valueint;
        ptr->keyButtonB4 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB4")->valueint;
        ptr->keyButtonL1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL1")->valueint;
        ptr->keyButtonL2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL2")->valueint;
        ptr->keyButtonR1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR1")->valueint;
        ptr->keyButtonR2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR2")->valueint;
        ptr->keyButtonS1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonS1")->valueint;
        ptr->keyButtonS2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonS2")->valueint;
        ptr->keyButtonL3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL3")->valueint;
        ptr->keyButtonR3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR3")->valueint;
        ptr->keyButtonA1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonA1")->valueint;
        ptr->keyButtonA2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonA2")->valueint;
        ptr->keyButtonFn = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonFn")->valueint;

        ptr->ledEnabled = cJSON_GetObjectItem(profileObj, "ledEnabled")->type == cJSON_True ? true: false;
        ptr->ledEffect = (LEDEffect) cJSON_GetObjectItem(profileObj, "ledEffect")->valueint;
        ptr->isDoubleColor = cJSON_GetObjectItem(profileObj, "isDoubleColor")->type == cJSON_True ? true: false;
        ptr->ledColor1 = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColor1")->valueint;
        ptr->ledColor2 = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColor2")->valueint;
        ptr->ledColor3 = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColor3")->valueint;
        ptr->ledBrightness = (uint8_t) cJSON_GetObjectItem(profileObj, "ledBrightness")->valueint;
        ptr->ledColorCalibrateTop = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColorCalibrateTop")->valueint;
        ptr->ledColorCalibrateBottom = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColorCalibrateBottom")->valueint;
        ptr->ledColorCalibrateComplete = (uint32_t) cJSON_GetObjectItem(profileObj, "ledColorCalibrateComplete")->valueint;
        ptr->ledBrightnesssCalibrate = (uint8_t) cJSON_GetObjectItem(profileObj, "ledBrightnesssCalibrate")->valueint;

        for(uint8_t l = 0; l < NUM_ADC_BUTTONS; l ++) {
            cJSON* pRTProfiles = cJSON_GetObjectItem(profileObj, "RTProfiles");
            ptr->RTProfiles[l]->pressAccuracy = (double_t) cJSON_GetObjectItem(pRTProfiles, "pressAccuracy")->valuedouble;
            ptr->RTProfiles[l]->releaseAccuracy = (double_t) cJSON_GetObjectItem(pRTProfiles, "releaseAccuracy")->valuedouble;
            ptr->RTProfiles[l]->topDeadzone = (double_t) cJSON_GetObjectItem(pRTProfiles, "topDeadzone")->valuedouble;
            ptr->RTProfiles[l]->bottomDeadzone = (double_t) cJSON_GetObjectItem(pRTProfiles, "bottomDeadzone")->valuedouble;
        }

        config.profiles[k] = ptr;
    }
    
    cJSON_Delete(root);

    return true;
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
    memset(ConfigJSONBuffer, '\0', sizeof(ConfigJSONBuffer));
    result = QSPI_W25Qxx_ReadString(ConfigJSONBuffer, CONFIG_ADDR);
    if(result == QSPI_W25Qxx_OK && strchr(ConfigJSONBuffer, '\0') != NULL) {
        // printf("fromStorage strlen: %d, content: %s\n", strlen(ConfigJSONBuffer), ConfigJSONBuffer);
        return ConfigUtils::fromJSON(config, (char*) ConfigJSONBuffer, strlen(ConfigJSONBuffer));
    } else {
        printf("ConfigUtils::fromStorage - QSPI_W25Qxx_ReadString failure.\n");
        return false;
    }
}