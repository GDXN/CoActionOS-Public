/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include "hwpl/debug.h"
#include "hwpl/gpio.h"
#include "gpio_flags.h"
#include "hwpl/core.h"



int hwpl_gpio_getattr(int port, void * ctl){
	gpio_attr_t * ctlp = (gpio_attr_t*)ctl;
	//read the direction pin status
	ctlp->output = gpio_get_dir(port);
	ctlp->input = ~ctlp->output;
	return 0;
}

int hwpl_gpio_setattr(int port, void * ctl){
	gpio_attr_t * ctlp = (gpio_attr_t*)ctl;

	//set the input pins as input
	gpio_clr_dir(port, ctlp->input);

	//set output pins as output
	gpio_set_dir(port, ctlp->output);

	return 0;
}

int hwpl_gpio_setmask(int port, void * ctl){
	gpio_set(port, (int)ctl);
	return 0;
}

int hwpl_gpio_clrmask(int port, void * ctl){
	gpio_clr(port, (int)ctl);
	return 0;
}

int hwpl_gpio_get(int port, void * ctl){
	return gpio_rd(port);
}

int hwpl_gpio_set(int port, void * ctl){
	gpio_wr(port, (uint32_t)ctl);
	return 0;
}
