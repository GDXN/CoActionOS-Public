/*
 * Token.cpp
 *
 *  Created on: Nov 21, 2013
 *      Author: tgil
 */

#include <cstdio>
#include "Token.hpp"


void Token::construct(const char * delim){
	char * p;
	unsigned int i;
	int j;
	int len;
	bool on_token = false;
	num_tokens = 0;
	len = strlen(delim);
	p = data();
	string_size = String::size();
	for(i=0; i < string_size; i++){
		for(j=0; j < len; j++){
			if( *p == delim[j] ){
				*p = 0;
				break;
			}
		}
		if( *p != 0 ){
			if( on_token == false ){
				num_tokens++;
				on_token = true;
			}
		} else {
			on_token = false;
		}
		p++;
	}
}

const char * Token::at(unsigned int n) const {
	const char * p;
	unsigned int i;
	bool on_token = false;
	unsigned int token = 0;
	p = c_str();

	if( num_tokens <= n ){
		return 0;
	}

	i = 0;
	while( (i < string_size) && (*p == 0) ){
		i++;
		p++;
	}

	on_token = true;
	while( token != n ){
		p++;
		if( *p != 0 ){
			if( on_token == false ){
				token++;
				on_token = true;
			}
		} else {
			on_token = false;
		}
	}

	return p;
}





