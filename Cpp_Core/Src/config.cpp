#include "config.hpp"
#include "constant.hpp"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>


#define ex_config_addr      EX_CONFIG_ADDR
#define config_label        CONFIG_LABEL
#define config_version      CONFIG_VERSION


__attribute__((section("._Text_Area"))) static ADCButton defaultADCButtons[] = {
    {  0,  0 }, {  1,  1 }, {  2,  2 }, {  3,  3 },
    {  4,  4 }, {  5,  5 }, {  6,  6 }, {  7,  7 },
    {  8,  8 }, {  9,  9 }, { 10, 10 }, { 11, 11 },
    { 12, 12 }, { 13, 13 }, { 14, 14 }, { 15, 15 },
};

__attribute__((section("._Text_Area"))) static GPIOButton defaultGPIOButtons[] = {
    { (uint32_t) GPIOA, (uint16_t) GPIO_PIN_0, (uint32_t) 16 },
    { (uint32_t) GPIOB, (uint16_t) GPIO_PIN_1, (uint32_t) 17 },
    { (uint32_t) GPIOC, (uint16_t) GPIO_PIN_2, (uint32_t) 18 },
    { (uint32_t) GPIOD, (uint16_t) GPIO_PIN_3, (uint32_t) 19 },
};

__attribute__((section("._Text_Area"))) static GamepadOptions defaultProfile = {
    .inputMode      = INPUT_MODE_XINPUT,
    .dpadMode       = DPAD_MODE_DIGITAL,
    .socdMode       = SOCD_MODE_NEUTRAL,
    .invertXAxis    = false,
    .invertYAxis    = false,
    .fourWayMode    = false,
    .keyDpadUp      = (uint32_t) 0x0000000000000001,
    .keyDpadDown    = (uint32_t) 0x0000000000000001,
    .keyDpadLeft    = (uint32_t) 0x0000000000000001,
    .keyDpadRight   = (uint32_t) 0x0000000000000001,
    .keyButtonB1    = (uint32_t) 0x0000000000000001,
    .keyButtonB2    = (uint32_t) 0x0000000000000001,
    .keyButtonB3    = (uint32_t) 0x0000000000000001,
    .keyButtonB4    = (uint32_t) 0x0000000000000001,
    .keyButtonL1    = (uint32_t) 0x0000000000000001,
    .keyButtonR1    = (uint32_t) 0x0000000000000001,
    .keyButtonL2    = (uint32_t) 0x0000000000000001,
    .keyButtonR2    = (uint32_t) 0x0000000000000001,
    .keyButtonS1    = (uint32_t) 0x0000000000000001,
    .keyButtonS2    = (uint32_t) 0x0000000000000001,
    .keyButtonL3    = (uint32_t) 0x0000000000000001,
    .keyButtonR3    = (uint32_t) 0x0000000000000001,
    .keyButtonA1    = (uint32_t) 0x0000000000000001,
    .keyButtonA2    = (uint32_t) 0x0000000000000001,
    .keyButtonFn    = (uint32_t) 0x0000000000000001,
};

__attribute__((section("._Text_Area"))) static Config defaultConfig = 
{
    .profileIndex   = 0,
    .ADCButtons     = defaultADCButtons,
    .GPIOButtons    = defaultGPIOButtons,
    .profiles       = {}
};

bool ConfigUtils::load(Config& config)
{
    char versionLabel[31];
    strcpy(versionLabel, (char*) ex_config_addr);
    
    // 不是第一次load
    if(strcmp(versionLabel, config_label) == 0) {
        uint8_t cversion = *(uint8_t*) (ex_config_addr + 31);
        //...do nothing
        return fromStorage(config);
    } else {
        memcpy((char*) ex_config_addr, config_label, strlen(config_label) + 1);
        *(uint8_t*) (ex_config_addr + 31) = (uint8_t) config_version;
        //初始化配置表
        memcpy(&config, &defaultConfig, sizeof(Config));
        for(uint8_t i = 0; i < NUM_PROFILES; i ++) {
            memcpy(config.profiles[i], &defaultProfile, sizeof(GamepadOptions));
        }
        bool result = save(config);
        if(result) {
            return fromStorage(config);
        } else {
            return false;
        }
    }
    return false;
}

