#include "adc_btns/adc_btns_marker.hpp"

// 定义静态成员变量
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsMarker::ADC_Values[NUM_ADC_BUTTONS];
__attribute__((section("._RAM_D1_Area"))) StepInfo ADCBtnsMarker::step_info;
__attribute__((section("._RAM_D1_Area"))) uint8_t ADCBtnsMarker::num_value_tmp;
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsMarker::value_tmp;


/**
 * @brief 构造函数
 * 初始化DMA缓存，临时值，标记值，映射名称，标记状态
 * 用于初始化ADC值标记器
 */

ADCBtnsMarker::ADCBtnsMarker() {
    memset(ADC_Values, 0, sizeof(ADC_Values));
    memset(&step_info, 0, sizeof(step_info));
    value_tmp = 0;
    num_value_tmp = 0;
}

/**
 * @brief 重置ADC值标记器
 */
void ADCBtnsMarker::reset() {
    value_tmp = 0;
    num_value_tmp = 0;

    memset(&step_info, 0, sizeof(step_info));
    
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        printf("ADCValuesMarker: Failed to stop DMA\n");
    }

    // 取消订阅ADC转换完成回调
    if (messageHandler) {
        MC.unsubscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);
        messageHandler = nullptr;
    }

    // 清空DMA缓存
    memset(ADC_Values, 0, sizeof(ADC_Values));
}

/**
 * @brief 初始化ADC值标记器
 * @param mapping_name 映射名称
 */
ADCBtnsError ADCBtnsMarker::setup(const char* id) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;

    reset();

    ADCValuesMapping* mapping = ADC_VALUES_MAPPING.getMapping(id);

    if (!mapping) return ADCBtnsError::MAPPING_NOT_FOUND;

    // 初始化步进信息
    strncpy(step_info.id, id, sizeof(step_info.id) - 1);
    step_info.id[sizeof(step_info.id) - 1] = '\0';
    strncpy(step_info.mapping_name, mapping->name, sizeof(step_info.mapping_name) - 1);
    step_info.mapping_name[sizeof(step_info.mapping_name) - 1] = '\0';
    step_info.index = 0;
    step_info.length = mapping->length;
    step_info.step = mapping->step;
    memset(step_info.values, 0, sizeof(step_info.values));
    step_info.is_marking = true;
    step_info.is_completed = false;
    step_info.is_sampling = false;

    // 注册ADC转换完成回调
    messageHandler = [this](const void* data) {
        if (data) {
            this->process((ADC_HandleTypeDef*)data);
        }
    };
    MC.subscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);

    // 校准ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC1_CALIB_FAILED;
    }

    // 启动DMA1
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC1_BUTTONS) != HAL_OK) {
        return ADCBtnsError::DMA1_START_FAILED;
    }


    return ADCBtnsError::SUCCESS;
}


/**
 * @brief 步进
 * 将标记值保存到映射中，并重置标记器
 * 每次步进后，切换到下一个标记值，并重置临时值
 * 如果标记值已满，则将标记值保存到映射中，并重置标记器
 */
ADCBtnsError ADCBtnsMarker::step() {
    if(!step_info.is_marking) {
        return ADCBtnsError::NOT_MARKING;
    }

    if(step_info.is_sampling) {
        return ADCBtnsError::ALREADY_SAMPLING;
    }

    if(step_info.index >= step_info.length) {
        markingFinish();
        return ADCBtnsError::SUCCESS;
    }

    // 如果未采样，则准备采样
    value_tmp = 0;
    num_value_tmp = 0;
    tmpValueMin = UINT32_MAX;
    tmpValueMax = 0;
    tmpSamplingNoise = 0;
    tmpSamplingFrequency = 0;
    t = HAL_GetTick();
    step_info.is_sampling = true;
    return ADCBtnsError::SUCCESS;   
}

/**
 * @brief 处理ADC值
 * 将DMA值累加到临时值中，并更新临时值
 * 如果临时值已满，则将临时值保存到标记值中，并重置临时值
 */
void ADCBtnsMarker::loop() {
    //...
}

