/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DELAY
 * @{
 *
 * \ingroup HWPL
 */

/*! \file */


#ifndef DELAY_H_
#define DELAY_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef F_CPU
#define F_CPU 72000000UL
#endif

//user F_CPU to create millisecond and microsecond delay loops
static inline void _delay_loop_1(uint32_t __count) HWPL_ALWAYS_INLINE;

//This is 6 clock cycles per loop
void _delay_loop_1(uint32_t __count){
	asm volatile (
	"L_%=:"	"subs	%0, #1" "\n\t"
			"uxth	%0, %0" "\n\t"
			"cmp	%0, #0" "\n\t"
			"bne.n	L_%=" "\n\t"
			: "=r" (__count)
			: "0" (__count)
			);
}

static inline void _delay_us(uint32_t __us) HWPL_ALWAYS_INLINE __attribute__((optimize("3")));
static inline void _delay_ms(uint32_t __ms) HWPL_ALWAYS_INLINE __attribute__((optimize("3")));

static inline void _delay_ms(uint32_t __ms){
	_delay_us(__ms*1000);
}

static inline void _delay_us(uint32_t __us){
	uint32_t __ticks;
	__ticks = F_CPU / 1000000;
	__ticks *= __us;
	__ticks /= 6;
	_delay_loop_1(__ticks);
}



#ifdef __cplusplus
}
#endif


#endif /* DELAY_H_ */

/*! @} */
