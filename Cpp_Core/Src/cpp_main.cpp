#include "cpp_main.hpp"
#include <stdio.h>
#include <stdint.h>
#include "bsp/board_api.h"
#include "tusb.h"
#include "enums.hpp"
#include "drivermanager.hpp"
#include "drivers/net/NetDriver.hpp"
#include "configmanager.hpp"
#include "board_cfg.h"
#include "qspi-w25q64.h"
#include "ff.h"
#include "led.h"
#include "fsdata.h"
// #include "net_init.h"

int cpp_main(void) 
{   

    board_init();
    printf("board_init success ... \r\n");
    
    // tusb_init();
    // tud_init(BOARD_TUD_RHPORT);
    // net_init();

    // InputMode inputMode = INPUT_MODE_CONFIG;
    InputMode inputMode = INPUT_MODE_XINPUT;
    ConfigType configType = CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    bool configMode = false;

    uint32_t t = HAL_GetTick();
    
    while(1) {
        // LED1_Toggle;
        // printf("FS_ROOT\r\n");
        
        // HAL_Delay(1000);

        if(HAL_GetTick() - t >= 1000)
        {
            LED1_Toggle;
            t = HAL_GetTick();
        }

        if(configMode) {
            ConfigManager::getInstance().loop();
        } else {
            tud_task();
        }
    }

    return 0;
} 