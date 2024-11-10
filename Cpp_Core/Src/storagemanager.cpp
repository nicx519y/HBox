#include "storagemanager.hpp"
#include "config.hpp"
#include "stm32h750xx.h"
#include <stdio.h>


void Storage::init() {
	printf("================== Storage::init begin ==========================\n");
	ConfigUtils::load(config);
}

bool Storage::save()
{
	return ConfigUtils::save(config);
}

bool Storage::ResetSettings()
{
	printf("================== Storage::resettings begin ==========================\n");
	return ConfigUtils::reset(config);
	// NVIC_SystemReset();				//reboot
}

