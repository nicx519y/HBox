#ifndef STORAGE_H_
#define STORAGE_H_

// #include "config.hpp"
#include "gamepad.hpp"
#include <stdint.h>
#include <string>

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

	void init();
	bool save();
	bool ResetSettings(); 				// EEPROM Reset Feature
	GamepadProfile* getGamepadProfile(char* id);
	GamepadProfile* getDefaultGamepadProfile() { 
		return getGamepadProfile(config.defaultProfileId); 
	}
	
	Config config;
private:
	Storage() {}
	
	Gamepad* gamepad;
};

#endif
