/*
 * MG_types.h
 *
 *  Created on: Jun 13, 2013
 *      Author: tgil
 */

#ifndef MG_TYPES_H_
#define MG_TYPES_H_

#include <hwpl.h>

#define MG_MAX (32767)
#define MG_MIN (-32768)

enum {
	MG_LINE,
	MG_ARC,
	MG_RECT,
	MG_TRIANGLE,
	MG_TYPE_TOTAL
};

#define MG_TYPE_MASK (0xFF)
#define MG_ENABLE_FLAG (1<<15)
#define MG_FILL_FLAG (1<<14)

typedef struct HWPL_PACK {
	int16_t start;
	int16_t end;
} mg_arc2_t;

typedef struct HWPL_PACK {
	int16_t todo;
} mg_triangle_t;

typedef struct HWPL_PACK {
	int16_t x, y;
} mg_point_t;

typedef struct HWPL_PACK {
	uint16_t w, h;
} mg_dim_t;

typedef struct HWPL_PACK {
	uint16_t type; //type of primitive object
	int16_t rotation; //vector space rotation
	mg_point_t shift;
	mg_dim_t dim; //vector space size
	union {
		mg_arc2_t arc;
		mg_triangle_t triangle;
	} primitive;
} mg_t;

typedef struct HWPL_PACK {
	//list of points

} mg_poly_t;

typedef struct HWPL_PACK {
	//list of points
	mg_point_t center;
	uint16_t inner, outer; //inner/outer radii
	int16_t start, stop; //start/stop angles
} mg_arc_t;

typedef struct HWPL_PACK {
	mg_point_t shift; //shift within screen (absolute)
	int16_t rotation; //rotate within screen (absolute)
	mg_dim_t size;
} mg_map_t;

#endif /* MG_TYPES_H_ */
