/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup CAOS
 * @{
 */

/*! \file */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "config.h"
#include "caos.h"
#include "sysfs.h"
#include "hwpl/debug.h"
#include "hwpl/pio.h"
#include "link.h"


extern const pio_t gled;

void gled_priv_error(void * args);

static void led_startup(void);
static int init_fs(void);
static int startup_fs(void);

void priv_check_reset_source(void * args){
	core_attr_t attr;

	hwpl_core_getattr(0, &attr);

	switch(attr.reset_type){
	case CORE_RESET_SRC_WDT:
		hwpl_priv_debug("wdt rst\n");
		break;
	case CORE_RESET_SRC_POR:
		hwpl_priv_debug("por rst\n");
		break;
	case CORE_RESET_SRC_BOR:
		hwpl_priv_debug("bor rst\n");
		break;
	case CORE_RESET_SRC_EXTERNAL:
		hwpl_priv_debug("ext rst\n");
		break;
	}
}

#include <hwpl.h>

void * initial_thread(void * arg){

	hwpl_core_privcall(priv_check_reset_source, NULL);

	//Initialize the file systems
	if ( init_fs() < 0 ){
		;
	}

	//Initialize the CoActionOS Link interface
	if ( link_init() < 0 ){
		hwpl_core_privcall(gled_priv_error, 0);
	}

	startup_fs();

	open("/dev/rtc", O_RDWR);

	usleep(100*1000);
	led_startup();
	link_update(NULL); 	//Run the link update thread--never returns

	return NULL;
}


void led_startup(void){
	int i;
	int duty;
	const int factor = 30;
	duty = 0;
	for(i=0; i < 100; i++){
		duty = i*factor;
		hwpl_core_privcall(gled_priv_on, 0);
		usleep(duty);
		hwpl_core_privcall(gled_priv_off, 0);
		usleep(100*factor - duty);
	}

	for(i=0; i < 100; i++){
		duty = i*factor;
		hwpl_core_privcall(gled_priv_on, 0);
		usleep(100*factor - duty);
		hwpl_core_privcall(gled_priv_off, 0);
		usleep(duty);
	}
}

int init_fs(void){
	int i;
	i = 0;
	while( sysfs_isterminator(&sysfs_list[i]) == false ){
		hwpl_debug("init %s\n", sysfs_list[i].mount_path);
		if ( sysfs_list[i].init( sysfs_list[i].cfg ) < 0 ){
			hwpl_debug("failed to init\n");
			return -1;
		}
		i++;
	}
	return 0;
}

int startup_fs(void){
	int i;
	i = 0;
	int started;
	int ret;
	started = 0;
	while( sysfs_isterminator(&sysfs_list[i]) == false ){
		ret = sysfs_list[i].startup( sysfs_list[i].cfg );
		if ( ret > 0){
			started += ret;
		}
		i++;
	}
	//the number of processes that have been started
	return started;
}

void gled_priv_on(void * args){
	pio_attr_t attr;
	attr.mask = (1<<gled.pin);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	hwpl_pio_setattr(gled.port, &attr);
	hwpl_pio_clrmask(gled.port, (void*)(1<<gled.pin));
}

/*! \details This function turns the green LED off by setting the line to high impedance.
 *
 */
void gled_priv_off(void * args){
	pio_attr_t attr;
	attr.mode = PIO_MODE_INPUT | PIO_MODE_DIRONLY;
	attr.mask = (1<<gled.pin);
	if( hwpl_pio_setattr(gled.port, &attr) < 0 ){
		hwpl_debug("failed to setattr\n");
	}
}

