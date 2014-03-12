/*
 * Nhdc12832.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#include <cstring>
#include "Timer.hpp"
#include "Pwm.hpp"
#include "Spi.hpp"
#include "Pin.hpp"
#include "Nhdc12832.hpp"

Nhdc12832::Nhdc12832(uint8_t spi_port,
		uint8_t cs_port, uint32_t cs_pin,
		uint8_t a0_port, uint32_t a0_pin) :
MLcd(128, 32),
spi_port(spi_port),
cs_port(cs_port), cs_pin(cs_pin),
a0_port(a0_port), a0_pin(a0_pin) {
}

int Nhdc12832::init(uint32_t spi_freq, uint8_t spi_pinassign){
	Pin cs(cs_port, cs_pin);
	Pin a0(a0_port, a0_pin);
	Spi spi(spi_port);
	//This is the startup sequence specified by the manufacturer
	const char start[] = {0xA0, 0xAE, 0xC0, 0xA2, 0x2F, 0x21, 0x81, 0x2F};


	cs.init(Pin::OUTPUT);
	cs = true;
	a0.init(Pin::OUTPUT);
	a0 = true;

	if( spi.init(spi_freq, spi_pinassign, SPI_ATTR_MODE0) < 0 ){
		return -1;
	}

	cmd_out(start, 8);
	clear();
	refresh();
	return 0;
}

void Nhdc12832::clear(void){
	memset(nhd_mem, 0, MEM_ROWS*MEM_COLUMNS);
}

void Nhdc12832::setpixel(unsigned int x, unsigned int y){
	if( (x <= xmax()) && (y <= ymax()) ){
		nhd_mem[x][y/8] |= mask[7-(y&0x07)];
	}
}

void Nhdc12832::clrpixel(unsigned int x, unsigned int y){
	if( (x < 128) && (y < 32) ){
		nhd_mem[x][y/8] &= ~mask[7-(y&0x07)];
	}
}


void Nhdc12832::refresh(void){
	Spi spi(spi_port);
	uint8_t buffer[MEM_ROWS];
	unsigned int i,j;
	unsigned char page = 0xB0;			//Page Address + 0xB0
	//cmd_out(0xAE);					//Display OFF
	cmd_out(0x40);					//Display start address + 0x40
	for(i=0;i<MEM_COLUMNS;i++){				//32pixel display / 8 pixels per page = 4 pages
		assert_cmd();
		buffer[0] = page;	//send page address
		buffer[1] = 0x10;	//column address upper 4 bits + 0x10
		buffer[2] = 0x00;	//column address lower 4 bits + 0x00
		assert_cs();
		spi.write(buffer, 3);
		deassert_cs();
		for(j=0;j<MEM_ROWS;j++){			//128 rows wide
			buffer[j] = nhd_mem[j][i];
		}
		assert_data();
		assert_cs();
		spi.write(buffer, MEM_ROWS);
		deassert_cs();
		page++;					//after 128 columns, go to next page
	}
	cmd_out(0xAF);					//Display ON
}

void Nhdc12832::assert_cs(void){
	Pin p(cs_port, cs_pin);
	p = false;
	Timer::wait_usec(200);
}
void Nhdc12832::deassert_cs(void){
	Pin p(cs_port, cs_pin);
	p = true;
}

void Nhdc12832::assert_cmd(void){
	Pin p(a0_port, a0_pin);
	p = false;
	Timer::wait_usec(100);
}
void Nhdc12832::assert_data(void){
	Pin p(a0_port, a0_pin);
	p = true;
	Timer::wait_usec(100);
}

void Nhdc12832::data_out(const void * data, int nbyte){
	Spi spi(spi_port);
	int i;
	const char * p = (const char*)data;
	assert_data();
	for(i=0; i < nbyte; i++){
		assert_cs();
		spi.swap(*p++);
		deassert_cs();
	}
}

void Nhdc12832::data_out(unsigned char c){
	Spi spi(spi_port);
	assert_data();
	assert_cs();
	spi.swap(c);
	deassert_cs();
}

void Nhdc12832::cmd_out(unsigned char c){
	Spi spi(spi_port);
	assert_cmd();
	assert_cs();
	spi.swap(c);
	deassert_cs();
}

void Nhdc12832::cmd_out(const void * cmd, int nbyte){
	Spi spi(spi_port);
	int i;
	const unsigned char * p = (const unsigned char*)cmd;
	assert_cmd();
	for(i=0; i < nbyte; i++){
		assert_cs();
		spi.swap(*p++);
		deassert_cs();
	}
}


