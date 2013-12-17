/*! \addtogroup PIO
 * @{
 *
 * \brief Pin Input/Output
 *
 */


#ifndef APPLIB_PIO_H_
#define APPLIB_PIO_H_

#include <dev/pio.h>
#include "periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Open \a port */
int pio_open(port_t port);

/*! \brief Close \a port */
int pio_close(port_t port);
/*! \brief Get the port attributes */
int pio_getattr(port_t port, pio_attr_t * attr);
/*! \brief Set the port attributes */
int pio_setattr(port_t port, const pio_attr_t * attr);
/*! \brief Set the port action */
int pio_setaction(port_t port, const pio_action_t * action);
/*! \brief Set the mask for \a port */
int pio_setmask(port_t port, pio_sample_t mask);
/*! \brief Clear the mask for \a port */
int pio_clrmask(port_t port, pio_sample_t mask);
/*! \brief Get the value of \a port */
pio_sample_t pio_get(port_t port);
/*! \brief Set the value of \a port */
int pio_set(port_t port, pio_sample_t value);

#ifdef __cplusplus
}
#endif

#endif /* APPLIB_PIO_H_ */

/*! @} */
