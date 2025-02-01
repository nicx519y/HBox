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
    char id[16];               // 映射ID
    char name[16];              // 映射名称
    size_t length;            // 映射长度
    float_t step;               // 步长
    uint32_t originalValues[MAX_ADC_VALUES_LENGTH];
    uint32_t calibratedValues[MAX_ADC_VALUES_LENGTH];
};

struct ADCValuesMappingStore {
    uint32_t version;
    uint8_t num;
    char defaultId[16];
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
        ADCBtnsError remove(const char* id);
        ADCBtnsError rename(const char* id, const char* name);
        ADCBtnsError update(const char* id, const ADCValuesMapping& mapping);

        ADCBtnsError mark(const char* id, uint32_t* values, uint8_t length);
        ADCBtnsError calibration(const char* id, uint8_t buttonIndex, float_t firstValue, float_t lastValue);
        ADCBtnsError calibrationAll(const char* id, float_t* firstValues, float_t* lastValues);
        ADCBtnsError setDefault(const char* id);
        std::vector<ADCValuesMapping*> getMappingList();
        std::string getDefault();
        float_t map(const char* id, uint32_t value, uint8_t buttonIndex);
        float_t getMaxDistance(const char* id);
        float_t getStep(const char* id);
        uint8_t getLength(const char* id);
        int8_t findIndex(const char* id);

        char* getMappingDefaultId();
        ADCValuesMapping* getMapping(const char* id);

        bool isIncrement(const char* id);
        
    private:
        ADCValuesMappingUtils();
};

#define ADC_VALUES_MAPPING ADCValuesMappingUtils::getInstance()

#endif // __ADC_VALUES_MAPPING_HPP__