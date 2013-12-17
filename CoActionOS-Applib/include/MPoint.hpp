/*
 * MPoint.hpp
 *
 *  Created on: Jun 13, 2013
 *      Author: tgil
 */

#ifndef MPOINT_HPP_
#define MPOINT_HPP_

#include "mg_types.h"

/*
 *
 */
class MPoint {
public:
	MPoint(){ d.x = 0; d.y = 0; }
	MPoint(mg_point_t p){ d = p; }
	MPoint(mg_dim_t p, int16_t sign = 1){ d.x = sign*(p.w>>2); d.y = sign*(p.h>>1); }
	MPoint(const mg_t & a, int16_t sign = 1);

	operator mg_point_t(){ return d; }
	MPoint & operator=(const mg_point_t & a){ d = a; return *this; }
	MPoint & operator+(const mg_point_t & a);
	MPoint & operator-(const mg_point_t & a);

	inline int16_t x(){ return d.x; }
	inline int16_t y(){ return d.y; }

	void map(const mg_map_t & m);

	void rotate(int16_t angle);
	void scale(uint16_t x, uint16_t y);
	void scale(uint16_t a);
	void shift(int16_t x, int16_t y);
	void shift(const mg_point_t & p);

private:
	mg_point_t d;
};


#endif /* MPOINT_HPP_ */
