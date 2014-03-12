/*
 * Settings.hpp
 *
 *  Created on: Dec 4, 2013
 *      Author: tgil
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <stdint.h>

class Settings {
public:
	Settings();


	uint32_t value_u32(int key);
	int32_t value_i32(int key);
	uint16_t value_u16(int key);
	int16_t value_i16(int key);
	uint8_t value_u8(int key);
	int8_t value_i8(int key);
	const char * value_str(int key);

	int set(int key, uint32_t v);
	int set(int key, int32_t v);
	int set(int key, uint16_t v);
	int set(int key, int16_t v);
	int set(int key, uint8_t v);
	int set(int key, int8_t v);
	int set(int key, const char * str);

	virtual int load(const char * name, bool appfs = false){ return -1; }
	virtual int load(uint32_t address){ return -1; }

	virtual int save(const char * name, bool appfs = false){ return -1; }
	virtual int save(uint32_t address){ return -1; }

protected:
	virtual void * mem() = 0;
	virtual int size() = 0;

private:

	enum {
		SETTINGS_U32,
		SETTINGS_I32,
		SETTINGS_U16,
		SETTINGS_I16,
		SETTINGS_U8,
		SETTINGS_I8,
		SETTINGS_STR,
		SETTINGS_INVALID //erased entries
	};

	typedef struct HWPL_PACK {
		union {
			uint32_t * u32;
			int32_t * i32;
			uint16_t * u16;
			int16_t * i16;
			uint8_t * u8;
			int8_t * i8;
			char * c;
		};
		uint16_t size;
		uint16_t type;
	} settings_storage_t;


};

#endif /* SETTINGS_HPP_ */
