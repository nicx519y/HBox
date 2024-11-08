#include "constant.hpp"
#include "fsdata.h"
#include "qspi-w25q64.h"
#include <stdbool.h>
#include <string.h>

#define file_NULL (struct fsdata_file *) NULL

#ifndef FS_FILE_FLAGS_HEADER_INCLUDED
#define FS_FILE_FLAGS_HEADER_INCLUDED 1
#endif
#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT
#define FS_FILE_FLAGS_HEADER_PERSISTENT 0
#endif
/* FSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */
#ifndef FSDATA_FILE_ALIGNMENT
#define FSDATA_FILE_ALIGNMENT 0
#endif
#ifndef FSDATA_ALIGN_PRE
#define FSDATA_ALIGN_PRE
#endif
#ifndef FSDATA_ALIGN_POST
#define FSDATA_ALIGN_POST
#endif
#if FSDATA_FILE_ALIGNMENT==2
#include "fsdata_alignment.h"
#endif

#define ex_fsdata_addr FSDATA_ADDR

#define __Text_Area__ __attribute__((section("._Text_Area")))

static bool fsdata_inited = false;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__assets_index_2a2af97b_css = 0;
#endif
__Text_Area__ static unsigned char data__assets_index_2a2af97b_css[34142] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__assets_index_3470b205_js = 1;
#endif
__Text_Area__ static unsigned char data__assets_index_3470b205_js[343912] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__favicon_ico = 2;
#endif
__Text_Area__ static unsigned char data__favicon_ico[1829] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__images_logo_png = 3;
#endif
__Text_Area__ static unsigned char data__images_logo_png[2375] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__index_html = 4;
#endif
__Text_Area__ static unsigned char data__index_html[657] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__manifest_json = 5;
#endif
__Text_Area__ static unsigned char data__manifest_json[440] FSDATA_ALIGN_PRE;

const struct fsdata_file file__assets_index_2a2af97b_css[] = {{
file_NULL,
data__assets_index_2a2af97b_css,
data__assets_index_2a2af97b_css + 28,
sizeof(data__assets_index_2a2af97b_css) - 28,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__assets_index_3470b205_js[] = {{
file__assets_index_2a2af97b_css,
data__assets_index_3470b205_js,
data__assets_index_3470b205_js + 28,
sizeof(data__assets_index_3470b205_js) - 28,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__favicon_ico[] = {{
file__assets_index_3470b205_js,
data__favicon_ico,
data__favicon_ico + 16,
sizeof(data__favicon_ico) - 16,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__images_logo_png[] = {{
file__favicon_ico,
data__images_logo_png,
data__images_logo_png + 20,
sizeof(data__images_logo_png) - 20,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__index_html[] = {{
file__images_logo_png,
data__index_html,
data__index_html + 12,
sizeof(data__index_html) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__manifest_json[] = {{
file__index_html,
data__manifest_json,
data__manifest_json + 16,
sizeof(data__manifest_json) - 16,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_SSI
}};


const struct fsdata_file * getFSRoot(void)
{
	if(fsdata_inited == false) {
		int8_t result;
        uint8_t d[4];
        uint32_t len;
        uint32_t addr;
        uint32_t size;

		result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr, 4);
        len = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]); // 文件数量
	addr = ex_fsdata_addr + 4 * (len + 1);
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 0), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__assets_index_2a2af97b_css, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 1), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__assets_index_3470b205_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 2), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__favicon_ico, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 3), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__images_logo_png, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 4), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__index_html, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 5), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__manifest_json, addr, size);
		
		fsdata_inited = true;
	}

	return file__manifest_json;
}
	
const uint8_t numfiles = 6;
