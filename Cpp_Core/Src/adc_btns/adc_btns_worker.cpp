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
void ADCBtnsWorker::loop() {
    std::array<uint16_t, NUM_ADC_BUTTONS> adcValues = ADC_MANAGER.readADCValues();

    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        // 获取按钮对象
        ADCBtn* btn = buttonPtrs[i];
        if(!btn || !btn->initCompleted) {
            continue;
        }

        // 获取当前ADC值和对应的行程索引
        uint16_t adcValue = adcValues[i];
        if(adcValue == 0 || adcValue > UINT16_MAX) {
            continue;
        }
        
        uint8_t currentIndex = this->searchIndexInMapping(i, adcValue);
        
        // 检测按钮状态变化
        if(!btn->isPressed) {
            // 当前未按下，检测是否需要标记为按下
            // 当索引小于上次触发索引时，表示按钮被按下（因为0代表完全按下）
            if(currentIndex < btn->lastTriggerIndex) {
                btn->isPressed = true;
                buttonTriggerStatusChanged = true;
                this->virtualPinMask |= (1u << btn->virtualPin);
                
                ADC_DEBUG_PRINT("Button %d pressed, index: %d -> %d\n", 
                    i, btn->lastTriggerIndex, currentIndex);
            }
        } else {
            // 当前已按下，检测是否需要标记为释放
            // 当索引大于上次触发索引时，表示按钮被释放
            if(currentIndex > btn->lastTriggerIndex) {
                btn->isPressed = false;
                buttonTriggerStatusChanged = true;
                this->virtualPinMask &= ~(1u << btn->virtualPin);
                
                ADC_DEBUG_PRINT("Button %d released, index: %d -> %d\n", 
                    i, btn->lastTriggerIndex, currentIndex);
            }
        }
        
        // 更新上次触发索引
        btn->lastTriggerIndex = currentIndex;
    }

    // 如果按钮状态有变化，发送消息通知
    if(buttonTriggerStatusChanged) {
        MC.publish(MessageId::ADC_BTNS_STATE_CHANGED, &this->virtualPinMask);
        buttonTriggerStatusChanged = false;
    }
}

ADCBtnsError ADCBtnsWorker::setup() {

    std::string id = ADC_MANAGER.getDefaultMapping();
    if(id.empty()) {
        return ADCBtnsError::MAPPING_NOT_FOUND;
    }

    GamepadProfile* profile = STORAGE_MANAGER.getGamepadProfile(STORAGE_MANAGER.config.defaultProfileId);
    ADCButton* adcButtons = STORAGE_MANAGER.config.ADCButtons;
    ADCValuesMapping* mapping = ADC_MANAGER.getMapping(id.c_str());

    if(profile == nullptr) {
        return ADCBtnsError::GAMEPAD_PROFILE_NOT_FOUND;
    }

    if(mapping == nullptr) {
        return ADCBtnsError::MAPPING_NOT_FOUND;
    }

    this->mapping = mapping;
    
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

    ADC_MANAGER.startADCSamping();

    ADC_DEBUG_PRINT("ADCBtnsWorker::setup success. startADCSamping\n");

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsWorker::deinit() {
    ADC_MANAGER.stopADCSamping();
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
    ADCBtn* btn = buttonPtrs[buttonIndex];
    if(!btn || !mapping) {
        return 0;
    }

    // 处理边界情况
    if(value <= btn->valueMapping[0]) {
        return 0;
    }
    if(value >= btn->valueMapping[mapping->length - 1]) {
        return mapping->length - 1;
    }

    // 二分查找最接近的索引
    uint8_t left = 0;
    uint8_t right = mapping->length - 1;

    while(left <= right) {
        uint8_t mid = (left + right) / 2;
        uint16_t midValue = btn->valueMapping[mid];

        if(value == midValue) {
            return mid;
        }
        
        if(value < midValue) {
            if(mid == 0 || value > btn->valueMapping[mid - 1]) {
                // 找到最接近的值
                return (value - btn->valueMapping[mid - 1] < midValue - value) ? 
                       (mid - 1) : mid;
            }
            right = mid - 1;
        } else {
            if(mid == mapping->length - 1 || value < btn->valueMapping[mid + 1]) {
                // 找到最接近的值
                return (btn->valueMapping[mid + 1] - value < value - midValue) ? 
                       (mid + 1) : mid;
            }
            left = mid + 1;
        }
    }

    return left;
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

ADCBtnsError ADCBtnsWorker::test() {
    
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
