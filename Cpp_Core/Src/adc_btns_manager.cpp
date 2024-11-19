#include "adc_btns_manager.hpp"
#include "storagemanager.hpp"
#include "adc.h"
#include <stdio.h>
#include "constant.hpp"

// 声明ADC DMA的内存地址，为了避免自动分配到DTCMRAM(DTCMRAM直连CPU，DMA不能访问)，所以为变量指定了内存区域为指向AXISRAM的地址
__RAM_Area__ static uint32_t ADC_Values[NUM_ADC_BUTTONS];

__DTCMRAM_Area__ static ADCButtonState ADC_btnStates[NUM_ADC_BUTTONS];
__DTCMRAM_Area__ static uint8_t ADC_timesOfCalibrate[NUM_ADC_BUTTONS];
__DTCMRAM_Area__ static double_t ADC_tmp[NUM_ADC_BUTTONS];
__DTCMRAM_Area__ static double_t ADC_lastTriggerPositions[NUM_ADC_BUTTONS];
__DTCMRAM_Area__ static bool ADC_lastActions[NUM_ADC_BUTTONS];

ADCBtnsManager::ADCBtnsManager():
    btns((&Storage::getInstance().config)->ADCButtons)
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

    HAL_Delay(50);

    switch(this->state) {
        // 工作状态
        case ADCButtonManagerState::WORKING:
            this->virtualPinMask = 0x0;
            memset(&ADC_lastTriggerPositions[0], 0, sizeof(ADC_lastTriggerPositions));
            memset(&ADC_lastActions[0], false, sizeof(ADC_lastActions));

            for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
                ADCButton* btn = btns[i];
                
                if(btn->magnettization > 0) {
                    ADC_btnStates[i] = ADCButtonState::READY;
                } else {
                    ADC_btnStates[i] = ADCButtonState::NOT_READY;
                }
            }

            break;
        // 校准状态
        case ADCButtonManagerState::CALIBRATING:
            this->calibrate_t = 0;
            memset(&ADC_timesOfCalibrate[0], 0, sizeof(ADC_timesOfCalibrate));
            memset(&ADC_tmp[0], 0, sizeof(ADC_tmp));
            break;
        default:

            break;
    }
}

void ADCBtnsManager::deinit()
{
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_Stop_DMA(&hadc2);

    HAL_Delay(50);
}


Mask_t ADCBtnsManager::read()
{
    if(this->state == ADCButtonManagerState::WORKING) {

        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));
        // 遍历所有按钮
        for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {
            
            // 如果当前按钮状态时未校准 则不读取
            if(btns[i]->magnettization <= 0) continue;

            // 获取当前磁场强度
            double_t value = (double_t) abs((int32_t)ADC_Values[i] - (int32_t)MAGNETIC_BASE_VALUE);
            // 获取当前测试点位置
            double_t pos = find_distance_for_axial_field(
                MAGNETIC_TICKNESS,
                MAGNETIC_RADIUS,
                btns[i]->magnettization, 
                value
            );
            
            // 按下
            if(ADC_lastActions[i] == false) {
                // 如果当前测试点位置和上次测试点位置的差值大于等于按键的按下精度，则认为按键按下
                if(ADC_lastTriggerPositions[i] - pos >= btns[i]->pressAccuracy && pos < btns[i]->topPosition - btns[i]->topDeadzone) {
                    ADC_lastTriggerPositions[i] = pos;
                    ADC_lastActions[i] = true;
                    this->virtualPinMask |= btns[i]->virtualPin;
                // 如果当前测试点位置和上次测试点位置的差值大于0，则更新测试点位置
                } else if(pos - ADC_lastTriggerPositions[i] > 0) {
                    ADC_lastTriggerPositions[i] = pos;
                }
            // 弹起
            } else {
                // 如果当前测试点位置和上次测试点位置的差值大于等于按键的回弹精度，则认为按键弹起
                if(pos - ADC_lastTriggerPositions[i] >= btns[i]->releaseAccuracy && pos > btns[i]->bottomPosition + btns[i]->bottomDeadzone) {
                    ADC_lastTriggerPositions[i] = pos;
                    ADC_lastActions[i] = false;
                    this->virtualPinMask &= ~ btns[i]->virtualPin;
                // 如果当前测试点位置和上次测试点位置的差值大于0，则更新测试点位置
                } else if(ADC_lastTriggerPositions[i] - pos > 0) {
                    ADC_lastTriggerPositions[i] = pos;
                }
            }
        }

        return this->virtualPinMask;

    } else {
        return (Mask_t) 0x0;
    }
}

