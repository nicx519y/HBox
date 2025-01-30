#ifndef __ADC_BTNS_MARKER_HPP__
#define __ADC_BTNS_MARKER_HPP__

#include <stdio.h>
#include "stm32h7xx.h"
#include "constant.hpp"
#include "adc_values_mapping.hpp"
#include "storagemanager.hpp"
#include "adc.h"
#include "message_center.hpp"
#include "adc_btns_error.hpp"

#define MAX_NUM_TMP_MARKING 100

// 步进信息结构体
struct StepInfo {
    char mapping_name[16];
    float_t step;
    uint8_t length;
    uint8_t index;
    uint32_t values[MAX_ADC_VALUES_LENGTH];
    bool is_marking;
    bool is_completed;
    bool is_sampling;
};

class ADCBtnsMarker {
    public:
        ADCBtnsMarker(ADCBtnsMarker const&) = delete;
        void operator=(ADCBtnsMarker const&) = delete;
        static ADCBtnsMarker& getInstance() {
            static ADCBtnsMarker instance;
            return instance;
        }
        ADCBtnsError setup(const char* mapping_name);
        ADCBtnsError step();
        
        void loop();
        void reset();
        uint32_t* getCurrentMarkingValues();
        
        // 添加新的公共访问方法
        StepInfo& getStepInfo() { return step_info; }
        cJSON* getStepInfoJSON();

        // 将 ADC_Values 移到 public 部分并声明为 static
        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC_Values[NUM_ADC_BUTTONS];

    private:
        ADCBtnsMarker();
        void stepFinish();
        void markingFinish();
        uint32_t value_tmp;     // 临时值和
        uint32_t num_value_tmp;  // 临时值个数 
        StepInfo step_info;
};

#define ADC_BTNS_MARKER ADCBtnsMarker::getInstance()

#endif // __ADC_BTNS_MARKER_HPP__