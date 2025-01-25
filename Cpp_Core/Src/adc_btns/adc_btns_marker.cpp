#include "adc_btns/adc_btns_marker.hpp"

// 定义静态成员变量
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsMarker::ADC_Values[NUM_ADC_BUTTONS];

/**
 * @brief 构造函数
 * 初始化DMA缓存，临时值，标记值，映射名称，标记状态
 * 用于初始化ADC值标记器
 */

ADCBtnsMarker::ADCBtnsMarker() : 
    is_dma_started(false) {
    memset(ADC_Values, 0, sizeof(ADC_Values));
    memset(marking_value, 0, sizeof(marking_value));
    memset(mapping_name, 0, sizeof(mapping_name));
    memset(&step_info, 0, sizeof(step_info));
    value_tmp = 0;
    num_value_tmp = 0;
    marking_length = 0;
}

/**
 * @brief 重置ADC值标记器
 */
void ADCBtnsMarker::reset() {
    value_tmp = 0;
    num_value_tmp = 0;
    marking_length = 0;

    memset(marking_value, 0, sizeof(marking_value));
    memset(mapping_name, 0, sizeof(mapping_name));
    memset(&step_info, 0, sizeof(step_info));
    
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        printf("ADCValuesMarker: Failed to stop DMA\n");
    }
    is_dma_started = false;
    // 清空DMA缓存
    memset(ADC_Values, 0, sizeof(ADC_Values));
}

/**
 * @brief 初始化ADC值标记器
 * @param mapping_name 映射名称
 */
ADCBtnsError ADCBtnsMarker::setup(const char* name) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;

    reset();
    ADC_VALUES_MAPPING.init(name);


    // 保存映射名称
    strncpy(mapping_name, name, sizeof(mapping_name));
    mapping_name[sizeof(mapping_name) - 1] = '\0';


    // 初始化步进信息
    strncpy(step_info.mapping_name, mapping_name, sizeof(step_info.mapping_name) - 1);
    step_info.mapping_name[sizeof(step_info.mapping_name) - 1] = '\0';
    step_info.index = 0;
    step_info.value = 0;

    // 校准ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC1_CALIB_FAILED;
    }

    // 启动DMA1
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC1_BUTTONS) != HAL_OK) {
        return ADCBtnsError::DMA1_START_FAILED;
    }

    is_dma_started = true;

    return ADCBtnsError::SUCCESS;
}


/**
 * @brief 步进
 * 将标记值保存到映射中，并重置标记器
 * 每次步进后，切换到下一个标记值，并重置临时值
 * 如果标记值已满，则将标记值保存到映射中，并重置标记器
 */
ADCBtnsError ADCBtnsMarker::step() {
    if(step_info.is_marking) {
        return ADCBtnsError::ALREADY_MARKING;
    }

    value_tmp = 0;
    num_value_tmp = 0;
    step_info.is_marking = true;

    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 处理ADC值
 * 将DMA值累加到临时值中，并更新临时值
 * 如果临时值已满，则将临时值保存到标记值中，并重置临时值
 */
void ADCBtnsMarker::process() {
    if(!step_info.is_marking) {
        return;
    }

    // 如果标记值已满，则标记完成
    if(marking_length >= ADC_VALUES_MAPPING.getLength()) {
        markingFinish();
        return;
    }

    // 如果临时值已满，则步进完成
    if(num_value_tmp >= MAX_NUM_MARKING_VALUE) {
        stepFinish();
    } else { // 否则，累加临时值
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));
        
        // 使用64位整数进行溢出检查
        uint64_t new_value = static_cast<uint64_t>(value_tmp) + ADC_Values[0];
        if(new_value <= UINT32_MAX) {
            value_tmp = static_cast<uint32_t>(new_value);
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
    step_info.is_marking = false;

    marking_value[marking_length] = value_tmp / num_value_tmp;

    step_info.index = marking_length;
    step_info.value = marking_value[marking_length];
    MC.publish(MessageId::ADC_BTNS_MARKER_STEP_FINISH, &step_info);

    marking_length++;
}

/**
 * @brief 标记完成
 * 将标记值保存到映射中，并重置标记器
 */
void ADCBtnsMarker::markingFinish() {
    step_info.is_marking = false;
    
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        printf("ADCValuesMarker: Failed to stop DMA\n");
    }
    is_dma_started = false;

    step_info.is_completed = true;

    ADC_VALUES_MAPPING.mark(marking_value, marking_length);
    MC.publish(MessageId::ADC_BTNS_MARKER_FINISH, 0);
}

uint32_t* ADCBtnsMarker::getCurrentMarkingValues() {
    return marking_value;
}
