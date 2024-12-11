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
static const unsigned int dummy_align__404_html = 0;
#endif
__Text_Area__ static unsigned char data__404_html[13000] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__favicon_ico = 1;
#endif
__Text_Area__ static unsigned char data__favicon_ico[9291] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__file_svg = 2;
#endif
__Text_Area__ static unsigned char data__file_svg[378] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__globe_svg = 3;
#endif
__Text_Area__ static unsigned char data__globe_svg[650] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__index_html = 4;
#endif
__Text_Area__ static unsigned char data__index_html[12894] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__index_txt = 5;
#endif
__Text_Area__ static unsigned char data__index_txt[1342] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__next_svg = 6;
#endif
__Text_Area__ static unsigned char data__next_svg[831] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__vercel_svg = 7;
#endif
__Text_Area__ static unsigned char data__vercel_svg[242] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align__window_svg = 8;
#endif
__Text_Area__ static unsigned char data__window_svg[332] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_chunks_polyfills_42372ed130431b0a_js = 9;
#endif
__Text_Area__ static unsigned char data___next_static_chunks_polyfills_42372ed130431b0a_js[39521] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_app_layout_7babd7c48f95155c_js = 10;
#endif
__Text_Area__ static unsigned char data___next_static_js_app_layout_7babd7c48f95155c_js[176927] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt = 11;
#endif
__Text_Area__ static unsigned char data___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt[429] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_app_page_9240397c0eac09b8_js = 12;
#endif
__Text_Area__ static unsigned char data___next_static_js_app_page_9240397c0eac09b8_js[121760] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt = 13;
#endif
__Text_Area__ static unsigned char data___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt[429] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js = 14;
#endif
__Text_Area__ static unsigned char data___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js[1048] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_main_app_c36e96490a7903a1_js = 15;
#endif
__Text_Area__ static unsigned char data___next_static_js_main_app_c36e96490a7903a1_js[99153] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt = 16;
#endif
__Text_Area__ static unsigned char data___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt[404] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_main_c71d0941f1d7360c_js = 17;
#endif
__Text_Area__ static unsigned char data___next_static_js_main_c71d0941f1d7360c_js[83044] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt = 18;
#endif
__Text_Area__ static unsigned char data___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt[432] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_pages__app_cf3ad177392cf414_js = 19;
#endif
__Text_Area__ static unsigned char data___next_static_js_pages__app_cf3ad177392cf414_js[338] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_js_pages__error_a0996bbb06df5e95_js = 20;
#endif
__Text_Area__ static unsigned char data___next_static_js_pages__error_a0996bbb06df5e95_js[354] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js = 21;
#endif
__Text_Area__ static unsigned char data___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js[531] FSDATA_ALIGN_PRE;

#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js = 22;
#endif
__Text_Area__ static unsigned char data___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js[227] FSDATA_ALIGN_PRE;

