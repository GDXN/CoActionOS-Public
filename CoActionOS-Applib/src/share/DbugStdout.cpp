/*
 * DbugStdout.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: tgil
 */

#include <cstdio>
#include <unistd.h>
#include "DbugStdout.hpp"

DbugStdout::DbugStdout() {
	if( fileno(stdout) == 0 ){
		printf("\r\n");
	}
}

int DbugStdout::open(void){
	//no need to open
	return 0;
}

int DbugStdout::close(void){
	//no need to close
	return 0;
}

int DbugStdout::read(void * buf, int nbyte){
	return -1;
}

int DbugStdout::write(const void * buf, int nbyte){
	return ::write(fileno(stdout), buf, nbyte);
}

