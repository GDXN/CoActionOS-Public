/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"

void _hwpl_core_reset_handler(void);
void _hwpl_core_nmi_isr(void) HWPL_WEAK;
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) HWPL_WEAK;

/*! \details
 */
void _hwpl_core_fault_handler(void) HWPL_WEAK;
void _hwpl_core_hardfault_handler(void) HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_memfault_handler(void) HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_busfault_handler(void) HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_usagefault_handler(void) HWPL_ALIAS(_hwpl_core_fault_handler);

void dled_fault_flash(void) HWPL_WEAK;
void dled_nmi_flash(void) HWPL_WEAK;
void dled_isr_flash(void) HWPL_WEAK;

void _hwpl_core_default_isr(void) HWPL_WEAK;
void _hwpl_core_os_handler(void) HWPL_WEAK;
void _hwpl_core_svcall_handler(void) HWPL_ALIAS(_hwpl_core_os_handler); //Supervisor call (OS environment)
void _hwpl_core_debugmon_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);
void _hwpl_core_pendsv_handler(void) HWPL_ALIAS(_hwpl_core_os_handler); //Interrupt request for system-level service (OS environment)
void _hwpl_core_systick_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);


//ISR's -- weakly bound to default handler
void _hwpl_core_wdt_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_flash_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_spi_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_ssp0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_ssp1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pio0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pio1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pio2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pio3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_adc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_bod_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usb_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usbfiq_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);



void _hwpl_core_eint0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint5_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint6_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint7_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint8_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint9_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint10_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint11_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);

void _hwpl_core_eint12_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint13_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint14_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint15_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint16_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint17_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint18_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint19_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint20_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint21_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint22_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint23_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);

void _hwpl_core_eint24_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint25_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint26_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint27_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint28_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint29_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint30_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint31_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint32_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint33_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint34_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint35_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);

void _hwpl_core_eint36_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint37_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint38_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint39_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);



void (* const _hwpl_core_vector_table[])(void) __attribute__ ((section(".startup"))) = {
		(void *)&_top_of_stack, // The initial stack pointer
		_hwpl_core_reset_handler, 	 // The reset handler
		_hwpl_core_nmi_isr, 	// The NMI handler -14
		_hwpl_core_hardfault_handler, // The hard fault handler -13
		_hwpl_core_memfault_handler, // The MPU fault handler -12
		_hwpl_core_busfault_handler, // The bus fault handler -11
		_hwpl_core_usagefault_handler, // The usage fault handler -10
		0, 	// Reserved
		0, 	// Reserved
		0, 	// Reserved
		0, 	// Reserved
		_hwpl_core_svcall_handler, 	// SVCall handler -5
		_hwpl_core_debugmon_handler, // Debug monitor handler -4
 		0, 	// Reserved
		_hwpl_core_pendsv_handler, 	// The PendSV handler -2
		_hwpl_core_systick_handler, // The SysTick handler -1

		_hwpl_core_eint0_isr,  //0
		_hwpl_core_eint1_isr,
		_hwpl_core_eint2_isr,
		_hwpl_core_eint3_isr,
		_hwpl_core_eint4_isr,
		_hwpl_core_eint5_isr,
		_hwpl_core_eint6_isr,
		_hwpl_core_eint7_isr,
		_hwpl_core_eint8_isr,
		_hwpl_core_eint9_isr,
		_hwpl_core_eint10_isr,
		_hwpl_core_eint11_isr,  //11
		_hwpl_core_eint12_isr,  //12
		_hwpl_core_eint13_isr,
		_hwpl_core_eint14_isr,
		_hwpl_core_eint15_isr,
		_hwpl_core_eint16_isr,
		_hwpl_core_eint17_isr,
		_hwpl_core_eint18_isr,
		_hwpl_core_eint19_isr,
		_hwpl_core_eint20_isr,
		_hwpl_core_eint21_isr,
		_hwpl_core_eint22_isr,
		_hwpl_core_eint23_isr,  //23
		_hwpl_core_eint24_isr,  //24
		_hwpl_core_eint25_isr,
		_hwpl_core_eint26_isr,
		_hwpl_core_eint27_isr,
		_hwpl_core_eint28_isr,
		_hwpl_core_eint29_isr,
		_hwpl_core_eint30_isr,
		_hwpl_core_eint31_isr,
		_hwpl_core_eint32_isr,
		_hwpl_core_eint33_isr,
		_hwpl_core_eint34_isr,
		_hwpl_core_eint35_isr, //35
		_hwpl_core_eint36_isr,
		_hwpl_core_eint37_isr,
		_hwpl_core_eint38_isr,
		_hwpl_core_eint39_isr,  //39
		_hwpl_core_i2c_isr, // I2C0 -- 40
		_hwpl_core_tmr0_isr, // CT16B0 (16-bit Timer 0)
		_hwpl_core_tmr1_isr, // CT16B1 (16-bit Timer 1)
		_hwpl_core_tmr2_isr, // CT32B0 (32-bit Timer 0)
		_hwpl_core_tmr3_isr, // CT32B1 (32-bit Timer 1)
		_hwpl_core_ssp0_isr, //SSP0
		_hwpl_core_uart_isr, //UART0 --46
		_hwpl_core_usb_isr, //USB IRQ
		_hwpl_core_usbfiq_isr, //USB FIQ
		_hwpl_core_adc_isr, // ADC  (A/D Converter)
		_hwpl_core_wdt_isr, // WDT  (Watchdog Timer)
		_hwpl_core_bod_isr, // BOD  (Brownout Detect)
		_hwpl_core_flash_isr, // Flash (IP2111 Flash Memory Controller)
		_hwpl_core_pio3_isr, // PIO INT3
		_hwpl_core_pio2_isr, // PIO INT2
		_hwpl_core_pio1_isr, // PIO INT1
		_hwpl_core_pio0_isr, // PIO INT0
		_hwpl_core_ssp1_isr //SSP 0

};

