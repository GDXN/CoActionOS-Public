/*! \addtogroup CORE
 * @{
 *
 * \brief Core MCU Access
 *
 */


/*! \file */

#ifndef APPLIB_CORE_H_
#define APPLIB_CORE_H_

#include <dev/core.h>
#include "periph.h"

/*! \details This function opens a core port.
 *
 */
int core_open(port_t port);

/*! \details This function closes a core port.
 *
 */
int core_close(port_t port);

/*! \details This function gets the core attributes.
 *
 */
int core_getattr(port_t port, core_attr_t * attr);

/*! \details This function sets the core attributes.
 *
 */
int core_setattr(port_t port, const core_attr_t * attr);

/*! \details This function assigns the specified function
 * to the specific pin.
 */
int core_setpinfunc(port_t port, const core_pinfunc_t * req);

/*! \details This function causes the device to sleep at
 * the specified level.
 */
int core_sleep(port_t port, core_sleep_t level);

/*! \details This function resets the microcontroller.
 * It does not return.
 */
void core_reset(port_t port);

/*! \details This function invokes the bootloader.  It does not
 * return.
 */
void core_invokebootloader(port_t port);

/*! \details This function sets the interrupt priority.
 *
 */
int core_setirqprio(port_t port, const core_irqprio_t * req);

/*! \brief This function configures the clock out pin. */
int core_setclkout(port_t port, const core_clkout_t * clkout);


#endif /* APPLIB_CORE_H_ */

/*! @} */
