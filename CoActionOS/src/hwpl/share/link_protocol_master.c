
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "link_protocol.h"

#include "hwpl/debug.h"


#ifdef __win32
#define TIMEOUT_VALUE 500
#else
#define TIMEOUT_VALUE 2000
#endif


#define pkt_checksum(pktp) ((pktp)->data[(pktp)->size])

static int wait_ack(link_phy_t handle, uint8_t checksum, int timeout);

int link_protocol_masterread(link_phy_t handle, void * buf, int nbyte){
	link_pkt_t pkt;
	char * p;
	int bytes;
	int err;

	bytes = 0;
	p = buf;
	do {

		if( (err = link_protocol_wait_start(handle, &pkt, TIMEOUT_VALUE)) < 0 ){
			link_phy_flush(handle);
			return err;
		}

		if( (err = link_protocol_wait_packet(handle, &pkt, TIMEOUT_VALUE)) < 0 ){
			link_phy_flush(handle);
			return err;
		}

		//a packet has arrived -- checksum it
		if( link_protocol_checksum_isok(&pkt) == false ){
			return LINK_PROT_ERROR;
		}

		//callback to handle incoming data as it arrives
		//copy the valid data to the buffer
		memcpy(p, pkt.data, pkt.size);
		bytes += pkt.size;
		p += pkt.size;

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE));

	return bytes;
}

int link_protocol_masterwrite(link_phy_t handle, const void * buf, int nbyte){
	link_pkt_t pkt;
	char * p;
	int bytes;
	int err;

	bytes = 0;
	p = (void*)buf;
	pkt.start = LINK_PACKET_START;
	do {

		if( (nbyte - bytes) > LINK_PACKET_DATA_SIZE ){
			pkt.size = LINK_PACKET_DATA_SIZE;
		} else {
			pkt.size = nbyte - bytes;
		}

		memcpy(pkt.data, p, pkt.size);

		link_protocol_insert_checksum(&pkt);

		//send packet
		if( link_phy_write(handle, &pkt, pkt.size + LINK_PACKET_HEADER_SIZE) != (pkt.size + LINK_PACKET_HEADER_SIZE) ){
			return LINK_PHY_ERROR;
		}

		//received ack of the checksum
		if( (err = wait_ack(handle, pkt_checksum(&pkt), TIMEOUT_VALUE)) < 0 ){
			link_phy_flush(handle);
			return err;
		}

		bytes += pkt.size;
		p += pkt.size;

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE) );

	return bytes;
}


int wait_ack(link_phy_t handle, uint8_t checksum, int timeout){
	link_ack_t ack;
	char * p;
	int count;
	int bytes_read;
	int ret;

	count = 0;
	p = (char*)&ack;
	bytes_read = 0;
	do {
		ret = link_phy_read(handle, p, sizeof(ack) - bytes_read);
		if( ret < 0 ){
			return LINK_PHY_ERROR;
		}

		if( ret > 0 ){
			bytes_read += ret;
			p += ret;
			count = 0;
		} else {
#ifndef __WINDOWS
			link_phy_wait(1);
#endif
			count+=1;
			if( count >= timeout ){
				return LINK_PROT_ERROR;
			}
		}
	} while(bytes_read < sizeof(ack));


	if( ack.checksum != checksum ){
		return LINK_PROT_ERROR;
	}

	return ack.ack;
}

