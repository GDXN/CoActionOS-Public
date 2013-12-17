/*
 * Font.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#include "Font.hpp"

static const char ascii_charset_[Font::CHARSET_SIZE+1] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

Font::Font() {
	// TODO Auto-generated constructor stub

}

const char * Font::charset(){
	return ascii_charset_;
}

int Font::to_charset(char ascii){
	if( (ascii < ' ') ||
			(ascii > '~') ){
		return -1;
	}
	return (int)(ascii - ' ');
}
