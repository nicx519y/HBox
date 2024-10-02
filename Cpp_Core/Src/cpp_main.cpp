#include "cpp_main.hpp"
#include <stdio.h>
#include <stdint.h>
#include "bsp/board_api.h"
#include "tusb.h"
#include "enums.hpp"
#include "drivermanager.hpp"
#include "drivers/net/NetDriver.hpp"
#include "configmanager.hpp"
// #include "net_init.h"
#include "board_cfg.h"
#include "qspi-w25q64.h"
#include "ff.h"
#include "led.h"
#include "ex_flash.h"

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
    printf("board_init success ... \r\n");
    
    
    // tusb_init();
    // tud_init(BOARD_TUD_RHPORT);
    // net_init();

    // InputMode inputMode = INPUT_MODE_CONFIG;
    // DriverManager::getInstance().setup(inputMode);      
    // GPDriver * inputDriver = DriverManager::getInstance().getDriver();

    // ConfigManager::getInstance().setup(CONFIG_TYPE_WEB);

    
    while(1) {
        LED1_Toggle;
        printf("LED1_Toggle...\r\n");
        HAL_Delay(1000);
        // board_led_write(false);
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