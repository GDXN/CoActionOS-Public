/*
 * MBFont.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

#include "MBFont.hpp"

MBFont::MBFont(const void * font) {
	// TODO Auto-generated constructor stub
	font_ = font;

	mbfont_hdr_t * hdr;
	hdr = (mbfont_hdr_t *)font_;

	max_byte_width_ = hdr->max_byte_width;
	max_height_ = hdr->max_height;
	bitmap_ = new MBitmap(max_byte_width_*8, max_height_);
}

MBFont::MBFont(const char * name, int offset) {
	// TODO Auto-generated constructor stub
	mbfont_hdr_t hdr;
	font_ = 0;
	if( f.open(name, File::RDONLY) < 0 ){
		return;
	}

	if( f.read(offset, &hdr, sizeof(hdr)) != sizeof(hdr) ){
		f.close();
		return;
	}

	max_byte_width_ = hdr.max_byte_width;
	max_height_ = hdr.max_height;
	offset_ = offset;
	bitmap_ = new MBitmap(max_byte_width_*8, max_height_);
}

MBFont::~MBFont(){
	if( bitmap_ != 0 ){
		free(bitmap_->data());
	}
	delete bitmap_;
}

MBitmap * MBFont::bitmap(char c){
	if( bitmap_ == 0 ){
		return 0;
	}

	mbfont_char_t ch;
	if( load_char(ch, c) < 0 ){
		return 0;
	}

	if ( load_bitmap(ch, c) < 0 ){
		return 0;
	}

	return bitmap_;
}

int MBFont::len(const char * str){
	mbfont_char_t ch;
	int l;
	l = 0;
	while( *str != 0 ){
		if( load_char(ch, *str) < 0){
			return -1;
		}

		l += ch.width;
		str++;
	}
	return l;
}

int MBFont::load_char_file(mbfont_char_t & ch, char c){
	int offset;
	int ind;
	ind = to_charset(c);
	offset = sizeof(mbfont_hdr_t) + ind*sizeof(mbfont_char_t) + offset_;
	if(f.read(offset, &ch, sizeof(ch)) != sizeof(ch) ){
		return -1;
	}
	return 0;
}

int MBFont::load_char_ptr(mbfont_char_t & ch, char c){
	int offset;
	int ind;
	mbfont_char_t * chp;
	ind = to_charset(c);
	offset = sizeof(mbfont_hdr_t) + ind*sizeof(mbfont_char_t);
	chp = (mbfont_char_t *)((char*)font_ + offset);

	memcpy(&ch, chp, sizeof(mbfont_char_t));
	return 0;
}

int MBFont::load_char(mbfont_char_t & ch, char c){
	if( font_ == 0 ){
		return load_char_file(ch, c);
	} else {
		return load_char_ptr(ch, c);
	}
}

int MBFont::load_bitmap(const mbfont_char_t & ch,char c){
	if( font_ == 0 ){
		return load_bitmap_file(ch, c);
	} else {
		return load_bitmap_ptr(ch, c);
	}
}

int MBFont::load_bitmap_file(const mbfont_char_t & ch, char c){
	int s;
	//calculate number of bytes to read
	s = MBitmap::size(ch.width, ch.height);
	if( f.read(ch.offset + offset_, bitmap_->data(), s) != s ){
		return -1;
	}
	bitmap_->setsize(ch.width, ch.height);
	return 0;
}

int MBFont::load_bitmap_ptr(const mbfont_char_t & ch, char c){
	int s;
	//calculate number of bytes to read
	s = MBitmap::size(ch.width, ch.height);
	//copy the memory
	memcpy(bitmap_->data(), (uint8_t*)font_ + ch.offset, s);
	bitmap_->setsize(ch.width, ch.height);
	return 0;
}

