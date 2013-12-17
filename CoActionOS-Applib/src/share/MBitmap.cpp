/*
 * MBitmapp.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#include <stdlib.h>
#include "MBitmap.hpp"

MBitmap::MBitmap(unsigned int w, unsigned int h) : w(w), y_max(h){
	columns = (w + 7) >> 3;
	mem_write = (uint8_t*)malloc(size(w,h));
	mem = mem_write;
	data_size = size(w,h);
}

MBitmap::MBitmap(const uint8_t * mem, unsigned int w, unsigned int h) :
	mem(mem),
	mem_write(0),
	w(w),
	y_max(h){
	columns = byte_width(w);
	data_size = size(w,h);
}

MBitmap::MBitmap(uint8_t * mem, unsigned int w, unsigned int h) :
	mem(mem),
	mem_write(mem),
	w(w),
	y_max(h){
	columns = byte_width(w);
	data_size = size(w,h);
}

int MBitmap::byte_width(int w){
	return (w + 7) >> 3;
}

int MBitmap::setsize(int w, int h){
	if( size(w,h) <= data_size ){
		this->w = w;
		this->y_max = h;
		this->columns = byte_width(w);
		return 0;
	}
	return -1;
}


bool MBitmap::tstpixel(unsigned int x, unsigned int y) const {
	if( mem[offset(x,y)] & mask(x) ){
		return true;
	}
	return false;
}

void MBitmap::setpixel(unsigned int x, unsigned int y) {
	if( (x < w) && (y < y_max) ){
		mem_write[offset(x,y)] |= mask(x);
	}
}

void MBitmap::clrpixel(unsigned int x, unsigned int y) {
	if( (x < w) && (y < y_max) ){
		mem_write[offset(x,y)] &= ~mask(x);
	}
}

void MBitmap::clear(void){
	memset(mem_write, 0, columns * y_max);
}
