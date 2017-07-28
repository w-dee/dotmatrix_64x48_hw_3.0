#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
#include "settings.h"
#include "microtar.h"

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
bool settings_write(const String & _key, const void * ptr, size_t size, settings_overwrite_t overwrite)
{
	if(_key.length()  > MAX_KEY_LEN) return false;
	String key = String(F("/")) + _key;

	if(overwrite.overwrite == false)
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
bool settings_read(const String & _key, void *ptr, size_t size)
{
	if(_key.length()  > MAX_KEY_LEN) return false;
	String key = String(F("/")) + _key;

	const char mode[2]  = { 'r',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	if(!settings_check_crc(file)) { file.close(); return false; }

	bool success = size == file.read(reinterpret_cast<uint8_t *>(ptr), size);

	file.close();
	return success;
}



//! read a string setting from specified settings entry
bool settings_read(const String & _key, String & value)
{
	if(_key.length()  > MAX_KEY_LEN) return false;
	String key = String(F("/")) + _key;

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


//! Write string vector settings
bool settings_write_vector(const String & key, const string_vector & value, settings_overwrite_t overwrite)
{
	// compute all string length sum
	size_t sum = 0;
	for(auto && v : value) sum += v.length() + 1;

	// allocate memory enough to contain all list
	char *blk = new char[sum];
	if(!blk) return false;

	// copy string content to the blk. the delimiter is \0
	char *p = blk;
	for(auto && v : value)
	{
		strcpy(p, v.c_str());
		p += v.length() + 1;
	}

	// write settings
	bool res = settings_write(key, blk, sum, overwrite);

	// free blk and return
	delete [] blk;
	return res;
}

//! Read string vector settings
bool settings_read_vector(const String & _key, string_vector & value)
{
	if(_key.length()  > MAX_KEY_LEN) return false;
	String key = String(F("/")) + _key;

	value.clear();

	const char mode[2]  = { 'r',  0  };
	File file = SETTINGS_SPIFFS.open(key, mode);
	if(!file) return false;

	if(!settings_check_crc(file)) { file.close(); return false; }

	size_t size = file.size() - CHECKSUM_SIZE;

	char *ptr = new char[size + 1];
	if(!ptr) return false; // memory error

	bool success = size == file.read(reinterpret_cast<uint8_t *>(ptr), size);
	if(!success) { delete [] ptr; file.close(); return false; /* read error*/ }

	ptr[size] = 0; // force terminate at last

	char *p = ptr;

	while(p < ptr + size)
	{
		String t = p;
		p += t.length();
		++ p; // skip \0
		value.push_back(t);
	}

	delete [] ptr;
	file.close();
	return success;

}


//! Serialize settings to specified main fs partition filename
bool settings_export(const String & target_name,
	const String & exclude_prefix)
{
	const char wmode[2]  = { 'w',  0 };
	const char rmode[2]  = { 'r',  0 };
	const char rootstr[2] = { '/', 0 };
	Dir dir;

	// allocate mtar_t. use heap to reduce stack usage.
	mtar_t *p_tar = new mtar_t;
	if(!p_tar) return false;

	// open tar archive for wriring
	if(MTAR_ESUCCESS != mtar_open(p_tar, target_name.c_str(), wmode))
		goto error_end; // open error

	// scan settings directory
	Serial.printf_P(PSTR("Scanning dir ... \r\n"));
	dir = SETTINGS_SPIFFS.openDir(rootstr);
	while(dir.next())
	{
		// skip excluded file name
		String filename = dir.fileName();
		Serial.printf_P(PSTR("Exporting setting %s ... \r\n"), filename.c_str());
		if(exclude_prefix.length() != 0 &&
			filename.startsWith(exclude_prefix)) continue;

		// write content as base64
		File in = dir.openFile(rmode);
		size_t size = in.size() - CHECKSUM_SIZE;
		if(!settings_check_crc(in)) { in.close(); continue; }

		// write header
		if(MTAR_ESUCCESS != mtar_write_file_header(p_tar, filename.c_str(), size))
			goto error_end; // write error

		// write content
		uint8_t buf[128];
		while(true)
		{
			size_t isz = in.read(buf, sizeof(buf));
			if(isz == 0) break;
			if(MTAR_ESUCCESS != mtar_write_data(p_tar, buf, isz))
				goto error_end; // write error
		}
	}
	Serial.printf_P(PSTR("Done scanning dir.\r\n"));

	mtar_finalize(p_tar);
	mtar_close(p_tar);

	delete p_tar;
	return true;

error_end:
	delete p_tar;
	return false;
}


//! import settings from specified main fs partition filename
bool settings_import(const String & target_name)
{
	const char wmode[2]  = { 'w',  0 };
	const char rmode[2]  = { 'r',  0 };
	const char nullstr[1] = { 0 };

	// allocate mtar_t and its header. use heap to reduce stack usage.
	mtar_t *p_tar;
	mtar_header_t *p_h;

	p_tar = new mtar_t;
	p_h = new mtar_header_t;
	if(!p_tar || !p_h) goto error_end; // memory error

	// open archive for reading
	if(MTAR_ESUCCESS != mtar_open(p_tar, target_name.c_str(), rmode))
		goto error_end; // open error

	// read input archive
	while( (mtar_read_header(p_tar, p_h)) != MTAR_ENULLRECORD)
	{
		// extract basename of the filename
		String fn = p_h->name;
		int last_slash = fn.lastIndexOf('/');
		if(last_slash != -1)
			fn = fn.c_str() + last_slash + 1; // extract basename

		// open file
		File out = SETTINGS_SPIFFS.open(fn.c_str(), wmode);

		// reserve checksum space
		uint32_t crc = 0xffffffff;
		if(CHECKSUM_SIZE !=
			out.write(reinterpret_cast<const uint8_t *>(&crc),
				CHECKSUM_SIZE)) return false; // file write error
		crc = INITIAL_CRC_VALUE;

		// copy value
		uint8_t ibuf[64];
		size_t size = p_h->size;
		while(true)
		{
			size_t one_size = sizeof(ibuf) < size ? sizeof(ibuf) : size;
			if(one_size == 0) break;
			if(MTAR_ESUCCESS != mtar_read_data(p_tar, ibuf, one_size))
				goto error_end; // read error
			crc = crc_update(crc, ibuf, one_size);
			if(one_size != out.write(ibuf, one_size)) goto error_end;
		}

		// write CRC back
		out.seek(0);
		if(CHECKSUM_SIZE !=
			out.write(reinterpret_cast<const uint8_t *>(&crc),
				CHECKSUM_SIZE)) goto error_end; // file write error
		out.close();
	}

	if(p_tar) delete p_tar;
	if(p_h) delete p_h;
	return true;

error_end:
	if(p_tar) delete p_tar;
	if(p_h) delete p_h;
	return false;

}



