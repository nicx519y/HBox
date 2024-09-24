#include "cpp_main.hpp"
#include "bsp/board_api.h"
#include "tusb.h"
#include "enums.hpp"
#include "drivermanager.hpp"
#include "drivers/net/NetDriver.hpp"
#include "configmanager.hpp"
// #include "net_init.h"

#include "qspi-w25q64.h"
#include "ff.h"
#include <stdio.h>

FATFS fs;				//声明文件系统对象
FIL fp;					   //创建文件
UINT fnum;				   //接收�?/写返回的数量
FRESULT res;			   //获取返回�?
BYTE formatBuffer[1024*4] = {0};
UINT br;
UINT bw;

int cpp_main(void) 
{
    
    board_init();
    board_led_write(false);
    
    // tusb_init();
    // tud_init(BOARD_TUD_RHPORT);
    // net_init();

    // InputMode inputMode = INPUT_MODE_CONFIG;
    // DriverManager::getInstance().setup(inputMode);      
    // GPDriver * inputDriver = DriverManager::getInstance().getDriver();

    // ConfigManager::getInstance().setup(CONFIG_TYPE_WEB);

    // err=dl_load_file(&use_handle,"0:/dll_generate.bin");
    // void* export_func=dl_get_func(&use_handle,"export_func");
    // export_func();
    // dl_get_entry(&use_handle)(0,NULL);
    // dl_destroy_lib(&use_handle);

    while(1) {
        // HAL_Delay(1000);
        // res = f_open(&fp,"0:/Dem3.TXT",FA_CREATE_NEW | FA_WRITE);	
        // if ( res == FR_OK )		//新建并打开了该文件
        // { 
        //     res = f_write(&fp, formatBuffer, sizeof(formatBuffer), &bw); 
        //     printf( "\r\n file create success \n" ); 
        // //新建或打开文件后，一定要关闭   
        //     f_close(&fp);      
        // }
        // else if ( res == FR_EXIST )	//文件已存在
        // {
        //     printf( "\r\n file is existed \n" );
        //     f_close(&fp);		
        // }
        // else
        // {
        //     printf("\r\n failure. \n");
        // }
        // ConfigManager::getInstance().loop();
        // tud_task();
        // service_traffic();
    }

    return 0;
}