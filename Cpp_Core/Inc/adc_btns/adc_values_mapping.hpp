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
#include "adc.h"

struct ADCValuesMapping {
    char id[16];                                            // 映射ID
    char name[16];                                          // 映射名称
    size_t length;                                          // 映射长度
    float_t step;                                           // 步长
    uint16_t samplingNoise;                                 // 噪声阈值
    uint16_t samplingFrequency;                             // 采样频率
    uint32_t originalValues[MAX_ADC_VALUES_LENGTH];         // 采集原始值
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

        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC_Values[NUM_ADC_BUTTONS];

        ADCBtnsError create(const char* name, size_t length, float_t step);
        ADCBtnsError remove(const char* id);
        ADCBtnsError rename(const char* id, const char* name);
        ADCBtnsError update(const char* id, const ADCValuesMapping& mapping);

        ADCBtnsError startADCSamping();
        ADCBtnsError stopADCSamping();

        ADCBtnsError mark(const char* id, uint32_t* values, uint32_t samplingNoise, uint32_t samplingFrequency);
        ADCBtnsError setDefault(const char* id);
        std::string getDefault();
        std::vector<ADCValuesMapping*> getMappingList();
        int8_t findIndex(const char* id);

        ADCValuesMapping* getMapping(const char* id);
    private:
        ADCValuesMappingUtils();
};

#define ADC_VALUES_MAPPING ADCValuesMappingUtils::getInstance()

#endif // __ADC_VALUES_MAPPING_HPP__