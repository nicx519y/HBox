#include "fsdata.h"
#include "qspi-w25q64.h"
#include <stdbool.h>

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

#define ex_fsdata_addr 0x0

#define __Text_Area__ __attribute__((section("._Text_Area")))

static bool fsdata_inited = false;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__assets_index_2a2af97b_css = 0;
#endif
__Text_Area__ static unsigned char data__assets_index_2a2af97b_css[34142] FSDATA_ALIGN_PRE = {};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__assets_index_3470b205_js = 1;
#endif
__Text_Area__ static unsigned char data__assets_index_3470b205_js[343912] FSDATA_ALIGN_PRE = {};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__favicon_ico = 2;
#endif
__Text_Area__ static unsigned char data__favicon_ico[1829] FSDATA_ALIGN_PRE = {};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__images_logo_png = 3;
#endif
__Text_Area__ static unsigned char data__images_logo_png[2375] FSDATA_ALIGN_PRE = {};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__index_html = 4;
#endif
__Text_Area__ static unsigned char data__index_html[657] FSDATA_ALIGN_PRE = {};

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__manifest_json = 5;
#endif
__Text_Area__ static unsigned char data__manifest_json[440] FSDATA_ALIGN_PRE = {};

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


static void convert_buffer(uint8_t* buffer, uint32_t* output, size_t buffer_size) {
	if (buffer_size % 4 != 0) {
		// Handle error: buffer size is not divisible by 4
		printf("buffer size is not divisible by 4\r\n");
		return;
	}
	
	for (size_t i = 0; i < buffer_size / 4; i++) {
		output[i] = (buffer[i * 4] << 24) |  // Most significant byte
					(buffer[i * 4 + 1] << 16) |  // Next most significant byte
					(buffer[i * 4 + 2] << 8) |  // Least significant byte
					buffer[i * 4 + 3];  // Least significant byte
	}
}
	
const struct fsdata_file * getFSRoot(void)
{
	if(fsdata_inited == false) {
		uint8_t lenBufferIn[4];
		uint32_t lenBufferOut[1];
		uint32_t addr = ex_fsdata_addr;
		uint32_t size = 4;
		uint8_t result = QSPI_W25Qxx_ReadBuffer(lenBufferIn, addr, size);
		if(result != QSPI_W25Qxx_OK) {
			printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
		} 
		convert_buffer(lenBufferIn, lenBufferOut, 4);

		uint32_t len = lenBufferOut[0];                 // 文件数量
		uint8_t sizesBufferIn[4 * len];
		uint32_t sizesBufferOut[len];
		addr += size;
		size = 4 * len;
		result = QSPI_W25Qxx_ReadBuffer(sizesBufferIn, addr, size);
		if(result != QSPI_W25Qxx_OK) {
			printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
		}
		convert_buffer(sizesBufferIn, sizesBufferOut, 4 * len); // 文件size
	
		addr += size;
        size = sizesBufferOut[0];
        result = QSPI_W25Qxx_ReadBuffer(data__assets_index_2a2af97b_css, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		addr += size;
        size = sizesBufferOut[1];
        result = QSPI_W25Qxx_ReadBuffer(data__assets_index_3470b205_js, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		addr += size;
        size = sizesBufferOut[2];
        result = QSPI_W25Qxx_ReadBuffer(data__favicon_ico, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		addr += size;
        size = sizesBufferOut[3];
        result = QSPI_W25Qxx_ReadBuffer(data__images_logo_png, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		addr += size;
        size = sizesBufferOut[4];
        result = QSPI_W25Qxx_ReadBuffer(data__index_html, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		addr += size;
        size = sizesBufferOut[5];
        result = QSPI_W25Qxx_ReadBuffer(data__manifest_json, addr, size);
        if(result != QSPI_W25Qxx_OK) {
            printf("Read QSPI_W25QXX failure! Err Code: %d \r\n", result);
        } else {
            printf("Read QSPI_W25QXX success!");
        }
		
		fsdata_inited = true;
	}

	return file__manifest_json;
}
	
const uint8_t numfiles = 6;
