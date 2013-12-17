
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "link_protocol.h"

#include "hwpl/core.h"
#include "hwpl/debug.h"

#define TIMEOUT_VALUE 500


#define pkt_checksum(pktp) ((pktp)->data[(pktp)->size])

static int send_ack(link_phy_t handle, uint8_t ack, uint8_t checksum);

int link_protocol_slaveread(link_phy_t handle, void * buf, int nbyte, int (*callback)(void*,void*,int), void * context){
	link_pkt_t pkt;
	char * p;
	int bytes;
	uint8_t checksum;
	int err;

	bytes = 0;
	p = buf;
	do {

		if( (err = link_protocol_wait_start(handle, &pkt, TIMEOUT_VALUE)) < 0 ){
			send_ack(handle, LINK_PACKET_NACK, 0);
			link_phy_wait(TIMEOUT_VALUE);
			link_phy_flush(handle);
			return -1;
		}

		if( (err = link_protocol_wait_packet(handle, &pkt, TIMEOUT_VALUE)) < 0 ){
			send_ack(handle, LINK_PACKET_NACK, 0);
			link_phy_wait(TIMEOUT_VALUE);
			link_phy_flush(handle);
			return -1;
		}

		if( pkt.start != LINK_PACKET_START ){
			//if packet does not start with the start byte then it is not a packet
			send_ack(handle, LINK_PACKET_NACK, 0);
			link_phy_wait(TIMEOUT_VALUE*5);
			link_phy_flush(handle);
			return -1;
		}

		checksum = pkt_checksum(&pkt);
		//a packet has arrived -- checksum it
		if( link_protocol_checksum_isok(&pkt) == false ){
			//bad checksum on packet -- treat as a non-packet
			send_ack(handle, LINK_PACKET_NACK, checksum);
			link_phy_wait(TIMEOUT_VALUE);
			link_phy_flush(handle);
			return -1;
		}

		//callback to handle incoming data as it arrives
		if( callback == NULL ){
			//copy the valid data to the buffer
			memcpy(p, pkt.data, pkt.size);
			bytes += pkt.size;
			p += pkt.size;
			send_ack(handle, LINK_PACKET_ACK, checksum);
		} else {
			if( callback(context, pkt.data, pkt.size) < 0 ){
				send_ack(handle, LINK_PACKET_NACK, checksum);
				return -1;
			} else {
				bytes += pkt.size;
				if( send_ack(handle, LINK_PACKET_ACK, checksum) < 0 ){
					return -1;
				}
			}
		}

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE) );

	return bytes;
}

int link_protocol_slavewrite(link_phy_t handle, const void * buf, int nbyte, int (*callback)(void*,void*,int), void * context){
	link_pkt_t pkt;
	char * p;
	int bytes;
	int ret;

	bytes = 0;
	p = (void*)buf;
	pkt.start = LINK_PACKET_START;
	do {

		if( (nbyte - bytes) > LINK_PACKET_DATA_SIZE ){
			pkt.size = LINK_PACKET_DATA_SIZE;
		} else {
			pkt.size = nbyte - bytes;
		}

		if( callback != NULL ){
			if( (ret = callback(context, pkt.data, pkt.size)) < 0 ){
				//could not get the desired data
				pkt.size = 0;
			} else {
				pkt.size = ret;
			}
		} else {
			//copy data from buf
			memcpy(pkt.data, p, pkt.size);
		}
		link_protocol_insert_checksum(&pkt);

		//send packet
		if( link_phy_write(handle, &pkt, pkt.size + LINK_PACKET_HEADER_SIZE) != (pkt.size + LINK_PACKET_HEADER_SIZE) ){
			return -1;
		}

		bytes += pkt.size;
		p += pkt.size;

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE) );



	return bytes;
}

int send_ack(link_phy_t handle, uint8_t ack, uint8_t checksum){
	link_ack_t ack_pkt;
	ack_pkt.ack = ack;
	ack_pkt.checksum = checksum;
	return link_phy_write(handle, &ack_pkt, sizeof(ack_pkt));
}

