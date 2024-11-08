#include "cpp_main.hpp"
#include <stdio.h>
#include "bsp/board_api.h"
#include "main_state_machine.hpp"
#include "fsdata.h"


int cpp_main(void) 
{   

    board_init();
    printf("board_init success ... \r\n");
    
    getFSRoot();
    

    // MainStateMachine::getInstance().setup();
    InputMode inputMode = InputMode::INPUT_MODE_CONFIG;
    // InputMode inputMode = InputMode::INPUT_MODE_XINPUT;
    ConfigType configType = ConfigType::CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    Gamepad& gamepad = Gamepad::getInstance();
    gamepad.setup();

    GPDriver * inputDriver = DriverManager::getInstance().getDriver();
    
    bool configMode = true;
    
    // Start the TinyUSB Device functionality
    tud_init(TUD_OPT_RHPORT);

    uint32_t t = HAL_GetTick();

    while(1) {

        if(HAL_GetTick() - t >= 1)
        {
            // LED1_Toggle;s

            gamepad.loop();
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