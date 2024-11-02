#include "adc_btns_manager.hpp"
#include "storagemanager.hpp"
#include "adc.h"
#include <stdio.h>

ADCBtnsManager::ADCBtnsManager():
    btns(Storage::getInstance().getADCButtonOptions())
{}

/**
 * @brief  通过配置 初始化计算所需要的模拟值
 * 
 */
void ADCBtnsManager::setup()
{

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC1_BUTTONS) != HAL_OK) {
        printf("ADCBtnsManager: DMA1 start fail. \n");
    } else {
        printf("ADCBtnsManager: DMA1 start success.\n");
    }

    HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    if(HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADC_Values[NUM_ADC1_BUTTONS], NUM_ADC2_BUTTONS) != HAL_OK) {
        printf("ADCBtnsManager: DMA2 start fail. \n");
    } else {
        printf("ADCBtnsManager: DMA2 start success.\n");
    }

    HAL_Delay(100);

    switch(this->state) {
        case ADCButtonManagerState::WORKING:
            this->virtualPinMask = 0x0;
            SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));

            for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
                ADCButton* btn = btns[i];
                
                if(btn->ready) {
                    ADC_btnStates[i] = ADCButtonState::READY;
                } else {
                    ADC_btnStates[i] = ADCButtonState::NOT_READY;
                }

                ADC_topValues[i] = (int32_t) btn->topValue;
                ADC_bottomValues[i] = (int32_t) btn->bottomValue;
                double_t valuePerMicron = (double)((ADC_bottomValues[i] - ADC_topValues[i]) / btn->keyTravel);
                ADC_pressAccValues[i] = (int32_t) round((double_t)btn->pressAccuracy * valuePerMicron);
                ADC_releaseAccValues[i] = (int32_t) (- round((double_t)btn->releaseAccuracy * valuePerMicron));
                ADC_topDeadZoneValues[i] = (int32_t) round((double_t)btn->topDeadzone * valuePerMicron);
                ADC_bottomDeadZoneValues[i] = (int32_t) (- round((double_t)btn->bottomDeadzone * valuePerMicron)); 
                ADC_virtualPinMasks[i] = (uint32_t) (1 << btn->virtualPin);
                ADC_lastTriggerValues[i] = (int32_t) ADC_Values[i];

                uint16_t t = (int32_t) abs((int32_t)ADC_Values[i] - ADC_bottomValues[i]);

                // 如果当前adc值在bottom值附近，则认为目前是按下状态 否则是弹起状态
                if(t <= ADC_VOLATILITY) {
                    ADC_lastActionValues[i] = 1;
                    this->virtualPinMask |= ADC_virtualPinMasks[i];
                } else {
                    ADC_lastActionValues[i] = 0;
                }
            }

            break;
        default:

            break;
    }
}

void ADCBtnsManager::deinit()
{
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);
    memset(&ADC_Values[0], 0, sizeof(ADC_Values));
    memset(&ADC_btnStates[0], ADCButtonState::NOT_READY, sizeof(ADC_btnStates));
    memset(&ADC_topValues[0], 0, sizeof(ADC_topValues));
    memset(&ADC_bottomValues[0], 0, sizeof(ADC_bottomValues));
    memset(&ADC_pressAccValues[0], 0, sizeof(ADC_pressAccValues));
    memset(&ADC_releaseAccValues[0], 0, sizeof(ADC_releaseAccValues));
    memset(&ADC_topDeadZoneValues[0], 0, sizeof(ADC_topDeadZoneValues));
    memset(&ADC_bottomDeadZoneValues[0], 0, sizeof(ADC_bottomDeadZoneValues));
    memset(&ADC_virtualPinMasks[0], 0, sizeof(ADC_virtualPinMasks));
    memset(&ADC_lastTriggerValues[0], 0, sizeof(ADC_lastTriggerValues));
    memset(&ADC_lastActionValues[0], 0, sizeof(ADC_lastActionValues));
    memset(&ADC_timesOfCalibrate[0], 0, sizeof(ADC_timesOfCalibrate));
    memset(&ADC_tmp[0], 0, sizeof(ADC_tmp));
    this->virtualPinMask = 0x0;

}

/**
 * @brief 判断按下和回弹
 * 
 */
