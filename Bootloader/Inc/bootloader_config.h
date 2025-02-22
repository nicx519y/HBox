#ifndef __BOOTLOADER_CONFIG_H
#define __BOOTLOADER_CONFIG_H

#include <stdint.h>  // 添加这行来支持 uint32_t 类型

// 调试开关
#define BOOTLOADER_DEBUG 1  // 设置为 0 可以关闭调试输出

// 调试输出宏
#if BOOTLOADER_DEBUG
    #define BOOT_DBG(fmt, ...) printf("[BOOT] " fmt "\r\n", ##__VA_ARGS__)
    #define BOOT_ERR(fmt, ...) printf("[BOOT] ERROR: " fmt "\r\n", ##__VA_ARGS__)
#else
    #define BOOT_DBG(fmt, ...) ((void)0)
    #define BOOT_ERR(fmt, ...) ((void)0)
#endif

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

// QSPI Flash 分区定义
#define QSPI_APP_ADDRESS        0x90000000  // 应用程序起始地址
#define QSPI_APP_SIZE          0x100000    // 应用程序大小 (1MB)

#define QSPI_NEW_FIRM_ADDRESS   0x90100000  // 新固件存储地址
#define QSPI_NEW_FIRM_SIZE     0x100000    // 新固件区域大小 (1MB)

#define QSPI_USER_DATA_ADDRESS  0x90200000  // 用户数据起始地址
#define QSPI_USER_DATA_SIZE    0x600000    // 用户数据区域大小 (6MB)

#define BOOTLOADER_VERSION "1.0.0"
#define APPLICATION_ADDRESS 0x08020000
#define MAX_FLASH_SIZE        0x00200000  // 2MB

// 添加元数据结构定义
typedef struct {
    uint32_t magic;          /* Magic number: 'MTAD' */
    struct {
        uint32_t vma_start;
        uint32_t vma_end;
        uint32_t lma_start;
        uint32_t lma_end;
    } sections[5];           /* .text, .data, .bss, .rodata, .isr_vector */
} AppMetadata;

// 定义魔数 (MTAD in little-endian)
#define METADATA_MAGIC         (('D') | ('A' << 8) | ('T' << 16) | ('M' << 24))  // 0x4D544144
#define METADATA_MAGIC_STR     "MTAD"  // 用于打印

#endif /* __BOOTLOADER_CONFIG_H */ 