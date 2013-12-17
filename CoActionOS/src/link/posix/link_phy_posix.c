/*
 * link_phy_posix.c
 *
 *  Created on: Oct 6, 2012
 *      Author: tgil
 */

#include <stdbool.h>
#include <dirent.h>
#include <termios.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include "../link_flags.h"

#define BAUDRATE 460800

//This is the mac osx prefix -- this needs to be in a list so it can also check bluetooth
#ifdef __macosx
#define TTY_DEV_PREFIX "cu.usbmodem"
#endif

#ifdef __linux
#define TTY_DEV_PREFIX "ttyACM"
#endif

int link_phy_getname(char * dest, const char * last, int len){
	//lookup the next eligible device
	struct dirent entry;
	struct dirent * result;
	DIR * dirp;
	int pre_len;
	int past_last;


	dirp = opendir("/dev");
	if( dirp == NULL ){
		link_error("/dev directory not found");
		return LINK_PHY_ERROR;
	}

	pre_len = strlen(TTY_DEV_PREFIX);
	past_last = false;
	if( strlen(last) == 0 ){
		past_last = true;
	}

	while( (readdir_r(dirp, &entry, &result) == 0) && (result != NULL) ){
		if( strncmp(TTY_DEV_PREFIX, entry.d_name, pre_len) == 0 ){
			//the entry matches the prefix

			if( past_last == true ){
				if( strlen(entry.d_name) > len ){
					//name won't fit in destination
					closedir(dirp);
					return LINK_PHY_ERROR;
				}

				strcpy(dest, entry.d_name);
				closedir(dirp);
				return 0;
			} else if( strcmp(last, entry.d_name) == 0 ){
				past_last = true;
			}
		}
	}

	//no more entries to be found
	closedir(dirp);
	return LINK_PHY_ERROR;
}


link_phy_t link_phy_open(const char * name, int baudrate){
	char path[PATH_MAX];
	link_phy_t fd;
	struct termios options;

	//build the path
	strcpy(path, "/dev/");
	strcat(path, name);

	//open serial port
	fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( fd < 0 ){
		link_error("Failed to open %s %d\n", name, errno);
		perror("Failed to open");
		fflush(stderr);
		return LINK_PHY_ERROR;
	}

	if( ioctl(fd, TIOCEXCL) == -1 ){
		link_error("failed to make device exclusive\n");
		return -1;
	}

	memset(&options, 0, sizeof(options));

	/*
	if( tcgetattr(fd, &options) < 0 ){
		link_error("failed to get options\n");
		return LINK_PHY_ERROR;
	}
	 */

	//make the buffer raw
	cfmakeraw(&options); //raw with no buffering

	//115200 baud
	cfsetspeed(&options, BAUDRATE);
	//even parity
	//8 bit data
	//one stop bit
	options.c_cflag |= PARENB; //parity on
	options.c_cflag &= ~PARODD; //parity is not odd (use even parity)
	options.c_cflag |= CSTOPB; //two stop bits
	options.c_cflag |= CREAD; //enable receiver
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CREAD | CLOCAL | CS8;  //8 bit data
	options.c_cc[VMIN]=0;
	options.c_cc[VTIME]=0;

	//set the attributes
	if( tcflush(fd, TCIFLUSH) < 0 ){
		return LINK_PHY_ERROR;
	}

	if( tcsetattr(fd, TCSANOW, &options) < 0 ){
		return LINK_PHY_ERROR;
	}

	usleep(100*1000); //The MBED outputs some data when first connecting
	link_phy_flush(fd);
	return fd;
}

int link_phy_status(link_phy_t handle){
	struct termios options;
	if( tcgetattr(handle, &options) < 0 ){
		return LINK_PHY_ERROR;
	}
	return 0;
}

int link_phy_write(link_phy_t handle, const void * buf, int nbyte){
	struct termios options;
	if( tcgetattr(handle, &options) < 0 ){
		return LINK_PHY_ERROR;
	}

	if( write(handle, buf, nbyte) != nbyte ){
		return LINK_PHY_ERROR;
	}

	return nbyte;
}

int link_phy_read(link_phy_t handle, void * buf, int nbyte){
	int ret;
	int tmp;
	struct termios options;
	tmp = errno;

	if( tcgetattr(handle, &options) < 0 ){
		return LINK_PHY_ERROR;
	}

	ret = read(handle, buf, nbyte);
	if( ret < 0 ){
		if ( errno == EAGAIN ){
			errno = tmp;
			return 0;
		}
		return LINK_PHY_ERROR;
	}

	return ret;
}

int link_phy_close(link_phy_t handle){
	if( close(handle) < 0 ){
		return LINK_PHY_ERROR;
	}
	return 0;
}

void link_phy_wait(int msec){
	usleep(msec*1000);
}

void link_phy_flush(link_phy_t handle){
	//int i;
	unsigned char c;
	//i = 0;
	while( link_phy_read(handle, &c, 1) == 1 ){
		//i++;
	}
}


