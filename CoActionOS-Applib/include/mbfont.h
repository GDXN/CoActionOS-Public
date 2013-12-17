/*
 * mbfont.h
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#ifndef MBFONT_H_
#define MBFONT_H_


#include <hwpl/types.h>

/*! \brief Defines the header for a monochrome bitmap font */
/*! \details A monochrome bitmap font looks like this in memory (or a file):
 *
 * mbfont_hdr_t hdr;
 * mbfont_char_t char0;
 * mbfont_char_t char1;
 * ...
 * mbfont_char_t char94;
 * const char char0_data[char0.height][MFont::byte_width(char0.width)];
 * const char char1_data[char1.height][MFont::byte_width(char1.width)];
 * ...
 * const char char94_data[char0.height][MFont::byte_width(char94.width)];
 *
 */
typedef struct HWPL_PACK {
	uint8_t num_chars;
	uint8_t max_byte_width;
	uint8_t max_height;
	uint32_t size;
} mbfont_hdr_t;


typedef struct HWPL_PACK {
	uint8_t width;
	uint8_t height;
	uint16_t offset;
} mbfont_char_t;


#endif /* MBFONT_H_ */
