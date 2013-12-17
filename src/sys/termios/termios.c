/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */



#include <errno.h>
#include <unistd.h>
#include "dev/tty.h"
#include "sys/ioctl.h"


int tcgetattr(int fd, struct termios * termios_p){
	return ioctl(fd, I_TTY_GETATTR, termios_p);
}

int tcsetattr(int fd, int optional_actions, const struct termios * termios_p){
	if ( optional_actions == TCSANOW ){
		return ioctl(fd, I_TTY_SETATTR, termios_p);
	}

	errno = ENOTSUP;
	return -1;
}
