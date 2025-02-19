#ifndef __UPDATE_HPP
#define __UPDATE_HPP

#include "main.h"
#include "bootloader_config.h"

// 固件头部结构
struct FirmwareHeader {
    uint32_t magic;          // 魔数，用于验证
    uint32_t version;        // 固件版本
    uint32_t size;           // 固件大小
    uint32_t checksum;       // CRC32校验和
};

class FirmwareUpdater {
public:
    bool checkForUpdate();
    bool performUpdate();
    bool verifyApplication();
    
private:
    bool eraseApplicationSpace();
    bool writeFlash(uint32_t address, uint8_t* data, uint32_t length);
    uint32_t calculateChecksum(uint8_t* data, uint32_t length);
    bool verifyChecksum(uint8_t* data, uint32_t length, uint32_t expected);
    bool unlockFlash();
    bool lockFlash();
};

extern FirmwareUpdater firmwareUpdater;

#endif // __UPDATE_HPP 