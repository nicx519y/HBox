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

// 文件数据指针
static uint8_t* data__fonts_icomoon_ttf = NULL;
static uint8_t* data__index_html = NULL;
static uint8_t* data___next_static_js_app_layout_34e57ec92c0a0069_js = NULL;
static uint8_t* data___next_static_js_app_page_363da752049c5ec7_js = NULL;
static uint8_t* data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = NULL;
static uint8_t* data___next_static_js_main_app_027e88ef81fce2e2_js = NULL;

// 文件大小常量
#define SIZE__FONTS_ICOMOON_TTF 1437
#define SIZE__INDEX_HTML 24289
#define SIZE___NEXT_STATIC_JS_APP_LAYOUT_34E57EC92C0A0069_JS 189366
#define SIZE___NEXT_STATIC_JS_APP_PAGE_363DA752049C5EC7_JS 122332
#define SIZE___NEXT_STATIC_JS_APP__NOT_FOUND_PAGE_74CC9060C45C4B1E_JS 1041
#define SIZE___NEXT_STATIC_JS_MAIN_APP_027E88EF81FCE2E2_JS 98670

static bool fsdata_inited = false;

struct fsdata_file file__fonts_icomoon_ttf[] = {{
    file_NULL,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE__FONTS_ICOMOON_TTF - 20,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file__index_html[] = {{
    file__fonts_icomoon_ttf,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE__INDEX_HTML - 12,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app_layout_34e57ec92c0a0069_js[] = {{
    file__index_html,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE___NEXT_STATIC_JS_APP_LAYOUT_34E57EC92C0A0069_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app_page_363da752049c5ec7_js[] = {{
    file___next_static_js_app_layout_34e57ec92c0a0069_js,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE___NEXT_STATIC_JS_APP_PAGE_363DA752049C5EC7_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js[] = {{
    file___next_static_js_app_page_363da752049c5ec7_js,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE___NEXT_STATIC_JS_APP__NOT_FOUND_PAGE_74CC9060C45C4B1E_JS - 60,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

struct fsdata_file file___next_static_js_main_app_027e88ef81fce2e2_js[] = {{
    file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js,
    NULL,  // 将在运行时设置
    NULL,  // 将在运行时设置
    SIZE___NEXT_STATIC_JS_MAIN_APP_027E88EF81FCE2E2_JS - 48,
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

static void update_file_pointers(void) {
    // 更新undefined的指针
    ((struct fsdata_file *)file__fonts_icomoon_ttf)->name = data__fonts_icomoon_ttf;
    ((struct fsdata_file *)file__fonts_icomoon_ttf)->data = data__fonts_icomoon_ttf + 20;

    // 更新undefined的指针
    ((struct fsdata_file *)file__index_html)->name = data__index_html;
    ((struct fsdata_file *)file__index_html)->data = data__index_html + 12;

    // 更新undefined的指针
    ((struct fsdata_file *)file___next_static_js_app_layout_34e57ec92c0a0069_js)->name = data___next_static_js_app_layout_34e57ec92c0a0069_js;
    ((struct fsdata_file *)file___next_static_js_app_layout_34e57ec92c0a0069_js)->data = data___next_static_js_app_layout_34e57ec92c0a0069_js + 48;

    // 更新undefined的指针
    ((struct fsdata_file *)file___next_static_js_app_page_363da752049c5ec7_js)->name = data___next_static_js_app_page_363da752049c5ec7_js;
    ((struct fsdata_file *)file___next_static_js_app_page_363da752049c5ec7_js)->data = data___next_static_js_app_page_363da752049c5ec7_js + 48;

    // 更新undefined的指针
    ((struct fsdata_file *)file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js)->name = data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js;
    ((struct fsdata_file *)file___next_static_js_app__not_found_page_74cc9060c45c4b1e_js)->data = data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js + 60;

    // 更新undefined的指针
    ((struct fsdata_file *)file___next_static_js_main_app_027e88ef81fce2e2_js)->name = data___next_static_js_main_app_027e88ef81fce2e2_js;
    ((struct fsdata_file *)file___next_static_js_main_app_027e88ef81fce2e2_js)->data = data___next_static_js_main_app_027e88ef81fce2e2_js + 48;

}

static bool allocate_memory(void) {
    data__fonts_icomoon_ttf = (uint8_t *)ram_alloc(SIZE__FONTS_ICOMOON_TTF);
    data__index_html = (uint8_t *)ram_alloc(SIZE__INDEX_HTML);
    data___next_static_js_app_layout_34e57ec92c0a0069_js = (uint8_t *)ram_alloc(SIZE___NEXT_STATIC_JS_APP_LAYOUT_34E57EC92C0A0069_JS);
    data___next_static_js_app_page_363da752049c5ec7_js = (uint8_t *)ram_alloc(SIZE___NEXT_STATIC_JS_APP_PAGE_363DA752049C5EC7_JS);
    data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = (uint8_t *)ram_alloc(SIZE___NEXT_STATIC_JS_APP__NOT_FOUND_PAGE_74CC9060C45C4B1E_JS);
    data___next_static_js_main_app_027e88ef81fce2e2_js = (uint8_t *)ram_alloc(SIZE___NEXT_STATIC_JS_MAIN_APP_027E88EF81FCE2E2_JS);

    // 检查内存分配是否成功
    if (data__fonts_icomoon_ttf == NULL || data__index_html == NULL || data___next_static_js_app_layout_34e57ec92c0a0069_js == NULL || data___next_static_js_app_page_363da752049c5ec7_js == NULL || data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js == NULL || data___next_static_js_main_app_027e88ef81fce2e2_js == NULL) {
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

        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 0), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__fonts_icomoon_ttf, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 1), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__index_html, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 2), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_layout_34e57ec92c0a0069_js, addr, size);
        addr += size;


        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 3), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_page_363da752049c5ec7_js, addr, size);
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
        free(data__fonts_icomoon_ttf);
        free(data__index_html);
        free(data___next_static_js_app_layout_34e57ec92c0a0069_js);
        free(data___next_static_js_app_page_363da752049c5ec7_js);
        free(data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js);
        free(data___next_static_js_main_app_027e88ef81fce2e2_js);

        // 重置指针为 NULL
        data__fonts_icomoon_ttf = NULL;
        data__index_html = NULL;
        data___next_static_js_app_layout_34e57ec92c0a0069_js = NULL;
        data___next_static_js_app_page_363da752049c5ec7_js = NULL;
        data___next_static_js_app__not_found_page_74cc9060c45c4b1e_js = NULL;
        data___next_static_js_main_app_027e88ef81fce2e2_js = NULL;

        fsdata_inited = false;
    }
}


	const uint8_t numfiles = 6;
	