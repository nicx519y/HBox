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
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);    /* end of "RAM_D1" Ram type memory */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400;     /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    DTCMRAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
    RAM_D1 (xrw)        : ORIGIN = 0x24000000, LENGTH = 512K
    RAM_D2 (xrw)        : ORIGIN = 0x30000000, LENGTH = 288K
    RAM_D3 (xrw)        : ORIGIN = 0x38000000, LENGTH = 64K
    ITCMRAM (xrw)       : ORIGIN = 0x00000000, LENGTH = 64K
    QSPIFLASH (rx)      : ORIGIN = 0x90000000, LENGTH = 256K  /* application 区域 */
    WEB_RESOURCES (rx)  : ORIGIN = 0x90100000, LENGTH = 1M  /* web resources 区域 */
    ADC_VALUES_MAPPING (rx) : ORIGIN = 0x00200000, LENGTH = 1M  /* adc values mapping 区域 */
    CONFIG (rx)         : ORIGIN = 0x00300000, LENGTH = 1M  /* config 区域 */
}

/* Define output sections */
SECTIONS
{
    /* Web resources in QSPI Flash */
    .web_resources :
    {
        . = ALIGN(4);
        KEEP(*(.web_resources))
        KEEP(*(.fsdata))
        . = ALIGN(4);
    } >WEB_RESOURCES

    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } >RAM_D2

    /* The program code and other data goes into FLASH */
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
    } >RAM_D2

    /* Constant data goes into FLASH */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } >RAM_D2


    /* used by the startup to initialize data */
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
    } >DTCMRAM

    /* Uninitialized data section */
    . = ALIGN(4);
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

    .application :
    {
        . = ALIGN(4);
        *(.application)
        *(.application*)
        . = ALIGN(4);
    } >RAM_D2 AT >QSPIFLASH    /* 应用程序存储在QSPI Flash */

    /* Define symbols for application */
    PROVIDE(_application_src = LOADADDR(.application));     /* QSPI Flash起始地址: 0x90000000 */
    PROVIDE(_application_dst = ADDR(.application));        /* RAM_D2起始地址 */
    PROVIDE(_application_size = LENGTH(QSPIFLASH));      /* QSPI Flash大小: 256K */
}




