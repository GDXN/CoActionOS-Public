/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */


#include <unistd.h>


#include "hwpl.h"


static uid_t uid HWPL_SYS_MEM;
static uid_t euid HWPL_SYS_MEM;
static gid_t gid HWPL_SYS_MEM;
static gid_t egid HWPL_SYS_MEM;

uid_t geteuid(void){
	return euid;
}

uid_t getuid(void){
	return uid;
}

int seteuid(uid_t uid){
	//needs to be priv'd
	return 0;
}

gid_t getgid(void){
	return gid;
}

gid_t getegid(void){
	return egid;
}



