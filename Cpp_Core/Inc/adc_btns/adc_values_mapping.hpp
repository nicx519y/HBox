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

struct ADCValuesMapping {
    char name[16];              // 映射名称
    size_t length;            // 映射长度
    float_t step;               // 步长
    uint32_t originalValues[MAX_ADC_VALUES_LENGTH];
    uint32_t calibratedValues[MAX_ADC_VALUES_LENGTH];
};

struct ADCValuesMappingStore {
    uint32_t version;
    uint8_t num;
    char defaultName[16];
    ADCValuesMapping mapping[NUM_ADC_VALUES_MAPPING];
};


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

        ADCBtnsError mark(const char* name, uint32_t* values, uint8_t length);
        ADCBtnsError calibration(const char* name, uint8_t buttonIndex, float_t firstValue, float_t lastValue);
        ADCBtnsError calibrationAll(const char* name, float_t* firstValues, float_t* lastValues);
        ADCBtnsError setDefault(const char* name);
        std::vector<std::string> getMappingNameList();
        std::string getDefault();
        float_t map(const char* name, uint32_t value, uint8_t buttonIndex);
        float_t getMaxDistance(const char* name);
        float_t getStep(const char* name);
        uint8_t getLength(const char* name);
        int8_t findIndex(const char* name);


        char* getMappingDefaultName();
        ADCValuesMapping* getMapping(const char* name);

        bool isIncrement(const char* name);
        
    private:
        ADCValuesMappingUtils();
};

#define ADC_VALUES_MAPPING ADCValuesMappingUtils::getInstance()

#endif // __ADC_VALUES_MAPPING_HPP__