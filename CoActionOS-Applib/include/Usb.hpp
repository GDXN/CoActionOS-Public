/*! \file */

#ifndef APPLIB_USB_HPP_
#define APPLIB_USB_HPP_

#include <dev/usb.h>
#include "Pblock.hpp"

/*! \brief USB Class
 * \details This class implements a USB transceiver.
 */
class Usb : public Pblock {
public:
	Usb(port_t port);
	static int getports(){ return HWPL_USB_PORTS; }
	int open(int flags = 0);
	/*! \brief Get USB attributes */
	int getattr(usb_attr_t * attr);
	/*! \brief Set USB attributes */
	int setattr(usb_attr_t * attr);
	/*! \brief Reset USB */
	int reset(void);
	int attach(void);
	int configure(void);
	int detach(void);
	int disableep(int ep);
	int enableep(int ep);
	bool isconnected(void);
	int resetep(int ep);
	int setaddr(int addr);
	int stallep(int ep);
	int unstallep(int ep);

#ifdef __HWPL_ONLY__
	using Pblock::read;
	using Pblock::write;
	int read(void * buf, int nbyte);
	int write(const void * buf, int nbyte);
	int read(Aio & aio);
	int write(Aio & aio);
	int close(void);
#endif

private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_USB_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_USB_HPP_ */
