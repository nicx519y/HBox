/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);    /* end of "RAM_D1" Ram type memory */

_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400;     /* required amount of stack */

/* Memories definition */
MEMORY
{
  FLASH    (rx)    : ORIGIN = 0x08000000, LENGTH = 128K    /* 只读和执行权限 */
  RAM_D1   (xrw)    : ORIGIN = 0x24000000, LENGTH = 512K   
  RAM_D2   (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K   /* AHB SRAM */
  RAM_D3   (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K    /* AHB SRAM */
  ITCMRAM  (xrw)   : ORIGIN = 0x00000000, LENGTH = 64K    /* ITCM RAM */
  QSPIFLASH (rx)  : ORIGIN = 0x90000000, LENGTH = 256K    /* QSPI FLASH */
}

/* Sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

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
  } >FLASH

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

  .preinit_array     :
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
  } >RAM_D1 AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
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

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .application :
  {
      . = ALIGN(4);
      *(.application)
      *(.application*)
      . = ALIGN(4);
  } >RAM_D2 AT >QSPIFLASH    /* 应用程序存储在QSPI Flash */

  .ARM.attributes 0 : { *(.ARM.attributes) }


  PROVIDE(_application_src = LOADADDR(.application));     /* QSPI Flash起始地址: 0x90000000 */
  PROVIDE(_application_dst = ADDR(.application));        /* RAM_D2起始地址 */
  PROVIDE(_application_size = LENGTH(QSPIFLASH));      /* QSPI Flash大小: 256K */
} 




