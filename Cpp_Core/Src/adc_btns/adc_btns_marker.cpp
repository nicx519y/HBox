#include "adc_btns/adc_btns_marker.hpp"

// 定义静态成员变量
// __attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsMarker::ADC_Values[NUM_ADC_BUTTONS];
__attribute__((section("._RAM_D1_Area"))) StepInfo ADCBtnsMarker::step_info;
__attribute__((section("._RAM_D1_Area"))) uint8_t ADCBtnsMarker::num_value_tmp;
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsMarker::value_tmp;


#define ADC_Values ADCValuesMappingUtils::ADC_Values

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
    
    ADC_VALUES_MAPPING.stopADCSamping();

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
    /********************** test begin **********************/
    // if(strcmp(id, "11") == 0) {

    //     memset(ADC_Values, 0, sizeof(ADC_Values));
    //     step_info.is_sampling = true;
    //     messageHandler = [this](const void* data) {
    //     if (data) {
    //         this->process((ADC_HandleTypeDef*)data);
    //     }
    //     };
    //     MC.subscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);

    //     ADC_VALUES_MAPPING.startADCSamping();
    // }
    /********************** test end **********************/

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
    step_info.sampling_noise = 0;
    step_info.sampling_frequency = 0;

    // 注册ADC转换完成回调
    messageHandler = [this](const void* data) {
        if (data) {
            this->process((ADC_HandleTypeDef*)data);
        }
    };
    MC.subscribe(MessageId::DMA_ADC_CONV_CPLT, messageHandler);

    ADC_VALUES_MAPPING.startADCSamping();

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

    t = HAL_GetTick();
    step_info.is_sampling = true;
    return ADCBtnsError::SUCCESS;   
}

void ADCBtnsMarker::process(ADC_HandleTypeDef *hadc) {
    // 检查hadc是否为ADC1
    if(!hadc || hadc->Instance != ADC1) {
        return;
    }

    if(!step_info.is_sampling) {
        return;
    }

    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));
    uint32_t adcValue = ADC_Values[0];

    // printf("%d\n", adcValue);

    if(adcValue == 0 || adcValue > UINT16_MAX) {
        return;
    }


    // 如果临时值已满，则步进完成

    if(num_value_tmp >= MAX_NUM_TMP_MARKING) {
        stepFinish();
    } else if(adcValue != 0) { // 否则，累加临时值
        value_tmp += adcValue;
        if(adcValue < tmpValueMin) tmpValueMin = adcValue;
        if(adcValue > tmpValueMax) tmpValueMax = adcValue;
        num_value_tmp++;
    }
}

/**
 * @brief 步进完成
 * 将临时值保存到标记值中，并重置标记器
 */
void ADCBtnsMarker::stepFinish() {
    step_info.is_sampling = false;
    // 计算平均值，double_t精度更高，round四舍五入
    step_info.values[step_info.index] = static_cast<uint32_t>(round(static_cast<double_t>(value_tmp) / static_cast<double_t>(num_value_tmp)));
    uint32_t tsf = static_cast<uint32_t>(static_cast<float_t>(num_value_tmp) / (static_cast<float_t>(HAL_GetTick() - t) / 1000.0f)); // 记录采样频率，单位Hz
    // printf("ADCBtnsMarker: stepFinish - tsf: %d\n", tsf);
    tmpSamplingFrequency += tsf; // 记录采样频率，单位Hz
    tmpSamplingNoise += (tmpValueMax - tmpValueMin); // 记录采样噪声
    step_info.index ++;

}

/**
 * @brief 标记完成
 * 将标记值保存到映射中，并重置标记器
 */

void ADCBtnsMarker::markingFinish() {
    // printf("ADCBtnsMarker: markingFinish - begin mark save. tmpSamplingFrequency: %d, step_info.length: %d\n", tmpSamplingFrequency, step_info.length);
    tmpSamplingFrequency /= step_info.length; // 记录平均采样频率，单位Hz
    tmpSamplingNoise /= step_info.length; // 记录平均采样噪声，单位mV

    ADC_VALUES_MAPPING.stopADCSamping();

    ADCBtnsError err = ADC_VALUES_MAPPING.mark(step_info.id, step_info.values, tmpSamplingNoise, tmpSamplingFrequency);

    step_info.sampling_frequency = tmpSamplingFrequency;
    step_info.sampling_noise = tmpSamplingNoise;
    step_info.is_completed = true;
    step_info.is_sampling = false;
    step_info.is_marking = false;

    tmpSamplingFrequency = 0;
    tmpSamplingNoise = 0;

    if(err != ADCBtnsError::SUCCESS) {
        printf("ADCBtnsMarker: markingFinish - mark save failed. err: %d\n", err);
        return;
    }

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
    cJSON_AddNumberToObject(json, "sampling_noise", step_info.sampling_noise);
    cJSON_AddNumberToObject(json, "sampling_frequency", step_info.sampling_frequency);


    cJSON* valuesJSON = cJSON_CreateArray();
    for(uint8_t i = 0; i < step_info.length; i++) {
        cJSON_AddItemToArray(valuesJSON, cJSON_CreateNumber(step_info.values[i]));
    }
    cJSON_AddItemToObject(json, "values", valuesJSON);

    return json;
}


