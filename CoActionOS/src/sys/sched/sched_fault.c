/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <string.h>
#include "sched_flags.h"

char htoc(int nibble);

void htoa(char * dest, int num);

void sched_fault_build_string(char * dest){
	char hex_buffer[9];
	strcpy(dest, "\nFault:0x");
	htoa(hex_buffer, sched_fault.fault.num);
	strcat(dest, hex_buffer);

	strcat(dest, " Task:0x");
	htoa(hex_buffer, sched_fault.tid);
	strcat(dest, hex_buffer);

	strcat(dest, " 0x");
	htoa(hex_buffer, (int)sched_fault.fault.addr);
	strcat(dest, hex_buffer);

	strcat(dest, "\n\tpc:0x");
	htoa(hex_buffer, (int)sched_fault.fault.pc);
	strcat(dest, hex_buffer);

	strcat(dest, " caller:0x");
	htoa(hex_buffer, (int)sched_fault.fault.caller);
	strcat(dest, hex_buffer);

	strcat(dest, "\n\thpc:0x");
	htoa(hex_buffer, (int)sched_fault.fault.handler_pc);
	strcat(dest, hex_buffer);

	strcat(dest, " hcaller:0x");
	htoa(hex_buffer, (int)sched_fault.fault.handler_caller);
	strcat(dest, hex_buffer);

	strcat(dest, "\n");
}

char htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'A' - 10;
	}
}

void htoa(char * dest, int num){
	char nibble;
	int i;
	for(i=0; i < 8; i++){
		nibble = num & 0xF;
		num >>= 4;
		dest[7 - i] = htoc(nibble);
	}
	dest[8] = 0;
}