bool ConfigUtils::save(Config& config)
{
    char* configJSON = toJSON(config);

    if(!configJSON || strcmp(configJSON, "") == 0) {
        return false;
    }

    char* configDest = (char*) (ex_config_addr + 32);
    memcpy(configDest, configJSON, strlen(configJSON) + 1);             // 把config JSON字符串拷贝到外部flash内存映射地址
    return true;
}

char * ConfigUtils::toJSON(const Config& config)
{
    cJSON* pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "profileIndex", (double) config.profileIndex);

    cJSON* pADCButtons = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "ADCButtons", pADCButtons);

    ADCButton* ADCButtonPtr = (ADCButton*) config.ADCButtons;
    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
        cJSON* pADCButton = cJSON_CreateObject();
        cJSON_AddNumberToObject(pADCButton, "virtualPin", ADCButtonPtr[i].virtualPin || 0);
        cJSON_AddNumberToObject(pADCButton, "topValue", ADCButtonPtr[i].topValue || 0);
        cJSON_AddNumberToObject(pADCButton, "bottomValue", ADCButtonPtr[i].bottomValue || 0);
        cJSON_AddNumberToObject(pADCButton, "pressAccuracy", ADCButtonPtr[i].pressAccuracy || 0);
        cJSON_AddNumberToObject(pADCButton, "releaseAccuracy", ADCButtonPtr[i].releaseAccuracy || 0);
        cJSON_AddNumberToObject(pADCButton, "topDeadzone", ADCButtonPtr[i].topDeadzone || 0);
        cJSON_AddNumberToObject(pADCButton, "bottomDeadzone", ADCButtonPtr[i].bottomDeadzone || 0);
        cJSON_AddNumberToObject(pADCButton, "keyTravel", ADCButtonPtr[i].keyTravel || 0);
        cJSON_AddBoolToObject(pADCButton, "ready", ADCButtonPtr[i].ready || false);
        cJSON_AddItemToArray(pADCButtons, pADCButton);
    }

    cJSON* pGPIOButtons = cJSON_CreateArray();
    cJSON_AddItemToObject(pRoot, "GPIOButtons", pGPIOButtons);

    GPIOButton* GPIOButtonPtr = (GPIOButton*) config.GPIOButtons;
    for(uint8_t j = 0; j < NUM_GPIO_BUTTONS; j ++) {
        cJSON* pGPIOButton = cJSON_CreateObject();
        cJSON_AddNumberToObject(pGPIOButton, "GPIOPort", GPIOButtonPtr[j].GPIOPort || 0);
        cJSON_AddNumberToObject(pGPIOButton, "GPIOPin", GPIOButtonPtr[j].GPIOPin || 0);
        cJSON_AddNumberToObject(pGPIOButton, "virtualPin", GPIOButtonPtr[j].virtualPin || 0);
        cJSON_AddItemToArray(pGPIOButtons, pGPIOButton);
    }

    cJSON* pProfiles = cJSON_CreateObject();
    cJSON_AddItemToObject(pRoot, "profiles", pProfiles);

    GamepadOptions* profilesPtr = (GamepadOptions*) config.profiles;
    for(uint8_t k = 0; k < NUM_PROFILES; k ++) {
        cJSON* pOptions = cJSON_CreateObject();
        cJSON_AddItemToArray(pProfiles, pOptions);

        cJSON_AddNumberToObject(pOptions, "inputMode", profilesPtr[k].inputMode);
        cJSON_AddNumberToObject(pOptions, "dpadMode", profilesPtr[k].dpadMode);
        cJSON_AddNumberToObject(pOptions, "socdMode", profilesPtr[k].socdMode);
        cJSON_AddBoolToObject(pOptions, "invertXAxis", profilesPtr[k].invertXAxis);
        cJSON_AddBoolToObject(pOptions, "invertYAxis", profilesPtr[k].invertYAxis);
        cJSON_AddBoolToObject(pOptions, "fourWayMode", profilesPtr[k].fourWayMode);

        cJSON_AddNumberToObject(pOptions, "keyDpadUp", profilesPtr[k].keyDpadUp);
        cJSON_AddNumberToObject(pOptions, "keyDpadDown", profilesPtr[k].keyDpadDown);
        cJSON_AddNumberToObject(pOptions, "keyDpadLeft", profilesPtr[k].keyDpadLeft);
        cJSON_AddNumberToObject(pOptions, "keyDpadRight", profilesPtr[k].keyDpadRight);
        cJSON_AddNumberToObject(pOptions, "keyButtonB1", profilesPtr[k].keyButtonB1);
        cJSON_AddNumberToObject(pOptions, "keyButtonB2", profilesPtr[k].keyButtonB2);
        cJSON_AddNumberToObject(pOptions, "keyButtonB3", profilesPtr[k].keyButtonB3);
        cJSON_AddNumberToObject(pOptions, "keyButtonB4", profilesPtr[k].keyButtonB4);
        cJSON_AddNumberToObject(pOptions, "keyButtonL1", profilesPtr[k].keyButtonL1);
        cJSON_AddNumberToObject(pOptions, "keyButtonL2", profilesPtr[k].keyButtonL2);
        cJSON_AddNumberToObject(pOptions, "keyButtonR1", profilesPtr[k].keyButtonR1);
        cJSON_AddNumberToObject(pOptions, "keyButtonR2", profilesPtr[k].keyButtonR2);
        cJSON_AddNumberToObject(pOptions, "keyButtonS1", profilesPtr[k].keyButtonS1);
        cJSON_AddNumberToObject(pOptions, "keyButtonS2", profilesPtr[k].keyButtonS2);
        cJSON_AddNumberToObject(pOptions, "keyButtonL3", profilesPtr[k].keyButtonL3);
        cJSON_AddNumberToObject(pOptions, "keyButtonR3", profilesPtr[k].keyButtonR3);
        cJSON_AddNumberToObject(pOptions, "keyButtonA1", profilesPtr[k].keyButtonA1);
        cJSON_AddNumberToObject(pOptions, "keyButtonA2", profilesPtr[k].keyButtonA2);
        cJSON_AddNumberToObject(pOptions, "keyButtonFn", profilesPtr[k].keyButtonFn);
    }

    char* result = cJSON_Print(pRoot);

    cJSON_Delete(pRoot);

    return result;
}

