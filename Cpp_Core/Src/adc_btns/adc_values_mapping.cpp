#include "adc_btns/adc_values_mapping.hpp"

// 内存图
/*
 * QSPI Flash 内存布局 (从 ADC_VALUES_MAPPING_ADDR 开始):
 * 
 * ADCValuesMappingStore 结构体:
 * +------------------------+ 0x00
 * | 版本号 (4 bytes)      |
 * +------------------------+ 0x04
 * | 映射数量 (1 byte)     |
 * +------------------------+ 0x05
 * | 默认映射ID (16 bytes) |
 * +------------------------+ 0x15
 * | 映射数据              |
 * | - ADCValuesMapping[0] |
 * | - ADCValuesMapping[1] |
 * | ...                   |
 * +------------------------+
 */

// 存储实例
static ADCValuesMappingStore store;
__attribute__((section("._RAM_D1_Area"))) uint32_t ADCValuesMappingUtils::ADC_Values[NUM_ADC_BUTTONS];


ADCValuesMappingUtils::ADCValuesMappingUtils() {
    // 读取整个存储结构
    QSPI_W25Qxx_ReadBuffer((uint8_t*)&store, ADC_VALUES_MAPPING_ADDR, sizeof(ADCValuesMappingStore));
    
    printf("ADCValuesMappingUtils version: 0x%x\n", store.version);
    printf("ADC_MAPPING_VERSION == version: %d\n", ADC_MAPPING_VERSION == store.version);
    
    // 如果版本号不匹配，初始化整个存储
    if(store.version != ADC_MAPPING_VERSION) {
        // 擦除64K
        QSPI_W25Qxx_BufferErase(ADC_VALUES_MAPPING_ADDR, 64*1024);
        
        // 初始化存储结构
        memset(&store, 0, sizeof(ADCValuesMappingStore));
        store.version = ADC_MAPPING_VERSION;
        store.num = 0;
        strcpy(store.defaultId, "");
        
        // 写入初始化后的存储结构
        QSPI_W25Qxx_WriteBuffer((uint8_t*)&store, ADC_VALUES_MAPPING_ADDR, sizeof(ADCValuesMappingStore));
    }
}

// 保存整个存储结构到Flash
static int8_t saveStore() {
    return QSPI_W25Qxx_WriteBuffer((uint8_t*)&store, ADC_VALUES_MAPPING_ADDR, sizeof(ADCValuesMappingStore));
}

/**
 * @brief 查找映射ID的索引
 * @param id 映射ID
 * @return 映射ID的索引
 */
int8_t ADCValuesMappingUtils::findIndex(const char* id) {
    if (!id) return -1;
    
    // 遍历映射数据，检查名称
    for(uint8_t i = 0; i < store.num; i++) {
        if(strcmp(store.mapping[i].id, id) == 0) {
            return i;
        }
    }
    
    return -1;
}

/**
 * @brief 删除映射
 * @param id 映射ID
 * @return 是否删除成功
 */
