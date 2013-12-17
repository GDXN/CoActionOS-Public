/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef COMMOM_CM3_H_
#define COMMOM_CM3_H_

#include <stdint.h>
#include "hwpl/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

void _hwpl_core_priv_enable_interrupts(void * args);
void _hwpl_core_priv_disable_interrupts(void * args);
void _hwpl_core_priv_enable_irq(void * x);
void _hwpl_core_priv_disable_irq(void * x);
void _hwpl_core_priv_reset(void * args);
void _hwpl_core_priv_get_stack_ptr(void * ptr);
void _hwpl_core_priv_set_stack_ptr(void * ptr);
void _hwpl_core_priv_get_thread_stack_ptr(void * ptr);
void _hwpl_core_priv_set_thread_stack_ptr(void * ptr);

#define CORE_MAIN_RETURN 0xFFFFFFF9
#define CORE_THREAD_RETURN 0xFFFFFFFD

#define SYSTICK_CTRL_TICKINT (1<<1)

static inline void core_tick_enable_irq(void) HWPL_ALWAYS_INLINE;
void core_tick_enable_irq(void){
	SysTick->CTRL |= SYSTICK_CTRL_TICKINT;
}

static inline void core_tick_disable_irq(void) HWPL_ALWAYS_INLINE;
void core_tick_disable_irq(void){
	SysTick->CTRL &= ~SYSTICK_CTRL_TICKINT;
}

static inline void _hwpl_core_priv_setvectortableaddr(void * addr) HWPL_ALWAYS_INLINE;
void _hwpl_core_priv_setvectortableaddr(void * addr){
	SCB->VTOR = (uint32_t)addr;
}


typedef struct HWPL_PACK {
	uint32_t exec_return;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} core_hw_stack_frame_t;

typedef core_hw_stack_frame_t core_stack_frame_t;

typedef struct HWPL_PACK {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
} core_sw_stack_frame_t;



int core_set_interrupt_priority(int periph, int port, uint8_t priority);

#define TASK_MAIN_RETURN 0xFFFFFFF9
#define TASK_THREAD_RETURN 0xFFFFFFFD

#define SYSTICK_CTRL_TICKINT (1<<1)

typedef struct HWPL_PACK {
	uint32_t exc_return;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} task_hw_stack_frame_t;

typedef struct HWPL_PACK {
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
} task_sw_stack_frame_t;

static inline void task_save_context(void);
void task_save_context(void){
	uint32_t register scratch asm("r1");
	asm volatile ("MRS %0, psp\n\t"
			"STMDB %0!, {r4-r11}\n\t"
			"MSR psp, %0\n\t"  : "=r" (scratch) );
}

static inline void task_load_context(void);
void task_load_context(void){
	uint32_t register scratch asm("r1");
	asm volatile ("MRS %0, psp\n\t"
			"LDMFD %0!, {r4-r11}\n\t"
			"MSR psp, %0\n\t"  : "=r" (scratch) );
}


int core_fault_init(void (*handler)(int) );
void core_fault_reset_status(void);
int core_fault_debug(void);


static inline void _hwpl_core_unprivileged_mode(void) HWPL_ALWAYS_INLINE;
void _hwpl_core_unprivileged_mode(void){
	register uint32_t control;
	control = __get_CONTROL();
	control |= 0x01;
	__set_CONTROL(control);
}

static inline void _hwpl_core_thread_mode(void) HWPL_ALWAYS_INLINE;
void _hwpl_core_thread_mode(void){
	register uint32_t control;
	control = __get_CONTROL();
	control |= 0x02;
	__set_CONTROL(control);
}

#ifdef __cplusplus
}
#endif


#endif /* COMMOM_CM3_H_ */
