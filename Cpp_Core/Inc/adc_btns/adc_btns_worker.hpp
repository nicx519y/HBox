#ifndef __ADC_BTNS_WORKER_HPP__
#define __ADC_BTNS_WORKER_HPP__

#include <stdio.h>
#include "stm32h7xx.h"
#include "constant.hpp"
#include "adc_values_mapping.hpp"
#include "storagemanager.hpp"
#include "adc.h"
#include "message_center.hpp"
#include "adc_btns_error.hpp"
#include "ring_buffer_sliding_window.hpp"
#include "utils.h"

#define NUM_FIRST_VALUE_WINDOW_SIZE 6
#define NUM_LAST_VALUE_WINDOW_SIZE 6
#define NUM_MAPPING_INDEX_WINDOW_SIZE 32
#define DIRECTION_RELEASING -1  // 按钮释放方向（ADC值减小）
#define DIRECTION_PRESSING 1    // 按钮按下方向（ADC值增大）

// 错误码定义
enum class ADCBtnsWorkerError {
    SUCCESS = 0,
    INVALID_PARAMS = -1,        // 参数无效
    MEMORY_ERROR = -2,          // 内存错误
    ADC1_CALIB_FAILED = -3,     // ADC1校准失败
    ADC2_CALIB_FAILED = -4,     // ADC2校准失败
    DMA1_START_FAILED = -5,     // DMA1启动失败
    DMA2_START_FAILED = -6,     // DMA2启动失败
    MAPPING_ERROR = -7,         // 映射操作失败
    ALREADY_STARTED = -8,       // DMA已经启动
    NOT_STARTED = -9,           // DMA未启动
    BUTTON_CONFIG_ERROR = -10   // 按钮配置错误
};

typedef struct {
    // 按钮配置
    uint32_t virtualPin = 0;    // 虚拟引脚
    float_t pressAccuracy = 0;   // 按下精度
    float_t releaseAccuracy = 0; // 释放精度
    float_t topDeadzone = 0;     // 顶部死区
    float_t bottomDeadzone = 0;  // 底部死区

    // 校准参数
    bool initCompleted = false;     // 初始化完成
    int8_t movingDirection = 0;     // 记录运动趋势 (1: 增加, -1: 减少, 0: 初始/静止)
    uint8_t lastTriggerIndex = 0;  // 上一次触发索引
    uint8_t lastSearchIndex = 0;        // 上一次索引
    bool isPressed = false;           // 是否按下

    uint16_t valueMapping[MAX_ADC_VALUES_LENGTH] = {0};           // 值映射

} ADCBtn;

class ADCBtnsWorker {
    public:
        ADCBtnsWorker(ADCBtnsWorker const&) = delete;
        void operator=(ADCBtnsWorker const&) = delete;

        // static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC_Values[NUM_ADC_BUTTONS];

        static ADCBtnsWorker& getInstance() {
            static ADCBtnsWorker instance;
            return instance;
        }
        ADCBtnsError setup();
        ADCBtnsError deinit();
        ADCBtnsWorker();
        ~ADCBtnsWorker();

    private:
        void updateButtonMapping(uint16_t* mapping, uint16_t firstValue, uint16_t lastValue);
        uint8_t searchIndexInMapping(uint8_t buttonIndex, uint16_t value);
        void buttonWorking(uint8_t buttonIndex);
        void dynamicCalibADC(uint8_t buttonIndex);
        
        
        // 判断两个值的变化方向 (1: 增加, -1: 减少, 0: 不变)
        int8_t getChangeDirection(uint16_t current, uint16_t previous) {
            if (current > previous) return DIRECTION_PRESSING;
            if (current < previous) return DIRECTION_RELEASING;
            return 0;
        }

        // 处理ADC转换完成消息
        void handleADCConvComplete(ADC_HandleTypeDef* hadc);

        std::function<void(const void*)> messageHandler;
        ADCBtn* buttonPtrs[NUM_ADC_BUTTONS];
        uint32_t virtualPinMask = 0x0;  // 虚拟引脚掩码
        ADCValuesMapping* mapping;
        bool buttonTriggerStatusChanged = false;
        
        void resetButton(uint8_t buttonIndex) {
            if (buttonPtrs[buttonIndex]) {
                buttonPtrs[buttonIndex]->initCompleted = false;
                buttonPtrs[buttonIndex]->movingDirection = DIRECTION_RELEASING;
                buttonPtrs[buttonIndex]->isPressed = false;
                // ... 其他重置操作
            }
        }

};

#define ADC_BTNS_WORKER ADCBtnsWorker::getInstance()

#endif