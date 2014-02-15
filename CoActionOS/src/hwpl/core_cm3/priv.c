#include "hwpl.h"
#include "hwpl/core.h"
#include "hwdl/sys.h"

void hwpl_core_privcall(core_privcall_t call, void * args) HWPL_WEAK;

void hwpl_core_privcall(core_privcall_t call, void * args){
	asm volatile("SVC 0\n");
}

void _hwpl_core_svcall_handler(void){
	register uint32_t * frame;
	register core_privcall_t call;
	register void * args;
	asm volatile ("MRS %0, psp\n\t" : "=r" (frame) );
	call = (core_privcall_t)frame[0];
	args = (void*)(frame[1]);

#ifdef __SECURE
	register uint32_t caller;
	caller = frame[5];
	//check if euid is root OR the call is made within the OS
	if( sys_isroot() ||
		((caller >= (uint32_t)&_text) && (caller < (uint32_t)&_etext)) ){
		call(args);
	}
#else
	call(args);
#endif

}

void _hwpl_core_priv_reset(void * args){
	NVIC_SystemReset();
}

void _hwpl_core_priv_disable_irq(void * x){
	NVIC_DisableIRQ((IRQn_Type)x);
}

void _hwpl_core_priv_enable_irq(void * x){
	NVIC_EnableIRQ((IRQn_Type)x);
}

void _hwpl_core_priv_disable_interrupts(void * args){
	asm volatile ("cpsid i");
}

void _hwpl_core_priv_enable_interrupts(void * args){
	asm volatile ("cpsie i");
}

void _hwpl_core_priv_get_stack_ptr(void * ptr){
	void ** ptrp = (void**)ptr;
	void * result=NULL;
	asm volatile ("MRS %0, msp\n\t" : "=r" (result) );
	*ptrp = result;
}

void _hwpl_core_priv_set_stack_ptr(void * ptr){
	asm volatile ("MSR msp, %0\n\t" : : "r" (ptr) );
}

void _hwpl_core_priv_get_thread_stack_ptr(void * ptr){
	void ** ptrp = (void**)ptr;
	void * result=NULL;
	asm volatile ("MRS %0, psp\n\t" : "=r" (result) );
	*ptrp = result;
}

void _hwpl_core_priv_set_thread_stack_ptr(void * ptr){
	asm volatile ("MSR psp, %0\n\t" : : "r" (ptr) );
}

