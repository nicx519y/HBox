#ifndef __ADC_VALUES_MAPPING_HPP__
#define __ADC_VALUES_MAPPING_HPP__

#include "qspi-w25q64.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include "stm32h7xx.h"
#include "stm32h750xx.h"
#include "constant.hpp"
#include "adc_btns_error.hpp"
#include "cJSON.h"

struct 
ADCValuesMapping {
    char name[16];              // 映射名称
    size_t length;            // 映射长度
    float_t step;               // 步长
    uint32_t originalValues[MAX_ADC_VALUES_LENGTH];
    uint32_t calibratedValues[MAX_ADC_VALUES_LENGTH];
};

#define MAX_NUM_MARKING_VALUE 100 // 每个step最大采集值个数

class ADCValuesMappingUtils {
    public:
        ADCValuesMappingUtils(ADCValuesMappingUtils const&) = delete;
        void operator=(ADCValuesMappingUtils const&) = delete;
        
        static ADCValuesMappingUtils& getInstance() {
            static ADCValuesMappingUtils instance;
            return instance;    
        }

        ADCBtnsError create(const char* name, size_t length, float_t step);
        ADCBtnsError remove(const char* name);
        ADCBtnsError update(const char* name, const ADCValuesMapping& mapping);

        ADCBtnsError init(const char* name);
        
        ADCBtnsError mark(uint32_t* values, uint8_t length);
        ADCBtnsError calibration(uint8_t buttonIndex, float_t firstValue, float_t lastValue);
        ADCBtnsError calibrationAll(float_t* firstValues, float_t* lastValues);
        ADCBtnsError setDefault(const char* name);
        std::vector<std::string> getMappingNameList();
        std::string getDefault();
        float_t map(uint32_t value, uint8_t buttonIndex);
        float_t getMaxDistance();
        float_t getStep();
        uint8_t getLength();
        int8_t findIndex(const char* name);


        char* getMappingDefaultName();
        cJSON* getMappingJSON(const char* name);

        bool isIncrement();
        
    private:

        ADCValuesMappingUtils() = default;
        ADCValuesMapping mapping;
        float_t maxDistance = 0;
};

#define ADC_VALUES_MAPPING ADCValuesMappingUtils::getInstance()

#endif // __ADC_VALUES_MAPPING_HPP__