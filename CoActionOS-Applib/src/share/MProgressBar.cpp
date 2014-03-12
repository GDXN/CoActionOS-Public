/*
 * MProgressBar.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: tgil
 */

#include "MProgressBar.hpp"

MProgressBar::MProgressBar(uint8_t * mem, int width, int height, int max) : MBitmap(mem, width, height), max(max), current(0) {
	// TODO Auto-generated constructor stub
	drawborder();
}

void MProgressBar::drawborder(void){
	unsigned int i;
	//draw the border
	for(i=1; i < width()-1; i++){
		setpixel(i, 0);
		setpixel(i, height()-1);
	}

	for(i=1; i < height()-1; i++){
		setpixel(0, i);
		setpixel(width()-1, i);
	}
}

void MProgressBar::set(int value){
	unsigned int w;
	unsigned int i, j;
	clear();
	drawborder();
	w = value * width() / max;
	for(i=1; i < w; i++){
		for(j=1; j < height()-1; j++){
			setpixel(i,j);
		}
	}
}
