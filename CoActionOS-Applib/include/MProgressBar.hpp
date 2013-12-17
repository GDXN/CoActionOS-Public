/*
 * MProgressBar.hpp
 *
 *  Created on: Jun 3, 2013
 *      Author: tgil
 */

#ifndef MPROGRESSBAR_HPP_
#define MPROGRESSBAR_HPP_

#include "MBitmap.hpp"

class MProgressBar : public MBitmap {
public:
	MProgressBar(uint8_t * mem, int width, int height, int max);

	inline void setmax(int max){ this->max = max; }
	void set(int value);


private:
	int max;
	int current;

	void drawborder(void);
};

#endif /* MPROGRESSBAR_HPP_ */
