/*
 * Bmp.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: tgil
 */

#include "Bmp.hpp"


Bmp::Bmp(const char * name){
	bmp_header_t hdr;

	dib.width = -1;
	dib.height = -1;
	dib.bits_per_pixel = 0;

	if( this->open(name) < 0 ){
		return;
	}

	if( read(&hdr, sizeof(hdr)) != sizeof(hdr) ){
		close();
		return;
	}

	if( read(&dib, sizeof(dib)) != sizeof(dib) ){
		close();
		return;
	}

	if( seek(hdr.offset) != (int)hdr.offset ){
		dib.width = -1;
		dib.height = -1;
		dib.bits_per_pixel = 0;
		close();
		return;
	}

	offset_ = hdr.offset;
}

unsigned int Bmp::row_size() const{
	return (((dib.bits_per_pixel/8)*dib.width + 3) / 4) * 4;
}

int Bmp::seek_row(int32_t y){
	if( dib.height > 0 ){
		//image is upside down -- seek to beginning of row
		return seek(offset_ + row_size() * (dib.height - (y + 1)));
	}

	return seek(offset_ + row_size() * y);
}


