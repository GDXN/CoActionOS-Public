/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

typedef struct {
	//uint32_t exc_return;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} hw_stack_frame_t;

extern int fault_dev_save(fault_t * fault);
extern int fault_dev_load(fault_t * fault);

static core_stack_frame_t * handler_stack;
static void busfault_handler(uint32_t bus_status);
static void memfault_handler(uint32_t mem_status);
static void usagefault_handler(uint32_t usage_status);


#define get_pc(stack_reg) (((hw_stack_frame_t*)(stack_reg))->pc)
#define get_link(stack_reg) (((hw_stack_frame_t*)(stack_reg))->lr)

int hwpl_fault_init(void){
	//initialize the fault handling registers
	//Bus, mem, and usage faults are enabled so that they do not cause a hard fault
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk|
			SCB_SHCSR_BUSFAULTENA_Msk|
			SCB_SHCSR_MEMFAULTENA_Msk);

	return 0;
}

int hwpl_fault_save(fault_t * fault){
	return fault_dev_save(fault);
}

int hwpl_fault_load(fault_t * fault){
	return fault_dev_load(fault);
}

void _hwpl_core_hardfault_handler(void){
	_hwpl_core_priv_get_stack_ptr( &handler_stack );
	register uint32_t fault_status;
	hw_stack_frame_t * stack;
	_hwpl_core_priv_get_thread_stack_ptr( &stack );
	fault_t fault;

	fault_status = SCB->HFSR;
	SCB->HFSR = fault_status; //Clear the hard fault

	fault.addr = (void*)0xFFFFFFFF;
	fault.num = HWPL_FAULT_HARD_UNKNOWN;

	if ( fault_status & (1<<30) ){

		fault_status = SCB->CFSR;
		SCB->CFSR = fault_status;

		if ( fault_status & 0xFF ){
			return memfault_handler(0);
		} else if ( fault_status & 0xFF00 ){
			return busfault_handler(fault_status >> 8);
		} else if ( fault_status & 0xFFFF0000 ){
			return usagefault_handler(fault_status >> 16);
		}

	}


	if ( fault_status & (1<<1) ){
		fault.num = HWPL_FAULT_HARD_VECTOR_TABLE;
		fault.addr = (void*)stack->pc;
	}

	fault.pc = (void*)stack->pc;
	fault.caller = (void*)stack->lr;
	fault.handler_pc = (void*)handler_stack->pc;
	fault.handler_caller = (void*)handler_stack->lr;

	hwpl_fault_event_handler(&fault);
}


void _hwpl_core_default_isr(void){
	_hwpl_core_priv_get_stack_ptr( &handler_stack );
	hw_stack_frame_t * stack;
	_hwpl_core_priv_get_thread_stack_ptr( &stack );
	fault_t fault;
	fault.num = HWPL_FAULT_UNHANDLED_INTERRUPT;
	fault.pc = (void*)stack->pc;
	fault.caller = (void*)stack->lr;
	fault.handler_pc = (void*)handler_stack->pc;
	fault.handler_caller = (void*)handler_stack->lr;
	//! \todo The code should be the IRQ number that fired
	hwpl_fault_event_handler(&fault);
}


void _hwpl_core_memfault_handler(void){
	_hwpl_core_priv_get_stack_ptr( &handler_stack );
	register uint32_t status;
	status = SCB->CFSR;
	SCB->CFSR = status;
	memfault_handler(status);
}

void memfault_handler(uint32_t mem_status){
	fault_t fault;
	hw_stack_frame_t * stack;
	_hwpl_core_priv_get_thread_stack_ptr( &stack );

	fault.addr = (void*)0xFFFFFFFF;
	fault.num = HWPL_FAULT_MEM_UNKNOWN;

	if ( mem_status & (1<<7) ){
		fault.addr = (void*)SCB->MMFAR;
	} else {
		fault.addr = 0;
	}

	if ( mem_status & (1<<4) ){
		fault.num = HWPL_FAULT_MEMORY_STACKING;
		fault.addr = stack;
	}

	if ( mem_status & (1<<3) ){
		fault.num = HWPL_FAULT_MEMORY_UNSTACKING;
		fault.addr = stack;
	}

	if ( mem_status & (1<<1) ){
		fault.num = HWPL_FAULT_MEMORY_ACCESS_VIOLATION;
	}

	if ( mem_status & (1<<0) ){
		fault.num = HWPL_FAULT_MEMORY_ACCESS_VIOLATION;
		fault.addr = (void*)stack->pc;
	}

	fault.pc = (void*)stack->pc;
	fault.caller = (void*)stack->lr;
	fault.handler_pc = (void*)handler_stack->pc;
	fault.handler_caller = (void*)handler_stack->lr;

	//! \todo This should always send the PC and the link register of the offending instructions
	hwpl_fault_event_handler(&fault);

}

