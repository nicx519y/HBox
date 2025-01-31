#include "cpp_main.hpp"
#include <stdio.h>
#include "bsp/board_api.h"
#include "main_state_machine.hpp"
#include "fsdata.h"
#include "led.h"
#include "qspi-w25q64.h"
#include "message_center.hpp"
#include "adc.h"

int cpp_main(void) 
{   
    board_init();
    printf("================== board_init success. =======================\n");
    
    // Test FPU Status
    uint32_t fpscr = __get_FPSCR();
    printf("================== FPSCR = 0x%08lx =======================\r\n", fpscr);
     
    // 注册ADC消息
    MC.registerMessage(MessageId::DMA_ADC_CONV_CPLT);

    getFSRoot();
    printf("================== getFSRoot success. =======================\n");
    // MainStateMachine::getInstance().setup();

    // InputMode inputMode = InputMode::INPUT_MODE_CONFIG;
    // // InputMode inputMode = InputMode::INPUT_MODE_XINPUT;
    // ConfigType configType = ConfigType::CONFIG_TYPE_WEB;
    // DriverManager::getInstance().setup(inputMode);      
    // ConfigManager::getInstance().setup(configType);


    // bool configMode = true;
    
    // // Start the TinyUSB Device functionality
    // tud_init(TUD_OPT_RHPORT);

    // uint32_t t = HAL_GetTick();

    // while(1) {
    //     if(HAL_GetTick() - t >= 1000)
    //     {
    //         printf("================== process =======================\n");
    //         LED1_Toggle;
    //         // gamepad.loop();
    //         t = HAL_GetTick();
    //     }

    //     if(configMode) {
    //         ConfigManager::getInstance().loop();
    //     } else {
    //         // inputDriver->process(&gamepad);
    //         tud_task();
    //     }
    // }

    MAIN_STATE_MACHINE.setup();

    return 0;
} 

// ADC转换完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    MC.publish(MessageId::DMA_ADC_CONV_CPLT, hadc);
}