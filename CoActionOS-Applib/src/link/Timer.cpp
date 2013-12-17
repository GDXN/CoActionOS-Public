/*
 * Timer.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: tgil
 */

#include <cstdio>
#include <unistd.h>
#include <time.h>
#include "Timer.hpp"


#if defined __win32 || __win64
#include <windows.h>

#define CLOCK_REALTIME 0
int clock_gettime(int clock, struct timespec * t){
	return 0;
}

void sleep(int x){
	Sleep(x*1000);
}

void usleep(int x){
	Sleep(x/1000);
}


#endif

#if defined __macosx
#define CLOCK_REALTIME 0
int clock_gettime(int clock, struct timespec * t){
	return 0;
}

#endif


static struct timespec diff(struct timespec now, struct timespec then){
	struct timespec d;
	d.tv_nsec = now.tv_nsec - then.tv_nsec;
	d.tv_sec = now.tv_sec - then.tv_sec;
	if( d.tv_nsec < 0 ){
		d.tv_nsec += 1000000000;
		d.tv_sec -= 1;
	}
	return d;
}

static struct timespec sum(struct timespec now, uint32_t sec, uint32_t msec, uint32_t usec){
	struct timespec s;
	s.tv_nsec = now.tv_nsec + msec*1000000 + usec*1000;
	s.tv_sec = now.tv_sec + sec;
	if( s.tv_nsec > 1000000000 ){
		s.tv_nsec -= 1000000000;
		s.tv_sec += 1;
	}
	return s;
}

Timer::Timer() {
	// TODO Auto-generated constructor stub
	start_.tv_nsec = 0;
	start_.tv_sec = 0;
	stop_.tv_nsec = 0;
	stop_.tv_sec = 0;
	timeout_stop_.tv_nsec = 0;
	timeout_stop_.tv_sec = 0;
}

void Timer::settimeout_usec(uint32_t timeout){
	clock_gettime(CLOCK_REALTIME, &timeout_stop_);
	timeout_stop_ = sum(timeout_stop_, 0, 0, timeout);
}

void Timer::settimeout_msec(uint32_t timeout){
	clock_gettime(CLOCK_REALTIME, &timeout_stop_);
	timeout_stop_ = sum(timeout_stop_, 0, timeout, 0);
}


void Timer::settimeout_sec(uint32_t timeout){
	clock_gettime(CLOCK_REALTIME, &timeout_stop_);
	timeout_stop_ = sum(timeout_stop_, timeout, 0, 0);
}


bool Timer::isexpired(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	if(now.tv_sec > timeout_stop_.tv_sec ){
		return true;
	}

	if( now.tv_sec < timeout_stop_.tv_sec ){
		return false;
	}

	if( now.tv_nsec < timeout_stop_.tv_nsec ){
		return false;
	}

	return true;
}

void Timer::wait_sec(uint32_t timeout){
	sleep(timeout);
}

void Timer::wait_msec(uint32_t timeout){
	if( (timeout * 1000) < 1000000 ){
		usleep(timeout*1000);
	} else {
		for(uint32_t i = 0; i < timeout; i++){
			usleep(1000);
		}
	}
}

void Timer::wait_usec(uint32_t timeout){
	usleep(timeout);
}

void Timer::start(void){
	clock_gettime(CLOCK_REALTIME, &start_);
	stop_.tv_sec = -1;
}

uint32_t Timer::value(void){
	return usec();
}

uint32_t Timer::sec(void){
	struct timespec now;
	if( stop_.tv_sec < 0 ){
		clock_gettime(CLOCK_REALTIME, &now);
	} else {
		now = stop_;
	}
	//difference between now and start_
	return now.tv_sec - start_.tv_sec;
}

uint32_t Timer::msec(void){
	struct timespec now;
	if( stop_.tv_sec < 0 ){
		clock_gettime(CLOCK_REALTIME, &now);
	} else {
		now = stop_;
	}
	//difference between now and start_
	now = diff(now, start_);
	return now.tv_sec*1000 + (now.tv_nsec + 500000) / 1000000;
}

uint32_t Timer::usec(void){
	struct timespec now;
	if( stop_.tv_sec < 0 ){
		clock_gettime(CLOCK_REALTIME, &now);
	} else {
		now = stop_;
	}
	//difference between now and start_
	now = diff(now, start_);
	return now.tv_sec*1000000 + (now.tv_nsec + 500) / 1000;
}

void Timer::stop(void){
	clock_gettime(CLOCK_REALTIME, &stop_);
}
