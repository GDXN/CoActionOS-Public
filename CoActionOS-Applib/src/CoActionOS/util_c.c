/*
 * periph_c.c
 *
 *  Created on: Oct 13, 2012
 *      Author: tgil
 */


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "util.h"

int phylib_util_open(const char * name, port_t port, int * fd, int total, bool * init){
	char buffer[12];
	int len;
	int i;

	if ( !(*init) ){
		for(i = 0; i < total; i++){
			fd[i] = -1;
		}
		*init = true;
	}

	strcpy(buffer, "/dev/");
	strcat(buffer, name);
	len = strlen(buffer);
	if( port != NO_PORT_NUMBER ){
		buffer[len] = '0' + port;
		buffer[len+1] = '\0';
	} else {
		port = 0;
	}
	if ( fd[port] != -1 ){
		//the port is already open
		return 0;
	}
	fd[port] = open(buffer, O_RDWR);
	if( fd[port] < 0 ){
		return -1;
	}
	return 0;
}


int phylib_util_close(port_t port, int * fd){
	int ret = 0;
	if ( fd[port] != -1 ){  //see if the port is already closed
		ret = close(fd[port]);
		fd[port] = -1;
	}
	return ret;
}