bool ConfigUtils::fromJSON(Config& config, const char* data, size_t dataLen)
{
    cJSON* root = cJSON_Parse(data);

    if(!root) {
        return false;
    }
    
    config.profileIndex = (uint8_t) cJSON_GetObjectItem(root, "profileIndex")->valueint;

    cJSON* ADCButtonsArray = cJSON_GetObjectItem(root, "ADCButtons");
    uint8_t ADCButtonsSize = (uint8_t) cJSON_GetArraySize(ADCButtonsArray);

    for(uint8_t i = 0; i < ADCButtonsSize; i ++) {
        cJSON* ADCButtonObj = cJSON_GetArrayItem(ADCButtonsArray, i);

        ADCButton* button = (ADCButton *)malloc(sizeof(ADCButton));
        button->virtualPin = (uint32_t) cJSON_GetObjectItem(ADCButtonObj, "virtualPin")->valueint;
        button->topValue = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "topValue")->valueint;
        button->bottomValue = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "bottomValue")->valueint;
        button->pressAccuracy = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "pressAccuracy")->valueint;
        button->releaseAccuracy = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "releaseAccuracy")->valueint;
        button->topDeadzone = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "topDeadzone")->valueint;
        button->bottomDeadzone = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "topDeadzone")->valueint;
        button->keyTravel = (uint16_t) cJSON_GetObjectItem(ADCButtonObj, "keyTravel")->valueint;
        button->ready = cJSON_GetObjectItem(ADCButtonObj, "ready")->type == cJSON_True ? true: false;

        config.ADCButtons[i] = button;
    }
    
    cJSON* GPIOButtonsArray = cJSON_GetObjectItem(root, "GPIOButtons");
    uint8_t GPIOButtonsSize = (uint8_t) cJSON_GetArraySize(GPIOButtonsArray);

    for(uint8_t j = 0; j < GPIOButtonsSize; j ++) {
        cJSON* GPIOButtonObj = cJSON_GetArrayItem(GPIOButtonsArray, j);
        
        GPIOButton* button = (GPIOButton *) malloc(sizeof(GPIOButton));
        button->GPIOPort = (uint32_t) cJSON_GetObjectItem(GPIOButtonObj, "GPIOPort")->valueint;
        button->GPIOPin = (uint16_t) cJSON_GetObjectItem(GPIOButtonObj, "GPIOPin")->valueint;
        button->virtualPin = (uint32_t) cJSON_GetObjectItem(GPIOButtonObj, "virtualPin")->valueint;

        config.GPIOButtons[j] = button;
    }

    cJSON* profilesArray = cJSON_GetObjectItem(root, "profiles");
    uint8_t profilesSize = cJSON_GetArraySize(profilesArray);

    for(uint8_t k = 0; k < profilesSize; k ++) {
        cJSON* profileObj = cJSON_GetArrayItem(profilesArray, k);

        GamepadOptions * options = (GamepadOptions *) malloc(sizeof(GamepadOptions));
        options->inputMode = (InputMode) cJSON_GetObjectItem(profileObj, "inputMode")->valueint;
        options->dpadMode = (DpadMode) cJSON_GetObjectItem(profileObj, "dpadMode")->valueint;
        options->socdMode = (SOCDMode) cJSON_GetObjectItem(profileObj, "socdMode")->valueint;
        options->invertXAxis = cJSON_GetObjectItem(profileObj, "invertXAxis")->type == cJSON_True ? true: false;
        options->invertXAxis = cJSON_GetObjectItem(profileObj, "invertXAxis")->type == cJSON_True ? true: false;
        options->invertXAxis = cJSON_GetObjectItem(profileObj, "invertXAxis")->type == cJSON_True ? true: false;
        
        options->keyDpadUp = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadUp")->valueint;
        options->keyDpadDown = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadDown")->valueint;
        options->keyDpadLeft = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadLeft")->valueint;
        options->keyDpadRight = (uint32_t) cJSON_GetObjectItem(profileObj, "keyDpadRight")->valueint;
        options->keyButtonB1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB1")->valueint;
        options->keyButtonB2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB2")->valueint;
        options->keyButtonB3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB3")->valueint;
        options->keyButtonB4 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonB4")->valueint;
        options->keyButtonL1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL1")->valueint;
        options->keyButtonL2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL2")->valueint;
        options->keyButtonR1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR1")->valueint;
        options->keyButtonR2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR2")->valueint;
        options->keyButtonS1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonS1")->valueint;
        options->keyButtonS2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonS2")->valueint;
        options->keyButtonL3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonL3")->valueint;
        options->keyButtonR3 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonR3")->valueint;
        options->keyButtonA1 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonA1")->valueint;
        options->keyButtonA2 = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonA2")->valueint;
        options->keyButtonFn = (uint32_t) cJSON_GetObjectItem(profileObj, "keyButtonFn")->valueint;

        config.profiles[k] = options;
    }

    cJSON_Delete(root);

    return true;
}

bool ConfigUtils::fromStorage(Config& config)
{
    char* configChar;
    strcpy(configChar, (char *) (ex_config_addr + 32));
    bool result = fromJSON(config, configChar, strlen(configChar));
    free(configChar);
    return result;
}