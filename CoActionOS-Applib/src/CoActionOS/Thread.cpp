/*
 * Thread.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: tgil
 */

//#include <sched.h>
#include <hwpl/task.h>
#include "Thread.hpp"

Thread::Thread(int stack_size, bool detached) {
	// TODO Auto-generated constructor stub
	pthread_attr_init(&pthread_attr);
	pthread_attr_setstacksize(&pthread_attr, stack_size);

	if( detached == true ){
		pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
	} else {
		pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_JOINABLE);
	}

	id_var = -1;

}

int Thread::setstacksize(int size){
	return 0;
}

int Thread::stacksize(void){
	size_t stacksize;
	pthread_attr_getstacksize(&pthread_attr, &stacksize);
	return stacksize;
}

int Thread::setpriority(int prio, int policy){
	struct sched_param param;
	if( (int)id_var == -1 ){
		return -1;
	}
	param.sched_priority = prio;

	return pthread_setschedparam(id_var, policy, &param);
}

void Thread::yield(void){
	//sched_yield();
}

int Thread::priority(void){
	struct sched_param param;
	int policy;
	if( pthread_getschedparam(id_var, &policy, &param) < 0 ){
		return -1;
	}
	return param.sched_priority;
}

int Thread::policy(void){
	struct sched_param param;
	int policy;
	if( pthread_getschedparam(id_var, &policy, &param) < 0 ){
		return -1;
	}
	return policy;
}


int Thread::id(void){
	return id_var;
}

int Thread::create(void * (*func)(void*), void * args, int prio, int policy){
	if( (int)id_var != -1 ){
		return -1;
	}

	//First create the thread
	if( pthread_create(&id_var, &pthread_attr, func, args) < 0 ){
		return -1;
	}

	//now set the priority
	if( setpriority(prio, policy) < 0 ){
		return -1;
	}

	return 0;
}

bool Thread::isrunning(void){
	//check to see if the thread is running
	if( pthread_kill(id_var, 0) == 0 ){
		return true;
	}
	return false;
}

int Thread::wait(void**ret, int interval){

	//if thread is joinable, then join it

	//else just keep sampling until the thread completes

	return 0;
}

