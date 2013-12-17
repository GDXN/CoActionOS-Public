/*! \addtogroup I2C
 * @{
 *
 * \brief Inter-IC Communications
 *
 */

/*! \file */
#ifndef APPLIB_I2C_H_
#define APPLIB_I2C_H_

#include <dev/i2c.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \details This function opens the specified I2C port (e.g. /dev/i2c0).
 */
int i2c_open(port_t port);

/*! \details This functions closes the specified I2C port.
 *
 */
int i2c_close(port_t port);

/*! \details This function gets the I2C attributes from the specified I2C port.
 *
 */
int i2c_getattr(port_t port, i2c_attr_t * attr);
/*! \details This function sets the I2C attributes.
 *
 */
int i2c_setattr(const port_t port, const i2c_attr_t * attr);
/*! \details This function sets up an I2C transaction
 * according to \a req.
 */
int i2c_setup(const port_t port, const i2c_reqattr_t * req);
/*! \details This function get the error value for the most recent
 * transaction.
 */
int i2c_geterr(port_t port);

/*! \details This function reads data from the I2C port.
 *
 */
int i2c_read(port_t port, int loc, void * buf, int nbyte);
/*! \details This function writes data to the I2C port.
 *
 */
int i2c_write(port_t port, int loc, const void * buf, int nbyte);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_I2C_H_ */

/*! @} */
