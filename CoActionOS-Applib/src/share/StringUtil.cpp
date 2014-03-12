/*
 * StringUtil.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: tgil
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "StringUtil.hpp"

static const int bases[StringUtil::MODE_TOTAL] = {
		2, 8, 16, 16, 10
};


char StringUtil::Htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'A' - 10;
	}
}


char StringUtil::htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'a' - 10;
	}
}

int StringUtil::itoa(char dest[BUF_SIZE], int32_t num, int width){
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
int StringUtil::ftoa(char dest[BUF_SIZE], float num, int width){
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


int StringUtil::utoa(char dest[BUF_SIZE], uint32_t num, int base, bool upper, int width){
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

int StringUtil::utoa_mode(enum StringUtil::mode mode, char dest[BUF_SIZE], uint32_t num, int width){
	int len = 0;
	bool caps;
	if( mode == HEXADECIMAL ){
		caps = true;
	} else {
		caps = false;
	}

	len = utoa(dest, num, bases[mode], caps, width);

	if( len > BUF_SIZE ){
		len = BUF_SIZE;
	}

	return len;
}

