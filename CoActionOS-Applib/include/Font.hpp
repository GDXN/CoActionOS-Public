/*
 * Font.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#ifndef FONT_HPP_
#define FONT_HPP_

#include <stdint.h>

/*! \brief Font class */
class Font {
public:
	Font();

	static const char * charset();

	enum {
		CHARSET_SIZE = 95
	};

	/*! \brief Returns the length of \a str in pixels */
	virtual int len(const char * str){ return 0; }

protected:
	static int to_charset(char ascii);

};

#endif /* FONT_HPP_ */
