/*
 * Nhdc12832.hpp
 *
 *  Created on: Apr 30, 2013
 *      Author: tgil
 */

#ifndef NHDC12832_HPP_
#define NHDC12832_HPP_

#include "Pio.hpp"
#include "Spi.hpp"
#include "MLcd.hpp"

/*! \brief Class for the New Haven Display 128x32 LCD */
/*! \details This class is an implementation of the MLcd
 * abstract class using the New Haven Display 128x32 pixel LCD.  It is controlled
 * using the SPI bus, CS line, A0 line, and Reset line.
 *
 * See the ExpLcdDevice for an example.
 *
 */
class Nhdc12832 : public MLcd {
public:
	Nhdc12832(uint8_t spi_port,
			uint8_t cs_port, uint32_t cs_pin,
			uint8_t a0_port, uint32_t a0_pin);


	int init(uint32_t spi_freq, uint8_t spi_pinassign);
	void refresh(void);
	void clear(void);
	void setpixel(unsigned int x, unsigned int y);
	void clrpixel(unsigned int x, unsigned int y);

	bool tstpixel(unsigned int x, unsigned int y){ return true; }
	bool tstpixel_bounded(unsigned int x, unsigned int y){ return true;}
	void setpixel_bounded(unsigned int x, unsigned int y){ }
	void clrpixel_bounded(unsigned int x, unsigned int y){ }


private:
	enum {
		MEM_ROWS = 128,
		MEM_COLUMNS = 4
	};
	uint8_t nhd_mem[MEM_ROWS][MEM_COLUMNS];
	const uint8_t spi_port;
	const uint8_t cs_port;
	const uint32_t cs_pin;
	const uint8_t a0_port;
	const uint32_t a0_pin;

	void assert_cs(void);
	void deassert_cs(void);
	void assert_cmd(void);
	void assert_data(void);
	void data_out(const void * data, int nbyte);
	void cmd_out(const void * cmd, int nbyte);
	void data_out(unsigned char c);
	void cmd_out(unsigned char c);

};

#endif /* NHDC12832_HPP_ */
