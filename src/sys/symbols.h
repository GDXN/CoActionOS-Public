/*
 * symbols.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tgil
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#include <stdint.h>

extern uint32_t const symbols_table[];

static inline uint32_t symbols_total(void){
	int i;
	i = 0;
	while(symbols_table[i] != 0 ){
		i++;
	}
	return i;
}


#endif /* SYMBOLS_H_ */
