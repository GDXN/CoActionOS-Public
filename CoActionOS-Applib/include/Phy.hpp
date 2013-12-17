/*! \file */

/*! \mainpage
 *
 * \ref APPLIB_VERSION
 *
 * This is the CoActionOS Applib API documentation.  This is recommended for users that are writing
 * CoActionOS applications.  If you want to write device drivers or tweak the kernel, please see the
 * <a href="/projects/coactionos/embedded/index.html">CoActionOS API documentation</a>.
 *
 * \section INTRO Introduction
 *
 * Applib is an application programming interface to access microcontroller peripheral hardware.
 *
 * \section GETTING_STARTED Getting Started
 *
 * Using Applib with CoActionOS applications is simple.  The API headers are stored in the applib directory so that the source
 * code looks like:
 *
 * \code
 * #include <applib/Pin.hpp>
 *
 * int main(int argc, char * argv[]){
 *  Pin p(2, 10);
 *  p.init(Pin::OUTPUT); //open the port and configure pin 10 as an output
 *  p.clr(); //set the output to zero
 *  p.close(); //close the port (Pin retains state)
 * 	return 0;
 * }
 * \endcode
 *
 *
 * The application needs to be linked to -lapp_CoActionOS_arch where arch is the HWPL architecture such
 * as lpc17xx.  For example:
 *
 * \code
 * -lapp_CoActionOS_lpc17xx
 * \endcode
 *
 * \section CoActionOS Using Applib with CoActionOS
 *
 * CoActionOS uses file descriptors to access hardware using the following functions:
 * - open()
 * - close()
 * - read()
 * - write()
 * - ioctl()
 *
 * Applib creates a simplied API which uses the peripheral name with the function above. There is also a method for
 * each ioctl() request.  For example:
 *
 * \code
 * pio_attr_t attr;
 * port_2_fd = open("/dev/pio2", O_RDWR);
 * ioctl(port_2_fd, I_PIO_SETATTR, &attr);
 * close(port_2_fd);
 * \endcode
 *
 * Becomes:
 *
 * \code
 * pio_attr_t attr;
 * Pio pio(2);
 * pio.open();
 * pio.setattr(&attr);
 * pio.close();
 * \endcode
 *
 * When using Applib with CoActionOS, Applib manages all the file descriptors so that each application has at most
 * one file descriptor open for each device port.  For example, if two independent modules use the same \ref PIO port
 * to access different pins, Applib uses the same file descriptor.  Keep in mind, the port only needs to be opened once,
 * but opening additional times is harmless.
 *
 *
 * The first module can access the port as follows:
 * \code
 * int module_one_pio(void){
 *  Pio pio(2);
 * 	pio_attr_t attr;
 * 	if( pio.open() < 0 ){   //opens the port if it is not already open
 * 		perror("failed to open");
 * 		return -1;
 * 	}
 *
 * 	attr.mask = (1<<0);
 * 	attr.mode = Pio::OUTPUT;
 * 	if( pio.setattr(&attr) < 0 ){
 * 		perror("failed to set attr");
 * 		return -1;
 * 	}
 *
 * 	//drive 2.0 high
 * 	if( pio.setmask((1<<0)) < 0 ){
 * 		perror("failed to set mask");
 * 		return -1;
 * 	}
 *
 * 	return 0;
 *
 * }
 * \endcode
 * The second module can access the port as follows:
 * \code
 * int module_two_pio(void){
 *  Pio pio(2);
 * 	pio_attr_t attr;
 * 	if( pio.open() < 0 ){   //opens the port if it is not already open
 * 		perror("failed to open");
 * 		return -1;
 * 	}
 *
 * 	attr.mask = (1<<2);
 * 	attr.mode = Pio::OUTPUT;
 * 	if( pio.setattr(&attr) < 0 ){
 * 		perror("failed to set attr");
 * 		return -1;
 * 	}
 *
 * 	//drive 2.2 ;low
 * 	if( pio.clrmask(1<<2) < 0 ){
 * 		perror("failed to set mask");
 * 		return -1;
 * 	}
 *
 * 	pio.close(); //Closes the port for all instances of Pio
 *
 * 	return 0;
 *
 * }
 * \endcode
 *
 * Using Applib, a single file descriptor is used to access the pio in both modules above.  Therefore, if one module closes
 * the port, the file descriptor is closed for both modules.
 *
 * \section HWPL Using Applib with HWPL (without CoActionOS)
 *
 * Applib also has an interface which operates directly using HWPL calls.  This is for devices that are not running CoActionOS.
 * It turns HWPL read/write calls (which are typically asynchronous and non-blocking) into synchronous blocking calls.  It
 * likewise provides functions to access ioctl() requests but makes direct calls to HWPL functions.  For example,
 * Pio::setattr() is implemented as:
 * \code
 * int Pio::setattr(const pio_setattr_t * attr){
 * 	return hwpl_pio_setattr(this->port, attr);
 * }
 * \endcode
 *
 * Unlike the CoActionOS API, opening a port multiple times creates multiple references to the port.  Therefore, in order
 * to shut down a peripheral, it needs to be closed for every time it is opened.
 *
 */



#ifndef APPLIB_PHY_HPP_
#define APPLIB_PHY_HPP_

#include <link_protocol.h>
#include <fcntl.h>

/*! \brief Current Version */
#define APPLIB_VERSION "1.3.1"

/*! \brief Base Applib Class for Physical devices
 * \details This is the base class for physical devices.
 *
 */
class Phy {
public:
	Phy();

	enum {
		RDONLY /*! Open as read-only */ = LINK_O_RDONLY,
		READONLY /*! Open as read-only */ = LINK_O_RDONLY,
		WRONLY /*! Open as write-only */ = LINK_O_WRONLY,
		WRITEONLY /*! Open as write-only */ = LINK_O_WRONLY,
		CREATE /*! Create when opening (files) */ = LINK_O_CREAT,
		CREAT /*! Create when opening (files) */ = LINK_O_CREAT,
		TRUNCATE /*! Truncate when opening (files) */ = LINK_O_TRUNC,
		TRUNC /*! Truncate when opening (files) */ = LINK_O_TRUNC,
		APPEND /*! Append when opening (files)*/ = LINK_O_APPEND,
		EXCLUSIVE /*! Create exclusively (files) */ = LINK_O_EXCL,
		EXCL /*! Create exclusively (files) */ = LINK_O_EXCL,
		READWRITE /*! Open as read-write */ = LINK_O_RDWR,
		RDWR /*! Open as read-write */ = LINK_O_RDWR,
		NONBLOCK /*! Open as non-blocking */ = LINK_O_NONBLOCK,
		NDELAY /*! Open as non-blocking */ = LINK_O_NONBLOCK,

	};


#ifdef __link
	void sethandle(link_phy_t h){ handle = h; }
#endif

#ifndef __HWPL_ONLY__
	/*! \brief Return the file descriptor for the peripheral */
	virtual int ioctl(int req, void * arg) = 0;

	int ioctl(int req, const void * arg){ return ioctl(req, (void*)arg); }
	int ioctl(int req){ return ioctl(req, (void*)NULL); }
	int ioctl(int req, int arg){ return ioctl(req, (void*)(size_t)arg); }
#endif

protected:
#ifdef __link
	link_phy_t handle;
#endif



};

#endif /* APPLIB_PHYBASE_HPP_ */