inline void __attribute__((always_inline)) ADCBtnsManager::process()
{   
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));

    switch(this->state) {
        case ADCButtonManagerState::WORKING:
            // 判断是否触发 按下 或者 回弹
            for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
                
                if(ADC_btnStates[i] != ADCButtonState::READY) continue;

                int32_t value = (int32_t) ADC_Values[i];
                
                // 按下
                if(ADC_lastActionValues[i] == 0) {
                    if((ADC_pressAccValues[i] > 0 
                        && value >= ADC_lastTriggerValues[i] + ADC_pressAccValues[i] 
                        && value > ADC_topValues[i] + ADC_topDeadZoneValues[i])
                        || 
                        (ADC_pressAccValues[i] <= 0 
                        && value <= ADC_lastTriggerValues[i] + ADC_pressAccValues[i] 
                        && value < ADC_topValues[i] + ADC_topDeadZoneValues[i])) {
                        
                        ADC_lastTriggerValues[i] = value;
                        ADC_lastActionValues[i] = 1;
                        this->virtualPinMask |= ADC_virtualPinMasks[i];

                    } else if((ADC_pressAccValues[i] > 0 && value < ADC_lastTriggerValues[i])
                        || (ADC_pressAccValues[i] <= 0 && value > ADC_lastTriggerValues[i])) {

                        ADC_lastTriggerValues[i] = value;
                    }
                // 弹起
                } else {
                    if((ADC_releaseAccValues[i] < 0 
                        && value <= ADC_lastTriggerValues[i] + ADC_releaseAccValues[i] 
                        && value < ADC_bottomValues[i] + ADC_bottomDeadZoneValues[i])
                        || 
                        (ADC_releaseAccValues[i] >= 0 
                        && value >= ADC_lastTriggerValues[i] + ADC_releaseAccValues[i]
                        && value > ADC_bottomValues[i] + ADC_bottomDeadZoneValues[i])) {

                        ADC_lastTriggerValues[i] = value;
                        ADC_lastActionValues[i] = 0;
                        this->virtualPinMask &= ~ ADC_virtualPinMasks[i];

                    } else if((ADC_releaseAccValues[i] < 0 && value > ADC_lastTriggerValues[i])
                        || (ADC_releaseAccValues[i] >= 0 && value < ADC_lastTriggerValues[i])) {

                        ADC_lastTriggerValues[i] = value;

                    }
                }
            }
            break;
        case ADCButtonManagerState::CALIBRATING:

            for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {

                switch(ADC_btnStates[i]) {
                    case ADCButtonState::NOT_READY:
                        ADC_btnStates[i] = ADCButtonState::CALIBRATING_TOP;
                        break;
                    case ADCButtonState::CALIBRATING_TOP:
                        // 如果当前读取的adc value跟之前比跳动过大，则校准失败。需要连续到相近的adc values才算标定成功
                        if(ADC_timesOfCalibrate[i] > 0 && abs((int32_t)ADC_Values[i] - (int32_t) round((double_t)ADC_tmp[i-1]/(double_t)ADC_timesOfCalibrate[i])) > ADC_VOLATILITY) {
                            // 校准失败 这个按钮重新校准
                            ADC_tmp[i] = 0;
                            ADC_timesOfCalibrate[i] = 0;
                        } else {
                            ADC_tmp[i] += (int32_t)ADC_Values[i] ;
                            ADC_timesOfCalibrate[i] ++;
                        }
                        
                        if(ADC_timesOfCalibrate[i] >= TIMES_ADC_CALIBRATION) {
                            // 校准成功 ADC_topValues[i]
                            ADC_topValues[i] = (int32_t) round((double_t)ADC_tmp[i] / (double_t)TIMES_ADC_CALIBRATION);

                            ADC_btnStates[i] = ADCButtonState::CALIBRATING_BOTTOM;
                            ADC_tmp[i] = 0;
                            ADC_timesOfCalibrate[i] = 0;
                        }

                        break;
                    case ADCButtonState::CALIBRATING_BOTTOM:

                        // 如果当前读取的adc value跟之前比跳动过大，则校准失败。需要连续到相近的adc values才算标定成功
                        if(ADC_timesOfCalibrate[i] > 0 && abs((int32_t)ADC_Values[i] - (int32_t)abs((double_t) ADC_tmp[i-1] / (double_t)ADC_timesOfCalibrate[i])) > ADC_VOLATILITY) {
                            // 校准失败 这个按钮重新校准
                            ADC_tmp[i] = 0;
                            ADC_timesOfCalibrate[i] = 0;
                        } else {
                            ADC_tmp[i] += (int32_t) ADC_Values[i];
                            ADC_timesOfCalibrate[i] ++;
                        }

                        if(ADC_timesOfCalibrate[i] >= TIMES_ADC_CALIBRATION) {
                            // 校准成功
                            ADC_bottomValues[i] = (int32_t) round((double_t)ADC_tmp[i] / (double_t)TIMES_ADC_CALIBRATION);

                            ADC_btnStates[i] = ADCButtonState::READY;
                            ADC_tmp[i] = 0;
                            ADC_timesOfCalibrate[i] = 0;
                        }

                        break;
                    default:    // READY

                        break;
                }
            }

            HAL_Delay(DELAY_ADC_CALIBRATION);

            break;
        default:

            break;
    }
}

void ADCBtnsManager::setState(const ADCButtonManagerState state)
{
    switch(state) {
        case ADCButtonManagerState::WORKING:
            if(this->state == ADCButtonManagerState::WORKING) return;
            this->btnsConfigSave();
            this->state = ADCButtonManagerState::WORKING;
            this->deinit();
            this->setup();
            break;
        case ADCButtonManagerState::CALIBRATING:
            if(this->state == ADCButtonManagerState::CALIBRATING) return;
            this->state = ADCButtonManagerState::CALIBRATING;
            this->deinit();
            this->setup();
            break;
        default:
            break;
    }
}

ADCButtonManagerState ADCBtnsManager::getState()
{
    return this->state;
}

ADCButtonState* ADCBtnsManager::getButtonStates()
{
    return this->ADC_btnStates;
}

void ADCBtnsManager::btnsConfigSave()
{
    for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
        this->btns[i]->ready = this->ADC_btnStates[i] == ADCButtonState::READY? true: false;
        this->btns[i]->topValue = (uint16_t) this->ADC_topValues[i];
        this->btns[i]->bottomValue = (uint16_t) this->ADC_bottomValues[i];
    }
    Storage::getInstance().save();
}




