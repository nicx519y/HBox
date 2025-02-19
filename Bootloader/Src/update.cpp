#include "update.hpp"
#include "bootloader_main.h"
#include "bootloader_config.h"
#include "stm32h7xx_hal_flash.h"
#include "qspi-w25q64.h"

// Flash 相关宏定义
#define FLASH_TYPEPROGRAM_DOUBLEWORD   FLASH_TYPEPROGRAM_FLASHWORD  // H7系列使用 FLASHWORD
#define FLASH_FLAG_ALL_ERRORS  (FLASH_FLAG_EOP    | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |\
                               FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR)

bool FirmwareUpdater::checkForUpdate() {
    FirmwareHeader* header = (FirmwareHeader*)NEW_FIRMWARE_ADDRESS;
    
    // 检查魔数
    if(header->magic != BOOT_MAGIC_NUMBER) {
        return false;
    }
    
    // 检查版本号
    if(header->version <= BOOT_VERSION_NUMBER) {
        return false;
    }
    
    // 验证固件大小
    if(header->size > BOOT_SECTOR_SIZE) {
        return false;
    }
    
    return true;
}

bool FirmwareUpdater::performUpdate() {
    FirmwareHeader* header = (FirmwareHeader*)NEW_FIRMWARE_ADDRESS;
    uint8_t* newFirmware = (uint8_t*)(NEW_FIRMWARE_ADDRESS + sizeof(FirmwareHeader));
    
    // 擦除应用程序区域
    if(!eraseApplicationSpace()) {
        return false;
    }
    
    // 写入新固件
    if(!writeFlash(APP_ADDRESS, newFirmware, header->size)) {
        return false;
    }
    
    // 验证校验和
    if(!verifyChecksum((uint8_t*)APP_ADDRESS, header->size, header->checksum)) {
        return false;
    }
    
    return true;
}

bool FirmwareUpdater::verifyApplication() {
    uint32_t* appAddr = (uint32_t*)APP_ADDRESS;
    
    // 检查栈指针是否有效
    if((appAddr[0] & 0x2FFF0000) != 0x20000000) {
        return false;
    }
    
    // 检查程序计数器是否有效
    if((appAddr[1] & 0x2FFF0000) != APP_ADDRESS) {
        return false;
    }
    
    // 检查固件头部
    FirmwareHeader* header = (FirmwareHeader*)APP_ADDRESS;
    if(header->magic != BOOT_MAGIC_NUMBER) {
        return false;
    }
    
    // 验证校验和
    return verifyChecksum((uint8_t*)APP_ADDRESS, header->size, header->checksum);
}

bool FirmwareUpdater::eraseApplicationSpace() {
    // 使用 QSPI_W25Qxx_ 开头的函数
    return QSPI_W25Qxx_SectorErase(APP_ADDRESS) == QSPI_W25Qxx_OK;
}

bool FirmwareUpdater::writeFlash(uint32_t address, uint8_t* data, uint32_t length) {
    // 使用 QSPI_W25Qxx_ 开头的函数
    return QSPI_W25Qxx_WriteBuffer(data, address, length) == QSPI_W25Qxx_OK;
}

uint32_t FirmwareUpdater::calculateChecksum(uint8_t* data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;
    
    for(uint32_t i = 0; i < length; i++) {
        uint8_t byte = data[i];
        crc ^= byte;
        for(int j = 0; j < 8; j++) {
            if(crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
            else crc = crc >> 1;
        }
    }
    
    return ~crc;
}

bool FirmwareUpdater::verifyChecksum(uint8_t* data, uint32_t length, uint32_t expected) {
    return calculateChecksum(data, length) == expected;
}

bool FirmwareUpdater::unlockFlash() {
    HAL_StatusTypeDef status;
    
    status = HAL_FLASH_Unlock();
    if(status != HAL_OK) {
        return false;
    }
    
    // 清除 H7 系列的错误标志
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGSERR | FLASH_FLAG_STRBERR | FLASH_FLAG_INCERR |
                          FLASH_FLAG_CRCRDERR | FLASH_FLAG_RDPERR | FLASH_FLAG_SNECCERR |
                          FLASH_FLAG_DBECCERR | FLASH_FLAG_RDSERR);
    
    return true;
}

bool FirmwareUpdater::lockFlash() {
    return HAL_FLASH_Lock() == HAL_OK;
}

// 全局实例
FirmwareUpdater firmwareUpdater; 