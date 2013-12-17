/*
 * tmr_flags.h
 *
 *  Created on: Sep 29, 2012
 *      Author: tgil
 */

#ifndef TMR_FLAGS_H_
#define TMR_FLAGS_H_

#include "hwpl/arch.h"

#define NUM_CHANNELS 4

extern TIM_TypeDef * const hwpl_tmr_regs_table[HWPL_TMR_PORTS];

static inline void set_channel(TIM_TypeDef * regs, int chan, uint16_t value) HWPL_ALWAYS_INLINE;
void set_channel(TIM_TypeDef * regs, int chan, uint16_t value){
	volatile uint16_t * ccr_ptr;
	ccr_ptr = &regs->CCR1;
	ccr_ptr += (chan*2);
	*ccr_ptr = value;
}

static inline uint16_t get_channel(TIM_TypeDef * regs, int chan, uint16_t value) HWPL_ALWAYS_INLINE;
uint16_t get_channel(TIM_TypeDef * regs, int chan, uint16_t value){
	volatile uint16_t * ccr_ptr;
	ccr_ptr = &regs->CCR1;
	ccr_ptr += (chan*2);
	return *ccr_ptr;
}

struct tmr_cfg {
	uint8_t pin_assign;
	uint8_t enabled_oc_chans;
	uint8_t enabled_ic_chans;
};


typedef struct HWPL_PACK {
	hwpl_callback_t callback[NUM_CHANNELS];
	hwpl_callback_t overflow_callback;
	void * context[NUM_CHANNELS];
	void * overflow_context;
	struct tmr_cfg cfg;
	uint8_t ref_count;
} tmr_local_t;

extern tmr_local_t hwpl_tmr_local[HWPL_TMR_PORTS];


#endif /* TMR_FLAGS_H_ */
