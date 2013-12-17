/*! \file */

#ifndef PHY_PERIPH_HPP_
#define PHY_PERIPH_HPP_

#include <link_protocol.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <caos.h>

#ifndef __link
#include "Aio.hpp"
#else
#undef fileno
#endif

#include "Phy.hpp"


/*! \brief Peripheral Class
 * \details This is an abstract class for a microcontroller peripheral.
 *
 * All peripherals have a common interface based on the following function:
 * - open()
 * - close()
 * - read()
 * - write()
 * - ioctl()
 *
 * The classes that inherit Periph implement a method for each ioctl() call available
 * on the peripheral.  For example, the UART has an ioctl request called I_UART_SETATTR so
 * the method Uart::setattr() implements the ioctl request.
 *
 */
class Periph : public Phy {
public:
	typedef unsigned int port_t;
	Periph(port_t port, int numports, int * fd = NULL, bool * isinitialized = NULL);

	/*! \brief See details below.
	 * \details This method opens the peripheral.  For each instance, the peripheral
	 * only needs to be opened one time.  The port is typically opened with open() and
	 * configured with setattr().  After that, other instances of the peripheral can read
	 * and write without opening again.
	 */
	virtual int open(int flags){ return -1; };

	int open(const char * name, int flags = 0);

	int setflags(int flags);


	/*! \brief reads the device. */
	virtual int read(void * buf, int nbyte);

	/*! \brief reads a line from the device */
	virtual int readline(char * buf, int nbyte, int timeout = 10, char term = '\n');

	/*! \brief writes to the character device */
	virtual int write(const void * buf, int nbyte);

	int write(const char * s){ return write(s, strlen(s)); }


#ifndef __HWPL_ONLY__
	/*! \brief See details below.
	 * \details This method closes the peripheral.  Once a single instance closes the peripheral,
	 * no other instances will be able to access it.
	 */
	virtual int close();

#else
	virtual int close() = 0;
#endif



#ifndef __link
	/*! \brief Asynchronous read */
	virtual int read(Aio & aio);
	/*! \brief Asynchronous write */
	virtual int write(Aio & aio);
#endif


#ifndef __HWPL_ONLY__
	/*! \brief Return the file descriptor for the peripheral */
	int fileno(void){ return fd[port_]; }
	using Phy::ioctl;
	int ioctl(int req, void * arg);
#endif

	port_t port() const{ return port_; }

protected:
	port_t port_;
#ifndef __HWPL_ONLY__
	int * fd;
#else
	int flags;
#endif

	int _open(const char * name, int flags);

};

#endif /* PHY_PERIPH_HPP_ */
