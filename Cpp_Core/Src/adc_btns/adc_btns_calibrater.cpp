#include "adc_btns/adc_btns_calibrater.hpp"

// 定义静态成员变量
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCBtnsCalibrator::ADC_Values[NUM_ADC_BUTTONS];

ADCBtnsCalibrator::ADCBtnsCalibrator() : 
    is_dma_started(false) {
    memset(mapping_name, 0, sizeof(mapping_name));
    memset(min_value, 0, sizeof(min_value));
    memset(max_value, 0, sizeof(max_value));
}

ADCBtnsError ADCBtnsCalibrator::setup(const char* name) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    if (is_dma_started) return ADCBtnsError::DMA_ALREADY_STARTED;

    // 保存映射名称
    strncpy(mapping_name, name, sizeof(mapping_name));
    mapping_name[sizeof(mapping_name) - 1] = '\0';

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsCalibrator::start() {
    // 校准ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC1_CALIB_FAILED;
    }

    // 校准ADC2
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC2_CALIB_FAILED;
    }

    // 启动DMA1
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC1_BUTTONS) != HAL_OK) {
        return ADCBtnsError::DMA1_START_FAILED;
    }

    // 启动DMA2
    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADC_Values[NUM_ADC1_BUTTONS], NUM_ADC2_BUTTONS) != HAL_OK) {
        HAL_ADC_Stop_DMA(&hadc1);
        return ADCBtnsError::DMA2_START_FAILED;
    }

    MC.publish(MessageId::ADC_BTNS_CALIBRATOR_START, nullptr);

    is_dma_started = true;
    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsCalibrator::stop() {
    if (!is_dma_started) return ADCBtnsError::DMA_NOT_STARTED;

    // 停止DMA
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);
    is_dma_started = false;

    // 检查每个按钮的校准值差异是否足够大
    bool allButtonsValid = true;
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        uint32_t delta = (max_value[i] > min_value[i]) ? (max_value[i] - min_value[i]) : (min_value[i] - max_value[i]);
        if (delta < MIN_DIFF_ADC_VALUES_FIRST_AND_LAST) {
            allButtonsValid = false;
            break;
        }
    }

    if (!allButtonsValid) {
        MC.publish(MessageId::ADC_BTNS_CALIBRATOR_STOP_WITHOUT_FINISH, nullptr);
        return ADCBtnsError::CALIBRATION_INVALID_DATA;
    }

    // 发送校准完成消息
    struct {
        uint32_t min[NUM_ADC_BUTTONS];
        uint32_t max[NUM_ADC_BUTTONS];
    } data;
    memcpy(data.min, min_value, sizeof(min_value));
    memcpy(data.max, max_value, sizeof(max_value));
    MC.publish(MessageId::ADC_BTNS_CALIBRATOR_STOP_WITH_FINISH, &data);

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCBtnsCalibrator::process() {
    if (!is_dma_started) return ADCBtnsError::DMA_NOT_STARTED;

    // 清除DMA缓存以确保读取到最新数据
    SCB_InvalidateDCache_by_Addr((uint32_t*)&ADC_Values[0], sizeof(ADC_Values));

    // 更新最大最小值
    for (uint8_t i = 0; i < NUM_ADC_BUTTONS; i++) {
        if (ADC_Values[i] < min_value[i]) min_value[i] = ADC_Values[i];
        if (ADC_Values[i] > max_value[i]) max_value[i] = ADC_Values[i];
    }

    return ADCBtnsError::SUCCESS;
}


