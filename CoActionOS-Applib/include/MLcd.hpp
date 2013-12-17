/*
 * MLcd.hpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#ifndef MLCD_HPP_
#define MLCD_HPP_

#include <stdint.h>
#include "Font8.hpp"
#include "MBitmap.hpp"

/*! \brief Monochrome LCD
 * \details This is an abstract class for drawing graphics and text
 * on a monochrome LCD.  The typical implementation will keep a copy
 * of the bitmap in local memory and use the refresh() method to copy
 * the local memory to the LCD.
 *
 *
 */
class MLcd {
public:
	MLcd(uint8_t width, uint8_t height) : width_var(width), height_var(height), f(0){}

	/*! \brief Set the font to use with setchar() and setstr() */
	void setfont(const Font8 * font){ f = font; }

	/*! \brief Set \a c on the LCD at \a x, \a y */
	int setchar(const char c, int x, int y);
	int clrchar(const char c, int x, int y);
	/*! \brief Set \a str on the LCD at \a x, \a y */
	void setstr(const char * str, int x, int y);
	/*! \brief Clear \a str on the LCD at \a x, \a y */
	void clrstr(const char * str, int x, int y);
	void sethfill(int x, int y);
	void setvfill(int x, int y);

	/*! \brief Rotation values */
	enum Rotation {
		ROT0 /*! Zero degress */,
		ROT90 /*! 90 degress */,
		ROT180 /*! 180 degress */,
		ROT270 /*! 270 degress */,
		ROT360 /*! 360 degress */,
		ROT_SCALE = 65536
	};

	/*! \brief Set the pixels of \a bitmap at \a x, \a y */
	int setbitmap(const MBitmap & bitmap, int x, int y);
	int clrbitmap(const MBitmap & bitmap, int x, int y);
	/*! \brief Set the pixels of \a bitmap at \a x, \a y using the specified rotation */
	void setbitmap(const MBitmap * bitmap, int x, int y, enum MLcd::Rotation rotation = ROT0);

	void fade(int start, int stop, int max, int steps = 1000, int delay = 5000);

	/*! \brief Maximum x value */
	inline unsigned int xmax() const { return width_var-1; }
	/*! \brief Maximum y value */
	inline unsigned int ymax() const { return height_var-1; }
	/*! \brief LCD width */
	inline int width() const { return width_var; }
	/*! \brief LCD height */
	inline int height() const { return height_var; }

	/*! \brief Set the pixels of a vertical line */
	void setvline(int x, int ymin, int ymax);
	/*! \brief Set the pixels of a horizontal line */
	void sethline(int xmin, int xmax, int y);

	bool tsthline(int xmin, int xmax, int y, int & pos);
	void tsthedge(int x, int y, int & xmin, int & xmax);

	/*! \brief Set the pixels of a line */
	virtual void setline(int x1, int y1, int x2, int y2);

	bool yvisible(int y);
	bool xvisible(int x);

	void boundx(int & x);
	void boundy(int & y);

	/*! \brief Pure virtual function to clear the LCD memory */
	virtual void clear(void) = 0;

	/*! \brief Pure virtual function to initialize the LCD */
	virtual int init(void) = 0;

	/*! \brief Pure virtual function that copies local LCD memory to the LCD screen */
	virtual void refresh(void) = 0;

	/*! \brief Pure virtual function to set a pixel in local LCD memory */
	virtual void setpixel(unsigned int x, unsigned int y) = 0;

	/*! \brief Pure virtual function to test a pixel in local LCD memory */
	virtual bool tstpixel(unsigned int x, unsigned int y) = 0;

	/*! \brief Pure virtual function to clear a pixel in local LCD memory */
	virtual void clrpixel(unsigned int x, unsigned int y) = 0;

	/*! \brief Pure virtual function to test a pixel in local LCD memory (bounded) */
	virtual bool tstpixel_bounded(unsigned int x, unsigned int y) = 0;

	/*! \brief Pure virtual function to set a pixel in local LCD memory (bounded) */
	virtual void setpixel_bounded(unsigned int x, unsigned int y) = 0;

	/*! \brief Pure virtual function to clear a pixel in local LCD memory (bounded) */
	virtual void clrpixel_bounded(unsigned int x, unsigned int y) = 0;

	virtual void setbrightness(int value, int max){};

	/*! \brief Function for filling a bounded area in local LCD memory */
	virtual void setfill(unsigned int x, unsigned int y);


	static const uint8_t mask[8];
	static const int8_t rot_sin[5];
	static const int8_t rot_cos[5];

protected:

	const uint8_t width_var;
	const uint8_t height_var;

private:
	const Font8 * f;


};

#endif /* MLCD_HPP_ */