const struct fsdata_file file__404_html[] = {{
file_NULL,
data__404_html,
data__404_html + 12,
sizeof(data__404_html) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__favicon_ico[] = {{
file__404_html,
data__favicon_ico,
data__favicon_ico + 16,
sizeof(data__favicon_ico) - 16,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__file_svg[] = {{
file__favicon_ico,
data__file_svg,
data__file_svg + 12,
sizeof(data__file_svg) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__globe_svg[] = {{
file__file_svg,
data__globe_svg,
data__globe_svg + 12,
sizeof(data__globe_svg) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__index_html[] = {{
file__globe_svg,
data__index_html,
data__index_html + 12,
sizeof(data__index_html) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__index_txt[] = {{
file__index_html,
data__index_txt,
data__index_txt + 12,
sizeof(data__index_txt) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__next_svg[] = {{
file__index_txt,
data__next_svg,
data__next_svg + 12,
sizeof(data__next_svg) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__vercel_svg[] = {{
file__next_svg,
data__vercel_svg,
data__vercel_svg + 12,
sizeof(data__vercel_svg) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file__window_svg[] = {{
file__vercel_svg,
data__window_svg,
data__window_svg + 12,
sizeof(data__window_svg) - 12,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_chunks_polyfills_42372ed130431b0a_js[] = {{
file__window_svg,
data___next_static_chunks_polyfills_42372ed130431b0a_js,
data___next_static_chunks_polyfills_42372ed130431b0a_js + 52,
sizeof(data___next_static_chunks_polyfills_42372ed130431b0a_js) - 52,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_app_layout_7babd7c48f95155c_js[] = {{
file___next_static_chunks_polyfills_42372ed130431b0a_js,
data___next_static_js_app_layout_7babd7c48f95155c_js,
data___next_static_js_app_layout_7babd7c48f95155c_js + 48,
sizeof(data___next_static_js_app_layout_7babd7c48f95155c_js) - 48,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt[] = {{
file___next_static_js_app_layout_7babd7c48f95155c_js,
data___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt,
data___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt + 60,
sizeof(data___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt) - 60,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_app_page_9240397c0eac09b8_js[] = {{
file___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt,
data___next_static_js_app_page_9240397c0eac09b8_js,
data___next_static_js_app_page_9240397c0eac09b8_js + 48,
sizeof(data___next_static_js_app_page_9240397c0eac09b8_js) - 48,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt[] = {{
file___next_static_js_app_page_9240397c0eac09b8_js,
data___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt,
data___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt + 60,
sizeof(data___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt) - 60,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js[] = {{
file___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt,
data___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js,
data___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js + 60,
sizeof(data___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js) - 60,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_main_app_c36e96490a7903a1_js[] = {{
file___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js,
data___next_static_js_main_app_c36e96490a7903a1_js,
data___next_static_js_main_app_c36e96490a7903a1_js + 48,
sizeof(data___next_static_js_main_app_c36e96490a7903a1_js) - 48,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt[] = {{
file___next_static_js_main_app_c36e96490a7903a1_js,
data___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt,
data___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt + 60,
sizeof(data___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt) - 60,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_main_c71d0941f1d7360c_js[] = {{
file___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt,
data___next_static_js_main_c71d0941f1d7360c_js,
data___next_static_js_main_c71d0941f1d7360c_js + 44,
sizeof(data___next_static_js_main_c71d0941f1d7360c_js) - 44,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt[] = {{
file___next_static_js_main_c71d0941f1d7360c_js,
data___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt,
data___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt + 56,
sizeof(data___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt) - 56,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_pages__app_cf3ad177392cf414_js[] = {{
file___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt,
data___next_static_js_pages__app_cf3ad177392cf414_js,
data___next_static_js_pages__app_cf3ad177392cf414_js + 48,
sizeof(data___next_static_js_pages__app_cf3ad177392cf414_js) - 48,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_js_pages__error_a0996bbb06df5e95_js[] = {{
file___next_static_js_pages__app_cf3ad177392cf414_js,
data___next_static_js_pages__error_a0996bbb06df5e95_js,
data___next_static_js_pages__error_a0996bbb06df5e95_js + 52,
sizeof(data___next_static_js_pages__error_a0996bbb06df5e95_js) - 52,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js[] = {{
file___next_static_js_pages__error_a0996bbb06df5e95_js,
data___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js,
data___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js + 56,
sizeof(data___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js) - 56,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
}};

const struct fsdata_file file___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js[] = {{
file___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js,
data___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js,
data___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js + 52,
sizeof(data___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js) - 52,
FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT
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
        result = QSPI_W25Qxx_ReadBuffer(data__404_html, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 1), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__favicon_ico, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 2), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__file_svg, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 3), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__globe_svg, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 4), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__index_html, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 5), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__index_txt, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 6), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__next_svg, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 7), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__vercel_svg, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 8), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data__window_svg, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 9), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_chunks_polyfills_42372ed130431b0a_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 10), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_layout_7babd7c48f95155c_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 11), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_layout_7babd7c48f95155c_js_LICENSE_txt, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 12), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_page_9240397c0eac09b8_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 13), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app_page_9240397c0eac09b8_js_LICENSE_txt, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 14), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_app__not_found_page_a1b61b4cb5ca4872_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 15), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_main_app_c36e96490a7903a1_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 16), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_main_app_c36e96490a7903a1_js_LICENSE_txt, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 17), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_main_c71d0941f1d7360c_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 18), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_main_c71d0941f1d7360c_js_LICENSE_txt, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 19), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_pages__app_cf3ad177392cf414_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 20), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_js_pages__error_a0996bbb06df5e95_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 21), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_Z_a2f63Jjuyu3UeI0IAt9__buildManifest_js, addr, size);
		addr += size;
        result = QSPI_W25Qxx_ReadBuffer(d, ex_fsdata_addr + 4 * (1 + 22), sizeof(uint32_t));
        size = (uint32_t)((uint32_t)d[0]<<24 | (uint32_t)d[1]<<16 | (uint32_t)d[2]<<8 | (uint32_t)d[3]);
        result = QSPI_W25Qxx_ReadBuffer(data___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js, addr, size);
		
		fsdata_inited = true;
	}

	return file___next_static_Z_a2f63Jjuyu3UeI0IAt9__ssgManifest_js;
}
	
const uint8_t numfiles = 23;
