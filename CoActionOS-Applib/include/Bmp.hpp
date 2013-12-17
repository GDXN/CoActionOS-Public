/*
 * Bmp.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: tgil
 */

#ifndef BMP_HPP_
#define BMP_HPP_

#include <applib/File.hpp>

class Bmp: public File {
public:
	Bmp(const char * name);

	int32_t width(){ return dib.width; }
	int32_t height(){ return dib.height; }
	uint16_t bits_per_pixel(){ return dib.bits_per_pixel; }
	uint16_t planes(){ return dib.planes; }

	unsigned int row_size() const;

	int create(const char * name, uint32_t size, int32_t width, int32_t height, uint16_t planes, uint16_t bits_per_pixel);

	void rewind(void){ seek(offset_); }

	int seek_row(int32_t y);

	typedef struct HWPL_PACK {
		uint16_t signature;
		uint32_t size;
		uint16_t resd1;
		uint16_t resd2;
		uint32_t offset;
	} bmp_header_t;

	typedef struct HWPL_PACK {
		uint32_t hdr_size;
		int32_t width;
		int32_t height;
		uint16_t planes;
		uint16_t bits_per_pixel;
	} bmp_dib_t;

	enum {
		SIGNATURE = 0x424D
	};

private:

	bmp_dib_t dib;
	uint32_t offset_;
};

#endif /* BMP_HPP_ */
