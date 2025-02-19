#ifndef __BOOTLOADER_CONFIG_H
#define __BOOTLOADER_CONFIG_H

// Flash 相关定义
#define BOOTLOADER_ADDRESS   0x08000000      // Bootloader在内部Flash起始地址
#define APP_ADDRESS         0x90000000      // 应用程序在QSPI Flash起始地址
#define BOOT_SECTOR_SIZE    0x20000         // 128KB
#define BOOT_MAGIC_NUMBER   0x424F4F54      // "BOOT"
#define BOOT_VERSION_NUMBER 0x00000001

// 新固件临时存储地址 (QSPI Flash另一区域)
#define NEW_FIRMWARE_ADDRESS 0x90100000     // 新固件临时存储区域

// QSPI Flash 配置
#define QSPI_FLASH_SIZE    0x800000        // 8MB
#define QSPI_PAGE_SIZE     256
#define QSPI_SECTOR_SIZE   4096

// Flash 操作超时时间
#define FLASH_TIMEOUT       50000U      // 超时时间

#endif // __BOOTLOADER_CONFIG_H 