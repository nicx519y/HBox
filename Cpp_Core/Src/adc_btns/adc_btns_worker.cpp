#include "adc_btns/adc_btns_worker.hpp"

// 定义静态成员变量
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsWorker::ADC_Values[NUM_ADC_BUTTONS];

ADCBtnsWorker::ADCBtnsWorker() {
    // 初始化按钮配置
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        buttonPtrs[i] = nullptr;
        lastTriggerDistance[i] = 0;
        lastTriggerState[i] = false;
    }
    maxDistance = 0;
}

ADCBtnsError ADCBtnsWorker::setup(const char* name) {
    if (is_dma_started) {
        return ADCBtnsError::DMA_ALREADY_STARTED;
    }

    ADC_VALUES_MAPPING.init(name);
    GamepadProfile* profile = STORAGE_MANAGER.getGamepadProfile(STORAGE_MANAGER.config.defaultProfileId);
    ADCButton* adcButtons = STORAGE_MANAGER.config.ADCButtons;

    if(profile == nullptr) {
        return ADCBtnsError::GAMEPAD_PROFILE_NOT_FOUND;
    }
    
    memset(ADC_Values, 0, sizeof(ADC_Values));
    memset(lastTriggerDistance, ADC_VALUES_MAPPING.getMaxDistance(), sizeof(lastTriggerDistance));
    memset(lastTriggerState, false, sizeof(lastTriggerState));
    
    // 初始化按钮配置
    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {

        RapidTriggerProfile* triggerConfig = &profile->triggerConfigs.triggerConfigs[i];

        buttonPtrs[i] = new ADCBtn();
        buttonPtrs[i]->virtualPin = adcButtons[i].virtualPin;
        buttonPtrs[i]->pressAccuracy = triggerConfig->pressAccuracy;
        buttonPtrs[i]->releaseAccuracy = triggerConfig->releaseAccuracy;
        buttonPtrs[i]->topDeadzone = triggerConfig->topDeadzone;
        buttonPtrs[i]->bottomDeadzone = triggerConfig->bottomDeadzone;

    }

    // 校准ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC1_CALIB_FAILED;
    }

    // 校准ADC2
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC2_CALIB_FAILED;
    }

    // 启动DMA1
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC_BUTTONS / 2) != HAL_OK) {
        return ADCBtnsError::DMA1_START_FAILED;
    }

    // 启动DMA2
    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADC_Values[NUM_ADC_BUTTONS / 2], NUM_ADC_BUTTONS / 2) != HAL_OK) {
        HAL_ADC_Stop_DMA(&hadc1);
        return ADCBtnsError::DMA2_START_FAILED;
    }

    is_dma_started = true;
    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsWorker::deinit() {
    if (!is_dma_started) {
        return ADCBtnsError::DMA_NOT_STARTED;
    }

    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);
    is_dma_started = false;

    return ADCBtnsError::SUCCESS;
}

void ADCBtnsWorker::process() {
    if (!is_dma_started) {
        return;
    }

    // 清除DMA缓存以确保读取到最新数据
    SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[0], sizeof(ADC_Values));

    bool isChanged = false;
    // 处理每个按钮
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        if (buttonPtrs[i] == nullptr) {
            continue;
        }

        // 获取当前距离
        float_t distance = ADC_VALUES_MAPPING.map(ADC_Values[i], i);
        float_t maxDistance = ADC_VALUES_MAPPING.getMaxDistance();


        // 处理触发状态
        if(lastTriggerState[i] == false) {
            if(distance < maxDistance - buttonPtrs[i]->topDeadzone
                && lastTriggerDistance[i] - distance >= buttonPtrs[i]->pressAccuracy) {
                lastTriggerState[i] = true;
                lastTriggerDistance[i] = distance;

                virtualPinMask |= buttonPtrs[i]->virtualPin; // 设置虚拟引脚掩码

                // 发送触发消息
                MC.publish(MessageId::ADC_BTNS_PRESSED, &i);
                isChanged = true;
            }
            // 更新最大行程
            if(distance > lastTriggerDistance[i]) {
                lastTriggerDistance[i] = distance;
            }
        } else if(lastTriggerState[i] == true) {
            if(distance > buttonPtrs[i]->bottomDeadzone
                && distance - lastTriggerDistance[i] >= buttonPtrs[i]->releaseAccuracy) {
                lastTriggerState[i] = false;
                lastTriggerDistance[i] = distance;

                virtualPinMask &= ~buttonPtrs[i]->virtualPin; // 清除虚拟引脚掩码
                // 发送释放消息
                MC.publish(MessageId::ADC_BTNS_RELEASED, &i);
                isChanged = true;
            }
            // 更新最大行程
            if(distance < lastTriggerDistance[i]) {
                lastTriggerDistance[i] = distance;
            }
        }
    }
    

    if(isChanged) {
        MC.publish(MessageId::ADC_BTNS_STATE_CHANGED, &virtualPinMask);
    }
}

