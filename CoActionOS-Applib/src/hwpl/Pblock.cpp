/*
 * Block.cpp
 *
 *  Created on: Oct 14, 2012
 *      Author: tgil
 */

#include <cstdlib>
#include "Pblock.hpp"

Pblock::Pblock(port_t port, int numports, int * fd, bool * isinitialized) : Periph(port, numports, fd, isinitialized) {
	this->loc = 0;
}

int Pblock::read(int loc, void * buf, int nbyte){
	this->loc = loc;
	return read(buf, nbyte);
}

int Pblock::write(int loc, const void * buf, int nbyte){
	this->loc = loc;
	return write(buf, nbyte);
}
