#include "update.h"
#include "qspi-w25q64.h"

// 内部函数声明
static bool erase_application_space(void);
static bool write_flash(uint32_t address, uint8_t* data, uint32_t length);
static uint32_t calculate_checksum(uint8_t* data, uint32_t length);
static bool verify_checksum(uint8_t* data, uint32_t length, uint32_t expected);

// 检查是否需要更新
bool check_for_update(void)
{
    FirmwareHeader_t header;
    
    // 从 QSPI Flash 读取新固件头部
    if(QSPI_W25Qxx_ReadBuffer((uint8_t*)&header, NEW_FIRMWARE_ADDRESS, sizeof(header)) != QSPI_W25Qxx_OK) {
        BOOT_ERR("Failed to read firmware header");
        return false;
    }
    
    // 验证魔数
    if(header.magic != BOOT_MAGIC_NUMBER) {
        BOOT_DBG("No new firmware found");
        return false;
    }
    
    // 验证版本号
    if(header.version <= BOOT_VERSION_NUMBER) {
        BOOT_DBG("Firmware version is not newer");
        return false;
    }
    
    BOOT_DBG("New firmware found: v%lu", header.version);
    return true;
}

// 执行更新
bool perform_update(void)
{
    FirmwareHeader_t header;
    uint8_t buffer[QSPI_PAGE_SIZE];
    uint32_t remaining, addr, size;
    
    // 读取固件头部
    if(QSPI_W25Qxx_ReadBuffer((uint8_t*)&header, NEW_FIRMWARE_ADDRESS, sizeof(header)) != QSPI_W25Qxx_OK) {
        return false;
    }
    
    // 擦除应用程序空间
    if(!erase_application_space()) {
        return false;
    }
    
    // 复制新固件
    remaining = header.size;
    addr = 0;
    
    while(remaining > 0) {
        size = (remaining > QSPI_PAGE_SIZE) ? QSPI_PAGE_SIZE : remaining;
        
        // 读取一页数据
        if(QSPI_W25Qxx_ReadBuffer(buffer, NEW_FIRMWARE_ADDRESS + addr, size) != QSPI_W25Qxx_OK) {
            return false;
        }
        
        // 写入到应用程序区域
        if(!write_flash(APP_ADDRESS + addr, buffer, size)) {
            return false;
        }
        
        addr += size;
        remaining -= size;
    }
    
    // 验证写入的数据
    if(!verify_checksum((uint8_t*)APP_ADDRESS, header.size, header.checksum)) {
        BOOT_ERR("Firmware verification failed");
        return false;
    }
    
    BOOT_DBG("Firmware update completed");
    return true;
}

// 验证应用程序
bool verify_application(void)
{
    FirmwareHeader_t* header = (FirmwareHeader_t*)APP_ADDRESS;
    
    // 验证魔数
    if(header->magic != BOOT_MAGIC_NUMBER) {
        BOOT_ERR("Invalid application magic number");
        return false;
    }
    
    // 验证校验和
    if(!verify_checksum((uint8_t*)(APP_ADDRESS + sizeof(FirmwareHeader_t)), 
                        header->size - sizeof(FirmwareHeader_t), 
                        header->checksum)) {
        BOOT_ERR("Application checksum verification failed");
        return false;
    }
    
    return true;
}

// 内部函数实现
static bool erase_application_space(void)
{
    BOOT_DBG("Erasing application space...");
    return QSPI_W25Qxx_BufferErase(APP_ADDRESS, QSPI_APP_SIZE) == QSPI_W25Qxx_OK;
}

static bool write_flash(uint32_t address, uint8_t* data, uint32_t length)
{
    return QSPI_W25Qxx_Write(data, address, length) == QSPI_W25Qxx_OK;
}

static uint32_t calculate_checksum(uint8_t* data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    for(uint32_t i = 0; i < length; i++) {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

static bool verify_checksum(uint8_t* data, uint32_t length, uint32_t expected)
{
    uint32_t calculated = calculate_checksum(data, length);
    return calculated == expected;
} 