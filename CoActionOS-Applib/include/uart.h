
/*! \addtogroup UART
 * @{
 *
 * \brief UART
 */

/*! \file */

#ifndef APPLIB_UART_H_
#define APPLIB_UART_H_

#include <dev/uart.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function opens the specified UART port (e.g. /dev/uart0 for \a port is 0).
 * \return Zero on success
 */
int uart_open(port_t port);

/*! \details This functions closes the specified UART port.
 * \return Zero on success
 */
int uart_close(port_t port);

/*! \details This function gets the UART attributes from the specified UART port.
 * \return Zero on success
 */
int uart_getattr(port_t port, uart_attr_t * attr);

/*! \details This function sets the UART attributes for the specified port.
 * \return Zero on success
 */
int uart_setattr(port_t port, const uart_attr_t * attr);

/*! \details This function gets a single byte from the UART.
 * \return Zero on success; -1 if not data is avaiable to get.
 */
int uart_getbyte(port_t port, char * c);

/*! \details This function flushes the UART's TX/RX buffers.
 * \return Zero on success
 */
int uart_flush(port_t port);

/*! \details This function reads the UART.
 * \return Number of bytes read or -1 on error.
 *
 */
int uart_read(port_t port, void * buf, int nbyte);

/*! \details This function writes the UART.
 * \return Number of bytes written or -1 on error.
 */
int uart_write(port_t port, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif


#endif /* APPLIB_UART_H_ */

/*! @} */
