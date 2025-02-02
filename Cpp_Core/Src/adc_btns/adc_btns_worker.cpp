#include "adc_btns/adc_btns_worker.hpp"

// 定义静态成员变量
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsWorker::ADC_Values[NUM_ADC_BUTTONS];

ADCBtnsWorker::ADCBtnsWorker() {
    // 初始化按钮配置
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        buttonPtrs[i] = new ADCBtn();
    }
}

ADCBtnsError ADCBtnsWorker::setup() {
    if (is_dma_started) {
        return ADCBtnsError::DMA_ALREADY_STARTED;
    }

    std::string id = ADC_VALUES_MAPPING.getDefault();
    if(id.empty()) {
        return ADCBtnsError::MAPPING_NOT_FOUND;
    }

    GamepadProfile* profile = STORAGE_MANAGER.getGamepadProfile(STORAGE_MANAGER.config.defaultProfileId);
    ADCButton* adcButtons = STORAGE_MANAGER.config.ADCButtons;
    ADCValuesMapping* mapping = ADC_VALUES_MAPPING.getMapping(id.c_str());

    if(profile == nullptr) {
        return ADCBtnsError::GAMEPAD_PROFILE_NOT_FOUND;
    }

    if(mapping == nullptr) {
        return ADCBtnsError::MAPPING_NOT_FOUND;
    }

    this->mapping = mapping;
    this->valueDistance = (int32_t)mapping->originalValues[mapping->length - 1] - (int32_t)mapping->originalValues[0];
    this->samplingTimes = (int32_t) mapping->samplingFrequency / 500;         // 一个方向上，至少连续采样 1000 / 500 = 2 ms，samplingTimes代表连续采样的次数
    this->samplingNoise = (int32_t) mapping->samplingNoise;                   // 噪声阈值
    
    memset(ADC_Values, 0, sizeof(ADC_Values)); // DMA缓存清零  
    
    // 初始化按钮配置
    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {

        RapidTriggerProfile* triggerConfig = &profile->triggerConfigs.triggerConfigs[i];
        // 初始化按钮配置
        buttonPtrs[i]->virtualPin = adcButtons[i].virtualPin;
        buttonPtrs[i]->pressAccuracy = triggerConfig->pressAccuracy;
        buttonPtrs[i]->releaseAccuracy = triggerConfig->releaseAccuracy;
        buttonPtrs[i]->topDeadzone = triggerConfig->topDeadzone;
        buttonPtrs[i]->bottomDeadzone = triggerConfig->bottomDeadzone;
        buttonPtrs[i]->lastTriggerState = false; // 当前的状态是弹起
        buttonPtrs[i]->lastTriggerDistance = (this->mapping->length - 1) * this->mapping->step;

        // 校准参数
        buttonPtrs[i]->firstValue = 0;
        buttonPtrs[i]->lastValue = 0;
        buttonPtrs[i]->initCompleted = false;
        buttonPtrs[i]->tmpValue = 0;
        buttonPtrs[i]->tmpDirection = 0;
        buttonPtrs[i]->tmpNumSameDirection = 0;

        // 初始化滑动窗口
        memset(buttonPtrs[i]->firstValueWindow, 0, sizeof(buttonPtrs[i]->firstValueWindow));
        memset(buttonPtrs[i]->lastValueWindow, 0, sizeof(buttonPtrs[i]->lastValueWindow));
        buttonPtrs[i]->firstValueWindowIndex = 0;
        buttonPtrs[i]->lastValueWindowIndex = 0;

        // 初始化按钮映射
        memcpy(buttonPtrs[i]->valueMapping, this->mapping->originalValues, this->mapping->length * sizeof(uint16_t));
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

void ADCBtnsWorker::buttonWorking(ADC_HandleTypeDef *hadc) {
    if (!hadc || !buttonPtrs || !this->mapping) {
        return;
    }

    uint8_t start;
    uint8_t length;
    if(hadc->Instance == ADC1) {
        start = 0;
        length = NUM_ADC1_BUTTONS;
        // 清除DMA缓存以确保读取到最新数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[0], sizeof(ADC_Values[0]) * NUM_ADC1_BUTTONS);
    } else if(hadc->Instance == ADC2) {
        start = NUM_ADC1_BUTTONS;
        length = NUM_ADC2_BUTTONS;
        // 清除DMA缓存以确保读取到最新数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[NUM_ADC1_BUTTONS], sizeof(ADC_Values[0]) * NUM_ADC2_BUTTONS);
    } else {
        return;
    }

    bool isChanged = false;
    float_t maxDistance = this->mapping->step * (this->mapping->length - 1);
    
    // 只处理当前ADC对应的按钮
    for (uint8_t i = start; i < start + length; i++) {
        if (buttonPtrs[i] == nullptr) {
            continue;
        }

        // 获取当前距离
        float_t distance = searchButtonDistance(buttonPtrs[i], ADC_Values[i]);

        // 处理触发状态
        if(buttonPtrs[i]->lastTriggerState == false) {
            if(distance < maxDistance - buttonPtrs[i]->topDeadzone
                && buttonPtrs[i]->lastTriggerDistance - distance >= buttonPtrs[i]->pressAccuracy) {
                buttonPtrs[i]->lastTriggerDistance = distance;
                buttonPtrs[i]->lastTriggerState = true;

                virtualPinMask |= buttonPtrs[i]->virtualPin; // 设置虚拟引脚掩码

                // 发送触发消息
                MC.publish(MessageId::ADC_BTNS_PRESSED, &i);
                isChanged = true;
            } else if(distance > buttonPtrs[i]->lastTriggerDistance) {
                buttonPtrs[i]->lastTriggerDistance = distance;
            }
        } else if(buttonPtrs[i]->lastTriggerState == true) {
            if(distance > buttonPtrs[i]->bottomDeadzone
                && distance - buttonPtrs[i]->lastTriggerDistance >= buttonPtrs[i]->releaseAccuracy) {
                buttonPtrs[i]->lastTriggerState = false;
                buttonPtrs[i]->lastTriggerDistance = distance;

                virtualPinMask &= ~buttonPtrs[i]->virtualPin; // 清除虚拟引脚掩码
                // 发送释放消息
                MC.publish(MessageId::ADC_BTNS_RELEASED, &i);
                isChanged = true;
            } else if(distance < buttonPtrs[i]->lastTriggerDistance) {
                buttonPtrs[i]->lastTriggerDistance = distance;
            }
        }
    }

    if(isChanged) {
        MC.publish(MessageId::ADC_BTNS_STATE_CHANGED, &virtualPinMask);
    }
}


int32_t int32Min(int32_t a, int32_t b) {
    return a < b ? a : b;
}

int32_t int32Max(int32_t a, int32_t b) {
    return a > b ? a : b;
}

/**
 * 计算窗口内的平均值
 */
int32_t calculateWindowAverage(int32_t* window) {
    int64_t sum = 0;
    for(uint8_t i = 0; i < NUM_WINDOW_SIZE; i++) {
        sum += window[i];
    }
    return (int32_t)(sum / NUM_WINDOW_SIZE);
}

/**
 * 更新滑动窗口
 */
void updateWindow(int32_t* window, uint8_t* index, int32_t value) {
    window[*index] = value;
    *index = (*index + 1) % NUM_WINDOW_SIZE;
}

/**
 * 更新按钮映射
 * @param mapping uint16_t数组指针，用于存储映射值
 * @param firstValue 新的起始值
 * @param lastValue 新的结束值
 */
void ADCBtnsWorker::updateButtonMapping(uint16_t* mapping, uint16_t firstValue, uint16_t lastValue) {
    if (!mapping || !this->mapping || firstValue == lastValue) {
        return;
    }

    // 计算原始范围和新范围
    int32_t oldRange = (int32_t)this->mapping->originalValues[this->mapping->length - 1] - 
                      (int32_t)this->mapping->originalValues[0];
    int32_t newRange = (int32_t)lastValue - (int32_t)firstValue;

    // 防止除零
    if (oldRange == 0) {
        return;
    }

    // 对每个值进行线性映射
    for (size_t i = 0; i < this->mapping->length; i++) {
        // 计算原始值在原范围内的相对位置 (0.0 到 1.0)
        float_t relativePosition = ((float_t)((int32_t)this->mapping->originalValues[i] - 
                                            (int32_t)this->mapping->originalValues[0])) / oldRange;
        
        // 使用相对位置计算新范围内的值
        int32_t newValue = (int32_t)(firstValue + (relativePosition * newRange));
        
        // 确保值在uint16_t范围内
        newValue = newValue < 0 ? 0 : (newValue > UINT16_MAX ? UINT16_MAX : newValue);
        
        // 更新映射值
        mapping[i] = (uint16_t)newValue;
    }
}

/**
 * 根据当前搜索按钮行程，线性插值
 * @param btn ADCBtn指针，包含映射数组和上次搜索位置
 * @param value 输入值
 * @return 返回距离
 */
float_t ADCBtnsWorker::searchButtonDistance(ADCBtn* btn, uint16_t value) {
    if (!btn || !this->mapping) {
        return 0.0f;
    }

    uint16_t* mapping = btn->valueMapping;

    // 处理边界情况
    if (value <= mapping[0]) {
        btn->lastSearchIndex = 0;
        return 0.0f;
    }
    if (value >= mapping[this->mapping->length - 1]) {
        btn->lastSearchIndex = this->mapping->length - 1;
        return this->mapping->step * (this->mapping->length - 1);
    }

    // 从上次查找位置开始搜索
    size_t left = btn->lastSearchIndex;
    size_t right = btn->lastSearchIndex + 1;

    // 如果value小于当前位置的值，向左搜索
    if (value < mapping[left]) {
        right = left;
        while (left > 0 && value < mapping[left - 1]) {
            left--;
        }
    }
    // 如果value大于下一个位置的值，向右搜索
    else if (right < this->mapping->length && value > mapping[right]) {
        left = right;
        right++;
        while (right < this->mapping->length && value > mapping[right]) {
            left = right;
            right++;
        }
    }

    // 更新上次查找位置
    btn->lastSearchIndex = left;

    // 如果找到精确匹配
    if (mapping[left] == value) {
        return this->mapping->step * left;
    }

    // 线性插值计算精确距离
    float_t segment = (float_t)(value - mapping[left]) / (mapping[right] - mapping[left]);
    float_t position = left + segment;
    
    return this->mapping->step * position;
}

/**
 * 校准ADC
 * @param hadc ADC句柄
 */
void ADCBtnsWorker::calibADC(ADC_HandleTypeDef *hadc) {
    if (!hadc || !buttonPtrs) {
        return;
    }

    uint8_t start;
    uint8_t length;
    if(hadc->Instance == ADC1) {
        start = 0;
        length = NUM_ADC1_BUTTONS;
        // 清除DMA缓存以确保读取到最新数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[0], sizeof(ADC_Values[0]) * NUM_ADC1_BUTTONS);
    } else if(hadc->Instance == ADC2) {
        start = NUM_ADC1_BUTTONS;
        length = NUM_ADC2_BUTTONS;
        // 清除DMA缓存以确保读取到最新数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[NUM_ADC1_BUTTONS], sizeof(ADC_Values[NUM_ADC1_BUTTONS]) * NUM_ADC2_BUTTONS);
    } else {
        return;
    }

    const int8_t DIRECTION_PRESSING = 1;    // 按下方向（ADC值增大）
    const int8_t DIRECTION_RELEASING = -1;  // 释放方向（ADC值减小）

    for (uint8_t i = start; i < start + length; i++) {
        if (!buttonPtrs[i]) {
            continue;
        }

        // 如果未初始化按钮
        if(buttonPtrs[i]->initCompleted == false) {
            if(buttonPtrs[i]->initTime == 0) {
                buttonPtrs[i]->initTime = HAL_GetTick();
                buttonPtrs[i]->firstValue = (int32_t)ADC_Values[i];
            } else if(HAL_GetTick() - buttonPtrs[i]->initTime > TIME_ADC_INIT) { // 1秒后结束初始化
                buttonPtrs[i]->lastValue = buttonPtrs[i]->firstValue + valueDistance;
                buttonPtrs[i]->tmpValue = buttonPtrs[i]->firstValue;
                buttonPtrs[i]->tmpDirection = (valueDistance >= 0) ? DIRECTION_PRESSING : DIRECTION_RELEASING;
                buttonPtrs[i]->tmpNumSameDirection = 0;
                buttonPtrs[i]->initCompleted = true;

                // 初始化滑动窗口
                memset(buttonPtrs[i]->firstValueWindow, buttonPtrs[i]->firstValue, sizeof(buttonPtrs[i]->firstValueWindow));
                memset(buttonPtrs[i]->lastValueWindow, buttonPtrs[i]->lastValue, sizeof(buttonPtrs[i]->lastValueWindow));
                buttonPtrs[i]->firstValueWindowIndex = 0;
                buttonPtrs[i]->lastValueWindowIndex = 0;

                // 更新按钮映射
                updateButtonMapping(buttonPtrs[i]->valueMapping, buttonPtrs[i]->firstValue, buttonPtrs[i]->lastValue);

            } else { // 不断更新firstValue，拿到最小/最大值
                if(valueDistance >= 0) {    
                    buttonPtrs[i]->firstValue = int32Min((int32_t)ADC_Values[i], buttonPtrs[i]->firstValue);
                } else {
                    buttonPtrs[i]->firstValue = int32Max((int32_t)ADC_Values[i], buttonPtrs[i]->firstValue);
                }
            }
        } else {
            // 校准，记录每次反弹的过程，在反弹时更新lastValue和lastValueWindow，以及firstValue和firstValueWindow
            int32_t v = (int32_t)ADC_Values[i];
            int32_t valueDiff = v - buttonPtrs[i]->tmpValue;

            // 检测按下方向的反弹（值突然减小）
            if(buttonPtrs[i]->tmpDirection == DIRECTION_PRESSING && valueDiff < -this->samplingNoise) {
                if(buttonPtrs[i]->tmpNumSameDirection >= this->samplingTimes){
                    updateWindow(buttonPtrs[i]->lastValueWindow, &buttonPtrs[i]->lastValueWindowIndex, buttonPtrs[i]->tmpValue);
                    buttonPtrs[i]->lastValue = calculateWindowAverage(buttonPtrs[i]->lastValueWindow);
                    updateButtonMapping(buttonPtrs[i]->valueMapping, buttonPtrs[i]->firstValue, buttonPtrs[i]->lastValue);
                }
                buttonPtrs[i]->tmpValue = v;
                buttonPtrs[i]->tmpDirection = DIRECTION_RELEASING;
                buttonPtrs[i]->tmpNumSameDirection = 1;
            } 
            // 检测释放方向的反弹（值突然增大）
            else if(buttonPtrs[i]->tmpDirection == DIRECTION_RELEASING && valueDiff > this->samplingNoise) {
                if(buttonPtrs[i]->tmpNumSameDirection >= this->samplingTimes){
                    updateWindow(buttonPtrs[i]->firstValueWindow, &buttonPtrs[i]->firstValueWindowIndex, buttonPtrs[i]->tmpValue);
                    buttonPtrs[i]->firstValue = calculateWindowAverage(buttonPtrs[i]->firstValueWindow);
                    updateButtonMapping(buttonPtrs[i]->valueMapping, buttonPtrs[i]->firstValue, buttonPtrs[i]->lastValue);
                }
                buttonPtrs[i]->tmpValue = v;
                buttonPtrs[i]->tmpDirection = DIRECTION_PRESSING;
                buttonPtrs[i]->tmpNumSameDirection = 1;
            } else {
                // 根据当前方向，只记录更大或更小的值
                if (buttonPtrs[i]->tmpDirection == DIRECTION_PRESSING) {
                    // 按下方向时只记录更大的值
                    buttonPtrs[i]->tmpValue = int32Max(v, buttonPtrs[i]->tmpValue);
                } else {
                    // 释放方向时只记录更小的值
                    buttonPtrs[i]->tmpValue = int32Min(v, buttonPtrs[i]->tmpValue);
                }
                
                if(buttonPtrs[i]->tmpNumSameDirection < this->samplingTimes){ 
                    buttonPtrs[i]->tmpNumSameDirection++;
                }
            }
        }
    }
}
