#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>

extern FS CONFIG_SPIFFS;


//! write a non-string setting to specified settings entry
bool settings_write(const String & key, const void * ptr, size_t size)
{
	const char mode[2]  = { 'w',  0  };
	File file = CONFIG_SPIFFS.open(key, mode);
	if(!file) return false;

	bool success = size == file.write(reinterpret_cast<const uint8_t *>(ptr), size);

	file.close();
	return success;
}

//! write a string setting to specified settings entry
bool settings_write(const String & key, const String & value)
{
	const char mode[2]  = { 'w',  0  };
	File file = CONFIG_SPIFFS.open(key, mode);
	if(!file) return false;

	size_t size = value.length();

	bool success = size == file.write(
		reinterpret_cast<const uint8_t *>(value.c_str()), size);

	file.close();
	return success;
}



//! read a non-string setting from specified settings entry
bool settings_read(const String & key, void *ptr, size_t size)
{
	const char mode[2]  = { 'r',  0  };
	File file = CONFIG_SPIFFS.open(key, mode);
	if(!file) return false;

	bool success = size == file.read(reinterpret_cast<uint8_t *>(ptr), size);

	file.close();
	return success;
}



//! read a string setting from specified settings entry
bool settings_read(const String & key, String & value)
{
	const char mode[2]  = { 'r',  0  };
	File file = CONFIG_SPIFFS.open(key, mode);
	if(!file) return false;

	size_t size = file.size();
	if(!value.reserve(size))
	{
		file.close();
		return false; // no memory ?
	}
	char * ptr = const_cast<char *> (value.c_str());
	bool success = size == file.read(reinterpret_cast<uint8_t *>(ptr), size);
	if(success) ptr[size] = '\0';

	file.close();
	return success;
}




