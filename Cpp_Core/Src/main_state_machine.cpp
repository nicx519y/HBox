#include "main_state_machine.hpp"

static uint32_t usTick = 0;

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
            

            // WS2812B_Start();
            // WS2812B_SetAllLEDColor(255, 255, 0);
            // WS2812B_SetAllLEDBrightness(30);
            
            // struct RGBColor color1 = {0, 0, 255};
            // struct RGBColor color2 = {255, 255, 0};

            ADC_BTNS_WORKER.setup();

            // MC.subscribe(MessageId::ADC_BTNS_STATE_CHANGED, [color1, color2](const void* data) {
            //     // printBinary("ADC_BTNS_STATE_CHANGED: ", *(uint32_t*) data);
            //     WS2812B_SetLEDColorByMask(color1, color2, *(uint32_t*) data);
            // });
            break;



    }
}

/**
 * @brief 8000Hz 定时器中断回调
 * 
 * @param htim 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 50us计时器
    if (htim->Instance == htim2.Instance)
    {
        usTick ++;  //计数增加
    }
}