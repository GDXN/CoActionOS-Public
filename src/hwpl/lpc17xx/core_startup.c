/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl.h"
#include "hwpl/debug.h"

static void core_init(void);
static const char sys_proc_name[] = "sys";
extern int _main(void);


void _hwpl_core_reset_handler(void) __attribute__ ((section(".reset_vector")));
void _hwpl_core_nmi_isr(void) HWPL_WEAK;
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) HWPL_WEAK;

void _hwpl_core_fault_handler(void) HWPL_WEAK;
void _hwpl_core_hardfault_handler(void) HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_memfault_handler(void)  HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_busfault_handler(void)  HWPL_ALIAS(_hwpl_core_fault_handler);
void _hwpl_core_usagefault_handler(void)  HWPL_ALIAS(_hwpl_core_fault_handler);

void dled_fault_flash(void) HWPL_WEAK;
void dled_nmi_flash(void) HWPL_WEAK;
void dled_isr_flash(void) HWPL_WEAK;

void _hwpl_core_default_isr(void) HWPL_WEAK;
void _hwpl_core_os_handler(void) HWPL_WEAK;
void _hwpl_core_svcall_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);  //Supervisor call (OS environment)
void _hwpl_core_debugmon_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);
void _hwpl_core_pendsv_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);  //Interrupt request for system-level service (OS environment)
void _hwpl_core_systick_handler(void) HWPL_ALIAS(_hwpl_core_os_handler);

//ISR's -- weakly bound to default handler
void _hwpl_core_wdt_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pwm1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_spi_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_ssp0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_ssp1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pll0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rtc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_adc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_bod_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usb_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_enet_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rit_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_mcpwm_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_qei_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pll1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
#ifdef __lpc177x_8x
void _hwpl_core_usbactivity_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_canactivity_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_ssp2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_lcd_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_gpio_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pwm0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eeprom_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
#else
void _hwpl_core_sw0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw5_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw6_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw7_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
#endif

/*! \details This is the startup code which gets written to
 * address 0 (or wherever the text starts if there is another bootloader) in flash memory
 */

