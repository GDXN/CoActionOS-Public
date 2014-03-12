/*
 * Nhdc12864.hpp
 *
 *  Created on: May 14, 2013
 *      Author: tgil
 */

#ifndef NHDC240128_HPP_
#define NHDC240128_HPP_

#include <stdint.h>
#include <applib/MLcd.hpp>
#include <applib/Pin.hpp>
#include <applib/Spi.hpp>
#include <applib/Pwm.hpp>

class Nhdc240128 : public MLcd{
public:
	Nhdc240128(uint8_t pio_port,
			uint8_t cs_port, uint32_t cs_pin,
			uint8_t rd_port, uint32_t rd_pin,
			uint8_t wr_port, uint32_t wr_pin,
			uint8_t data_cmd_port, uint32_t data_cmd_pin,
			uint8_t rst_port, uint32_t rst_pin,
			uint8_t pwm_port, uint8_t pwm_chan, uint8_t pwm_pinassign);


	int init(void);
	void refresh(void);
	void clear(void);
	bool tstpixel(unsigned int x, unsigned int y);
	void setpixel(unsigned int x, unsigned int y);
	void clrpixel(unsigned int x, unsigned int y);
	bool tstpixel_bounded(unsigned int x, unsigned int y);
	void setpixel_bounded(unsigned int x, unsigned int y);
	void clrpixel_bounded(unsigned int x, unsigned int y);
	void setbrightness(int value, int max);

	void * mem();
	int columns(){ return MEM_COLUMNS; }

private:
	enum {
		MEM_ROWS = 128,
		MEM_COLUMNS = 30
	};
	uint8_t nhd_mem[MEM_ROWS][MEM_COLUMNS];

	Pio data;
	Pin cs;
	Pin rd;
	Pin wr;
	Pin data_cmd;
	Pin rst;
	Pwm pwm;
	uint8_t pwm_chan, pwm_pinassign;

	void assert_cs(void);
	void deassert_cs(void);
	void assert_cmd(void);
	void assert_data(void);
	void data_out(const void * data, int nbyte);
	void cmd_out(const void * cmd, int nbyte);
	void data_out(unsigned char c);
	void cmd_out(unsigned char c);
};

#endif /* NHDC240128_HPP_ */
