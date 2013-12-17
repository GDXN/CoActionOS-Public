/*
 * Point.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: tgil
 */

#include "MPoint.hpp"

static const int8_t rot_sin[5] = {
		0,
		1,
		0,
		-1,
		0
};

static const int8_t rot_cos[5] = {
		1,
		0,
		-1,
		0,
		1
};

MPoint::MPoint(const mg_t & a, int16_t sign){
	d.x = sign * (a.dim.w >> 1);
	d.y = sign * (a.dim.h >> 1);
	rotate(a.rotation);
	shift(a.shift);
}

void MPoint::map(const mg_map_t & m){
	rotate(m.rotation);
	//map to the space
	int32_t tmp;
	tmp = ((d.x + MG_MAX) * m.size.w + MG_MAX) / (MG_MAX-MG_MIN);
	d.x = tmp - (m.size.w>>1) + m.shift.x;
	tmp = ((d.y + MG_MAX) * m.size.h + MG_MAX) / (MG_MAX-MG_MIN);
	d.y = tmp - (m.size.h>>1) + m.shift.y;
}


MPoint & MPoint::operator+(const mg_point_t & a){
	this->d.x += a.x;
	this->d.y += a.y;
	return *this;
}

MPoint & MPoint::operator-(const mg_point_t & a){
	this->d.x -= a.x;
	this->d.y -= a.y;
	return *this;
}


void MPoint::rotate(int16_t angle){
	mg_point_t end;
	int rc, rs;
	rc = rot_cos[angle];
	rs = rot_sin[angle];
	end.x = d.x * rc - d.y * rs;
	end.y = d.x * rs + d.y * rc;
	d = end;
}

void MPoint::scale(uint16_t x, uint16_t y){

}

void MPoint::scale(uint16_t a){

}

void MPoint::shift(int16_t x, int16_t y){
	d.x += x;
	d.y += y;
}
