#ifndef __BOOTLOADER_CONFIG_H
#define __BOOTLOADER_CONFIG_H

// Flash 相关定义
#define APP_ADDRESS          0x08020000  // 应用程序起始地址
#define BOOTLOADER_ADDRESS   0x08000000  // Bootloader起始地址
#define BOOT_SECTOR_SIZE    0x20000     // 128KB
#define BOOT_MAGIC_NUMBER    0x424F4F54   // "BOOT"
#define BOOT_VERSION_NUMBER  0x00000001

// 新固件存储地址 (QSPI Flash)
#define NEW_FIRMWARE_ADDRESS 0x90100000  // 在QSPI Flash中的地址

// Flash 操作超时时间
#define FLASH_TIMEOUT       50000U      // 超时时间

#endif // __BOOTLOADER_CONFIG_H 