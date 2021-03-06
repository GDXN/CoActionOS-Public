/*
 * Packet.hpp
 *
 *  Created on: Aug 22, 2013
 *      Author: tgil
 */

#ifndef PACKET_HPP_
#define PACKET_HPP_

#include <stdint.h>

class Packet {
public:
	Packet();

	int send(const void * data, int nbyte);
	int recv(void * data, int nbyte, int timeout = 1000);

	virtual int write(const void * buf, int nbyte) = 0;
	virtual int read(void * buf, int nbyte) = 0;

private:
	uint16_t calc_checksum(const void * data, int nbyte);

	typedef struct {
		uint16_t start;
		uint16_t size;
	} header_t;

	enum {
		START = 0x0101
	};
};

#endif /* PACKET_HPP_ */