void ADCBtnsMarker::process(ADC_HandleTypeDef *hadc) {
    // 检查hadc是否为ADC1
    if(!hadc || hadc->Instance != ADC1) {
        return;
    }

    if(!step_info.is_sampling) {
        return;
    }

    // 如果临时值已满，则步进完成
    if(num_value_tmp >= MAX_NUM_TMP_MARKING) {
        stepFinish();
    } else { // 否则，累加临时值
        // printf("ADCBtnsMarker::loop value_tmp: %d, num_value_tmp: %d, ADC_Values[0]: %d\n", value_tmp, num_value_tmp, ADC_Values[0]);
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));
        // 使用64位整数进行溢出检查
        uint64_t new_value = static_cast<uint64_t>(value_tmp) + ADC_Values[0];

        if(new_value <= UINT32_MAX) {
            value_tmp = static_cast<uint32_t>(new_value);
            if(value_tmp < tmpValueMin) tmpValueMin = value_tmp;
            if(value_tmp > tmpValueMax) tmpValueMax = value_tmp;
            num_value_tmp++;
        } else {
            // 溢出处理，将溢出值保存到标记值中，并开始下一个标记，提前结束步进
            printf("ADCBtnsMarker: Value overflow detected\n");
            stepFinish();
        }
    }
}

/**
 * @brief 步进完成
 * 将临时值保存到标记值中，并重置标记器
 */
void ADCBtnsMarker::stepFinish() {

    tmpSamplingFrequency += num_value_tmp / (HAL_GetTick() - t); // 记录采样频率，单位Hz
    tmpSamplingNoise += (tmpValueMax - tmpValueMin); // 记录采样噪声

    step_info.is_sampling = false;
    // 计算平均值，double_t精度更高，round四舍五入
    step_info.values[step_info.index] = static_cast<uint32_t>(round(static_cast<double_t>(value_tmp) / static_cast<double_t>(num_value_tmp)));
    printf("ADCBtnsMarker::stepFinish value_tmp: %d, num_value_tmp: %d, step_info.index: %d, step_info.values[step_info.index]: %d\n", value_tmp, num_value_tmp, step_info.index, step_info.values[step_info.index]);
    step_info.index ++;

}

/**
 * @brief 标记完成
 * 将标记值保存到映射中，并重置标记器
 */
void ADCBtnsMarker::markingFinish() {
    step_info.is_completed = true;
    step_info.is_sampling = false;
    step_info.is_marking = false;
    step_info.values[step_info.index] = value_tmp / num_value_tmp;
    tmpSamplingFrequency /= step_info.length; // 记录平均采样频率，单位Hz
    tmpSamplingNoise /= step_info.length; // 记录平均采样噪声，单位V
    
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        printf("ADCValuesMarker: Failed to stop DMA\n");
    }

    ADC_VALUES_MAPPING.mark(step_info.mapping_name, step_info.values, step_info.length, tmpSamplingFrequency, tmpSamplingNoise);
}

uint32_t* ADCBtnsMarker::getCurrentMarkingValues() {
    return step_info.values;
}

/**
 * @brief 获取步进信息JSON
 * @return cJSON* 
 */
cJSON* ADCBtnsMarker::getStepInfoJSON() {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "id", step_info.id);
    cJSON_AddStringToObject(json, "mapping_name", step_info.mapping_name);
    cJSON_AddNumberToObject(json, "step", step_info.step);
    cJSON_AddNumberToObject(json, "length", step_info.length);
    cJSON_AddNumberToObject(json, "index", step_info.index);
    cJSON_AddBoolToObject(json, "is_marking", step_info.is_marking);
    cJSON_AddBoolToObject(json, "is_completed", step_info.is_completed);
    cJSON_AddBoolToObject(json, "is_sampling", step_info.is_sampling);

    cJSON* valuesJSON = cJSON_CreateArray();
    for(uint8_t i = 0; i < step_info.length; i++) {
        cJSON_AddItemToArray(valuesJSON, cJSON_CreateNumber(step_info.values[i]));
    }
    cJSON_AddItemToObject(json, "values", valuesJSON);

    return json;
}


