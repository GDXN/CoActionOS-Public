/*! \file */

#ifndef APPLIB_UART_HPP_
#define APPLIB_UART_HPP_

#include <dev/uart.h>
#include "Pchar.hpp"

/*! \brief UART Class
 * \details This class implements a serial UART port.
 *
 * Here is an example of how to use the UART.
 *
 * \code
 * #include <applib/Uart.hpp>
 * Uart uart(0); //use UART0
 * char buffer[256];
 * uart.init(115200); //initializes using 115200 and default parameters (see init())
 * uart.read(buffer, 256); //this will block until at least one byte arrives
 * uart.close(); //free the file descriptors and power down the device
 * \endcode
 *
 * The above example opens using the UART in blocking mode.  If you want to be able
 * to read the UART without blocking until a byte arrives, you can use non-blocking mode.
 *
 * \code
 * #include <applib/Uart.hpp>
 * Uart uart(0);
 * char buffer[256];
 * uart.open(Uart::NONBLOCK|Uart::RDWR);
 *  //now set the attributes
 * uart.setattr(115200, //115200 baud rate
 * 	0, //pin assignment 0 (P0.2 and P0.3 on LPC17xx)
 * 	Uart::NONE, // no parity
 * 	Uart::STOP1, //one stop bit
 * 	8 //8 bits per character
 * 	);
 * uart.read(buffer, 256); //returns immediately even if no data is available (errno is set to EAGAIN if no data)
 * uart.close(); //free the resources
 * 	\endcode
 *
 */
class Uart : public Pchar {
public:
	Uart(port_t port);
	static int getports(){ return HWPL_UART_PORTS; }

	/*! \brief Open the UART port */
	int open(int flags = RDWR);
	/*! \brief Get UART attributes in \a attr */
	int getattr(uart_attr_t * attr);
	/*! \brief Set UART attributes to \a attr */
	int setattr(const uart_attr_t * attr);
	/*! \brief See details below */
	/*! \details This method gets a single byte (if available from the UART).  Upon
	 * success, the byte is written to the value pointed to by \a c.
	 * \returns Zero on successfully reading a byte, -1 if no bytes are available.
	 */
	int getbyte(char * c);
	/*! \brief Flush the TX/RX buffers */
	int flush(void);

#ifdef __HWPL_ONLY__
	void setfifo(const device_cfg_t * cfg){ fifocfg[port_] = cfg; }

	int read(void * buf, int nbyte);
	int write(const void * buf, int nbyte);
	int close(void);
#endif


	/*! \brief UART Parity values */
	/*! \details This is a list of the UART parity values */
	enum {
		NONE /*! No Parity */ = UART_PARITY_NONE,
		EVEN /*! Even Parity */ = UART_PARITY_EVEN,
		ODD /*! Odd Parity */ = UART_PARITY_ODD
	};

	/*! \brief UART Stop bits */
	/*! \details This is a list of the UART stop bit values */
	enum {
		STOP1 /*! One stop bit */ = UART_ATTR_STOP_BITS_1,
		STOP2 /*! Two stop bits */ = UART_ATTR_STOP_BITS_2
	};

	/*! \brief Set the attributes as specified */
	int setattr(int baudrate /*! Baudrate */ = 19200,
			int pin_assign /*! Pin assignment */ = 0,
			uart_parity_t parity /*! Parity */ = UART_PARITY_NONE,
			int stop /*! Number of stop bits */ = UART_ATTR_STOP_BITS_2,
			int width /*! bits per character */ = 8){
		uart_attr_t attr;
		attr.baudrate = baudrate;
		attr.pin_assign = pin_assign;
		attr.parity = parity;
		attr.stop = stop;
		attr.width = width;
		return setattr(&attr);
	}

	/*! \brief See details below.
	 * \details Initialize the UART to
	 * - 19200 baudrate
	 * - default pin assignment
	 * - No parity
	 * - 8 bit width
	 * - 2 stop bits
	 * \return Zero on success.
	 */
	int init(int baudrate = 19200,
			int pin_assign = 0,
			uart_parity_t parity = UART_PARITY_NONE,
			int stop = UART_ATTR_STOP_BITS_2,
			int width = 8){
		if( open() < 0 ){
			return -1;
		}
		return setattr(baudrate, pin_assign, parity, stop, width);
	}

private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_UART_PORTS];
	static bool isinitialized;
#else
	static const device_cfg_t * fifocfg[HWPL_UART_PORTS];
#endif
};

#endif /* APPLIB_UART_HPP_ */
