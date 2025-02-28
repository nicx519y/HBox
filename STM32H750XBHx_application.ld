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

/* 定义栈顶 */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400;     /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x90000000, LENGTH = 8192K
    DTCMRAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
    RAM_D1 (xrw)        : ORIGIN = 0x24000000, LENGTH = 512K
    RAM_D2 (xrw)        : ORIGIN = 0x30000000, LENGTH = 288K
    RAM_D3 (xrw)        : ORIGIN = 0x38000000, LENGTH = 64K
    ITCMRAM (xrw)       : ORIGIN = 0x00000000, LENGTH = 64K
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
        __vector_table = .;
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH  /* 确保在 QSPI Flash 中 */

    /* 测试函数放在固定偏移位置 */
    .text.test_func 0x90001000 :
    {
        . = ALIGN(4);
        KEEP(*(.text.test_func))  /* 添加 KEEP 确保不被优化掉 */
        . = ALIGN(4);
    } >FLASH

    /* 程序代码 */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } >FLASH  /* 确保在 QSPI Flash 中 */

    /* Constant data goes into FLASH */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } >FLASH

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH

    _sidata = LOADADDR(.data);
    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data : 
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */
    } >RAM_D1 AT> FLASH

    /* Uninitialized data section */
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >RAM_D1

    /* User_heap_stack section, used to check that there is enough RAM left */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM_D1

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

    /* Export important symbols */
    PROVIDE(_stack = ORIGIN(RAM_D1) + LENGTH(RAM_D1));
    PROVIDE(_flash_start = ORIGIN(FLASH));
    PROVIDE(_flash_end = ORIGIN(FLASH) + LENGTH(FLASH));
}





