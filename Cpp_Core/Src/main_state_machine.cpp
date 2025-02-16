#include "main_state_machine.hpp"

MainStateMachine::MainStateMachine() 
    : gamepad(Gamepad::getInstance())
    , storage(Storage::getInstance())
    , state(WebConfigState::getInstance())
{

}

void MainStateMachine::setup()
{
    printf("MainStateMachine::setup \n");
    Storage::getInstance().initConfig();
    printf("Storage initConfig success.\n");

    // BootMode bootMode = Storage::getInstance().config.bootMode;
    // BootMode bootMode = BootMode::BOOT_MODE_WEB_CONFIG;
    BootMode bootMode = BootMode::BOOT_MODE_INPUT;
    printf("BootMode: %d\n", bootMode);

    workTime = MICROS_TIMER.micros();

    switch(bootMode) {
        case BootMode::BOOT_MODE_WEB_CONFIG:
        
            state = WEB_CONFIG_STATE;
            state.setup();

            while(1) {
                state.loop();
            }

            break;
        case BootMode::BOOT_MODE_INPUT:

            /*** 初始化ADC按钮 & LED test begin ***/
            
            ADC_BTNS_WORKER.setup();
            GPIO_BTNS_WORKER.setup();

            workTime = MICROS_TIMER.micros();
            calibrationTime = MICROS_TIMER.micros();

            WS2812B_Start();
            WS2812B_SetAllLEDColor(255, 255, 0);
            WS2812B_SetAllLEDBrightness(30);
            
            // struct RGBColor color1 = {0, 0, 255};
            // struct RGBColor color2 = {255, 255, 0};

            // ADC_BTNS_WORKER.setup();
            // ADC_BTNS_WORKER.test();

            // MC.subscribe(MessageId::ADC_BTNS_STATE_CHANGED, [color1, color2](const void* data) {
            //     // printBinary("ADC_BTNS_STATE_CHANGED: ", *(uint32_t*) data);
            //     WS2812B_SetLEDColorByMask(color1, color2, *(uint32_t*) data);
            // });

            while(1) {
                if(MICROS_TIMER.checkInterval(READ_BTNS_INTERVAL, workTime)) {
                    virtualPinMask = GPIO_BTNS_WORKER.read() | ADC_BTNS_WORKER.read();
                }

                #if ENABLED_DYNAMIC_CALIBRATION == 1
                if(MICROS_TIMER.checkInterval(DYNAMIC_CALIBRATION_INTERVAL, calibrationTime)) {
                    ADC_BTNS_WORKER.dynamicCalibration();
                }
                #endif

            }

            break;



    }
}
