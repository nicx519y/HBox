#include "adc_btns/adc_values_mapping.hpp"

// 内存图
/*
 * QSPI Flash 内存布局 (从 ADC_VALUES_MAPPING_ADDR 开始):
 * 
 * +------------------------+ 0x00
 * | 映射数量 (1 byte)     |      // 当前存储的映射数量
 * +------------------------+ 0x01
 * | 默认映射名称 (16 bytes)|      // 默认映射名称
 * +------------------------+ 0x11
 * | 映射数据              |      // 所有映射的数据
 * | - ADCValuesMapping[0] |      // 第一个映射数据
 * | - ADCValuesMapping[1] |      // 第二个映射数据
 * | ...                   |
 * +------------------------+
 * 
 * 注意：
 * 1. 每个映射结构体大小固定
 * 2. calibratedValues 按按钮索引划分，每个按钮独立存储校准值
 * 3. 第n个映射的起始地址 = ADC_VALUES_MAPPING_ADDR + sizeof(uint8_t) + 16 + n*sizeof(ADCValuesMapping)
 */

// 获取映射数据的起始地址
static uint32_t getMappingDataAddr() {
    return ADC_VALUES_MAPPING_ADDR + 1 + 16;  // 跳过映射数量和默认映射名称
}

// 从存储中读取当前映射数量
static uint8_t getCurrentNum() {
    uint8_t num;
    QSPI_W25Qxx_ReadBuffer((uint8_t*)&num, ADC_VALUES_MAPPING_ADDR, sizeof(uint8_t));
    return num;
}

// 从存储中读取默认映射名称
static std::string getDefaultMappingName() {
    char name[17];
    QSPI_W25Qxx_ReadBuffer((uint8_t*)name, ADC_VALUES_MAPPING_ADDR + 1, 16);
    return std::string(name);
}

// 设置默认映射名称
static void setDefaultMappingName(const char* name) {
    QSPI_W25Qxx_WriteBuffer((uint8_t*)name, ADC_VALUES_MAPPING_ADDR + 1, 16);
}

/**
 * @brief 查找映射名称的索引
 * @param name 映射名称
 * @return 映射名称的索引
 */
int8_t ADCValuesMappingUtils::findIndex(const char* name) {
    if (!name) return -1;
    
    uint8_t num = getCurrentNum();
    if(num == 0) return -1;
    
    ADCValuesMapping mapping;
    uint32_t dataAddr = getMappingDataAddr();
    
    // 遍历映射数据，检查名称
    for(uint8_t i = 0; i < num; i++) {
        QSPI_W25Qxx_ReadBuffer((uint8_t*)&mapping, dataAddr + i * sizeof(ADCValuesMapping), sizeof(ADCValuesMapping));
        if(strcmp(mapping.name, name) == 0) {
            return i;
        }
    }
    
    return -1;
}

/**
 * @brief 删除映射
 * @param name 映射名称
 * @return 是否删除成功
 */
