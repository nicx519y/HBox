#include "main_state_machine.hpp"
#include "tim.h"
#include "qspi-w25q64.h"
#include "constant.hpp"

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
    BootMode bootMode = BootMode::BOOT_MODE_WEB_CONFIG;
    printf("BootMode: %d\n", bootMode);

    switch(bootMode) {
        case BootMode::BOOT_MODE_WEB_CONFIG:
        
            state = WEB_CONFIG_STATE;
            state.setup();

            

            while(1) {
                state.process();
            }

            break;
        case BootMode::BOOT_MODE_INPUT:
            // driverManager.setup((InputMode)Storage::getInstance().getGamepadOptions().inputMode);
            // driverManager.setup(inputMode);
            // driver = driverManager.getDriver();
            // gamepad.setup();
            // tud_init(TUD_OPT_RHPORT);
            // while(1) {
            //     gamepad.loop();                 // 获取按键状态
            //     driver->process(&gamepad);      // 把按键状态形成usb report
            //     tud_task();                     // usb device task.  report
            // }
            break;
        default:    // NONE
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