static void core_init(void);
const char sys_proc_name[] = "sys";

void core_init(void){
	uint32_t *src, *dest;
	//Copy the data section stored in Flash to RAM
	src = &_etext;
	for(dest = &_data; dest < &_edata; ){ *dest++ = *src++; }
	for(src = &_bss; src < &_ebss; ) *src++ = 0;

	//Zero out the sysmem as well
	for(src = &_sys; src < &_esys; ) *src++ = 0;

	//Re-entrancy initialization
	_REENT->procmem_base = (proc_mem_t*)&_ebss;
	_REENT->procmem_base->size = 0;
	_REENT->procmem_base->proc_name = sys_proc_name;

	memset(_REENT->procmem_base->open_file, 0, sizeof(open_file_t)*OPEN_MAX);

	//Initialize the global mutexes
	__lock_init_recursive_global(__malloc_lock_object);
	_REENT->procmem_base->__malloc_lock_object.flags |= PTHREAD_MUTEX_FLAGS_PSHARED; //Make the malloc lock pshared

	__lock_init_global(__tz_lock_object);
	__lock_init_recursive_global(__atexit_lock);
	__lock_init_recursive_global(__sfp_lock);
	__lock_init_recursive_global(__sinit_lock);
	__lock_init_recursive_global(__env_lock_object);

	//This is the de facto MCU initialization -- turn off power to peripherals that must be "open()"ed.
	LPC_SYSCON->SYSAHBCLKCTRL = (1<<PCGPIO)|(1<<PCROM)|(1<<PCRAM)|(1<<PCFLASHREG)|(1<<PCFLASHARRAY)|(1<<PCIOCON)|(1<<PCSYS);
}

extern int _main(void);

void _hwpl_core_getserialno(uint32_t * serial_number){
	_hwpl_lpc_flash_get_serialno(serial_number);
}

void _hwpl_core_reset_handler(void){
	core_init();
	_hwpl_core_priv_setvectortableaddr((void*)_hwpl_core_vector_table);
	_main(); //This function should never return
	while(1);
}

/*! \details
 */
void _hwpl_core_nmi_isr(void){
}

/*! \details
 */
void _hwpl_core_fault_handler(void){
}

/*! \brief
 * \details
 */
void _hwpl_core_os_handler(void){
}

/*! \brief
 * \details
 */
void _hwpl_core_default_isr(void){

}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
	return 0;
}

void dled_fault_flash(void){}
void dled_nmi_flash(void){}
void dled_isr_flash(void){}
