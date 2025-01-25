#ifndef _MAIN_STATE_MACHINE_
#define _MAIN_STATE_MACHINE_

#include "leds_manager.hpp"
#include "gpio_btns_worker.hpp"
#include "storagemanager.hpp"
#include "drivermanager.hpp"
#include "configmanager.hpp"
#include "gamepad.hpp"
#include "enums.hpp"
#include "config.hpp"
#include "stm32h7xx.h"
#include "states/base_state.hpp"
#include "states/webconfig_state.hpp"

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
        MainStateMachine();
        Gamepad& gamepad;
        Storage& storage;
        BaseState& state;

};

#define MAIN_STATE_MACHINE MainStateMachine::getInstance()

#endif // ! _MAIN_STATE_MACHINE_
