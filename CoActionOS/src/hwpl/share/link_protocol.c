
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "link_protocol.h"

#include "hwpl/core.h"
#include "hwpl/debug.h"

#define TIMEOUT_VALUE 500

#define pkt_checksum(pktp) ((pktp)->data[(pktp)->size])

void link_protocol_insert_checksum(link_pkt_t * pkt){
	int i;
	uint8_t checksum;
	checksum = 0;
	checksum ^= pkt->size;
	for(i=0; i < pkt->size; i++){
		checksum ^= pkt->data[i];
	}
	pkt->data[i] = checksum;
}

bool link_protocol_checksum_isok(link_pkt_t * pkt){
	uint8_t checksum;
	if( pkt->size <= LINK_PACKET_DATA_SIZE ){
		checksum = pkt->data[pkt->size];
	} else {
		return false;
	}

	link_protocol_insert_checksum(pkt);
	if( checksum == pkt_checksum(pkt) ){
		return true;
	}

	return false;
}

int link_protocol_wait_start(link_phy_t handle, link_pkt_t * pkt, int timeout){
	int bytes_read;
	int count;
	count = 0;

	do {
		bytes_read = link_phy_read(handle, pkt, 1);
		if( bytes_read < 0 ){
			return LINK_PHY_ERROR;
		}
		if( bytes_read > 0 ){
			if( pkt->start != LINK_PACKET_START){
				return LINK_PROT_ERROR;
			}
		} else {
			link_phy_wait(1);
			count++;
			if( count == timeout ){
				return LINK_PROT_ERROR;
			}
		}
	} while( bytes_read != 1);
	return 0;
}


int link_protocol_wait_packet(link_phy_t handle, link_pkt_t * pkt, int timeout){
	char * p;
	int bytes_read;
	int bytes;
	int count;
	int page_size;

	p = (char*)&(pkt->size);
	count = 0;
	bytes = 0;
	pkt->size = 0;
	do {

		if( bytes == 0 ){
			page_size = 1;
		} else {
			page_size = (pkt->size - bytes) + LINK_PACKET_HEADER_SIZE-1;
		}

		bytes_read = link_phy_read(handle, p, page_size);
		if( bytes_read < 0 ){
			return LINK_PHY_ERROR;
		}

		if( bytes_read > 0 ){
			if( pkt->size > LINK_PACKET_DATA_SIZE ){
				//this is erroneous data
				return LINK_PROT_ERROR;
			}

			bytes += bytes_read;
			p += bytes_read;
			count = 0;
		} else {
			link_phy_wait(1);
			count++;
			if( count == timeout ){
				return LINK_PROT_ERROR;
			}
		}

	} while( bytes < (pkt->size + LINK_PACKET_HEADER_SIZE-1));

	return 0;
}
