#include "adc_btns/adc_btns_worker.hpp"

// // 定义静态成员变量
// __attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsWorker::ADC_Values[NUM_ADC_BUTTONS];
#define ADC1_Values ADCValuesMappingUtils::ADC1_Values
#define ADC2_Values ADCValuesMappingUtils::ADC2_Values
#define ADC3_Values ADCValuesMappingUtils::ADC3_Values

ADCBtnsWorker::ADCBtnsWorker() {
    // 初始化指针数组为 nullptr
    memset(buttonPtrs, 0, sizeof(buttonPtrs));

    // 初始化按钮配置
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        buttonPtrs[i] = new ADCBtn();  // 动态分配新对象
    }
}

/**
 * 处理ADC转换完成消息
 * @param data ADC句柄指针
 */
void ADCBtnsWorker::handleADCConvComplete(ADC_HandleTypeDef* hadc) {

    if(hadc->Instance == ADC1) {
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC1_Values[0], sizeof(ADC1_Values[0]) * NUM_ADC1_BUTTONS);
        // 清除DMA缓存以确保读取到最新数据
    } else if(hadc->Instance == ADC2) {
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC2_Values[0], sizeof(ADC2_Values[0]) * NUM_ADC2_BUTTONS);
        // 清除DMA缓存以确保读取到最新数据
    } else if(hadc->Instance == ADC3) {
        SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC3_Values[0], sizeof(ADC3_Values[0]) * NUM_ADC3_BUTTONS);
        // 清除DMA缓存以确保读取到最新数据
    } else {
        return;
    }
    

    if(hadc->Instance == ADC1) {
        printf("ADC1:\n");
        printf("ADC1:%d\n", ADC1_Values[0]);
        printf("ADC1:%d\n", ADC1_Values[1]);
    } else if(hadc->Instance == ADC2) {
        printf("ADC2:\n");
        printf("ADC2:%d\n", ADC2_Values[0]);
        printf("ADC2:%d\n", ADC2_Values[1]);
    } else if(hadc->Instance == ADC3) {
        printf("ADC3:\n");
        printf("ADC3:%d\n", ADC3_Values[0]);
        printf("ADC3:%d\n", ADC3_Values[1]);   
    }

    for(uint8_t i = 0; i < NUM_ADC1_BUTTONS; i++) {
        if(ADC1_Values[i] == 0 || ADC1_Values[i] > UINT16_MAX) {
            continue;
        }

        // 判断按钮是否触发
        this->buttonWorking(i);

        // 实时动态校准
        this->dynamicCalibADC(i);
    }
    
    if(buttonTriggerStatusChanged) {
        MC.publish(MessageId::ADC_BTNS_STATE_CHANGED, &this->virtualPinMask);
        buttonTriggerStatusChanged = false;
    }
}

ADCBtnsError ADCBtnsWorker::setup() {

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

    
    memset(ADC1_Values, 0, sizeof(ADC1_Values)); // DMA缓存清零  
    memset(ADC2_Values, 0, sizeof(ADC2_Values)); // DMA缓存清零  
    memset(ADC3_Values, 0, sizeof(ADC3_Values)); // DMA缓存清零  
    
    // 初始化按钮配置
    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {

        RapidTriggerProfile* triggerConfig = &profile->triggerConfigs.triggerConfigs[i];
        // 初始化按钮配置
        buttonPtrs[i]->virtualPin = adcButtons[i].virtualPin;
        buttonPtrs[i]->pressAccuracy = triggerConfig->pressAccuracy;
        buttonPtrs[i]->releaseAccuracy = triggerConfig->releaseAccuracy;
        buttonPtrs[i]->topDeadzone = triggerConfig->topDeadzone;
        buttonPtrs[i]->bottomDeadzone = triggerConfig->bottomDeadzone;

        // 校准参数
        buttonPtrs[i]->initCompleted = false;
        // 初始化状态
        buttonPtrs[i]->lastTriggerIndex = 0;
        buttonPtrs[i]->lastSearchIndex = 0;
        buttonPtrs[i]->isPressed = false;
        buttonPtrs[i]->movingDirection = DIRECTION_RELEASING; // 初始化移动方向为弹起
        // 初始化按钮映射
        memcpy(buttonPtrs[i]->valueMapping, this->mapping->originalValues, this->mapping->length * sizeof(uint16_t));

    }

    messageHandler = [this](const void* data) {
        if (data) {
            this->handleADCConvComplete((ADC_HandleTypeDef*)data);
        }
    };
    MC.subscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);

    ADC_VALUES_MAPPING.startADCSamping();

    printf("ADCBtnsWorker::setup success. startADCSamping\n");

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsWorker::deinit() {
    // 取消注册回调
    if(messageHandler) {
        MC.unsubscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);
        messageHandler = nullptr;
    }

    ADC_VALUES_MAPPING.stopADCSamping();

    return ADCBtnsError::SUCCESS;
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
 * 根据当前搜索按钮值在映射数组中的位置，返回索引
 * @param buttonIndex 按钮索引
 * @param value ADC输入值
 * @return 返回在映射数组中的索引位置（最大值为 length-2）
 */
