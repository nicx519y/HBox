#include "cpp_main.hpp"
#include <stdio.h>
#include "bsp/board_api.h"
#include "main_state_machine.hpp"
#include "fsdata.h"
#include "led.h"
#include "qspi-w25q64.h"

int cpp_main(void) 
{   
    board_init();
    printf("================== board_init success. =======================\n");
    
    // Test FPU Status
    uint32_t fpscr = __get_FPSCR();
    printf("================== FPSCR = 0x%08lx =======================\r\n", fpscr);
     
    Storage::getInstance().init();
    printf("================== Storage init success. =======================\n");
    

    // 测试存储区读写功能
    int8_t result = QSPI_W25Qxx_Test(0x400000);
    if(result != QSPI_W25Qxx_OK) {
        printf("========================== QSPI_W25Qxx_Test failed! Error: %d====================\n", result);
    } else {
        printf("========================== QSPI_W25Qxx_Test passed!=============================\n");
    }

    getFSRoot();
    printf("================== getFSRoot success. =======================\n");
    // MainStateMachine::getInstance().setup();
    InputMode inputMode = InputMode::INPUT_MODE_CONFIG;
    // InputMode inputMode = InputMode::INPUT_MODE_XINPUT;
    ConfigType configType = ConfigType::CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    Gamepad& gamepad = Gamepad::getInstance();
    gamepad.setup();

    GPDriver * inputDriver = DriverManager::getInstance().getDriver();
    printf("================== DriverManager setup success. =======================\n");

    bool configMode = true;
    
    // Start the TinyUSB Device functionality
    tud_init(TUD_OPT_RHPORT);

    uint32_t t = HAL_GetTick();

    while(1) {
        if(HAL_GetTick() - t >= 1000)
        {
            LED1_Toggle;
            // gamepad.loop();
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