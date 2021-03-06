/*
 * Font.hpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#ifndef FONT8_HPP_
#define FONT8_HPP_

#include "Font8Util.hpp"
#include "MBitmap.hpp"

/*! \brief Bitmap Fonts (used with LCDs)
 * \details This class is for using fonts on LCDs.  It has
 * 95 characters per font with a size up to 8x8.
 *
 */
class Font8 {
public:
	Font8(const font8_t * f = 0);

	/*! \brief Load the font from disk.
	 * \details This function saves a font to disk.
	 */
	int load(const char * path);
	MBitmap bitmap(char c) const;
	inline int height() const { return f->h; }
	inline int width() const { return f->w; }

private:
	const font8_t * f;
};

#endif /* FONT8_HPP_ */
