/*
 * MFont.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#ifndef MFONT_HPP_
#define MFONT_HPP_

#include "Font.hpp"

/*! \brief Monochrome Font */
class MFont : public Font {
public:
	MFont();

	virtual int len(const char * str){ return 0; }


};

#endif /* MFONT_HPP_ */
