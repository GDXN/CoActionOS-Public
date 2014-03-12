/*! \file */

#ifndef DBUGUART_HPP_
#define DBUGUART_HPP_

#include "Uart.hpp"
#include "Dbug.hpp"

/*! \brief Debug UART Class
 * \details This class uses Dbug and Uart classes to
 * create a concrete debugging class.  The following is an example
 * of how to use the debugger:
 * \code
 * DbugUart d(0); //use UART port 0 for debugging
 * d.init(); //initialize the UART (only do this ONCE for all instances)
 * d.setmode(Dbug::DECIMAL, 8); //decimal mode with 8 leading zeros
 * d << "My number in DECIMAL " << 1123 << "\n";
 * d.setmode(Dbug::BINARY, 8); //decimal mode with 8 leading zeros
 * d << "My number in BINARY " << 1123 << "\n";
 * \endcode
 *
 */
class DbugUart : public Dbug, public Uart {
public:
	DbugUart(port_t port);

	int open(int mode = 0);
	int close(void);

	int read(void * buf, int nbyte);
	int write(const void * buf, int nbyte);
};

#endif /* DBUGUART_HPP_ */
