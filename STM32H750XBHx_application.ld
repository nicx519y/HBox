/*
******************************************************************************
**

**  File        : LinkerScript.ld
**
**  Author		: STM32CubeMX
**
**  Abstract    : Linker script for STM32H750XBHx series
**                128Kbytes FLASH and 1056Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed "as is," without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**   1. Redistributions of source code must retain the above copyright notice,
**      this list of conditions and the following disclaimer.
**   2. Redistributions in binary form must reproduce the above copyright notice,
**      this list of conditions and the following disclaimer in the documentation
**      and/or other materials provided with the distribution.
**   3. Neither the name of STMicroelectronics nor the names of its contributors
**      may be used to endorse or promote products derived from this software
**      without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);    /* end of "DTCMRAM" Ram type memory */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400;     /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 128K
    DTCMRAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
    RAM_D1 (xrw)        : ORIGIN = 0x24000000, LENGTH = 512K
    RAM_D2 (xrw)        : ORIGIN = 0x30000000, LENGTH = 288K
    RAM_D3 (xrw)        : ORIGIN = 0x38000000, LENGTH = 64K
    ITCMRAM (xrw)       : ORIGIN = 0x00000000, LENGTH = 64K
    QSPIFLASH (rx)      : ORIGIN = 0x90000000, LENGTH = 1M  /* application 区域 */
    WEB_RESOURCES (rx)  : ORIGIN = 0x90100000, LENGTH = 1M  /* web resources 区域 */
    HALL_BTNS_CONFIG (rx) : ORIGIN = 0x00200000, LENGTH = 1M  /* hall btns config 区域 */
    CONFIG (rx)         : ORIGIN = 0x00300000, LENGTH = 1M  /* config 区域 */
}

/* Define output sections */
SECTIONS
{
    /* 中断向量表必须在最前面 */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } >RAM_D2 AT >QSPIFLASH

    /* 程序代码紧跟在向量表后面 */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        . = ALIGN(4);
    } >RAM_D2 AT >QSPIFLASH

    /* Constant data goes into FLASH */
    .rodata :
    {
        . = ALIGN(4);
        __rodata_start = .; /* 定义 .rodata 段的起始地址 */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
        __rodata_end = .;  /* 定义 .rodata 段的结束地址 */
    } >RAM_D2 AT >QSPIFLASH

    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data : 
    {
        . = ALIGN(4);
        _sidata = LOADADDR(.data);  /* 定义初始化数据的起始地址 */
        __data_start = .;           /* 定义数据段的起始地址 */
        _sdata = .;                  /* 定义数据段的起始地址 */
        *(.data)                     /* .data sections */
        *(.data*)                    /* .data* sections */

        . = ALIGN(4);
        __data_end = .;             /* 定义 .data 段的结束地址 */
        _edata = .;                  /* define a global symbol at data end */
    } >RAM_D2 AT >QSPIFLASH

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start = .;   /* define alternative symbol for bss start */
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end = .;     /* define alternative symbol for bss end */
    } >RAM_D2

    /* User_heap_stack section, used to check that there is enough RAM left */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >DTCMRAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.ARM.exidx*)
        *(.ARM.exidx.*)
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }

    /* RAM sections */

    /* DMA buffer section. use to store DMA buffer data. ws2812b use this area. adc use this area. */
    ._RAM_D1_Area :
    {
        . = ALIGN(32);
        *(.RAM_D1_Area)         /* RAM_D1 area section */
        *(.RAM_D1_Area*)        /* RAM_D1 area section */
        . = ALIGN(32);
    } >RAM_D1

    ._RAM_D2_Area :
    {
        . = ALIGN(32);
        *(.RAM_D2_Area)         /* RAM_D2 area section */
        *(.RAM_D2_Area*)        /* RAM_D2 area section */
        . = ALIGN(32);
    } >RAM_D2

    ._RAM_D3_Area :
    {
        . = ALIGN(32);
        *(.RAM_D3_Area)         /* RAM_D3 area section */
        *(.RAM_D3_Area*)        /* RAM_D3 area section */
        . = ALIGN(32);
    } >RAM_D3

    ._DTCMRAM_Area :
    {
        . = ALIGN(32);
        *(.DTCMRAM_Area)     /* DTCMRAM area section */
        *(.DTCMRAM_Area*)    /* DTCMRAM area section */
        . = ALIGN(32);
    } >DTCMRAM

    /* Web resources in QSPI Flash */
    .web_resources :
    {
        . = ALIGN(4);
        KEEP(*(.web_resources))
        KEEP(*(.fsdata))
        . = ALIGN(4);
    } >WEB_RESOURCES

    /* Define symbols for application */
    PROVIDE(__text_vma_start = ADDR(.text));            /* RAM 中的地址 */
    PROVIDE(__text_vma_end = ADDR(.text) + SIZEOF(.text));
    PROVIDE(__text_lma_start = LOADADDR(.text));        /* QSPI Flash 中的地址 */
    PROVIDE(__text_lma_end = LOADADDR(.text) + SIZEOF(.text));

    PROVIDE(__data_vma_start = ADDR(.data));
    PROVIDE(__data_vma_end = ADDR(.data) + SIZEOF(.data));
    PROVIDE(__data_lma_start = LOADADDR(.data));
    PROVIDE(__data_lma_end = LOADADDR(.data) + SIZEOF(.data));
    
    PROVIDE(__bss_vma_start = ADDR(.bss));
    PROVIDE(__bss_vma_end = ADDR(.bss) + SIZEOF(.bss));

    PROVIDE(__rodata_vma_start = ADDR(.rodata));
    PROVIDE(__rodata_vma_end = ADDR(.rodata) + SIZEOF(.rodata));

    PROVIDE(__isr_vector_vma_start = ADDR(.isr_vector));
    PROVIDE(__isr_vector_vma_end = ADDR(.isr_vector) + SIZEOF(.isr_vector));
    PROVIDE(__isr_vector_lma_start = LOADADDR(.isr_vector));
    PROVIDE(__isr_vector_lma_end = LOADADDR(.isr_vector) + SIZEOF(.isr_vector));

    /* 确保这些符号被导出 */
    __isr_vector_start = LOADADDR(.isr_vector);
    __isr_vector_end = __isr_vector_start + SIZEOF(.isr_vector);
    __text_start = LOADADDR(.text);
    __text_end = __text_start + SIZEOF(.text);
}





