/*
 * MLcd.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "Timer.hpp"
#include "MLcd.hpp"

const uint8_t MLcd::mask[8] = {
		0x80,
		0x40,
		0x20,
		0x10,
		0x08,
		0x04,
		0x02,
		0x01
};

const int8_t MLcd::rot_sin[5] = {
		0,
		1,
		0,
		-1,
		0
};

const int8_t MLcd::rot_cos[5] = {
		1,
		0,
		-1,
		0,
		1
};

static int abs_value(int x){
	if( x < 0 ){
		return x*-1;
	}
	return x;
}


void MLcd::setstr(const char * str, int x, int y){
	int width;
	while( *str ){
		width = setchar(*str++, x, y);
		x += width;
	}
}

void MLcd::clrstr(const char * str, int x, int y){
	int width;
	while( *str ){
		width = clrchar(*str++, x, y);
		x += width;
	}
}

void MLcd::fade(int start, int stop, int max, int steps, int delay){
	int i;
	int delta_b = (stop - start) / steps;
	int b = start + delta_b;
	for(i=0; i < steps; i++){
		printf("set to %d of %d\n", b, max);
		setbrightness(b, max);
		b += delta_b;
		Timer::wait_usec(delay);
	}
}

int MLcd::setchar(const char c, int x, int y){
	if( f == 0 ){
		return 0;
	}
	return setbitmap(f->bitmap(c), x, y);
}

int MLcd::clrchar(const char c, int x, int y){
	if( f == 0 ){
		return 0;
	}
	return clrbitmap(f->bitmap(c), x, y);
}


int MLcd::setbitmap(const MBitmap & bitmap, int x, int y){
	int i, j;
	if( bitmap.isempty() == true ){
		return 0;
	}
	for(i=0; i < (int)bitmap.height(); i++){
		for(j=0; j < (int)bitmap.width(); j++){
			if( bitmap.tstpixel(j, i) ){
				setpixel(x + j, y + bitmap.height() - i);
			}
		}
	}
	return bitmap.width() + 1;
}

int MLcd::clrbitmap(const MBitmap & bitmap, int x, int y){
	int i, j;
	if( bitmap.isempty() == true ){
		return 0;
	}
	for(i=0; i < (int)bitmap.height(); i++){
		for(j=0; j < (int)bitmap.width(); j++){
			if( bitmap.tstpixel(j, i) ){
				clrpixel(x + j, y + bitmap.height() - i);
			}
		}
	}
	return bitmap.width() + 1;
}

void MLcd::setbitmap(const MBitmap * bitmap, int x, int y, enum MLcd::Rotation rotation){
	int i, j;
	int x1, y1;
	int tx, ty;
	int rc, rs;
	const int h2 = bitmap->height()>>1;
	const int w2 = bitmap->width()>>1;

	rc = rot_cos[rotation];
	rs = rot_sin[rotation];

	for(i = 0; i < (int)bitmap->width(); i++){
		tx = i - w2;
		for(j = 0; j < (int)bitmap->height(); j++){
			ty = j + h2;
			if( bitmap->tstpixel(i, j) ){
				x1 = tx * rc - ty * rs;
				y1 = tx * rs + ty * rc;
				setpixel(x + x1 + w2, y + h2+1 - y1);
			}
		}
	}
}

bool MLcd::yvisible(int y){
	if( y < 0 ) return false;
	if( y >= height_var ) return false;
	return true;
}

bool MLcd::xvisible(int x){
	if( x < 0 ) return false;
	if( x >= width_var ) return false;
	return true;
}

void MLcd::boundx(int & x){
	x = (x < 0) ? 0 : x;
	x = (x >= width_var) ? width_var - 1 : x;
}

void MLcd::boundy(int & y){
	y = (y < 0) ? 0 : y;
	y = (y >= height_var) ? height_var - 1 : y;
}

void MLcd::sethline(int xmin, int xmax, int y){
	int i;
	if( !yvisible(y) ){
		return;
	}
	boundx(xmin);
	boundx(xmax);
	for(i=xmin; i <= xmax; i++){
		setpixel_bounded(i, y);
	}
}

void MLcd::setvline(int x, int ymin, int ymax){
	int i;
	for(i=ymin; i <= ymax; i++){
		setpixel(x, i);
	}
}

void MLcd::setline(int x1, int y1, int x2, int y2){
	int dx, dy;
	int adx, ady;
	int x,y;
	int rise, run;

	if( y2 == y1 ){
		sethline(x1 < x2 ? x1 : x2, y1, abs_value(x2-x1));
		return;
	}

	if( x2 == x1 ){
		setvline(x2, y1 > y2 ? y1 : y2, abs_value(y2-y1));
		return;
	}

	if( y2 > y1 ){
		dy = 1;
	} else {
		dy = -1;
	}

	if( x2 > x1 ){
		dx = 1;
	} else {
		dx = -1;
	}

	adx = abs_value(x2 - x1);
	ady = abs_value(y2 - y1);
	rise = (y2 - y1);
	run = (x2 - x1);

	x = x1;
	y = y1;
	if( adx > ady ){

		while(x != x2 ){
			setpixel(x,y);
			x += dx;
			y = ((x - x1) * rise + dy*run/2) / run + y1;
		}
	} else {
		while(y != y2 ){
			setpixel(x,y);
			y += dy;
			x = ((y - y1) * run + dx*rise/2) / rise + x1;
		}
	}

	setpixel(x2,y2);
}

bool MLcd::tsthline(int xmin, int xmax, int y, int & pos){
	int i;
	if( !yvisible(y) ){
		return true;
	}
	boundx(xmin);
	boundx(xmax);
	for(i = xmin; i < xmax; i++){
		if( tstpixel_bounded(i,y) == false ){
			pos = i;
			return false;
		}
	}
	return true;
}


void MLcd::tsthedge(int x, int y, int & xmin, int & xmax){
	int min;
	int max;
	min = x;
	max = x;
	while( tstpixel(min,y) == false ){ min--; }
	while( tstpixel(max,y) == false ){ max++; }
	xmin = min+1;
	xmax = max;
	return;
}

void MLcd::setfill(unsigned int x, unsigned int y){
	int xmin, xmax;
	bool above, below;
	int xabove, xbelow;
	above = false;
	below = false;
	if( !xvisible(y) ){
		return;
	}
	tsthedge(x,y,xmin,xmax); //find the bounding points xmin and xmax
	above = !tsthline(xmin, xmax, y+1, xabove); //see if anywhere above the bounded region is blank
	below = !tsthline(xmin, xmax, y-1, xbelow); //see if anywhere below the bounded region is blank
	sethline(xmin, xmax, y);
	if( above == true ){
		setfill(xabove,y+1); //if the above line has a blank spot -- fill it
	}
	if( below == true ){
		setfill(xbelow,y-1); //if the below line has a blank spot -- fill it
	}
}


