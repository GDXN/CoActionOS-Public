
/*! \addtogroup SPI
 * @{
 *
 * \brief Serial Peripheral Interface
 *
 */

/*! \file */

#ifndef APPLIB_SPI_H_
#define APPLIB_SPI_H_

#include <dev/spi.h>
#include "periph.h"
#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function opens the specified SPI port (e.g. /dev/spi0).
 */
int spi_open(port_t port);

/*! \details This functions closes the specified SPI port.
 *
 */
int spi_close(port_t port);

/*! \details This function gets the SPI attributes from the specified SPI port.
 *
 */
int spi_getattr(port_t port, spi_attr_t * attr);

/*! \details This function sets the SPI attributes for \a port.
 *
 */
int spi_setattr(port_t port, spi_attr_t * attr);

/*! \details This function simulatenously puts/gets a byte from
 * the SPI bus.
 * \return byte read from the SPI
 */
int spi_swap(port_t port, int byte);

/*! \details This function sets an internal pointer to a buffer
 * to use for full duplex operation.  For example, when using spi_write(),
 * data that is read from the bus will be stored in this pointer.  If \a buf
 * is set to NULL, the SPI bus operates in half-duplex mode.
 *
 */
int spi_setduplex(port_t port, void * buf);

/*! \details This function reads data from the SPI.  If used with spi_setduplex(), this
 * can be a full-duplex operation.
 * \return Number of bytes read.
 */
int spi_read(port_t port, void * buf, int nbyte);

/*! \details This function writes data to the SPI.  If used with spi_setduplex(), this
 * can be a full-duplex operation.
 * \return Number of bytes read.
 */
int spi_write(port_t port, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif


#endif /* APPLIB_SPI_H_ */

/*! @} */