uint8_t ADCBtnsWorker::searchIndexInMapping(uint8_t buttonIndex, uint16_t value) {
    if (!buttonPtrs[buttonIndex] || !this->mapping) {
        return 0;
    }

    ADCBtn* btn = buttonPtrs[buttonIndex];
    uint8_t maxIndex = this->mapping->length - 2;  // 新增：定义最大索引值

    // 处理边界情况
    if (value >= btn->valueMapping[0]) {
        btn->lastSearchIndex = 0;
        return 0;  // 如果值大于等于最大值，返回0
    }
    if (value <= btn->valueMapping[maxIndex + 1]) {
        btn->lastSearchIndex = maxIndex;
        return maxIndex;  // 如果值小于等于最小值，返回最大允许索引
    }

    // 先检查上次搜索位置
    uint8_t lastIndex = btn->lastSearchIndex;
    
    if (lastIndex < maxIndex) {  // 修改边界检查
        // 检查值是否在上次位置的区间内
        if (value <= btn->valueMapping[lastIndex] && 
            value > btn->valueMapping[lastIndex + 1]) {
            return lastIndex;
        }
        // 检查相邻区间
        if (lastIndex > 0 && value <= btn->valueMapping[lastIndex - 1] && 
            value > btn->valueMapping[lastIndex]) {
            btn->lastSearchIndex = lastIndex - 1;
            return lastIndex - 1;
        }
        if (lastIndex < maxIndex - 1 &&  // 修改边界检查
            value <= btn->valueMapping[lastIndex + 1] && 
            value > btn->valueMapping[lastIndex + 2]) {
            btn->lastSearchIndex = lastIndex + 1;
            return lastIndex + 1;
        }
    }

    // 如果不在附近区间，进行二分查找
    uint8_t left = 0;
    uint8_t right = maxIndex;  // 修改右边界

    while (left <= right) {
        uint8_t mid = left + (right - left) / 2;

        // 如果找到精确匹配
        if (value == btn->valueMapping[mid]) {
            btn->lastSearchIndex = mid;
            return mid;
        }

        // 如果在两个值之间
        if (mid > 0 && value <= btn->valueMapping[mid - 1] && 
            value > btn->valueMapping[mid]) {
            btn->lastSearchIndex = mid - 1;
            return mid - 1;
        }

        if (value > btn->valueMapping[mid]) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    // 确保返回值不超过最大索引
    uint8_t result = right > maxIndex ? maxIndex : right;
    btn->lastSearchIndex = result;
    return result;
}

/**
 * 动态校准ADC
 * 初始化校准
 * 实时动态校准，在按钮出现反弹时，更新firstValueWindow 以及 lastValueWindow，并用滑动窗口的平均值更新valueMapping，减小误差
 * lastValue处于磁距远端，对噪音比较敏感，所以做了减去noise的处理
 * @param buttonIndex 按钮索引
 */
void ADCBtnsWorker::dynamicCalibADC(uint8_t buttonIndex) {
    // 获取当前按钮的ADC值
    // uint16_t currentValue = ADC1_Values[buttonIndex];
    // ADCBtn* btn = buttonPtrs[buttonIndex];

    // if(!buttonPtrs[buttonIndex]->initCompleted) {
    //     buttonPtrs[buttonIndex]->initCompleted = true;
    //     uint16_t noise = this->mapping->samplingNoise;
    //     // 初始化按钮映射
    //     uint16_t lastValue = currentValue + noise;
    //     uint16_t firstValue = currentValue + this->mapping->originalValues[0] - this->mapping->originalValues[this->mapping->length - 1];

    //     this->updateButtonMapping(btn->valueMapping, firstValue, lastValue);
    // } else {
    //     uint16_t noise = this->mapping->samplingNoise;
    //     if(currentValue > btn->valueMapping[0]) {
    //         this->updateButtonMapping(btn->valueMapping, currentValue, btn->valueMapping[this->mapping->length - 1]);
    //     } else if(currentValue < btn->valueMapping[this->mapping->length - 1] - noise) {
    //         this->updateButtonMapping(btn->valueMapping, btn->valueMapping[0], currentValue + noise);
    //     }

    // }
}

/**
 * 按钮工作
 * @param buttonIndex 按钮索引 判断按钮是否触发，如果触发，改变buttonTriggerStatusChanged
 */
void ADCBtnsWorker::buttonWorking(uint8_t buttonIndex) {

    // if(!buttonPtrs[buttonIndex]->initCompleted) {
    //     return;
    // }

    // // 获取当前按钮的ADC值
    // ADCBtn* btn = buttonPtrs[buttonIndex];
    // uint8_t searchIndex = this->searchIndexInMapping(buttonIndex, ADC_Values[buttonIndex]);
    
    // // if(buttonIndex == 0) {
    // //     printf("buttonIndex: %d, ADC_Values[buttonIndex]: %d, searchIndex: %d, firstValue: %d, lastValue: %d\n",     
    // //         buttonIndex, ADC_Values[buttonIndex], searchIndex, buttonPtrs[buttonIndex]->valueMapping[0], buttonPtrs[buttonIndex]->valueMapping[this->mapping->length - 1]);
    // // }

    // // if(buttonIndex == 0) {
    // //     printf("btn->isPressed: %d, btn->lastTriggerIndex: %d, searchIndex: %d\n", btn->isPressed, btn->lastTriggerIndex, searchIndex);
    // // }

    // if(!btn->isPressed) {
    //     if(searchIndex < btn->lastTriggerIndex){
    //         btn->isPressed = true;
    //         buttonTriggerStatusChanged = true;
    //         this->virtualPinMask |= (1 << btn->virtualPin);
    //         // if(buttonIndex == 0) {
    //         //     printf("btn->isPressed: %d\n", btn->isPressed);
    //         // }

    //     }
    // } else {
    //     if(searchIndex > btn->lastTriggerIndex) {
    //         btn->isPressed = false;
    //         buttonTriggerStatusChanged = true;
    //         this->virtualPinMask &= ~(1 << btn->virtualPin);
    //         // if(buttonIndex == 0) {
    //         //     printf("btn->isPressed: %d\n", btn->isPressed);
    //         // }
    //     }
    // }

    // // if(buttonIndex == 0) {
    // //     printBinary("virtualPinMask: ", this->virtualPinMask);
    // // }
    
    // btn->lastTriggerIndex = searchIndex;
}

ADCBtnsError ADCBtnsWorker::test() {
    
    messageHandler = [this](const void* data) {
        ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)data;
        // if(hadc->Instance == ADC1) {
        //     SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC1_Values[0], sizeof(ADC1_Values[0]) * NUM_ADC1_BUTTONS);
        //     printf("ADC1_Values[0]: %d, ADC1_Values[1]: %d\n", ADC1_Values[0], ADC1_Values[1]);
        // }  
        // if(hadc->Instance == ADC2) {
        //     SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC2_Values[0], sizeof(ADC2_Values[0]) * NUM_ADC2_BUTTONS);
        //     printf("ADC2_Values[0]: %d, ADC2_Values[1]: %d\n", ADC2_Values[0], ADC2_Values[1]);
        // }
        if(hadc->Instance == ADC3) {
            SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC3_Values[0], sizeof(ADC3_Values[0]) * NUM_ADC3_BUTTONS);
            printf("ADC3_Values[0]: %d, ADC3_Values[1]: %d\n", ADC3_Values[0], ADC3_Values[1]);
        }
    };
    MC.subscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);

    ADC_VALUES_MAPPING.startADCSamping();

    return ADCBtnsError::SUCCESS;
}

ADCBtnsWorker::~ADCBtnsWorker() {
    // 释放动态分配的内存
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        if (buttonPtrs[i] != nullptr) {
            delete buttonPtrs[i];
            buttonPtrs[i] = nullptr;
        }
    }
}
