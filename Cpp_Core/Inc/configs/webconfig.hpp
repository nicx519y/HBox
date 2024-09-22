#ifndef _WEBCONFIG_H_
#define _WEBCONFIG_H_

#include "gpconfig.hpp"

class WebConfig : public GPConfig
{
public:
    virtual void setup();
    virtual void loop();

    enum BootModes {
        GAMEPAD,
        WEBCONFIG,
        BOOTSEL
    };
private:
};

#endif