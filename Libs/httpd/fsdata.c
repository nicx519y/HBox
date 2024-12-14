#include "constant.hpp"
#include "fsdata.h"
#include "qspi-w25q64.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define file_NULL (struct fsdata_file *) NULL

#ifndef FS_FILE_FLAGS_HEADER_INCLUDED
#define FS_FILE_FLAGS_HEADER_INCLUDED 1
#endif
#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT
#define FS_FILE_FLAGS_HEADER_PERSISTENT 0
#endif
#ifndef FSDATA_FILE_ALIGNMENT
#define FSDATA_FILE_ALIGNMENT 0
#endif
#ifndef FSDATA_ALIGN_PRE
#define FSDATA_ALIGN_PRE
#endif
#ifndef FSDATA_ALIGN_POST
#define FSDATA_ALIGN_POST
#endif

#define ex_fsdata_addr FSDATA_ADDR

// 定义RAM区域的起始地址和大小（根据链接器脚本中的定义）
#define RAM_START_ADDR      0x24000000
#define RAM_SIZE           (512 * 1024)  // 512KB
#define RAM_ALIGNMENT      32

// 用于跟踪RAM分配的简单分配器
static uint32_t current_ram_addr = RAM_START_ADDR;

// 文件数据指针
static uint8_t* data__favicon_ico = NULL;
static uint8_t* data__index_html = NULL;
static uint8_t* data___next_static_js_app_layout_63d4b2e6c165e510_js = NULL;
static uint8_t* data___next_static_js_app_page_6c355a02f395b974_js = NULL;
static uint8_t* data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = NULL;
static uint8_t* data___next_static_js_main_app_027e88ef81fce2e2_js = NULL;

// 文件大小常量
#define SIZE_FAVICON_ICO 9291
#define SIZE_INDEX_HTML 12893
#define SIZE_APP_LAYOUT_JS 175924
#define SIZE_APP_PAGE_JS 121303
#define SIZE_NOT_FOUND_PAGE_JS 1038
#define SIZE_MAIN_APP_JS 98667

static bool fsdata_inited = false;

// 简单的内存分配函数，返回对齐的地址
static void* ram_alloc(size_t size) {
    // 确保大小是32字节对齐的
    size = (size + RAM_ALIGNMENT - 1) & ~(RAM_ALIGNMENT - 1);
    
    // 检查是否有足够的空间
    if (current_ram_addr + size > RAM_START_ADDR + RAM_SIZE) {
        return NULL;
    }
    
    // 保存当前地址
    void* allocated_addr = (void*)current_ram_addr;
    
    // 更新下一个可用地址
    current_ram_addr += size;
    
    return allocated_addr;
}

