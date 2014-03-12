/*
 * MBFont.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#ifndef MBFONT_HPP_
#define MBFONT_HPP_

#include "mbfont.h"

#include "MFont.hpp"
#include "MBitmap.hpp"
#include "File.hpp"

/*! \brief Monochrome Bitmap Font class */
class MBFont : public MFont {
public:

	/*! \brief Construct using a font stored in memory */
	MBFont(const void * font);

	/*! \brief Construct using a font stored on a file */
	MBFont(const char * name, int offset = 0);

	~MBFont();

	/*! \brief Return the max byte width of the font (used to construct a target bitmap) */
	uint8_t max_byte_width();
	/*! \brief Return the max height for the font (used to construct a target bitmap) */
	uint8_t max_height();

	/*! \brief Draw a character on the bitmap */
	/*! \details This methods draws a character (\a c) on
	 * the \a bitmap.
	 * @param bitmap The bitmap to draw the character on
	 * @param c The character to draw
	 */
	MBitmap * bitmap(char c);

	int len(const char * str);

private:
	File f;
	const void * font_;
	uint8_t max_byte_width_;
	uint8_t max_height_;
	MBitmap * bitmap_;
	int offset_;

	int load_char(mbfont_char_t & ch, char c);
	int load_bitmap(const mbfont_char_t & ch,char c);

	int load_char_file(mbfont_char_t & ch, char c);
	int load_char_ptr(mbfont_char_t & ch, char c);

	int load_bitmap_file(const mbfont_char_t & ch,char c);
	int load_bitmap_ptr(const mbfont_char_t & ch, char c);

};

#endif /* MBFONT_HPP_ */
