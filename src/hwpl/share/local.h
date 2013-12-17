

#include <errno.h>
#include "dev/ioctl.h"
#include "device.h"

#define GET_PORT() (((device_periph_t*)cfg)->port)


static inline int hwpl_open(const device_cfg_t * cfg,
		int num_ports,
		int (*powered_on)(int),
		void (*power_on)(int)
) HWPL_ALWAYS_INLINE;
int hwpl_open(const device_cfg_t * cfg,
		int num_ports,
		int (*powered_on)(int),
		void (*power_on)(int)
){
	//Check to see if the port is value
	if ( GET_PORT() >= num_ports ){
		errno = EINVAL;
		return -1;
	}

	//Turn the device on
	power_on(GET_PORT());
	return 0;
}

static inline int hwpl_ioctl(const device_cfg_t * cfg,
		int request,
		void * ctl,
		int num_ports,
		int (*powered_on)(int),
		int (* const ioctl_func_table[])(int, void*),
		const int ioctl_func_table_size) HWPL_ALWAYS_INLINE;
int hwpl_ioctl(const device_cfg_t * cfg,
		int request,
		void * ctl,
		int num_ports,
		int (*powered_on)(int),
		int (* const ioctl_func_table[])(int, void*),
		const int ioctl_func_table_size){
	uint32_t periph_request;

	//Check to see if the port is value
	if ( GET_PORT() >= num_ports ){
		errno = EINVAL;
		return -1;
	}

	if ( !powered_on(GET_PORT()) ){
		errno = EIO;
		return -1;
	}

	periph_request = _IOCTL_NUM(request);

	if ( periph_request < ioctl_func_table_size ) {
		return ioctl_func_table[periph_request](GET_PORT(), ctl);
	}

	errno = EINVAL;
	return -1;
}

static inline int hwpl_read(const device_cfg_t * cfg,
		device_transfer_t * rop,
		int num_ports,
		int (*powered_on)(int),
		int (*read)(const device_cfg_t * cfg, device_transfer_t*) ) HWPL_ALWAYS_INLINE;
int hwpl_read(const device_cfg_t * cfg,
		device_transfer_t * rop,
		int num_ports,
		int (*powered_on)(int),
		int (*read)(const device_cfg_t * cfg, device_transfer_t*) ){
	//Check to see if the port is value
	if ( GET_PORT() >= num_ports ){
		errno = EINVAL;
		return -1;
	}

	//Make sure the device is powered up
	if ( !powered_on(GET_PORT()) ){
		errno = EIO;
		return -1;
	}

	return read(cfg, rop);
}

static inline int hwpl_write(const device_cfg_t * cfg,
		device_transfer_t * wop,
		int num_ports,
		int (*powered_on)(int),
		int (*write)(const device_cfg_t * cfg, device_transfer_t*) ) HWPL_ALWAYS_INLINE;
int hwpl_write(const device_cfg_t * cfg,
		device_transfer_t * wop,
		int num_ports,
		int (*powered_on)(int),
		int (*write)(const device_cfg_t * cfg, device_transfer_t*) ){
	//Check to see if the port is value
	if ( GET_PORT() >= num_ports ){
		errno = EINVAL;
		return -1;
	}

	//Make sure the device is powered up
	if ( !powered_on(GET_PORT()) ){
		errno = EIO;
		return -1;
	}

	return write(cfg, wop);
}



static inline int hwpl_close(const device_cfg_t * cfg,
		int num_ports,
		int (*powered_on)(int),
		void (*power_off)(int) ) HWPL_ALWAYS_INLINE;
int hwpl_close(const device_cfg_t * cfg,
		int num_ports,
		int (*powered_on)(int),
		void (*power_off)(int) ){
	//Check to see if the port is value
	if ( GET_PORT() >= num_ports ){
		errno = EINVAL;
		return -1;
	}

	power_off(GET_PORT());
	return 0;
}
