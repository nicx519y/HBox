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
    memset(newMapping.originalValues, 0, sizeof(newMapping.originalValues));
    memset(newMapping.calibratedValues, 0, sizeof(newMapping.calibratedValues));
    
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

ADCBtnsError ADCValuesMappingUtils::update(const char* id, const ADCValuesMapping& mapping) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;
    if (mapping.length == 0 || mapping.length > MAX_ADC_VALUES_LENGTH) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;
    
    // 更新映射数据
    memcpy(&store.mapping[idx], &mapping, sizeof(ADCValuesMapping));
    
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

/**
 * @brief 判断映射是否是递增的
 * @return 是否递增
 */
bool ADCValuesMappingUtils::isIncrement(const char* id) {
    if (!id) return false;
    
    int idx = findIndex(id);
    if(idx == -1) return false;
    
    return store.mapping[idx].originalValues[0] < store.mapping[idx].originalValues[store.mapping[idx].length - 1];
}

/**
 * @brief 获取最大行程
 * @return 最大行程
 */
float_t ADCValuesMappingUtils::getMaxDistance(const char* id) {
    if (!id) return 0;
    
    int idx = findIndex(id);
    if(idx == -1) return 0;
    
    return store.mapping[idx].step * (store.mapping[idx].length - 1);
}

/**
 * @brief 获取步长
 * @return 步长
 */
float_t ADCValuesMappingUtils::getStep(const char* id) {
    if (!id) return 0;

    int idx = findIndex(id);
    if(idx == -1) return 0;
    
    return store.mapping[idx].step;
}

uint8_t ADCValuesMappingUtils::getLength(const char* id) {
    if (!id) return 0;
        
    int idx = findIndex(id);
    if(idx == -1) return 0;
    
    return store.mapping[idx].length;
}

/**
 * @brief 校准ADC值映射
 * 用新的首尾值校准ADC值映射，拉伸或压缩映射范围
 * @param buttonIndex 按钮索引
 * @param firstValue 新的首值
 * @param lastValue 新的尾值
 */
ADCBtnsError ADCValuesMappingUtils::calibration(const char* id, uint8_t buttonIndex, float_t firstValue, float_t lastValue) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;

    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    ADCValuesMapping& mapping = store.mapping[idx];

    // 参数验证
    if (mapping.length < 2) return ADCBtnsError::INVALID_PARAMS;
    if (buttonIndex >= NUM_ADC_BUTTONS) return ADCBtnsError::INVALID_PARAMS;  // 检查buttonIndex范围

    // 对于uint32_t类型，直接计算差值的绝对值
    uint32_t diff = (firstValue > lastValue) ? (firstValue - lastValue) : (lastValue - firstValue);
    if (diff < MIN_DIFF_ADC_VALUES_FIRST_AND_LAST) return ADCBtnsError::INVALID_PARAMS; // 防止除零错误
    
    uint32_t originalFirst = mapping.originalValues[0];
    uint32_t originalLast = mapping.originalValues[mapping.length - 1];
    if (originalFirst == originalLast) return ADCBtnsError::INVALID_PARAMS; // 防止除零错误

    // 保存原始状态用于回滚
    uint32_t* oldValues = (uint32_t*)calloc(mapping.length * sizeof(uint32_t), 1);
    if (!oldValues) return ADCBtnsError::MEMORY_ERROR;
    
    uint8_t start = buttonIndex * MAX_ADC_VALUES_LENGTH;
    if (start + mapping.length > sizeof(mapping.calibratedValues)/sizeof(mapping.calibratedValues[0])) {
        free(oldValues);
        return ADCBtnsError::INVALID_PARAMS;
    }
    
    // 保存原始值
    memcpy(oldValues, &mapping.calibratedValues[start], mapping.length * sizeof(uint32_t));
    
    // 计算校准系数
    float_t k = (float_t)(lastValue - firstValue) / (originalLast - originalFirst);
    float_t b = firstValue - k * originalFirst;
    
    // 应用校准
    for (uint8_t i = 0; i < mapping.length; i++) {
        mapping.calibratedValues[start + i] = k * mapping.originalValues[i] + b;
    }

    // 如果更新失败，回滚更改
    if (update(mapping.name, mapping) != ADCBtnsError::SUCCESS) {
        memcpy(&mapping.calibratedValues[start], oldValues, mapping.length * sizeof(uint32_t));
        return ADCBtnsError::INVALID_PARAMS;
    }
    
    free(oldValues);
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 校准所有按钮
 * @param firstValues 首值数组
 * @param lastValues 尾值数组
 */
