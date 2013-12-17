/*
 * StringBasic.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: tgil
 */

#include <cstdio>
#include <cstring>
#include "StringUtil.hpp"

#include "String.hpp"

String::String(){ m = StringUtil::DECIMAL; width = 0; }
String::String(const char * s){ m = StringUtil::DECIMAL; width = 0; }
String::String(const String & s){ m = StringUtil::DECIMAL; width = 0; }

void String::setformat(enum StringUtil::mode m, int width){
	this->m = m;
	this->width = width;
}

//const char * conversion
String::operator const char * (){ return c_str(); }

//Stream like
String& String::operator<<(const String & a){ append(a.c_str()); return *this; }
String& String::operator<<(char a){ append(a); return *this; }
String& String::operator<<(const char * a){ append(a); return *this; }

//assignment
String& String::operator=(const char * a){ assign(a); return *this; }
String& String::operator=(const String & a){ assign(a.c_str()); return *this; }

//Comparators
bool String::operator==(const String & cmp) const { return (strcmp(this->c_str(), cmp.c_str()) == 0); }
bool String::operator==(const char * cmp) const { return (strcmp(this->c_str(), cmp) == 0); }
bool String::operator!=(const String & cmp) const { return !(*this == cmp); }
bool String::operator!=(const char * cmp) const { return !(*this == cmp); }

String& String::operator<<(int a){
	char buffer[StringUtil::BUF_SIZE];
	StringUtil::itoa(buffer, a, width);
	return *this << buffer;
}

String& String::operator<<(unsigned int a){
	char buffer[StringUtil::BUF_SIZE];
	StringUtil::utoa_mode(m, buffer, a, width);
	return *this << buffer;
}

int String::atoi() const { return ::atoi(c_str()); }

char String::at(unsigned int pos) const {
	const char * s = c_str();
	if (pos < strlen(s)){
		return s[pos];
	}
	return 0;
}

unsigned int String::size() const {
	return strlen(c_str());
}

String& String::assign(const char * a){
	return assign(a, capacity());
}

String& String::assign(const String& a){
	return assign(a.c_str());
}

String& String::assign(const String& a, unsigned int subpos, unsigned int sublen){
	return assign(a.c_str() + subpos, sublen);
}

String& String::assign(const char * a, unsigned int n){
	clear();
	if( n < capacity() ){
		strncpy(data(), a, n);
	} else {
		strncpy(data(), a, capacity());
	}
	return *this;
}

void String::append(const char * a){
	const char * s = c_str();
	if( strlen(s) + strlen(a) <= capacity() ){
		strcat(data(),a);
	}
}
void String::append(char c){
	const char * s = c_str();
	if( strlen(s) + 1 <= capacity() ){
		strncat(data(),&c,1);
	}
}

void String::clear(void){
	memset(data(), 0, capacity() + 1);
}

bool String::csv(String & dest, int n, char sep, char term){
	dest.clear();
	int element = 0;
	int start = 0;
	int end = 0;
	char c;
	for(unsigned int i=0; i < size(); i++){
		c = at(i);
		if( (c == sep) || (c == term) ){
			end = i;
			if( n == element ){
				copy(dest.data(), end - start, start);
				return true;
			}
			element++;
			start = i+1;
		}
	}
	return false;
}

int String::csv_size(char sep, char term){
	int elements = 1;
	//return the total number of elements
	for(unsigned int i=0; i < size(); i++){
		if( at(i) == sep ){
			elements++;
		}
		if( at(i) == term ){
			break;
		}
	}
	return elements;
}

String& String::insert(unsigned int pos, const String& str){
	return insert(pos, str.c_str());
}

String& String::insert(unsigned int pos, const char * str){
	char * p = data();
	unsigned int s;
	char buffer[capacity()];

	s = strlen(str);

	//check for room
	if( size() + s <= capacity() ){
		//make room
		strcpy(buffer, p+pos);
		strcpy(p+pos, str);
		strcat(p, buffer);
	}

	return *this;
}

String& String::erase(unsigned int pos, unsigned int len){
	char * p = data();
	if( ((int)len != -1) && (pos + len < size()) ){
		strcpy(p + pos, p + pos + len);
		p[pos+len] = 0;
	} else if (pos < size() ){
		p[pos] = 0;
	}
	return *this;
}


void String::copy(char * s, unsigned int n, unsigned int pos) const {
	const char * p = c_str();
	if( pos < size() ){
		strncpy(s,p+pos,n);
	}
}


bool String::empty() const {
	return (size() == 0);
}



