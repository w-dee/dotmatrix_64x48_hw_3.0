#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
#include "settings.h"

extern FS SETTINGS_SPIFFS;
extern FS SPIFFS;
static constexpr size_t MAX_KEY_LEN = 30;
static constexpr int CHECKSUM_SIZE = sizeof(uint32_t); // in bytes

//! initial value for crc.
//! 0x00000000 or 0xffffffff is not suitable because it is
//! indistinguishable from all-cleared RAM or all-cleared FLASH ROM.
static constexpr uint32_t INITIAL_CRC_VALUE = 0x12345678;

extern "C" { uint32_t crc_update(uint32_t crc, const uint8_t *data, size_t length); } // available in eboot_command.c

//! check checksum for a setting.
//! file pointer is set just after the setting checksum.
//! returns whether the check sum is valid.
static bool settings_check_crc(File & file)
{
	uint32_t file_crc = 0;
	if(file.read(reinterpret_cast<uint8_t *>(&file_crc), CHECKSUM_SIZE) != CHECKSUM_SIZE)
		return false; // read error

	uint8_t ibuf[64];
	uint32_t crc = INITIAL_CRC_VALUE;

	while(true)
	{
		size_t isz = file.read(ibuf, sizeof(ibuf));
		if(isz == 0) break;
		crc = crc_update(crc, ibuf, isz);
	}

	file.seek(CHECKSUM_SIZE); // set file pointer just after the check sum

//	Serial.printf_P(PSTR("file checksum: %08x   computed checksum: %08x\r\n"), file_crc, crc); 
	return crc == file_crc;
}


// write checksum 

//! write a non-string setting to specified settings entry
bool settings_write(const String & key, const void * ptr, size_t size, settings_overwrite_t overwrite)
{
	if(key.length()  > MAX_KEY_LEN) return false;

	if(overwrite == SETTINGS_NO_OVERWRITE)
	{
		// check key existence
		const char mode[2]  = { 'r',  0  };
		File file = SETTINGS_SPIFFS.open(key, mode);
		if(file)
		{
			if(settings_check_crc(file))
			{
				// valid key already exists; do not overwrite.
				file.close();
				return false;
			}
		}
		file.close();	
	}

	const char mode[2]  = { 'w',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	uint32_t crc = INITIAL_CRC_VALUE;
	crc = crc_update(crc, reinterpret_cast<const uint8_t *>(ptr), size);
	bool success;
	success = CHECKSUM_SIZE == file.write(reinterpret_cast<const uint8_t *>(&crc), CHECKSUM_SIZE);
	if(!success) { file.close(); return false; }

	success = size == file.write(reinterpret_cast<const uint8_t *>(ptr), size);

	file.close();
	return success;
}

//! write a string setting to specified settings entry
bool settings_write(const String & key, const String & value, settings_overwrite_t overwrite)
{
	return settings_write(key, value.c_str(), value.length(), overwrite);
}



//! read a non-string setting from specified settings entry
bool settings_read(const String & key, void *ptr, size_t size)
{
	const char mode[2]  = { 'r',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	if(!settings_check_crc(file)) { file.close(); return false; }

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

	if(!settings_check_crc(file)) { file.close(); return false; }

	size_t size = file.size() - CHECKSUM_SIZE;
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
		if(!settings_check_crc(in)) { in.close(); continue; }
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

		// reserve checksum space
		uint32_t crc = 0xffffffff;
		if(CHECKSUM_SIZE !=
			out.write(reinterpret_cast<const uint8_t *>(&crc),
				CHECKSUM_SIZE)) return false; // file write error
		crc = INITIAL_CRC_VALUE;

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
			crc = crc_update(crc, obuf, osz);
			if(osz != out.write(obuf, osz)) return false;
			if(end_found) break;
		}

		// write CRC back
		out.seek(0);
		if(CHECKSUM_SIZE !=
			out.write(reinterpret_cast<const uint8_t *>(&crc),
				CHECKSUM_SIZE)) return false; // file write error
	}

	return true;
}



