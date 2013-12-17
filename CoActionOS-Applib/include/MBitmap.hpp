/*
 * MBitmap.hpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#ifndef MBITMAP_HPP_
#define MBITMAP_HPP_

#include <stdint.h>
#include <string.h>

class MBitmap {
public:
	MBitmap(const uint8_t * mem, unsigned int w, unsigned int h);
	MBitmap(uint8_t * mem, unsigned int w, unsigned int h);
	MBitmap(unsigned int w, unsigned int h);

	static int byte_width(int w);

	bool tstpixel(unsigned int x, unsigned int y) const;
	void setpixel(unsigned int x, unsigned int y);
	void clrpixel(unsigned int x, unsigned int y);
	void clear(void);

	/*! \brief Return a pointer to the bitmap memory */
	void * data(){ return mem_write; }
	/*! \brief Return size of bitmap memory */
	int size(){ return y_max*byte_width(w); }

	int setsize(int w, int h);

	/*! \brief Return the size of a bitmap of specified size */
	static int size(int w, int h){ return h*byte_width(w); }

	bool isempty(void) const { return mem == 0; }
	unsigned int height(void) const { return y_max; }
	unsigned int width(void) const { return w; }
	unsigned int cols(void) const { return columns; }

protected:
	inline int offset(unsigned int x, unsigned int y) const { return (x>>3) + y*columns; }
	inline uint8_t mask(unsigned int x) const { return (1 << (7 - (x&0x07))); }

private:
	const uint8_t * mem;
	uint16_t data_size;
	uint8_t * mem_write;
	uint16_t w;
	uint16_t columns;
	uint16_t y_max;



};

#endif /* MBITMAP_HPP_ */
