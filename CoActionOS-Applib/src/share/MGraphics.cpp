/*
 * MGraphics.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: tgil
 */

#include "MGraphics.hpp"
#include "MPoint.hpp"


static void draw_line(const mg_t * p, MLcd * lcd);
static void draw_arc(const mg_t * p, MLcd * lcd);
static void draw_rect(const mg_t * p, MLcd * lcd);
static void draw_triangle(const mg_t * p, MLcd * lcd);

static void (*draw_func [MG_TYPE_TOTAL])(const mg_t * p, MLcd * lcd) = {
		draw_line,
		draw_arc,
		draw_rect,
		draw_triangle
};

void MGraphics::draw(const mg_t * prim) const{
	int type;
	type = prim->type & MG_TYPE_MASK;
	if( type & MG_ENABLE_FLAG ){
		if( (prim->type & MG_TYPE_MASK) < MG_TYPE_TOTAL ){
			draw_func[type](prim, lcd);
		}
	}
}

void MGraphics::draw(const mg_t * prim_list[], unsigned int total) const {
	unsigned int i;
	for(i=0; i < total; i++){
		draw(prim_list[i]);
	}
}

void MGraphics::draw(const mg_t * prim_list[]) const {
	unsigned int i;
	for(i=0; prim_list[i] != 0; i++){
		draw(prim_list[i]);
	}
}

void draw_line(const mg_t * p, MLcd * lcd){
	mg_map_t * map = 0;
	//draw a line from bottom left to top right
	MPoint p1(*p, -1); //initialize, rotate and shift
	MPoint p2(*p);

	//apply bitmap space rotation
	p1.map(*map); //rotate, scale, and shift
	p2.map(*map);
	//now draw a line between the two points
	lcd->setline(p1.x(), p1.y(), p2.x(), p2.y());
}

void draw_arc(const mg_t * p, MLcd * lcd){

}

void draw_rect(const mg_t * p, MLcd * lcd){

}

void draw_triangle(const mg_t * p, MLcd * lcd){

}

mg_t MGraphics::line(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
	mg_t ret;
	memset(&ret, 0, sizeof(mg_t));
	ret.type = MG_LINE | MG_ENABLE_FLAG;
	if( x2 > x1 ){
		ret.dim.w = x2 - x1;
		ret.rotation = 0;
	} else {
		ret.dim.w = x1 - x2;
		ret.rotation = 90;
	}

	if( y2 > y1 ){
		ret.dim.h = y2 - y1;
	} else {
		ret.dim.h = y1 - y2;
		ret.rotation -= 90;
	}

	ret.shift.x = (x1 + x2) / 2;
	ret.shift.y = (y1 + y2) / 2;
	return ret;
}

mg_t MGraphics::rect(int16_t x1, int16_t y1, uint16_t w, uint16_t h, bool fill, int16_t rotation){
	mg_t ret;
	memset(&ret, 0, sizeof(mg_t));
	ret.type = MG_RECT | MG_ENABLE_FLAG;
	if( fill ){
		ret.type |= MG_FILL_FLAG;
	}
	ret.shift.x = x1;
	ret.shift.y = y1;
	ret.dim.w = w;
	ret.dim.h = h;
	ret.rotation = rotation;
	return ret;
}