void (* const _hwpl_core_vector_table[])(void) __attribute__ ((section(".startup"))) = {
		// Core Level - CM3
		(void*)&_top_of_stack,					// The initial stack pointer
		_hwpl_core_reset_handler,						// The reset handler
		_hwpl_core_nmi_isr,							// The NMI handler
		_hwpl_core_hardfault_handler,					// The hard fault handler
		_hwpl_core_memfault_handler,					// The MPU fault handler
		_hwpl_core_busfault_handler,					// The bus fault handler
		_hwpl_core_usagefault_handler,				// The usage fault handler
		0,										// Reserved -- this is the checksum addr for ISP programming 0x1C
		0,										// Reserved -- this is the hwpl security word 0x20
		0,										// Reserved -- this is the kernel signature checksum value 0x24
		0,										// Reserved
		_hwpl_core_svcall_handler,					// SVCall handler
		_hwpl_core_debugmon_handler,					// Debug monitor handler
		0,										// Reserved
		_hwpl_core_pendsv_handler,					// The PendSV handler
		_hwpl_core_systick_handler,					// The SysTick handler
		//Non Cortex M3 interrupts
		_hwpl_core_wdt_isr              , // 16: WDT
		_hwpl_core_tmr0_isr           , // 17: tmr0
		_hwpl_core_tmr1_isr           , // 18: tmr1
		_hwpl_core_tmr2_isr           , // 19: tmr2
		_hwpl_core_tmr3_isr           , // 20: tmr3
		_hwpl_core_uart0_isr            , // 21: uart0
		_hwpl_core_uart1_isr            , // 22: uart1
		_hwpl_core_uart2_isr            , // 23: uart2
		_hwpl_core_uart3_isr            , // 24: uart3
		_hwpl_core_pwm1_isr             , // 25: PWM1
		_hwpl_core_i2c0_isr             , // 26: I2C0
		_hwpl_core_i2c1_isr             , // 27: I2C1
		_hwpl_core_i2c2_isr             , // 28: I2C2
		_hwpl_core_spi_isr              , // 29: SPI
		_hwpl_core_ssp0_isr             , // 30: SSP0
		_hwpl_core_ssp1_isr             , // 31: SSP1
		_hwpl_core_pll0_isr             , // 32: PLL0 Lock (Main PLL)
		_hwpl_core_rtc_isr              , // 33: Real Time Clock
		_hwpl_core_eint0_isr            , // 34: External Interrupt 0
		_hwpl_core_eint1_isr            , // 35: External Interrupt 1
		_hwpl_core_eint2_isr            , // 36: External Interrupt 2
		_hwpl_core_eint3_isr            , // 37: External Interrupt 3 -- shared with GPIO
		_hwpl_core_adc_isr              , // 38: A/D Converter
		_hwpl_core_bod_isr              , // 39: Brown-Out Detect
		_hwpl_core_usb_isr              , // 40: USB
		_hwpl_core_can_isr              , // 41: CAN
		_hwpl_core_dma_isr              , // 42: General Purpose DMA
		_hwpl_core_i2c_isr              , // 43: I2S
		_hwpl_core_enet_isr             , // 44: Ethernet
		_hwpl_core_rit_isr              , // 45: Repetitive Interrupt tmr
		_hwpl_core_mcpwm_isr            , // 46: Motor Control PWM
		_hwpl_core_qei_isr              , // 47: Quadrature Encoder Interface
		_hwpl_core_pll1_isr             , // 48: PLL1 Lock (USB PLL)

#ifdef __lpc177x_8x
		_hwpl_core_usbactivity_isr,
		_hwpl_core_canactivity_isr,
		_hwpl_core_uart4_isr,
		_hwpl_core_ssp2_isr,
		_hwpl_core_lcd_isr,
		_hwpl_core_gpio_isr,
		_hwpl_core_pwm0_isr,
		_hwpl_core_eeprom_isr
#else
		_hwpl_core_sw0_isr,
		_hwpl_core_sw1_isr,
		_hwpl_core_sw2_isr,
		_hwpl_core_sw3_isr,
		_hwpl_core_sw4_isr,
		_hwpl_core_sw5_isr,
		_hwpl_core_sw6_isr,
		_hwpl_core_sw7_isr
#endif

};

int _hwpl_core_getsignature(int port, void * arg){
	const uint32_t ** ptr;
	ptr = (const uint32_t**)_hwpl_core_vector_table;
	return (uint32_t)(ptr[9]);
}

void _hwpl_core_getserialno(uint32_t * serial_number){
	_hwpl_lpc_flash_get_serialno(serial_number);
}

extern void led_error(void);

void core_init(void){
	uint32_t *src, *dest;
	src = &_etext; //point src to copy of data that is stored in flash
	for(dest = &_data; dest < &_edata; ){ *dest++ = *src++; } //Copy from flash to RAM (data)
	for(src = &_bss; src < &_ebss; ) *src++ = 0; //Zero out BSS section
	for(src = &_sys; src < &_esys; ) *src++ = 0; //Zero out sysmem

	//Re-entrancy initialization
	//If the program faults on the next line, make sure the etext and data are aligned properly in the linker script (4 byte boundary)
	_REENT->procmem_base = (proc_mem_t*)&_ebss;
	_REENT->procmem_base->size = 0;
	_REENT->procmem_base->sigactions = NULL;
	_REENT->procmem_base->siginfos = NULL;
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
	LPC_SC->PCONP = (1<<PCGPIO)|(1<<PCRTC);
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
	while(1){
		dled_nmi_flash();
	}
}

/*! \details
 */
void _hwpl_core_fault_handler(void){
	while(1){
		dled_fault_flash();
	}
}

/*! \brief
 * \details
 */
void _hwpl_core_os_handler(void){
	return;
}

/*! \brief
 * \details
 */
void _hwpl_core_default_isr(void){
	while(1){
		dled_isr_flash();
	}
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
	return 0;
}

void dled_fault_flash(void){}
void dled_nmi_flash(void){}
void dled_isr_flash(void){}




