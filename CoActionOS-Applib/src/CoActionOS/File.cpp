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
}

int File::remove(const char * name){
	return ::remove(name);
}


int File::open(const char * name, int access, int perms){
	if( fd != -1 ){
		return -1;
	}

	fd = ::open(name, access, perms);
	if( fd < 0 ){
		return -1;
	}

	return 0;
}

int File::ioctl(int req, void * arg){
	return ::ioctl(fd, req, arg);
}

int File::create(const char * name, bool overwrite, int perms){
	int access = O_RDWR | O_CREAT;
	if( overwrite ){
		access |= O_TRUNC;
	}
	return open(name, access, perms);
}

int File::read(void * buf, int nbyte){
	return ::read(fd, buf, nbyte);
}

int File::write(const void * buf, int nbyte){
	return ::write(fd, buf, nbyte);
}

int File::read(int loc, void * buf, int nbyte){
	lseek(fd, loc, SEEK_SET);
	return ::read(fd, buf, nbyte);
}

int File::write(int loc, const void * buf, int nbyte){
	lseek(fd, loc, SEEK_SET);
	return ::write(fd, buf, nbyte);
}

int File::seek(int loc, int whence){
	return lseek(fd, loc, whence);
}

int File::fileno(void){
	return fd;
}

int File::size(void){
	struct stat st;
	if ( fstat(fd, &st) < 0 ){
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

char * File::gets(char * s, int n, char term){
	int t;
	int ret;
	int i;
	char buffer[GETS_BUFFER_SIZE];

	if( n < 1 ){
		return 0;
	}

	s[0] = 0;
	t = 0;
	do {
		ret = read(buffer, GETS_BUFFER_SIZE);
		for(i=0; i < ret; i++){
			s[t] = buffer[i];
			s[t+1] = 0;
			if( s[t] == '\n'){
				break;
			}
			if( t == (n-1) ){
				break;
			}
			t++;
		}
	} while( (t < (n-1)) && (s[t] != term) && (ret > 0) );

	if( t == 0 ){
		return 0;
	}

	if( ret > 0 ){
		seek(i - ret + 1, CURRENT);
	}

	return s;

}
