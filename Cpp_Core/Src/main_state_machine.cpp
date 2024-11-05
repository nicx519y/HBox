#include "main_state_machine.hpp"

MainStateMachine::MainStateMachine(){}

void MainStateMachine::setup()
{
    Storage::getInstance().init();
    MainState mainState = Storage::getInstance().getMainState();

    switch(mainState) {
        case MainState::WEB_CONFIG:
            DriverManager::getInstance().setup(InputMode::INPUT_MODE_CONFIG);      
            ConfigManager::getInstance().setup(ConfigType::CONFIG_TYPE_WEB);
            Gamepad::getInstance().setup();
            tud_init(TUD_OPT_RHPORT);
            this->webConfigRun();
            break;
        case MainState::ADC_BTNS_CALIBRATING:

            break;
        default:    // INPUT
            break;
    }
}

void MainStateMachine::webConfigRun()
{
    ConfigManager& ConfigManager = ConfigManager::getInstance();

    while(1)
    {
        ConfigManager.loop();
    }
}

void MainStateMachine::calibratingRun()
{
    Gamepad& gamepad = Gamepad::getInstance();
}

void MainStateMachine::inputRun()
{

}