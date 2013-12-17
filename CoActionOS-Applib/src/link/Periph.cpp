/*
 * Periph.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */
#include <unistd.h>
#include <fcntl.h>
#include <hwpl.h>
#include <link_protocol.h>

#include "Periph.hpp"
#include "Timer.hpp"

#include <stdio.h>

#define NO_PORT_NUMBER 0xAAEE5566

Periph::Periph(port_t port, int numports, int * fd, bool * isinitialized) {
	int i;
	this->port_ = port;
	this->fd = fd;

	if ( port < (port_t)numports ){
		this->port_ = port;
	} else {
		this->port_ = 0;
	}

	if ( !(*isinitialized) ){

		for(i=0; i < numports; i++){
			fd[i] = -1;
		}

		*isinitialized = true;
	}
}


int Periph::_open(const char * name, int flags){
	char buffer[12];
	int len;

	strcpy(buffer, "/dev/");
	strcat(buffer, name);
	len = strlen(buffer);
	if( port_ != NO_PORT_NUMBER ){
		buffer[len] = '0' + port_;
		buffer[len+1] = '\0';
	} else {
		port_ = 0;
	}
	if ( fd[port_] != -1 ){
		//the port is already open
		printf("port is already open\n");
		return 0;
	}
	fd[port_] = link_open(handle, buffer, LINK_O_RDWR | flags);
	if( fd[port_] < 0 ){
		printf("Port failed to open %d\n", link_errno);
		return -1;
	}
	return 0;
}

int Periph::close(){
	int ret = 0;
	if ( fd[port_] != -1 ){  //see if the port is already closed
		ret = link_close(this->handle, fd[port_]);
		fd[port_] = -1;
	}
	return ret;
}

int Periph::read(void * buf, int nbyte){
	return link_read(handle, fd[port_], buf, nbyte);
}

int Periph::write(const void * buf, int nbyte){
	return link_write(handle, fd[port_], buf, nbyte);
}

int Periph::ioctl(int req, void * arg){
	return link_ioctl(handle, fd[port_], req, arg);
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
