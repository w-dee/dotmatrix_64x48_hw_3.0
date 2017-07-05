#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include "libb64/cdecode.h"
#include "libb64/cencode.h"

extern FS SETTINGS_SPIFFS;
extern FS SPIFFS;
static constexpr size_t MAX_KEY_LEN = 30;

//! write a non-string setting to specified settings entry
bool settings_write(const String & key, const void * ptr, size_t size)
{
	const char mode[2]  = { 'w',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	bool success = size == file.write(reinterpret_cast<const uint8_t *>(ptr), size);

	file.close();
	return success;
}

//! write a string setting to specified settings entry
bool settings_write(const String & key, const String & value)
{
	if(key.length()  > MAX_KEY_LEN) return false;

	const char mode[2]  = { 'w',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
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
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	bool success = size == file.read(reinterpret_cast<uint8_t *>(ptr), size);

	file.close();
	return success;
}



//! read a string setting from specified settings entry
bool settings_read(const String & key, String & value)
{
	const char mode[2]  = { 'r',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
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

//! Serialize settings to specified main fs partition filename
bool settings_export(const String & target_name,
	const String & exclude_prefix)
{
	const char wmode[2]  = { 'w',  0 };
	const char rmode[2]  = { 'r',  0 };
	const char nullstr[1] = { 0 };
	File exp = SPIFFS.open(target_name, wmode);

	Dir dir = SETTINGS_SPIFFS.openDir(nullstr);

	while(dir.next())
	{
		// skip excluded file name
		String filename = dir.fileName();
		if(exclude_prefix.length() == 0 &&
			filename.startsWith(exclude_prefix)) continue;

		// write settings key as
		// "<name>:'<value in base64>'\r\n" representation
		exp.print(filename);
		exp.print(':');
		exp.print('\'');

		// write content as base64
		File in = dir.openFile(rmode);
		base64_encodestate b64state;
		base64_init_encodestate(&b64state);
		uint8_t ibuf[64];
		uint8_t obuf[128];
		while(true)
		{
			size_t isz = in.read(ibuf, sizeof(ibuf));
			size_t osz;
			if(isz == 0)
				osz = base64_encode_blockend((char *)obuf, &b64state);
			else
				osz = base64_encode_block((const char *)ibuf, isz, (char *)obuf, &b64state);
			if(osz != exp.write(obuf, osz)) return false;
			if(isz == 0) break;
		}
		exp.print('\'');
		exp.print('\r');
		exp.print('\n');
	}

	return true;
}

//! Skip whitespace in the file stream; returns false if the stream reachs the end
static bool skipWS(File & s)
{
	int n;
	do
	{
		n = s.read();
		if(n < 0) return false;
	} while(n <= 0x20) ;

	// at this point, the file pointer points a
	// position just after the non-space character;
	// walk back a character.
	s.seek(s.position() - 1);

	return true;
}


//! import settings from specified main fs partition filename
bool settings_import(const String & target_name)
{
	const char wmode[2]  = { 'w',  0 };
	const char rmode[2]  = { 'r',  0 };
	const char nullstr[1] = { 0 };
	File imp = SPIFFS.open(target_name, rmode);

	// parse input file
	while(true)
	{
		// skip whitespace
		if(!skipWS(imp)) break;

		// parse name
		char key_name[MAX_KEY_LEN + 1];
		size_t sz = imp.readBytesUntil(':', key_name, sizeof(key_name) - 1);
		if(sz == 0) break;
		key_name[MAX_KEY_LEN] = 0;
		if(!skipWS(imp)) break;

		if(imp.read() != '\'') return false; // opening "'" not found

		// open file
		File out = SETTINGS_SPIFFS.open(key_name, wmode);

		// parse value
		uint8_t ibuf[64];
		uint8_t obuf[64];
		base64_decodestate b64state;
		base64_init_decodestate(&b64state);
		while(true)
		{
			bool end_found = false;
			size_t pos_start = imp.position();
			size_t isz = imp.read(ibuf, sizeof(ibuf));
			for(size_t i = 0; i < isz; ++i)
			{
				if(ibuf[i] == '\'')
				{
					// delimiter found
					end_found = true;
					isz = i;
					imp.seek(pos_start + isz + 1); // just after '\'' position
					break;
				}
			}

			size_t osz;
			if(isz == 0) break;
			osz = base64_decode_block((const char *)ibuf, isz, (char *)obuf, &b64state);
			if(osz != out.write(obuf, osz)) return false;
			if(end_found) break;
		}
	}

	return true;
}



