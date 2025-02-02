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
    uint32_t virtualPin;    // 虚拟引脚
    float_t pressAccuracy;   // 按下精度
    float_t releaseAccuracy; // 释放精度
    float_t topDeadzone;     // 顶部死区
    float_t bottomDeadzone;  // 底部死区
    bool lastTriggerState;   // 上次触发状态
    float lastTriggerDistance; // 上次触发行程

    // 校准参数
    int32_t firstValue;     // 第一个值
    int32_t lastValue;      // 最后一个值
    int32_t tmpValue;       // 临时值
    uint32_t tmpNumSameDirection;     // 临时同方向采样计数
    int8_t tmpDirection;     // 临时方向
    uint32_t initTime;      // 初始化时间
    bool initCompleted;     // 初始化完成

    // 滑动窗口
    int32_t firstValueWindow[NUM_WINDOW_SIZE];    // firstValue的滑动窗口
    uint8_t firstValueWindowIndex;                // firstValueWindow的索引
    int32_t lastValueWindow[NUM_WINDOW_SIZE];     // lastValue的滑动窗口
    uint8_t lastValueWindowIndex;                 // lastValueWindow的索引

    uint16_t valueMapping[MAX_ADC_VALUES_LENGTH]; // 值映射

} ADCBtn;

class ADCBtnsWorker {
    public:
        ADCBtnsWorker(ADCBtnsWorker const&) = delete;
        void operator=(ADCBtnsWorker const&) = delete;

        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC_Values[NUM_ADC_BUTTONS];

        static ADCBtnsWorker& getInstance() {
            static ADCBtnsWorker instance;
            return instance;
        }
        ADCBtnsError setup();
        ADCBtnsError deinit();
    private:
        ADCBtnsWorker() {}
        void updateButtonMapping(uint16_t* mapping, uint16_t firstValue, uint16_t lastValue);
        float_t searchButtonDistance(uint16_t* mapping, uint16_t value);
        void buttonWorking(ADC_HandleTypeDef *hadc);
        void calibADC(ADC_HandleTypeDef *hadc);
        bool is_dma_started = false;
        ADCBtn* buttonPtrs[NUM_ADC_BUTTONS];
        uint32_t virtualPinMask = 0x0;  // 虚拟引脚掩码
        ADCValuesMapping* mapping;
        int32_t valueDistance;          // lastValue - firstValue
        int32_t samplingTimes;          // 校准用 一次方向运动至少的采样次数
        int32_t samplingNoise;          // 校准用 噪声阈值
};

#define ADC_BTNS_WORKER ADCBtnsWorker::getInstance()

#endif