#include "main_state_machine.hpp"
#include "tim.h"
#include "qspi-w25q64.h"
#include "constant.hpp"

static uint32_t usTick = 0;

MainStateMachine::MainStateMachine():
    driverManager(DriverManager::getInstance()),
    configManager(ConfigManager::getInstance()),
    gamepad(Gamepad::getInstance()),
    storage(Storage::getInstance())
{}

void MainStateMachine::setup()
{
    printf("MainStateMachine::setup \n");
    Storage::getInstance().init();
    printf("Storage init success.\n");

    BootMode bootMode = Storage::getInstance().config.bootMode;
    printf("BootMode: %d\n", bootMode);

    GamepadProfile* opts = Storage::getInstance().getDefaultGamepadProfile();
    InputMode inputMode = opts->keysConfig.inputMode;
    printf("InputMode: %d\n", inputMode);
    // mainState = MainState::MAIN_STATE_WEB_CONFIG;

    // printf("MainState: %d\n", (uint8_t) mainState);

    GPDriver* driver;

    switch(bootMode) {
        case BootMode::BOOT_MODE_WEB_CONFIG:
        
            driverManager.setup(InputMode::INPUT_MODE_CONFIG);      
            configManager.setup(ConfigType::CONFIG_TYPE_WEB);
            gamepad.setup();
            tud_init(TUD_OPT_RHPORT);
            while(1) {
                configManager.loop();
                if(this->ust != usTick) {
                    this->ust = usTick;
                    gamepad.loop();
                }
            }
            break;
        case BootMode::BOOT_MODE_ADC_BTNS_CALIBRATING:
            
            gamepad.setup();
            gamepad.ADCBtnsCalibrateStart();

            while(1) {
                gamepad.loop();
            }

            break;
        case BootMode::BOOT_MODE_INPUT:
            // driverManager.setup((InputMode)Storage::getInstance().getGamepadOptions().inputMode);
            driverManager.setup(inputMode);
            driver = driverManager.getDriver();
            gamepad.setup();
            tud_init(TUD_OPT_RHPORT);
            while(1) {
                gamepad.loop();                 // 获取按键状态
                driver->process(&gamepad);      // 把按键状态形成usb report
                tud_task();                     // usb device task.  report
            }
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