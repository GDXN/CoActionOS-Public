/*
 * crt.c
 *
 *  Created on: Jan 12, 2013
 *      Author: tgil
 */

#include <stdint.h>
#include <hwpl/core.h>
#include <hwpl/task.h>

extern void (*_ctors)(void);
extern int _ctors_size;

extern void (*_dtors)(void);
extern int _dtors_size;

static void constructors(void);
static void destructors(void);

void core_setclock(void) HWPL_WEAK;
void core_setclock(void){
	_hwpl_core_setclockinternal(0); //use default internal clock
}


extern uint32_t _sram_size;
extern uint32_t _data;
extern int main(void);

int _main(void){
	core_setclock();
	_hwpl_core_priv_enable_interrupts(NULL);
	constructors();
	task_init_single(main, (void*)&_data, (size_t)&_sram_size);
	destructors();
	return 0;
}

void constructors(void){
	int i;
	for(i=0; i < (int)&_ctors_size; i++){
		(&_ctors)[i]();
	}
}
void destructors(void){
	int i;
	for(i=0; i < (int)&_dtors_size; i++){
		(&_dtors)[i]();
	}
}
