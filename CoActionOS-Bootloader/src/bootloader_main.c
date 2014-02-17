/*! \addtogroup USBDFUAPP
 *
 * @{
 */

//#include "config.h"
#include <hwpl.h>
#include <dev/usb.h>
#include <device.h>
#include <hwpl/core.h>

#include <applib/pio.h>
#include <applib/debug.h>

#include "debug.h"
#include "link.h"

#define USB_DEV_DFU_TRANSFER_SIZE 1024

#ifdef __SECURE
#include <applib/aes256.h>

const unsigned char key[32] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char decrypt_buf[USB_DEV_DFU_TRANSFER_SIZE];
#endif

const pio_t gled = { .port = GLED_PORT, .pin = GLED_PIN };

void gled_on(void);
void gled_off(void);
void gled_open(void);

void init_hw(void);

//#define DEBUG_UART_BUFFER_SIZE 128
//static char rx_buffer[DEBUG_UART_BUFFER_SIZE];
//static char tx_buffer[DEBUG_UART_BUFFER_SIZE];

static void * stack_ptr;
static void (*app_reset)(void);

char dnload_buf[USB_DEV_DFU_TRANSFER_SIZE];
#define SW_BOOT_APP_OVERRIDE 0x55664422
uint32_t * const bootloader_start = (uint32_t*)SW_REQ_LOC;

void run_bootloader(void);

void led_flash(void){
	gled_on();
	_delay_ms(500);
	gled_off();
	_delay_ms(500);
}

void led_error(void){
	while(1){
		led_flash();
	}
}

void led_flash_run_bootloader(void){
	int i;
	for(i=0; i < 3; i++){
		gled_on();
		_delay_ms(3);
		gled_off();
		_delay_ms(3);
	}
}


static int check_run_app(void);
void run_bootloader(void);

/*! \details
 */
int _main(void){
	stack_ptr = (void*)(((uint32_t*)PROGRAM_START_ADDR)[0]);
	app_reset = (void (*)(void))( (((uint32_t*)PROGRAM_START_ADDR)[1]) );

	if ( check_run_app() ){
		app_reset();
		while(1);
	} else {
		led_flash_run_bootloader();
		run_bootloader();
	}

	while(1);
	return 0;
}



void run_bootloader(void){
	init_hw();
	link_init();
	//initialize link and run link update
	link_update(NULL);
	while(1);
}

/*! \details This function checks to see if the application should be run
 * or if the device should enter DFU mode.
 * \return Non-zero if the application should be run.
 */
int check_run_app(void){

	//! \todo Check to see if end of text is less than app program start

	if ( (uint32_t)stack_ptr == 0xFFFFFFFF ){
		//code is not valid
		*bootloader_start = 0;
		return 0;
	}

	if ( !(pio_get(HW_REQ_PORT) & (1<<HW_REQ_PIN)) ){
		*bootloader_start = 0;
		return 0;
	}

	if ( *bootloader_start == SW_REQ_VALUE ){
		*bootloader_start = 0;
		return 0;
	}

	return 1;
}


#ifdef DEBUG_BOOTLOADER
static int debug_write_func(const void * buf, int nbyte){
	memset(hwpl_debug_buffer, 0, HWPL_DEBUG_BUFFER_SIZE);
	memcpy(hwpl_debug_buffer, buf, nbyte);
	hwpl_priv_write_debug_uart(NULL);
	return nbyte;
}
#endif

void init_hw(void){
	int i;
	core_irqprio_t req;
	_hwpl_core_setclock(72000000, 12000000);
	_hwpl_core_priv_enable_interrupts(NULL); //Enable the interrupts

	for(i=0; i < 100; i++){
		_delay_ms(5000);
	}


	//This only needs to be enabled for debugging
	req.periph = CORE_PERIPH_UART;
	req.port = 0;
	req.prio = 14;
	hwpl_core_setirqprio(0, &req);
	req.periph = CORE_PERIPH_USB;
	req.prio = 15;
	hwpl_core_setirqprio(0, &req);

#ifdef DEBUG_BOOTLOADER
	hwpl_debug_init();
	dsetmode(0);
	dsetwritefunc(debug_write_func);
	dstr("STARTING UP\n");

	if ( (uint32_t)stack_ptr == 0xFFFFFFFF ){
		dstr("Stack pointer is invalid\n");
	}

	if ( !(pio_get(HW_REQ_PORT) & (1<<HW_REQ_PIN)) ){
		dstr("Hardware bootloader request\n");
	}

	if ( *bootloader_start == SW_REQ_VALUE ){
		dstr("Software bootloader request\n");
	}


	dstr("STACK: "); dhex((uint32_t)stack_ptr); dstr("\n");
	dstr("APP: "); dhex((uint32_t)app_reset); dstr("\n");


#endif
}



char htoc(int nibble){
	if ( nibble >= 0 && nibble < 10 ){
		return nibble + '0';
	} else {
		return nibble + 'A' - 10;
	}
}


void htoa(char * hexbuf, int value){
	hexbuf[8] = 0;
	hexbuf[7] = htoc( (value) & 0xF );
	hexbuf[6] = htoc( (value >> 4) & 0xF );
	hexbuf[5] = htoc( (value >> 8) & 0xF );
	hexbuf[4] = htoc( (value >> 12) & 0xF );
	hexbuf[3] = htoc( (value >> 16) & 0xF );
	hexbuf[2] = htoc( (value >> 20) & 0xF );
	hexbuf[1] = htoc( (value >> 24) & 0xF );
	hexbuf[0] = htoc( (value >> 28) & 0xF );
}


void gled_on(void){
	pio_attr_t attr;
	attr.mask = (1<<gled.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	pio_setattr(gled.port, &attr);
#if GLED_ACTIVE == 0
	pio_clrmask(gled.port, attr.mask);
#else
	pio_setmask(gled.port, attr.mask);
#endif
}

/*! \details This function turns the green LED off by setting the line to high impedance.
 *
 */
void gled_off(void){
	pio_attr_t attr;
	attr.mask = (1<<gled.pin);
	attr.mode = PIO_MODE_INPUT | PIO_MODE_DIRONLY;
	pio_setattr(gled.port, &attr);

}

/*! \details This function enables the green LED (sets line to output).
 *
 */
void gled_open(void){
	pio_clrmask(gled.port, (1<<gled.pin));
}

/*! @} */
