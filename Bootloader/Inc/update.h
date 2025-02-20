#ifndef __UPDATE_H
#define __UPDATE_H

#include "main.h"
#include "bootloader_config.h"

// 固件头部结构
typedef struct {
    uint32_t magic;          // 魔数，用于验证
    uint32_t version;        // 固件版本
    uint32_t size;           // 固件大小
    uint32_t checksum;       // CRC32校验和
} FirmwareHeader_t;

// 只保留对外的函数声明
bool check_for_update(void);
bool perform_update(void);
bool verify_application(void);

#endif // __UPDATE_H 