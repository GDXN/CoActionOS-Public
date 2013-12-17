/*
 * File.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: tgil
 */

#include <stdio.h>

#include "File.hpp"

File::File() {
	// TODO Auto-generated constructor stub
	fd = -1; //The file is not open
	handle = (link_phy_t)-1;
}

int File::remove(const char * name){
	return ::remove(name);
}


int File::open(const char * name, int access, int perms){
	if( fd != -1 ){
		return -1;
	}

	fd = link_open(handle, name, access, perms);
	if( fd < 0 ){
		return -1;
	}

	return 0;
}

int File::ioctl(int req, void * arg){
	return link_ioctl(handle, fd, req, arg);
}

int File::create(const char * name, bool overwrite, int perms){
	int access = O_RDWR | O_CREAT;
	if( overwrite ){
		access |= O_TRUNC;
	}
	return open(name, access, perms);
}

int File::read(void * buf, int nbyte){
	return link_read(handle, fd, buf, nbyte);
}

int File::write(const void * buf, int nbyte){
	return link_write(handle, fd, buf, nbyte);
}

int File::seek(int loc, int whence){
	return link_lseek(handle, fd, loc, whence);
}

int File::fileno(void){
	return fd;
}

int File::size(void){
	struct link_stat st;
	if ( link_fstat(handle, fd, &st) < 0 ){
		return -1;
	}
	return st.st_size;
}

int File::close(void){
	int ret;
	ret = ::close(fd);
	fd = -1;
	return ret;
}
