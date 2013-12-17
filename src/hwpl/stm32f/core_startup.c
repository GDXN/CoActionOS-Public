/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include "hwpl.h"


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

extern int _main(void);

//ISR's -- weakly bound to default handler
void _hwpl_core_wdt_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rtc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint59_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_adc12_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma11_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma12_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma13_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma14_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma15_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma16_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma17_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rit_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_mcpwm_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_qei_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pll1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw5_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw6_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sw7_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_flash_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rcc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tamper_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_pvd_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can1tx_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can1rx0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can1rx1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can1sce_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1brk_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1up_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1trigcom_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr1cc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c1ev_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c1er_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c2ev_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_i2c2er_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_spi1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_spi2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usart1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usart2_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usart3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_eint1015_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_rtcalarm_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usbotgfswkup_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr8brk_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr8up_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr8trigcom_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr8ic_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_adc3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_fsmc_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_sdio_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr5_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_spi3_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart4_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_uart5_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr6_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_tmr7_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma21_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma22_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma23_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma24_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_dma25_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_enet_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_enetwkup_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can2tx_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can2rx0_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can2rx1_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_can2sce_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);
void _hwpl_core_usbotgfs_isr(void) HWPL_ALIAS(_hwpl_core_default_isr);


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
		0,										// Reserved -- this is the hwpl_security word 0x20
		0,										// Reserved -- this is the kernel signature checksum value 0x24
		0,										// Reserved
		_hwpl_core_svcall_handler,					// SVCall handler
		_hwpl_core_debugmon_handler,					// Debug monitor handler
		0,										// Reserved
		_hwpl_core_pendsv_handler,					// The PendSV handler
		_hwpl_core_systick_handler,					// The SysTick handler
		//Non Cortex M3 interrupts
		_hwpl_core_wdt_isr                   ,      //< Window WatchDog Interrupt
		_hwpl_core_pvd_isr                    ,      //< PVD through EXTI Line detection Interrupt
		_hwpl_core_tamper_isr                 ,      //< Tamper Interrupt
		_hwpl_core_rtc_isr                    ,      //< RTC global Interrupt
		_hwpl_core_flash_isr                  ,      //< FLASH global Interrupt
		_hwpl_core_rcc_isr                    ,      //< RCC global Interrupt
		_hwpl_core_eint0_isr                  ,      //< EXTI Line0 Interrupt
		_hwpl_core_eint1_isr                  ,      //< EXTI Line1 Interrupt
		_hwpl_core_eint2_isr                  ,      //< EXTI Line2 Interrupt
		_hwpl_core_eint3_isr                  ,      //< EXTI Line3 Interrupt
		_hwpl_core_eint4_isr                  ,     //< EXTI Line4 Interrupt
		_hwpl_core_dma11_isr          ,     //< DMA1 Channel 1 global Interrupt
		_hwpl_core_dma12_isr          ,     //< DMA1 Channel 2 global Interrupt
		_hwpl_core_dma13_isr          ,     //< DMA1 Channel 3 global Interrupt
		_hwpl_core_dma14_isr          ,     //< DMA1 Channel 4 global Interrupt
		_hwpl_core_dma15_isr          ,     //< DMA1 Channel 5 global Interrupt
		_hwpl_core_dma16_isr          ,     //< DMA1 Channel 6 global Interrupt
		_hwpl_core_dma17_isr          ,     //< DMA1 Channel 7 global Interrupt
		_hwpl_core_adc12_isr                 ,     //< ADC1 and ADC2 global Interrupt
		_hwpl_core_can1tx_isr                ,     //< USB Device High Priority or CAN1 TX Interrupts
		_hwpl_core_can1rx0_isr               ,     //< USB Device Low Priority or CAN1 RX0 Interrupts
		_hwpl_core_can1rx1_isr               ,     //< CAN1 RX1 Interrupt
		_hwpl_core_can1sce_isr               ,     //< CAN1 SCE Interrupt
		_hwpl_core_eint59_isr                ,     //< External Line[9:5] Interrupts
		_hwpl_core_tmr1brk_isr               ,     //< TIM1 Break Interrupt
		_hwpl_core_tmr1up_isr                ,     //< TIM1 Update Interrupt
		_hwpl_core_tmr1trigcom_isr           ,     //< TIM1 Trigger and Commutation Interrupt
		_hwpl_core_tmr1cc_isr                ,     //< TIM1 Capture Compare Interrupt
		_hwpl_core_tmr2_isr                   ,     //< TIM2 global Interrupt
		_hwpl_core_tmr3_isr                   ,     //< TIM3 global Interrupt
		_hwpl_core_tmr4_isr                   ,     //< TIM4 global Interrupt
		_hwpl_core_i2c1ev_isr                ,     //< I2C1 Event Interrupt
		_hwpl_core_i2c1er_isr                ,     //< I2C1 Error Interrupt
		_hwpl_core_i2c2ev_isr                ,     //< I2C2 Event Interrupt
		_hwpl_core_i2c2er_isr                ,     //< I2C2 Error Interrupt
		_hwpl_core_spi1_isr                   ,     //< SPI1 global Interrupt
		_hwpl_core_spi2_isr                   ,     //< SPI2 global Interrupt
		_hwpl_core_usart1_isr                 ,     //< USART1 global Interrupt
		_hwpl_core_usart2_isr                 ,     //< USART2 global Interrupt
		_hwpl_core_usart3_isr                 ,     //< USART3 global Interrupt
		_hwpl_core_eint1015_isr              ,     //< External Line[15:10] Interrupts
		_hwpl_core_rtcalarm_isr               ,     //< RTC Alarm through EXTI Line Interrupt
		_hwpl_core_usbotgfswkup_isr            ,     //< USB OTG FS WakeUp from suspend through EXTI Line Interrupt
		_hwpl_core_tmr8brk_isr               ,     //< TIM8 Break Interrupt
		_hwpl_core_tmr8up_isr                ,     //< TIM8 Update Interrupt
		_hwpl_core_tmr8trigcom_isr           ,     //< TIM8 Trigger and Commutation Interrupt
		_hwpl_core_tmr8ic_isr                ,     //< TIM8 Capture Compare Interrupt
		_hwpl_core_adc3_isr                   ,     //< ADC3 global Interrupt
		_hwpl_core_fsmc_isr                   ,     //< FSMC global Interrupt
		_hwpl_core_sdio_isr                   ,     //< SDIO global Interrupt
		_hwpl_core_tmr5_isr                   ,     //< TIM5 global Interrupt
		_hwpl_core_spi3_isr                   ,     //< SPI3 global Interrupt
		_hwpl_core_uart4_isr                  ,     //< UART4 global Interrupt
		_hwpl_core_uart5_isr                  ,     //< UART5 global Interrupt
		_hwpl_core_tmr6_isr                   ,     //< TIM6 global Interrupt
		_hwpl_core_tmr7_isr                   ,     //< TIM7 global Interrupt
		_hwpl_core_dma21_isr          ,     //< DMA2 Channel 1 global Interrupt
		_hwpl_core_dma22_isr          ,     //< DMA2 Channel 2 global Interrupt
		_hwpl_core_dma23_isr          ,     //< DMA2 Channel 3 global Interrupt
		_hwpl_core_dma24_isr          ,     //< DMA2 Channel 4 global Interrupt
		_hwpl_core_dma25_isr          ,     //< DMA2 Channel 5 global Interrupt
		_hwpl_core_enet_isr                    ,     //< Ethernet global Interrupt
		_hwpl_core_enetwkup_isr               ,     //< Ethernet Wakeup through EXTI line Interrupt
		_hwpl_core_can2tx_isr                ,     //< CAN2 TX Interrupt
		_hwpl_core_can2rx0_isr               ,     //< CAN2 RX0 Interrupt
		_hwpl_core_can2rx1_isr               ,     //< CAN2 RX1 Interrupt
		_hwpl_core_can2sce_isr               ,     //< CAN2 SCE Interrupt
		_hwpl_core_usbotgfs_isr                 ,      //< USB OTG FS global Interrupt
		_hwpl_core_sw0_isr,
		_hwpl_core_sw1_isr,
		_hwpl_core_sw2_isr,
		_hwpl_core_sw3_isr,
		_hwpl_core_sw4_isr,
		_hwpl_core_sw5_isr,
		_hwpl_core_sw6_isr,
		_hwpl_core_sw7_isr

};

uint32_t _hwpl_core_getsignature(void){
	const uint32_t ** ptr;
	ptr = (const uint32_t**)_hwpl_core_vector_table;
	return (uint32_t)(ptr[9]);
}


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
	//LPC_SC->PCONP = (1<<PCGPIO)|(1<<PCRTC);
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



