/*
 * Periph.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */

#include "Periph.hpp"
#include "Timer.hpp"

Periph::Periph(port_t port, int numports, int * fd, bool * isinitialized){
	// TODO Auto-generated constructor stub
	if( port < (port_t)numports ){
		this->port_ = port;
	} else {
		this->port_ = 0;
	}
	flags = 0;
}


int Periph::read(void * buf, int nbyte){
	return -1;
}

int Periph::write(const void * buf, int nbyte){
	return -1;
}

int Periph::read(Aio & aio){
	return -1;
}

int Periph::write(Aio & aio){
	return -1;
}

int Periph::setflags(int flags){
	this->flags = flags;
	return 0;
}


int Periph::readline(char * buf, int nbyte, int timeout, char term){
	int t;
	int bytes_recv;
	char c;
	t = 0;
	bytes_recv = 0;
	do {
		if( read(&c, 1) == 1 ){
			*buf = c;
			buf++;
			bytes_recv++;
			if( c == term ){
				return bytes_recv;
			}
		} else {
			t++;
			Timer::wait_msec(1);
		}
	} while( (bytes_recv < nbyte) && (t < timeout) );

	return bytes_recv;
}
