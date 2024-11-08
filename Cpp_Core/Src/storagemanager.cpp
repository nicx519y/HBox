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

MainState Storage::getMainState()
{
	return config.mainState;
}

bool Storage::setMainState(MainState state)
{
	if(state == config.mainState)
		return false;
	else {
		config.mainState = state;
		return true;
	}
}

bool Storage::setProfileIndex(uint8_t idx)
{
	if(config.profileIndex == idx)
		return false;
	else {
		config.profileIndex = idx;
		return true;
	}
}

GamepadOptions &Storage::getGamepadOptions()
{
	return *config.profiles[config.profileIndex];
}

ADCButton* (&Storage::getADCButtonOptions())[NUM_ADC_BUTTONS]
{
	return config.ADCButtons;
}

GPIOButton* (&Storage::getGPIOButtonOptions())[NUM_GPIO_BUTTONS]
{
	return config.GPIOButtons;
}


void Storage::ResetSettings()
{
	printf("================== Storage::resettings begin ==========================\n");
	ConfigUtils::reset(config);
	// NVIC_SystemReset();				//reboot
}

