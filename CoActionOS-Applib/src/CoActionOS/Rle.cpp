/*
 * Rle.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: tgil
 */

#include <cstdio>
#include "Rle.hpp"

Rle::Rle(){}


int Rle::size(const void * src, int nbyte){
	int bp; //bytes processed
	uint8_t size;
	int next_dest_size;
	const uint8_t * srcp = (const uint8_t*)src;
	next_dest_size = 0;
	bp = 0;
	do {
		size = 1;
		bp++;
		srcp++;
		while( (size != 255) &&
				(bp < nbyte) &&
				(*(srcp-1) == *(srcp)) ){
			size++;
			bp++;
			srcp++;
		}
		next_dest_size += sizeof(element_t);

	} while(bp < nbyte);
	return next_dest_size;
}

int Rle::encode(void * dest, int & dest_size, const void * src, int src_size){
	int bp; //bytes processed
	uint8_t value;
	uint8_t size;
	int next_dest_size;
	const uint8_t * srcp = (const uint8_t*)src;
	element_t * elements = (element_t*)dest;

	next_dest_size = 0;
	bp = 0;
	do {
		if( next_dest_size + (int)sizeof(element_t) > dest_size ){
			dest_size = next_dest_size;
			return bp;
		}

		size = 1;
		bp++;
		value = *srcp++;
		while( (size != 255) &&
				(bp < src_size) &&
				(*(srcp-1) == *(srcp)) ){
			size++;
			bp++;
			srcp++;
		}

		elements->data = value;
		elements->size = size;
		elements++;
		next_dest_size += sizeof(element_t);

	} while(bp < src_size);
	dest_size = next_dest_size;
	return bp;
}

int Rle::decode(void * dest, int & dest_size, const void * src, int src_size){
	int i;
	element_t * elements = (element_t*)src;
	uint8_t * destp = (uint8_t*)dest;
	int next_dest_size = 0;
	for(i=0; i < src_size; i+=sizeof(element_t)){
		if( (next_dest_size + elements->size) <= dest_size ){
			memset(destp, elements->data, elements->size);
			next_dest_size += elements->size;
			destp += elements->size;
			elements++;
		} else {
			break;
		}
	}

	dest_size = next_dest_size;
	return i;
}


int RleFile::write(const void * buf, int nbyte){
	int ret;
	int bw;
	int dest_size;
	const char * p = (const char *)buf;
	bw = 0;
	do {
		dest_size = 64;
		ret = encode(this->buf, dest_size, p, nbyte);
		if( ret > 0 ){
			p += ret;
			nbyte -= ret;
			bw += ret;
			printf("write %d bytes to file\n", dest_size);
			File::write(this->buf, dest_size);
			printf("done\n");
		}
	} while((ret > 0) && (nbyte > 0));

	return bw;
}
