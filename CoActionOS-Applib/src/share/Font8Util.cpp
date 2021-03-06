/*
 * FontUtil.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#include <unistd.h>
#include <fcntl.h>
#include "Font8Util.hpp"

Font8Util::Font8Util() {
	// TODO Auto-generated constructor stub

}


int Font8Util::save(const char * path, const font8_t * f){
	int fd;
	fd = ::open(path, O_RDWR | O_TRUNC | O_CREAT, 0666);
	if ( fd < 0 ){
		return -1;
	}

	if( write(fd, f, sizeof(font8_t)) < 0){
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