void _hwpl_core_busfault_handler(void){
	_hwpl_core_priv_get_stack_ptr( &handler_stack );
	register uint32_t status;
	status = SCB->CFSR;
	SCB->CFSR = status; //clear the bits (by writing one)
	busfault_handler(status >> 8);
}

void busfault_handler(uint32_t bus_status){
	fault_t fault;
	hw_stack_frame_t * stack;
	_hwpl_core_priv_get_thread_stack_ptr( &stack );	//Clear the fault

	fault.addr = (void*)0xFFFFFFFF;
	fault.num = HWPL_FAULT_BUS_UNKNOWN;

	if ( bus_status & (1<<7) ){
		fault.addr = (void*)SCB->BFAR;
	}

	if ( bus_status & (1<<4) ){
		fault.num = HWPL_FAULT_BUS_STACKING;
		fault.addr = stack;
	}

	if ( bus_status & (1<<3) ){
		fault.num = HWPL_FAULT_BUS_UNSTACKING;
		fault.addr = stack;
	}

	if ( bus_status & (1<<2) ){
		fault.num = HWPL_FAULT_BUS_IMPRECISE;
	}

	if ( bus_status & (1<<1) ){
		fault.num = HWPL_FAULT_BUS_PRECISE;
	}

	if ( bus_status & (1<<0) ){
		fault.num = HWPL_FAULT_BUS_INSTRUCTION;
		fault.addr = (void*)stack->pc;
	}

	fault.pc = (void*)stack->pc;
	fault.caller = (void*)stack->lr;
	fault.handler_pc = (void*)handler_stack->pc;
	fault.handler_caller = (void*)handler_stack->lr;

	//! \todo This should always send the PC and the link register of the offending instructions
	hwpl_fault_event_handler(&fault);
}

void _hwpl_core_usagefault_handler(void){
	_hwpl_core_priv_get_stack_ptr( &handler_stack );
	register uint32_t status;
	status = SCB->CFSR;
	SCB->CFSR = status;  //clear the bits by writing one
	usagefault_handler(status >> 16);
}

void usagefault_handler(uint32_t usage_status){
	//Clear the fault
	fault_t fault;
	hw_stack_frame_t * stack;
	_hwpl_core_priv_get_thread_stack_ptr( &stack );

	fault.addr = (void*)0xFFFFFFFF;
	fault.num = HWPL_FAULT_USAGE_UNKNOWN;

	if ( usage_status & (1<<9) ){
		fault.num = HWPL_FAULT_USAGE_DIVBYZERO;
	}

	if ( usage_status & (1<<8) ){
		fault.num = HWPL_FAULT_USAGE_UNALIGNED;
	}

	if ( usage_status & (1<<3) ){
		fault.num = HWPL_FAULT_USAGE_NO_COPROCESSOR;
	}

	if ( usage_status & (1<<2) ){
		fault.num = HWPL_FAULT_USAGE_INVALID_PC;
		fault.addr = (void*)stack->pc;
	}

	if ( usage_status & (1<<1) ){
		fault.num = HWPL_FAULT_USAGE_INVALID_STATE;
		fault.addr = (void*)stack->pc;
	}

	if ( usage_status & (1<<0) ){
		fault.num = HWPL_FAULT_USAGE_UNDEFINED_INSTRUCTION;
		fault.addr = (void*)stack->pc;
	}

	fault.pc = (void*)stack->pc;
	fault.caller = (void*)stack->lr;
	fault.handler_pc = (void*)handler_stack->pc;
	fault.handler_caller = (void*)handler_stack->lr;

	hwpl_fault_event_handler(&fault);
}
