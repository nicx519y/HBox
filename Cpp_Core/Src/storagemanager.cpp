#include "storagemanager.hpp"
#include "config.hpp"
#include "stm32h750xx.h"


void Storage::init() {
	ConfigUtils::load(config);
}

bool Storage::save()
{
	return ConfigUtils::save(config);
}

GamepadOptions& Storage::getGamepadOptions()
{
	return *(config.profiles[config.profileIndex]);
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
	NVIC_SystemReset();				//reboot
}

void Storage::SetGamepad(Gamepad * newpad)
{
	gamepad = newpad;
}

Gamepad * Storage::GetGamepad()
{
	return gamepad;
}
