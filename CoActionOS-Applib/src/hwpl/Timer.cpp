/*
 * Timer.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: tgil
 */


#include <hwpl/tmr.h>
#include <hwpl/core.h>
#include "Timer.hpp"



Tmr::port_t Timer::port = 0;

static bool tmr_is_expired;
static int tmr_priv_expired(void * context, const void * data);

Timer::Timer() {
	// TODO Auto-generated constructor stub
	start_ = 0;
	stop_ = 0;
	timeout_stop_ = 0;
}

int Timer::clock_usec(void){
	return hwpl_tmr_get(port, 0);
}

int Timer::clock_msec(void){
	return clock_usec() / 1000;
}

int Timer::clock_sec(void){
	return clock_usec() / 1000000;
}


int Timer::init(Tmr::port_t port){
	Timer::port = port;
	tmr_action_t action;
	Tmr tmr(port);
	if(  tmr.init(1000000) < 0 ){
		return -1;
	}
	tmr.on();

	//initialize the interrupts
	action.channel = TMR_ACTION_CHANNEL_OC0;
	action.context = 0;
	action.callback = tmr_priv_expired;
	action.event = TMR_ACTION_EVENT_INTERRUPT;
	hwpl_tmr_setaction(port, &action);

	return 0;
}

void Timer::settimeout(uint32_t timeout){
	Tmr tmr(port);
	timeout_stop_ = tmr.get() + timeout;
}

bool Timer::isexpired(void){
	Tmr tmr(port);
	if( tmr.get() > timeout_stop_ ){
		return true;
	}
	return false;
}

int tmr_priv_expired(void * context, const void * data){
	tmr_is_expired = true;
	return 1;
}

void Timer::wait_usec(uint32_t timeout){
	Tmr tmr(port);
	tmr_reqattr_t chan_req;

	//enable the interrupt
	hwpl_tmr_off(port, 0);
	chan_req.channel =  TMR_ACTION_CHANNEL_OC0;
	chan_req.value = hwpl_tmr_get(port, 0) + timeout;
	hwpl_tmr_setoc(port, &chan_req);

	tmr_is_expired = false;
	hwpl_tmr_on(port, 0);
	while( !tmr_is_expired ){
		_hwpl_core_sleep(CORE_SLEEP);
	}
}

void Timer::wait_msec(uint32_t timeout){
	wait_usec(timeout*1000);
}

void Timer::wait_sec(uint32_t timeout){
	if( timeout > 4294 ){
		timeout = 4294;
	}
	wait_usec(timeout*1000000);
}

void Timer::start(void){
	Tmr tmr(port);
	start_ = tmr.get();
	stop_ = -1;
}


bool Timer::isrunning(){
	if( (int)stop_ == -1 ){
		return true;
	}
	return false;
}

void Timer::reset(void){
	start_ = 0;
	stop_ = 0;
}

uint32_t Timer::value(void){
	Tmr tmr(port);
	if( (int)stop_ != -1 ){
		return start_ - stop_;
	} else {
		return tmr.get() - start_;
	}
}

uint32_t Timer::usec(void){
	return value();
}

uint32_t Timer::msec(void){
	return value()/1000;
}

uint32_t Timer::sec(void){
	return value()/1000000;
}

void Timer::stop(void){
	Tmr tmr(port);
	if( (int)stop_ == -1 ){
		stop_ = tmr.get();
	}
}