ADCBtnsError ADCValuesMappingUtils::remove(const char* name) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    
    // 查找要删除的映射索引
    int8_t targetIdx = findIndex(name);
    if(targetIdx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;

    // 读取所有映射数据
    uint8_t num = getCurrentNum();
    ADCValuesMapping* dataBuffer = (ADCValuesMapping*)calloc(num * sizeof(ADCValuesMapping), 1);
    if(!dataBuffer) {
        return ADCBtnsError::MEMORY_ERROR;
    }
    
    uint32_t dataAddr = getMappingDataAddr();
    QSPI_W25Qxx_ReadBuffer((uint8_t*)dataBuffer, dataAddr, num * sizeof(ADCValuesMapping));

    // 移动数据
    if(targetIdx < num - 1) {
        memmove(&dataBuffer[targetIdx], 
                &dataBuffer[targetIdx + 1], 
                (num - targetIdx - 1) * sizeof(ADCValuesMapping));
    }
    
    num--;
    
    // 原子性写入：先写数据，后写数量
    if(num > 0) {
        if(QSPI_W25Qxx_WriteBuffer((uint8_t*)dataBuffer, dataAddr, num * sizeof(ADCValuesMapping)) != QSPI_W25Qxx_OK) {
            free(dataBuffer);
            return ADCBtnsError::MAPPING_DELETE_FAILED;
        }
    }
    
    // 更新映射数量
    if(QSPI_W25Qxx_WriteBuffer((uint8_t*)&num, ADC_VALUES_MAPPING_ADDR, sizeof(uint8_t)) != QSPI_W25Qxx_OK) {
        free(dataBuffer);
        return ADCBtnsError::MAPPING_DELETE_FAILED;
    }

    free(dataBuffer);
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 创建映射
 * @param name 映射名称
 * @param length 映射长度
 * @param step 步长
 * @return 是否创建成功
 */
ADCBtnsError ADCValuesMappingUtils::create(const char* name, size_t length, float_t step) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    
    // 检查映射名称是否已存在
    if(findIndex(name) >= 0) return ADCBtnsError::MAPPING_ALREADY_EXISTS;

    // 检查映射数量是否已满
    uint8_t num = getCurrentNum();
    if(num >= NUM_ADC_VALUES_MAPPING) return ADCBtnsError::MAPPING_STORAGE_FULL;
    
    // 创建空映射数据
    ADCValuesMapping emptyMapping = {0};
    strncpy(emptyMapping.name, name, sizeof(emptyMapping.name) - 1);
    emptyMapping.name[sizeof(emptyMapping.name) - 1] = '\0';
    emptyMapping.length = length;
    emptyMapping.step = step;
    memset(emptyMapping.originalValues, 0, sizeof(emptyMapping.originalValues));
    memset(emptyMapping.calibratedValues, 0, sizeof(emptyMapping.calibratedValues));
    
    // 写入映射数据
    uint32_t dataAddr = getMappingDataAddr();
    if(QSPI_W25Qxx_WriteBuffer((uint8_t*)&emptyMapping, dataAddr + num * sizeof(ADCValuesMapping), sizeof(ADCValuesMapping)) != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_CREATE_FAILED;
    }
    
    // 更新映射数量
    num++;
    if(QSPI_W25Qxx_WriteBuffer((uint8_t*)&num, ADC_VALUES_MAPPING_ADDR, sizeof(uint8_t)) != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_CREATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

ADCBtnsError ADCValuesMappingUtils::update(const char* name, const ADCValuesMapping& mapping) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    if (mapping.length == 0 || mapping.length > MAX_ADC_VALUES_LENGTH) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(name);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;
    
    // 复制整个映射数据
    memcpy(&this->mapping, &mapping, sizeof(ADCValuesMapping));
    
    uint32_t dataAddr = getMappingDataAddr();
    if(QSPI_W25Qxx_WriteBuffer((uint8_t*)&this->mapping, dataAddr + idx * sizeof(ADCValuesMapping), sizeof(ADCValuesMapping)) != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 初始化ADC值映射
 * 如果映射不存在，则创建一个空的映射
 * @param name 映射名称
 */
ADCBtnsError ADCValuesMappingUtils::init(const char* name) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    
    int idx = findIndex(name);
    // 如果映射不存在，则创建一个空的映射
    if(idx == -1) {
        ADCValuesMapping emptyMapping = {0};
        ADCBtnsError error = create(name, sizeof(emptyMapping), 0);
        if(error != ADCBtnsError::SUCCESS) {
            return error;
        }
    }
    
    // 读取映射数据
    uint32_t dataAddr = getMappingDataAddr();
    if(QSPI_W25Qxx_ReadBuffer((uint8_t*)&mapping, dataAddr + idx * sizeof(ADCValuesMapping), sizeof(ADCValuesMapping)) != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }

    return ADCBtnsError::SUCCESS;
}


/**
 * @brief 查找ADC值映射，拟合查找
 * 要保证mapping.calibratedValues 的值是单调递增或者递减，查找范围是0-mapping.length
 * @param value 要查找的值
 * @param buttonIndex 按钮索引
 * @return 返回值对应的距离
 */
float_t ADCValuesMappingUtils::map(uint32_t value, uint8_t buttonIndex) {
    if (mapping.length == 0) return 0;
    
    // 检查buttonIndex有效性
    if (buttonIndex >= NUM_ADC_BUTTONS) return 0;
    
    uint8_t start = buttonIndex * MAX_ADC_VALUES_LENGTH;
    if (start >= sizeof(mapping.calibratedValues)/sizeof(mapping.calibratedValues[0])) return 0;
    
    uint8_t end = start + mapping.length - 1;
    if (end >= sizeof(mapping.calibratedValues)/sizeof(mapping.calibratedValues[0])) return 0;
    
    // 边界检查
    if (value <= mapping.calibratedValues[start]) return 0;
    if (value >= mapping.calibratedValues[end]) {
        return (mapping.length - 1) * mapping.step;
    }

    // 使用静态变量缓存上次查找的位置
    static uint8_t lastLeft[NUM_ADC_BUTTONS] = {0};
    static uint8_t lastRight[NUM_ADC_BUTTONS] = {0};
    
    // 初始化查找范围
    uint8_t left = lastLeft[buttonIndex];
    uint8_t right = lastRight[buttonIndex];
    
    // 检查缓存的位置是否有效
    if (left < start || right > end || left >= right) {
        left = start;
        right = end;
    }
    
    // 如果值不在缓存范围内,重置查找范围
    if (value < mapping.calibratedValues[left] || value > mapping.calibratedValues[right]) {
        left = start;
        right = end;
    }
    
    // 二分查找
    while (left < right - 1) {
        uint8_t mid = (left + right) / 2;
        if (value >= mapping.calibratedValues[mid]) {
            left = mid;
        } else {
            right = mid;
        }
    }
    
    // 更新缓存
    lastLeft[buttonIndex] = left;
    lastRight[buttonIndex] = right;

    // 线性插值
    uint32_t value1 = mapping.calibratedValues[left];
    uint32_t value2 = mapping.calibratedValues[right];
    float_t distance1 = (left - start) * mapping.step;
    float_t distance2 = (right - start) * mapping.step;
    
    return distance1 + (distance2 - distance1) * (value - value1) / (value2 - value1);
}

/**
 * @brief 校准ADC值映射
 * 用新的首尾值校准ADC值映射，拉伸或压缩映射范围
 * @param buttonIndex 按钮索引
 * @param firstValue 新的首值
 * @param lastValue 新的尾值
 */
ADCBtnsError ADCValuesMappingUtils::calibration(uint8_t buttonIndex, float_t firstValue, float_t lastValue) {
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
ADCBtnsError ADCValuesMappingUtils::calibrationAll(float_t* firstValues, float_t* lastValues) {
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
bool validateMarkParams(uint32_t* values, uint8_t length) {
    if (!values || length == 0 || length > MAX_ADC_VALUES_LENGTH) {
        return false;
    }
    return true;
}

ADCBtnsError ADCValuesMappingUtils::mark(uint32_t* values, uint8_t length) {
    if (!validateMarkParams(values, length)) return ADCBtnsError::INVALID_PARAMS;
    
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
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    free(oldOriginValues);
    free(oldCalibratedValues);
    return ADCBtnsError::SUCCESS;
}

uint8_t ADCValuesMappingUtils::getLength() {
    return mapping.length;
}


/**
 * @brief 判断映射是否是递增的
 * @return 是否递增
 */
bool ADCValuesMappingUtils::isIncrement() {
    return mapping.originalValues[0] < mapping.originalValues[mapping.length - 1];
}

/**
 * @brief 获取最大行程
 * @return 最大行程
 */
float_t ADCValuesMappingUtils::getMaxDistance() {
    maxDistance = mapping.step * (mapping.length - 1);
    return maxDistance;
}

/**
 * @brief 获取步长
 * @return 步长
 */
float_t ADCValuesMappingUtils::getStep() {
    return mapping.step;
}

/**
 * @brief 设置默认映射
 * @param name 映射名称
 * @return 错误码
 */
ADCBtnsError ADCValuesMappingUtils::setDefault(const char* name) {
    if (!name) return ADCBtnsError::INVALID_PARAMS;
    
    uint8_t idx = findIndex(name);
    if(idx == -1) return ADCBtnsError::MAPPING_NOT_FOUND;
    
    if(QSPI_W25Qxx_WriteBuffer((uint8_t*)name, ADC_VALUES_MAPPING_ADDR + 1, 16) != QSPI_W25Qxx_OK) {
        return ADCBtnsError::MAPPING_UPDATE_FAILED;
    }
    
    return ADCBtnsError::SUCCESS;
}

/**
 * @brief 获取映射名称列表
 * @return 映射名称列表
 */
std::vector<std::string> ADCValuesMappingUtils::getMappingNameList() {
    std::vector<std::string> nameList;
    uint8_t num = getCurrentNum();
    if(num == 0) return nameList;
    
    // 一次性读取所有映射数据
    uint32_t totalSize = num * sizeof(ADCValuesMapping);
    uint8_t* buffer = (uint8_t*)malloc(totalSize);
    if(!buffer) return nameList;
    
    uint32_t dataAddr = getMappingDataAddr();
    QSPI_W25Qxx_ReadBuffer(buffer, dataAddr, totalSize);
    
    // 在内存中处理数据
    ADCValuesMapping* mappings = (ADCValuesMapping*)buffer;
    for(uint8_t i = 0; i < num; i++) {
        nameList.push_back(std::string(mappings[i].name));
    }
    
    free(buffer);
    return nameList;
}

/**
 * @brief 获取默认映射名称
 * @return 默认映射名称
 */
std::string ADCValuesMappingUtils::getDefault() {
    std::string name = getDefaultMappingName();
    return name;
}

/**
 * @brief 获取映射JSON
 * @param name 映射名称
 * @return 映射JSON
 */
cJSON* ADCValuesMappingUtils::getMappingJSON(const char* name) {
    if (!name) return nullptr;
    
    // 查找映射
    int idx = findIndex(name);
    if(idx == -1) return nullptr;
    
    // 读取映射数据
    ADCValuesMapping mapping;
    uint32_t dataAddr = getMappingDataAddr();
    QSPI_W25Qxx_ReadBuffer((uint8_t*)&mapping, dataAddr + idx * sizeof(ADCValuesMapping), sizeof(ADCValuesMapping));
    
    // 创建映射JSON
    cJSON* mappingJSON = cJSON_CreateObject();
    if(!mappingJSON) return nullptr;
    
    // 添加基本信息
    cJSON_AddStringToObject(mappingJSON, "name", mapping.name);
    cJSON_AddNumberToObject(mappingJSON, "length", mapping.length);
    cJSON_AddNumberToObject(mappingJSON, "step", mapping.step);
    
    // 添加原始值数组
    cJSON* originalValuesJSON = cJSON_CreateArray();
    if(!originalValuesJSON) {
        cJSON_Delete(mappingJSON);
        return nullptr;
    }
    for(size_t i = 0; i < mapping.length; i++) {
        cJSON_AddItemToArray(originalValuesJSON, cJSON_CreateNumber(mapping.originalValues[i]));
    }
    cJSON_AddItemToObject(mappingJSON, "originalValues", originalValuesJSON);
    
    // 添加校准值数组
    cJSON* calibratedValuesJSON = cJSON_CreateArray();
    if(!calibratedValuesJSON) {
        cJSON_Delete(mappingJSON);
        return nullptr;
    }
    for(size_t i = 0; i < mapping.length; i++) {
        cJSON_AddItemToArray(calibratedValuesJSON, cJSON_CreateNumber(mapping.calibratedValues[i]));
    }
    cJSON_AddItemToObject(mappingJSON, "calibratedValues", calibratedValuesJSON);
    
    return mappingJSON;
}