ADCBtnsError ADCValuesMappingUtils::remove(const char* id) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;
    
    // 查找要删除的映射索引
    int8_t targetIdx = findIndex(id);
    if(targetIdx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    // 如果只有一个映射，则不能删除
    if(store.num <= 1) return ADCBtnsError::MAPPING_DELETE_FAILED;

    // 移动数据
    if(targetIdx < store.num - 1) {
        memmove(&store.mapping[targetIdx], 
                &store.mapping[targetIdx + 1], 
                (store.num - targetIdx - 1) * sizeof(ADCValuesMapping));
    }
    
    store.num--;
    
    // 保存更新后的存储结构
    if(saveStore() != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_DELETE_FAILED;
    }

    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 创建映射
 * @param id射ID
 * @param length 映射长度
 * @param step 步长
 * @return 是否创建成功
 */
ADCBtnsError ADCValuesMappingUtils::create(const char* name, size_t length, float_t step) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    
    // 检查映射名称是否已存在
    for(uint8_t i = 0; i < store.num; i++) {
        if(strcmp(store.mapping[i].name, name) == 0) {
            return ADCBtnsError::MAPPING_ALREADY_EXISTS;
        }
    }

    // 检查映射数量是否已满
    if(store.num >= NUM_ADC_VALUES_MAPPING) return ADCBtnsError::MAPPING_STORAGE_FULL;
    
    char id[16];
    sprintf(id, "ADC-%d", HAL_GetTick());

    // 创建新映射
    ADCValuesMapping& newMapping = store.mapping[store.num];
    memset(&newMapping, 0, sizeof(ADCValuesMapping));
    strncpy(newMapping.id, id, sizeof(newMapping.id) - 1);
    newMapping.id[sizeof(newMapping.id) - 1] = '\0';
    strncpy(newMapping.name, name, sizeof(newMapping.name) - 1);
    newMapping.name[sizeof(newMapping.name) - 1] = '\0';
    newMapping.length = length;
    newMapping.step = step;
    newMapping.samplingNoise = 0;
    newMapping.samplingFrequency = 0;
    memset(newMapping.originalValues, 0, sizeof(newMapping.originalValues));
    
    store.num++;
    
    // 如果这是第一个映射，则设置为默认映射
    if(store.num == 1) {
        strncpy(store.defaultId, id, sizeof(store.defaultId) - 1);
        store.defaultId[sizeof(store.defaultId) - 1] = '\0';
    }

    // 保存更新后的存储结构
    if(saveStore() != QSPI_W25Qxx_OK) {
        store.num--;
        return ADCBtnsError::MAPPING_CREATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCValuesMappingUtils::rename(const char* id, const char* name) {
    if (!id || !name) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;
    
    strncpy(store.mapping[idx].name, name, sizeof(store.mapping[idx].name) - 1);
    store.mapping[idx].name[sizeof(store.mapping[idx].name) - 1] = '\0';
    
    // 保存更新后的存储结构

    if(saveStore() != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCValuesMappingUtils::update(const char* id, const ADCValuesMapping& map) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;
    if (map.length == 0 || map.length > MAX_ADC_VALUES_LENGTH) return ADCBtnsError::INVALID_PARAMS;
    

    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    // printf("ADCValuesMappingUtils: update - begin update mapping.\n");
    // printf("ADCValuesMappingUtils: update - mapping id: %s, name: %s, length: %d, step: %f, samplingNoise: %d, samplingFrequency: %d\n", 
    //        map.id, map.name, map.length, map.step, map.samplingNoise, map.samplingFrequency);

    // 更新映射数据
    memcpy(&store.mapping[idx], &map, sizeof(ADCValuesMapping));

    // 保存更新后的存储结构
    if(saveStore() != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 设置默认映射
 * @param id 映射ID
 * @return 错误码
 */
ADCBtnsError ADCValuesMappingUtils::setDefault(const char* id) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;
    
    uint8_t idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;
    
    strncpy(store.defaultId, id, sizeof(store.defaultId) - 1);
    store.defaultId[sizeof(store.defaultId) - 1] = '\0';
    
    // 保存更新后的存储结构
    if(saveStore() != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 获取映射列表
 * @return 映射列表
 */
 std::vector<ADCValuesMapping*> ADCValuesMappingUtils::getMappingList() {
    std::vector<ADCValuesMapping*> mappingList;
    for(uint8_t i = 0; i < store.num; i++) {
        mappingList.push_back(&store.mapping[i]);
    }
    return mappingList;
}

/**
 * @brief 获取默认映射名称
 * @return 默认映射名称
 */
std::string ADCValuesMappingUtils::getDefault() {
    // 如果映射数量为0，则返回空字符串
    if(store.num == 0) return "";
    // 如果默认映射名称未设置，则返回第一个映射名称
    if(store.defaultId[0] == '\0') {
        return std::string(store.mapping[0].id);
    };
    return std::string(store.defaultId);

}

/**
 * @brief 获取映射JSON
 * @param name 映射名称
 * @return 映射JSON
 */
ADCValuesMapping* ADCValuesMappingUtils::getMapping(const char* id) {
    if (!id) return nullptr;

    // 查找映射
    int idx = findIndex(id);
    if(idx == -1) return nullptr;
    
    return &store.mapping[idx];
}


// 参数验证辅助函数
bool validateMarkParams(const char* id, uint32_t* values, uint8_t length) {
    if (!id || !values || length == 0 || length > MAX_ADC_VALUES_LENGTH) {
        return false;
    }
    return true;
}

ADCBtnsError ADCValuesMappingUtils::mark(const char* id, uint32_t* values, uint32_t samplingNoise, uint32_t samplingFrequency) {
    if (!id || !values || samplingNoise == 0 || samplingFrequency == 0) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    ADCValuesMapping& mapping = store.mapping[idx];
    
    // 保存原始状态用于回滚
    uint8_t oldLength = mapping.length;
    uint32_t* oldOriginValues = (uint32_t*)calloc(sizeof(mapping.originalValues), 1);
    
    if (!oldOriginValues) {
        free(oldOriginValues);
        return ADCBtnsError::MEMORY_ERROR;
    }
    
    // 保存原始数据
    memcpy(oldOriginValues, mapping.originalValues, sizeof(mapping.originalValues));
    
    // 更新数据
    mapping.samplingNoise = samplingNoise;
    mapping.samplingFrequency = samplingFrequency;
    memset(mapping.originalValues, 0, sizeof(mapping.originalValues));
    memcpy(mapping.originalValues, values, mapping.length * sizeof(uint32_t));

    // printf("ADCValuesMappingUtils: mark - begin update mapping.\n");
    // printf("ADCValuesMappingUtils: mark - mapping id: %s, name: %s, length: %d, step: %f, samplingNoise: %d, samplingFrequency: %d\n", 
    // 如果更新失败，回滚所有更改
    
    ADCBtnsError err = update(mapping.id, mapping);
    if (err != ADCBtnsError::SUCCESS) {
        memcpy(mapping.originalValues, oldOriginValues, sizeof(mapping.originalValues));
        free(oldOriginValues);
        printf("ADCValuesMappingUtils: mark - update mapping failed. err: %d\n", err);
        return err;
    }
    
    free(oldOriginValues);
    return ADCBtnsError::SUCCESS;
}


ADCBtnsError ADCValuesMappingUtils::startADCSamping() {

    this->stopADCSamping();

    // 校准ADC1
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC1_CALIB_FAILED;
    }

    // 启动DMA1
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], NUM_ADC1_BUTTONS) != HAL_OK) {
        return ADCBtnsError::DMA1_START_FAILED;
    }


    // 校准ADC2
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return ADCBtnsError::ADC2_CALIB_FAILED;
    }

    // 启动DMA2
    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADC_Values[NUM_ADC1_BUTTONS], NUM_ADC2_BUTTONS) != HAL_OK) {
        return ADCBtnsError::DMA2_START_FAILED;
    }

    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCValuesMappingUtils::stopADCSamping() {
    if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        return ADCBtnsError::DMA1_STOP_FAILED;
    }

    if(HAL_ADC_Stop_DMA(&hadc2) != HAL_OK) {
        return ADCBtnsError::DMA2_STOP_FAILED;
    }

    return ADCBtnsError::SUCCESS;
}   








