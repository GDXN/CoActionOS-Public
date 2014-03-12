/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "hwpl/debug.h"
#include "hwpl/core.h"
#include "hwpl/task.h"
#include "task_flags.h"

static volatile uint32_t * volatile stack HWPL_SYS_MEM; //static keeps this from being stored on the stack
static void system_reset(void); //This is used if the OS process returns
void task_thread_stack_return(void);

int task_init_single(int (*initial_thread)(void),
		void * system_memory,
		int system_memory_size){

	void * system_stack;
	hw_stack_frame_t * frame;
	int i;

	//Initialize the main process
	if ( system_memory == NULL ){
		system_memory = &_data;
	}
	system_stack = system_memory + system_memory_size - sizeof(hw_stack_frame_t);
	frame = (hw_stack_frame_t *)system_stack;
	frame->r0 = 0;
	frame->r1 = 0;
	frame->r2 = 0;
	frame->r3 = 0;
	frame->r12 = 0;
	frame->pc = (uint32_t)initial_thread;
	frame->lr = (uint32_t)system_reset;
	frame->psr = 0x21000000; //default PSR value

	//enable use of PSP
	_hwpl_core_priv_set_thread_stack_ptr( system_stack );

	//Set the interrupt priorities
	for(i=0; i <= DEV_LAST_IRQ; i++){
		NVIC_SetPriority( i, DEV_MIDDLE_PRIORITY - 1 );
	}

	NVIC_SetPriority(SysTick_IRQn, DEV_MIDDLE_PRIORITY);
	NVIC_SetPriority(PendSV_IRQn, DEV_MIDDLE_PRIORITY);
	NVIC_SetPriority(SVCall_IRQn, DEV_MIDDLE_PRIORITY);

	//Turn on the SYSTICK timer and point to an empty interrupt
	core_tick_enable_irq();  //Enable context switching

	_hwpl_core_priv_set_stack_ptr( (void*)&_top_of_stack ); //reset the handler stack pointer
	task_priv_switch_context(NULL);

	while(1);
	return 0;
}

void task_priv_switch_context(void * args){
	SCB->ICSR |= (1<<28); //Force a pendSV interrupt
}

void _hwpl_core_pendsv_handler(void) HWPL_NAKED;
void _hwpl_core_pendsv_handler(void){
	//The value 0xFFFFFFFD is pushed
	asm("mvn.w r0, #2\n\t");
	asm volatile ("push {r0}\n\t");
	asm volatile ("pop {pc}\n\t"); //The value 0xFFFFFFFD is popped to do a thread stack return
}

void task_thread_stack_return(void){
	*stack = TASK_THREAD_RETURN;
}

void system_reset(void){
	hwpl_core_privcall(_hwpl_core_priv_reset, NULL);
}

int task_interrupt(task_interrupt_t * intr){
	//This function inserts the handler on the specified tasks stack
	return 0;
}

void task_restore(void){}

void _hwpl_core_systick_handler(void){

}



