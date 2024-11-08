#ifndef _MAIN_STATE_MACHINE_
#define _MAIN_STATE_MACHINE_

#include "adc_btns_manager.hpp"
#include "leds_manager.hpp"
#include "gpio_btns_manager.hpp"
#include "storagemanager.hpp"
#include "drivermanager.hpp"
#include "configmanager.hpp"
#include "gamepad.hpp"
#include "enums.hpp"
#include "config.hpp"
#include "stm32h7xx.h"

class MainStateMachine {
    public:
        MainStateMachine(MainStateMachine const&) = delete;
        void operator=(MainStateMachine const&) = delete;
        static MainStateMachine& getInstance() {
            static MainStateMachine instance;
            return instance;
        }
        void setup();

    private:
        uint32_t ust;
        
        DriverManager& driverManager;
        ConfigManager& configManager;
        Gamepad& gamepad;
        Storage& storage;

        MainStateMachine();
};

#endif // ! _MAIN_STATE_MACHINE_