// 文件结构体定义保持不变，因为它们被外部引用
struct fsdata_file file__favicon_ico[] = {{
    file_NULL,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE_FAVICON_ICO - 16,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file__index_html[] = {{
    file__favicon_ico,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE_INDEX_HTML - 12,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app_layout_63d4b2e6c165e510_js[] = {{
    file__index_html,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE_APP_LAYOUT_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app_page_6c355a02f395b974_js[] = {{
    file___next_static_js_app_layout_63d4b2e6c165e510_js,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE_APP_PAGE_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js[] = {{
    file___next_static_js_app_page_6c355a02f395b974_js,
    NULL,  // 将在运行时��置
    NULL,  // 将在运行时设置
    SIZE_NOT_FOUND_PAGE_JS - 60,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_main_app_027e88ef81fce2e2_js[] = {{
    file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE_MAIN_APP_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

static void update_file_pointers(void) {
    // 更新文件结构体中的指针
    ((struct fsdata_file *)file__favicon_ico)->name = data__favicon_ico;
    ((struct fsdata_file *)file__favicon_ico)->data = data__favicon_ico + 16;
    
    ((struct fsdata_file *)file__index_html)->name = data__index_html;
    ((struct fsdata_file *)file__index_html)->data = data__index_html + 12;
    
    ((struct fsdata_file *)file___next_static_js_app_layout_63d4b2e6c165e510_js)->name = data___next_static_js_app_layout_63d4b2e6c165e510_js;
    ((struct fsdata_file *)file___next_static_js_app_layout_63d4b2e6c165e510_js)->data = data___next_static_js_app_layout_63d4b2e6c165e510_js + 48;
    
    ((struct fsdata_file *)file___next_static_js_app_page_6c355a02f395b974_js)->name = data___next_static_js_app_page_6c355a02f395b974_js;
    ((struct fsdata_file *)file___next_static_js_app_page_6c355a02f395b974_js)->data = data___next_static_js_app_page_6c355a02f395b974_js + 48;
    
    ((struct fsdata_file *)file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js)->name = data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js;
    ((struct fsdata_file *)file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js)->data = data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js + 60;
    
    ((struct fsdata_file *)file___next_static_js_main_app_027e88ef81fce2e2_js)->name = data___next_static_js_main_app_027e88ef81fce2e2_js;
    ((struct fsdata_file *)file___next_static_js_main_app_027e88ef81fce2e2_js)->data = data___next_static_js_main_app_027e88ef81fce2e2_js + 48;
}

static bool allocate_memory(void) {
    // 重置内存分配器
    current_ram_addr = RAM_START_ADDR;
    
    // 使用自定义的ram_alloc函数分配内存
    data__favicon_ico = (uint8_t *)ram_alloc(SIZE_FAVICON_ICO);
    
    data__index_html = (uint8_t *)ram_alloc(SIZE_INDEX_HTML);
    
    data___next_static_js_app_layout_63d4b2e6c165e510_js = (uint8_t *)ram_alloc(SIZE_APP_LAYOUT_JS);
    
    data___next_static_js_app_page_6c355a02f395b974_js = (uint8_t *)ram_alloc(SIZE_APP_PAGE_JS);
    
    data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = (uint8_t *)ram_alloc(SIZE_NOT_FOUND_PAGE_JS);
    
    data___next_static_js_main_app_027e88ef81fce2e2_js = (uint8_t *)ram_alloc(SIZE_MAIN_APP_JS);

    // 检查内存分配是否成功
    if (data__favicon_ico == NULL || data__index_html == NULL || 
        data___next_static_js_app_layout_63d4b2e6c165e510_js == NULL ||
        data___next_static_js_app_page_6c355a02f395b974_js == NULL ||
        data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js == NULL ||
        data___next_static_js_main_app_027e88ef81fce2e2_js == NULL) {
        return false;
    }
    return true;
}

const struct fsdata_file * getFSRoot(void)
{
    if(fsdata_inited == false) {
        int8_t result;
        uint8_t d[4];
        uint32_t len;
        uint32_t addr;
        uint32_t size;
        // 分配内存
        if (!allocate_memory()) {
            return NULL;
        }

        printf("getFSRoot: allocate_memory success.\n");

        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr, 4);
        len = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]); // 文件数量
        addr = ex_fsdata_addr + 4 * (len + 1);

        // 读取文件数据
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 0), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__favicon_ico, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 1), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__index_html, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 2), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_layout_63d4b2e6c165e510_js, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 3), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_page_6c355a02f395b974_js, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 4), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 5), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_main_app_027e88ef81fce2e2_js, addr, size);


        // 更新文件结构体中的指针
        update_file_pointers();
        
        fsdata_inited = true;
    }

    return file___next_static_js_main_app_027e88ef81fce2e2_js;
}

void fsdata_cleanup(void)
{
    if (fsdata_inited) {
        // 释放所有动态分配的内存
        free(data__favicon_ico);
        free(data__index_html);
        free(data___next_static_js_app_layout_63d4b2e6c165e510_js);
        free(data___next_static_js_app_page_6c355a02f395b974_js);
        free(data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js);
        free(data___next_static_js_main_app_027e88ef81fce2e2_js);

        // 重置指针为 NULL
        data__favicon_ico = NULL;
        data__index_html = NULL;
        data___next_static_js_app_layout_63d4b2e6c165e510_js = NULL;
        data___next_static_js_app_page_6c355a02f395b974_js = NULL;
        data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = NULL;
        data___next_static_js_main_app_027e88ef81fce2e2_js = NULL;

        fsdata_inited = false;
    }
}

const uint8_t numfiles = 6;
