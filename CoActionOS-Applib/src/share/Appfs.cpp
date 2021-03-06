/*
 * Appfs.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: tgil
 */


#include <errno.h>
#include <link_protocol.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Appfs.hpp"
#include "File.hpp"


Appfs::Appfs() {
	// TODO Auto-generated constructor stub

}

int Appfs::create(const char * name, const void * buf, int nbyte, const char * mount, link_phy_t h){
	char buffer[LINK_PATH_MAX];
	File file;
	int tmp;
	const char * p = (const char*)buf;
	appfs_createattr_t attr;
	int loc;
	unsigned int bw; //bytes written
	link_appfs_file_t f;
	strcpy(buffer, mount);
	strcat(buffer, "/flash/");
	strcat(buffer, name);
#ifdef __link
	file.sethandle(h);
#endif

	//delete the settings if they exist
	tmp = errno;
	unlink(buffer);
	errno = tmp;

	strncpy(f.hdr.name, name, LINK_NAME_MAX);
	f.hdr.mode = 0666;
	f.exec.code_size = nbyte + sizeof(f); //total number of bytes in file
	f.exec.signature = APPFS_CREATE_SIGNATURE;

	if( file.open("/app/.install", File::WRONLY) < 0 ){
		return -1;
	}

	memcpy(attr.buffer, &f, sizeof(f));
	//now copy some bytes
	attr.nbyte = APPFS_PAGE_SIZE - sizeof(f);
	if( nbyte < (int)attr.nbyte ){
		attr.nbyte = nbyte;
	}
	memcpy(attr.buffer + sizeof(f), p, attr.nbyte);
	attr.nbyte += sizeof(f);
	loc = 0;
	bw = 0;
	do {
		if( loc != 0 ){ //when loc is 0 -- header is copied in
			if( (f.exec.code_size - bw) > APPFS_PAGE_SIZE ){
				attr.nbyte = APPFS_PAGE_SIZE;
			} else {
				attr.nbyte = f.exec.code_size - bw;
			}
			memcpy(attr.buffer, p, attr.nbyte);
		}
		attr.loc = loc;

		if( file.ioctl(I_APPFS_CREATE, &attr) < 0 ){
			file.close();
			printf("failed %d %d\n", attr.loc, attr.nbyte);
			fflush(stdout);
			return -1;
		}

		if( loc != 0 ){
			p += attr.nbyte;
		} else {
			p += (attr.nbyte - sizeof(f));
		}
		bw += attr.nbyte;
		loc += attr.nbyte;

	} while( bw < f.exec.code_size);

	file.close();
	return nbyte;
}
