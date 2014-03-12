
#ifndef MGRAPHICS_HPP_
#define MGRAPHICS_HPP_

#include <hwpl.h>
#include "MLcd.hpp"

#include "mg_types.h"


class MGraphics {
public:
	MGraphics(MLcd * lcd){ this->lcd = lcd; }

	void draw(const mg_t * prim) const;
	void draw(const mg_t * prim_list[], unsigned int total) const;
	void draw(const mg_t * prim_list[]) const; //zero terminated list of graphics

	static mg_t line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	static mg_t rect(int16_t x1, int16_t y1, uint16_t w, uint16_t h, bool fill = true, int16_t rotation = 0);

private:
	MLcd * lcd;
};

#endif /* MGRAPHICS_HPP_ */
