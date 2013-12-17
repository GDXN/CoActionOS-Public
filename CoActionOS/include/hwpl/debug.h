/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef _HWPL_DEBUG_H_
#define _HWPL_DEBUG_H_

#include "arch.h"
#include <stdio.h>
#include <string.h>
#include "../device.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __link

#define HWPL_DEBUG_PORT 0
#define NO_DEBUG

#define HWPL_DEBUG_BUFFER_SIZE 256
extern char hwpl_debug_buffer[HWPL_DEBUG_BUFFER_SIZE];

#ifdef NO_DEBUG
#define hwpl_debug_init() 0
#define hwpl_priv_write_debug_uart(x)
#define hwpl_debug(...)
#define hwpl_priv_debug(...)
#define hwpl_debug_irq(...)
#else
int hwpl_debug_init(void);
void hwpl_priv_write_debug_uart(void * args);

//UART debugging
#define hwpl_debug(...) do { \
	siprintf(hwpl_debug_buffer, __VA_ARGS__); \
	hwpl_core_privcall(hwpl_priv_write_debug_uart, NULL); \
} while(0)

#define hwpl_priv_debug(...) do { \
	siprintf(hwpl_debug_buffer, __VA_ARGS__); \
	hwpl_priv_write_debug_uart(NULL); \
} while(0)

#define hwpl_debug_irq(...) do { \
	int bytes; \
	device_transfer_t op; \
	device_periph_t periph; \
	sprintf(hwpl_debug_buffer, __VA_ARGS__); \
	bytes = strlen(hwpl_debug_buffer); \
	periph.port = HWPL_DEBUG_PORT; \
	op.buf = hwpl_debug_buffer; \
	op.nbyte = bytes; \
	op.callback = NULL; \
	hwpl_uart_write(&periph, &op); \
} while(0)

#endif


extern inline void gled_priv_on(void * args);
extern inline void gled_priv_off(void * args);
extern void gled_toggle(void);

#endif

#ifdef __cplusplus
}
#endif


#endif /* DEBUG_H_ */