void ADCBtnsManager::calibrate()
{
    if(this->state == ADCButtonManagerState::CALIBRATING) {

        #if TIMES_ADC_CALIBRATION > 0

        #if DELAY_ADC_CALIBRATION > 0
        
        if(this->calibrate_t == 0 || HAL_GetTick() - this->calibrate_t >= DELAY_ADC_CALIBRATION) {
            this->calibrate_t = HAL_GetTick();
        } else {  // 不到校准间隔不执行校准
            return;
        }

        #endif //DELAY_ADC_CALIBRATION

        int32_t value = 0;
        double_t vars[2];

        // 刷新ADC值
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));

        for(uint8_t i = 0; i < NUM_ADC_BUTTONS; i ++) {

            value = abs((int32_t) ADC_Values[i] - (int32_t) MAGNETIC_BASE_VALUE);

            switch(ADC_btnStates[i]) {
                case ADCButtonState::NOT_READY:
                    ADC_btnStates[i] = ADCButtonState::CALIBRATING_TOP;
                    ADC_tmp[i] = 0;
                    ADC_timesOfCalibrate[i] = 0;
                    break;
                case ADCButtonState::CALIBRATING_TOP:
                    // 如果当前读取的adc value跟之前比跳动过大，则校准失败。需要连续到相近的adc values才算标定成功
                    if(ADC_timesOfCalibrate[i] > 0 && abs(value - (int32_t) round(ADC_tmp[i-1]/(double_t)ADC_timesOfCalibrate[i])) > ADC_VOLATILITY) {
                        // 校准失败 这个按钮重新校准
                        ADC_tmp[i] = 0;
                        ADC_timesOfCalibrate[i] = 0;
                    } else {
                        ADC_tmp[i] += (double_t) value ;
                        ADC_timesOfCalibrate[i] ++;
                    }
                    
                    if(ADC_timesOfCalibrate[i] >= TIMES_ADC_CALIBRATION) {
                        // 校准成功 ADC_topPositions[i]
                        btns[i]->topPosition = round(ADC_tmp[i] / (double_t)ADC_timesOfCalibrate[i]);
                        ADC_btnStates[i] = ADCButtonState::CALIBRATING_BOTTOM;
                        ADC_tmp[i] = 0;
                        ADC_timesOfCalibrate[i] = 0;
                    }

                    break;
                case ADCButtonState::CALIBRATING_BOTTOM:

                    // 如果当前读取的adc value 和 topValue相差过小 并且跟之前的值比跳动过大，则校准失败。需要连续到相近的adc values才算标定成功
                    if(abs((double_t)value - btns[i]->topPosition) < ADC_VOLATILITY
                        || (ADC_timesOfCalibrate[i] > 0 && abs(value - (int32_t)round(ADC_tmp[i-1]/(double_t)ADC_timesOfCalibrate[i])) > ADC_VOLATILITY)) {
                        // 校准失败 这个按钮重新校准
                        ADC_tmp[i] = 0;
                        ADC_timesOfCalibrate[i] = 0;
                    } else {
                        ADC_tmp[i] += (double_t) value;
                        ADC_timesOfCalibrate[i] ++;
                    }

                    if(ADC_timesOfCalibrate[i] >= TIMES_ADC_CALIBRATION) {
                        // 校准成功
                        btns[i]->bottomPosition = round(ADC_tmp[i] / (double_t)ADC_timesOfCalibrate[i]);

                        newton_raphson(vars, btns[i]->topPosition, btns[i]->bottomPosition, MAGNETIC_TICKNESS, MAGNETIC_RADIUS, MAGNETIC_DISTANCE);
                        btns[i]->magnettization = vars[0];

                        ADC_btnStates[i] = ADCButtonState::READY;
                        ADC_tmp[i] = 0;
                        ADC_timesOfCalibrate[i] = 0;
                    }

                    break;
                default:    // READY
                    
                    break;
            }
        }

        #endif //TIMES_ADC_CALIBRATION
    }
}

void ADCBtnsManager::setState(const ADCButtonManagerState state)
{
    if(state == this->state) return;
    this->state = state;
    this->deinit();
    this->setup();
}

ADCButtonState* ADCBtnsManager::getButtonStates()
{
    return ADC_btnStates;
}