ADCBtnsError ADCValuesMappingUtils::calibrationAll(const char* id, float_t* firstValues, float_t* lastValues) {
    if (!id) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    ADCValuesMapping& mapping = store.mapping[idx];

    // 参数验证
    if (!firstValues || !lastValues) return ADCBtnsError::INVALID_PARAMS;  // 检查空指针
    if (mapping.length < 2) return ADCBtnsError::INVALID_PARAMS;  // 检查映射长度
    
    // 检查所有按钮的校准值差值
    for (uint8_t buttonIndex = 0; buttonIndex < NUM_ADC_BUTTONS; buttonIndex++) {
        uint32_t diff = (firstValues[buttonIndex] > lastValues[buttonIndex]) ? 
                       (firstValues[buttonIndex] - lastValues[buttonIndex]) : 
                       (lastValues[buttonIndex] - firstValues[buttonIndex]);
        if (diff < MIN_DIFF_ADC_VALUES_FIRST_AND_LAST) {
            return ADCBtnsError::INVALID_PARAMS;  // 如果任一按钮的差值不满足要求,直接返回
        }
    }
    
    // 保存原始状态用于回滚
    uint32_t* oldCalibratedValues = (uint32_t*)calloc(sizeof(mapping.calibratedValues), 1);
    if (!oldCalibratedValues) return ADCBtnsError::MEMORY_ERROR;
    
    // 保存原始校准值
    memcpy(oldCalibratedValues, mapping.calibratedValues, sizeof(mapping.calibratedValues));
    
    uint32_t originalFirst = mapping.originalValues[0];
    uint32_t originalLast = mapping.originalValues[mapping.length - 1];
    if (originalFirst == originalLast) {  // 防止除零错误
        free(oldCalibratedValues);
        return ADCBtnsError::INVALID_PARAMS;
    }
    
    bool success = true;
    // 对每个按钮进行校准
    for (uint8_t buttonIndex = 0; buttonIndex < NUM_ADC_BUTTONS; buttonIndex++) {
        uint8_t start = buttonIndex * MAX_ADC_VALUES_LENGTH;
        if (start + mapping.length > sizeof(mapping.calibratedValues)/sizeof(mapping.calibratedValues[0])) {
            success = false;
            break;
        }
        
        // 计算校准系数
        float_t k = (float_t)(lastValues[buttonIndex] - firstValues[buttonIndex]) / (originalLast - originalFirst);
        float_t b = firstValues[buttonIndex] - k * originalFirst;
        
        // 应用校准
        for (uint8_t i = 0; i < mapping.length; i++) {
            mapping.calibratedValues[start + i] = k * mapping.originalValues[i] + b;
        }
    }
    
    // 只在所有按钮都校准成功后，才进行一次存储更新
    if (success && update(mapping.name, mapping) != ADCBtnsError::SUCCESS) {
        success = false;
    }
    
    // 如果失败，回滚所有更改
    if (!success) {
        memcpy(mapping.calibratedValues, oldCalibratedValues, sizeof(mapping.calibratedValues));
    }
    
    free(oldCalibratedValues);
    return ADCBtnsError::SUCCESS;
}

// 参数验证辅助函数
bool validateMarkParams(const char* id, uint32_t* values, uint8_t length) {
    if (!id || !values || length == 0 || length > MAX_ADC_VALUES_LENGTH) {
        return false;
    }
    return true;
}

ADCBtnsError ADCValuesMappingUtils::mark(const char* id, uint32_t* values, uint8_t length) {
    if (!id || !values || length == 0 || length > MAX_ADC_VALUES_LENGTH) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(id);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    ADCValuesMapping& mapping = store.mapping[idx];
    
    // 保存原始状态用于回滚
    uint8_t oldLength = mapping.length;
    uint32_t* oldOriginValues = (uint32_t*)calloc(sizeof(mapping.originalValues), 1);
    uint32_t* oldCalibratedValues = (uint32_t*)calloc(sizeof(mapping.calibratedValues), 1);
    
    if (!oldOriginValues || !oldCalibratedValues) {
        free(oldOriginValues);
        free(oldCalibratedValues);
        return ADCBtnsError::MEMORY_ERROR;
    }
    
    // 保存原始数据
    memcpy(oldOriginValues, mapping.originalValues, sizeof(mapping.originalValues));
    memcpy(oldCalibratedValues, mapping.calibratedValues, sizeof(mapping.calibratedValues));
    
    // 更新数据
    mapping.length = length;
    memset(mapping.originalValues, 0, sizeof(mapping.originalValues));
    memset(mapping.calibratedValues, 0, sizeof(mapping.calibratedValues));
    memcpy(mapping.originalValues, values, length * sizeof(uint32_t));
    memcpy(mapping.calibratedValues, values, length * sizeof(uint32_t));

    // 如果更新失败，回滚所有更改
    if (update(mapping.name, mapping) != ADCBtnsError::SUCCESS) {
        mapping.length = oldLength;
        memcpy(mapping.originalValues, oldOriginValues, sizeof(mapping.originalValues));
        memcpy(mapping.calibratedValues, oldCalibratedValues, sizeof(mapping.calibratedValues));
        free(oldOriginValues);
        free(oldCalibratedValues);
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    free(oldOriginValues);
    free(oldCalibratedValues);
    return ADCBtnsError::SUCCESS;
}









