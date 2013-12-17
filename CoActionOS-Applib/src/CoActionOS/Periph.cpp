/*
 * Periph.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: tgil
 */
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "util.h"
#include "Periph.hpp"
#include "Timer.hpp"

Periph::Periph(port_t port, int numports, int * fd, bool * isinitialized) {
	int i;
	this->port_ = port;
	this->fd = fd;

	if ( port < (uint32_t)numports ){
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

int Periph::open(const char * name, int flags){
	if ( fd[port_] != -1 ){
		//the port is already open
		return 0;
	}
	fd[port_] = ::open(name, O_RDWR | flags);
	if( fd[port_] < 0 ){
		return -1;
	}
	return 0;


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
		return 0;
	}
	fd[port_] = ::open(buffer, O_RDWR | flags);
	if( fd[port_] < 0 ){
		return -1;
	}
	return 0;
}

int Periph::close(){
	int ret = 0;
	if ( fd[port_] != -1 ){  //see if the port is already closed
		ret = ::close(fd[port_]);
		fd[port_] = -1;
	}
	return ret;
}

int Periph::read(void * buf, int nbyte){
	return ::read(fd[port_], buf, nbyte);
}

int Periph::write(const void * buf, int nbyte){
	return ::write(fd[port_], buf, nbyte);
}

int Periph::read(Aio & aio){
	aio.aio_var.aio_fildes = (fd[port_]);
	return ::aio_read(&(aio.aio_var));
}

int Periph::write(Aio & aio){
	aio.aio_var.aio_fildes = (fd[port_]);
	return ::aio_write(&(aio.aio_var));
}

int Periph::ioctl(int req, void * arg){
	return ::ioctl(fd[port_], req, arg);
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
