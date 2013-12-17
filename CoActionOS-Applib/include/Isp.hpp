/*
 * Isp.hpp
 *
 *  Created on: Jun 1, 2013
 *      Author: tgil
 */

#ifndef ISP_HPP_
#define ISP_HPP_

class Isp {
public:
	Isp(){}

	virtual int program(const char * filename, int crystal, const char * dev, int (*progress)(float)) = 0;
	virtual int read(const char * filename, int crystal, int (*progress)(float)) = 0;
	virtual char ** getlist(void) = 0;
	virtual int copy_names(char * device, char * pio0, char * pio1) = 0;
	virtual int initphy(int uart_pinassign) = 0;
	virtual int exitphy(void) = 0;
	virtual int reset(void) = 0;

	static int name_maxsize(){ return 32; }

};

#endif /* ISP_HPP_ */
