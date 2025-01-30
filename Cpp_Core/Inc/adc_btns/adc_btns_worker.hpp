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
    uint32_t virtualPin;    // 虚拟引脚
    float_t pressAccuracy;   // 按下精度
    float_t releaseAccuracy; // 释放精度
    float_t topDeadzone;     // 顶部死区
    float_t bottomDeadzone;  // 底部死区
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
        void loop();
    private:
        ADCBtnsWorker();
        bool is_dma_started = false;
        ADCBtn* buttonPtrs[NUM_ADC_BUTTONS];
        
        float_t lastTriggerDistance[NUM_ADC_BUTTONS];  // 上次触发距离
        bool lastTriggerState[NUM_ADC_BUTTONS];  // 上次触发状态
        bool triggerStatusInited = false;  // 触发状态是否已初始化
        uint32_t virtualPinMask = 0x0;  // 虚拟引脚掩码
        float_t maxDistance;  // 最大行程
};

#define ADC_BTNS_WORKER ADCBtnsWorker::getInstance()

#endif