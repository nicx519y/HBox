#ifndef __ADC_VALUES_MAPPING_HPP__
#define __ADC_VALUES_MAPPING_HPP__

#include "qspi-w25q64.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <array>
#include "stm32h7xx.h"
#include "stm32h750xx.h"
#include "constant.hpp"
#include "adc_btns_error.hpp"
#include "cJSON.h"
#include "adc.h"
#include "message_center.hpp"

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

// 采样统计相关成员，每个ADC一个
struct ADCBufferInfo {
    uint32_t* buffer;
    uint32_t size;
    const uint8_t* indexMap;
    uint8_t count;
};

// 每个通道的统计信息
struct ADCChannelStats {
    uint8_t adcIndex;
    uint32_t samplingFreq;  // 采样频率
    uint32_t averageValue;  // 采样均值
    uint32_t count;         // 采样次数
    uint32_t minValue;      // 最小值
    uint32_t maxValue;      // 最大值
    uint32_t startTime;     // 开始时间
    uint32_t endTime;       // 结束时间
};

class ADCManager {
    public:
        ADCManager(ADCManager const&) = delete;
        void operator=(ADCManager const&) = delete;
        
        static ADCManager& getInstance() {
            static ADCManager instance;
            return instance;    
        }

        // 获取映射索引
        int8_t findMappingIndex(const char* id);

        // 获取映射
        ADCValuesMapping* getMapping(const char* id);

        // 获取映射列表
        std::vector<ADCValuesMapping*> getMappingList();

        // 创建映射
        ADCBtnsError createADCMapping(const char* name, size_t length, float_t step);

        // 删除映射
        ADCBtnsError removeADCMapping(const char* id);

        // 重命名映射
        ADCBtnsError renameADCMapping(const char* id, const char* name);

        // 更新映射
        ADCBtnsError updateADCMapping(const char* id, const ADCValuesMapping& mapping);

        // 标记映射
        ADCBtnsError markMapping(const char* id, uint32_t* values, uint32_t samplingNoise, uint32_t samplingFrequency);

        // 设置默认映射
        ADCBtnsError setDefaultMapping(const char* id);

        // 获取默认映射
        std::string getDefaultMapping();

        // 开始采样
        ADCBtnsError startADCSamping(bool enableSamplingRate = false, uint8_t buttonIndex = 0, uint32_t samplingCountMax = 0);

        // 停止采样
        ADCBtnsError stopADCSamping();

        // 读取ADC值
        inline const std::array<uint16_t, NUM_ADC_BUTTONS> readADCValues() {
            std::array<uint16_t, NUM_ADC_BUTTONS> values;
            auto info = adcBufferInfo;
            for(uint8_t i = 0; i < NUM_ADC; i++) {
                SCB_CleanInvalidateDCache_by_Addr(info[i].buffer, info[i].size);
                for(uint8_t j = 0; j < info[i].count; j++) {
                    values[info[i].indexMap[j]] = (uint16_t)info[i].buffer[j];
                }
            }
            return values;
        }

        /**
         * @brief 读取指定按钮的ADC值
         * @param buttonIndex 按钮索引
         * @return 指定按钮的ADC值
         */
        inline const uint32_t readADCValue(uint8_t buttonIndex) {
            auto indexInfo = findADCIndex(buttonIndex);  // 现在可以直接调用静态函数
            if(indexInfo.first == -1) {
                return 0;
            }
            const auto& info = adcBufferInfo[indexInfo.first];
            SCB_CleanInvalidateDCache_by_Addr(info.buffer, info.size);
            return info.buffer[indexInfo.second];
        }
        

    private:
        ADCManager();


        // ADC DMA 缓冲区必须保持静态
        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC1_Values[NUM_ADC1_BUTTONS];
        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC2_Values[NUM_ADC2_BUTTONS];
        static __attribute__((section("._RAM_D3_Area"))) uint32_t ADC3_Values[NUM_ADC3_BUTTONS];

        MessageHandler messageHandler;

        // 非静态成员变量
        ADCValuesMappingStore store;
        ADCBufferInfo adcBufferInfo[NUM_ADC];
        ADCChannelStats ADCButtonStats;
        bool samplingRateEnabled;
        uint32_t samplingCountMax;
        std::pair<uint8_t, uint8_t> samplingADCIndex;

        void handleADCStats(ADC_HandleTypeDef *hadc);
        int8_t saveStore();
        std::pair<uint8_t, uint8_t> findADCIndex(uint8_t buttonIndex);
};

#define ADC_MANAGER ADCManager::getInstance()

#endif // __ADC_VALUES_MAPPING_HPP__