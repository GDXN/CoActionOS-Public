/*
 * LpcIsp.hpp
 *
 *  Created on: Jun 1, 2013
 *      Author: tgil
 */

#ifndef LPCISP_HPP_
#define LPCISP_HPP_


#include "Isp.hpp"
#include "LpcPhy.hpp"

class LpcIsp : public Isp {
public:
	LpcIsp(Uart * uart, Pin * rst, Pin * ispreq) : phy(uart, rst, ispreq){}

	int program(const char * filename, int crystal, const char * dev, int (*progress)(float));
	int read(const char * filename, int crystal, int (*progress)(float));
	char ** getlist(void);

	int copy_names(char * device, char * pio0, char * pio1);
	int initphy(int uart_pinassign){ return phy.init(uart_pinassign); }
	int exitphy(void){ return phy.exit(); }
	int reset(void){ return phy.reset(); }


private:
	LpcPhy phy;
	int init_prog_interface(int crystal);
	int erase_dev(void);
	uint32_t write_progmem(void * data, uint32_t addr, uint32_t size, int (*update_disp)(float));
	uint32_t read_progmem(void * data, uint32_t addr, uint32_t size, int (*update_disp)(float));
	uint16_t verify_progmem(
			void * data,
			void * original_data,
			uint32_t addr,
			uint32_t size,
			int (*update_disp)(float));

	int write_vector_checksum(unsigned char * hex_buffer, const char * dev);
	int prog_shutdown(void);
};

#endif /* LPCISP_HPP_ */
