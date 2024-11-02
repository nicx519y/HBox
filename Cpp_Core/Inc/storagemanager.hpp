#ifndef STORAGE_H_
#define STORAGE_H_

// #include "config.hpp"
#include "gamepad.hpp"
#include <stdint.h>

#define SI Storage::getInstance()

// Storage manager for board, LED options, and thread-safe settings
class Storage {
public:
	Storage(Storage const&) = delete;
	void operator=(Storage const&)  = delete;
	static Storage& getInstance() // Thread-safe storage ensures cross-thread talk
	{
		static Storage instance;
		return instance;
	}

	// Config& getConfig() { return config; }

	GamepadOptions &getGamepadOptions();
	ADCButton* (&getADCButtonOptions())[NUM_ADC_BUTTONS];
	GPIOButton* (&getGPIOButtonOptions())[NUM_GPIO_BUTTONS];

	void init();
	bool save();

	void SetGamepad(Gamepad *); 		// MPGS Gamepad Get/Set
	Gamepad * GetGamepad();

	void ResetSettings(); 				// EEPROM Reset Feature

private:
	Storage() {}
	Config config;
	Gamepad* gamepad;
};

#endif
