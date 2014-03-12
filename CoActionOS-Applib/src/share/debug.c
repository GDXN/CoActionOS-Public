/*
 * debug.c
 *
 *  Created on: Nov 20, 2012
 *      Author: tgil
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "debug.h"


#define BUF_SIZE 36

static int width;
static int (*writefunc)(const void *, int);

static char Htoc(int nibble);
static char htoc(int nibble);
static int itoa(char dest[BUF_SIZE], int32_t num, int width);
#ifdef USE_FLOAT
static int ftoa(char dest[BUF_SIZE], float num, int width);
#endif
static int utoa(char dest[BUF_SIZE], uint32_t num, int base, bool upper, int width);
static void dwrite(char * str);

void dsetmode(int leading_zeros){
	width = leading_zeros;
}

void dsetwritefunc(int (*func)(const void *, int)){
	writefunc = func;
}

int dint(int x){
	char buf[BUF_SIZE];
	itoa(buf, x, width);
	dwrite(buf);
	return 0;
}

int duint(unsigned int x){
	char buf[BUF_SIZE];
	utoa(buf, x, 10, true, width);
	dwrite(buf);
	return 0;
}

int dstr(char * str){
	dwrite(str);
	return 0;
}

int dhex(int x){
	char buf[BUF_SIZE];
	utoa(buf, x, 16, false, width);
	dwrite(buf);
	return 0;
}

int dHex(int x){
	char buf[BUF_SIZE];
	utoa(buf, x, 16, true, width);
	dwrite(buf);
	return 0;
}

int dbin(int x){
	char buf[BUF_SIZE];
	utoa(buf, x, 2, false, width);
	dwrite(buf);
	return 0;
}

void dwrite(char * str){
	if ( writefunc != NULL ){
		writefunc(str, strlen(str));
	}
}


char Htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'A' - 10;
	}
}

char htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'a' - 10;
	}
}

int itoa(char dest[BUF_SIZE], int32_t num, int width){
	char buf[BUF_SIZE];
	int i, j;
	bool started;
	bool neg;
	j = 0;
	neg = false;
	if( num < 0 ){
		neg = true;
		num *= -1;
	}
	memset(buf, '0', BUF_SIZE);
	do {
		buf[j++] = htoc(num%10);
		num /= 10;
	} while( (num >  0) && (j < BUF_SIZE));
	started = false;
	j = 0;
	if( neg == true ){
		dest[j++] = '-';
	}
	for(i=BUF_SIZE-1; i >= 0; i--){
		if( (buf[i] != '0') || (started == true) || (i<width)){
			started = true;
			dest[j++] = buf[i];
		}
	}

	if( (j == 0) || ((j == 1) && neg) ){
		dest[j++] = '0';
	}

	dest[j] = 0;
	return j;
}

#ifdef USE_FLOAT

void dfloat(float x){

}

int ftoa(char dest[BUF_SIZE], float num, int width){
	char buf[24];
	int i, j;
	int dec;
	bool neg;
	bool started;
	char c;
	const int base = 10;
	int intnum;
	j = 0;
	dec = 0;

	if( num < 0.0 ){
		neg = true;
		num *= -1;
	}

	while( num != (int)num ){
		num *= base;
		dec++;
	}

	memset(buf, '0', 24);
	intnum = (int)num;
	while( intnum > 0){
		c = htoc(intnum%base);
		if( (j == dec) && (j > 0) ){
			buf[j++] = '.';
		}
		buf[j++] = c;
		intnum /= base;
	};
	if( j == dec ){
		buf[j++] = '.';
	}
	started = false;
	j = 0;
	if( neg == true ){
		dest[j++] = '-';
	}
	for(i=23; i >= 0; i--){
		if( (buf[i] != '0') || (started == true) || (i<width) || (buf[i] == '.') ){
			if( (started == false) && (buf[i] == '.') ){
				dest[j++] = '0';
			}
			started = true;
			dest[j++] = buf[i];
		}
	}

	if( (j == 0) || ((j == 1) && neg) ){
		dest[j++] = '0';
	}

	if( dest[j-1] == '.' ){
		dest[j++] = '0';
	}

	dest[j] = 0;
	return j;
}
#endif


int utoa(char dest[BUF_SIZE], uint32_t num, int base, bool upper, int width){
	char buf[BUF_SIZE];
	int i, j;
	bool started;
	char c;
	j = 0;
	memset(buf, '0', BUF_SIZE);
	while( (num > 0) && (j < BUF_SIZE)){
		if( upper ){
			c = Htoc(num%base);
		} else {
			c = htoc(num%base);
		}
		buf[j++] = c;
		num /= base;
	};
	started = false;
	j = 0;
	for(i=BUF_SIZE-1; i >= 0; i--){
		if( (buf[i] != '0') || (started == true) || (i<width)){
			started = true;
			dest[j++] = buf[i];
		}
	}
	if( j == 0 ){
		dest[j++] = '0';
	}
	dest[j] = 0;
	return j;
}
