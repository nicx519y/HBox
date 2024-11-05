#include "cpp_main.hpp"
#include <stdio.h>
#include <stdint.h>
#include "bsp/board_api.h"
#include "enums.hpp"
#include "drivermanager.hpp"
#include "drivers/net/NetDriver.hpp"
#include "configmanager.hpp"
#include "board_cfg.h"
#include "qspi-w25q64.h"
#include "led.h"
#include "adc.h"
#include "leds_manager.hpp"
#include "gamepad.hpp"



int cpp_main(void) 
{   

    board_init();
    printf("board_init success ... \r\n");
    

    // InputMode inputMode = InputMode::INPUT_MODE_CONFIG;
    InputMode inputMode = InputMode::INPUT_MODE_XINPUT;
    ConfigType configType = ConfigType::CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    Gamepad& gamepad = Gamepad::getInstance();
    gamepad.setup();

    GPDriver * inputDriver = DriverManager::getInstance().getDriver();

    bool configMode = false;
    
    // Start the TinyUSB Device functionality
    tud_init(TUD_OPT_RHPORT);

    uint32_t t = HAL_GetTick();

    while(1) {

        if(HAL_GetTick() - t >= 1)
        {
            // LED1_Toggle;s

            gamepad.runHandler();
            t = HAL_GetTick();
        }

        if(configMode) {
            ConfigManager::getInstance().loop();
        } else {
            inputDriver->process(&gamepad);
            tud_task();
        }
    }

    return 0;
} 