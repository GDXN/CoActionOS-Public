/*! \file */
#ifndef APPLIB_CORE_HPP_
#define APPLIB_CORE_HPP_

#include <dev/core.h>
#include "Pchar.hpp"

/*! \brief Core Class
 * \details This is the Core class.  It is used to
 * access core MCU information such as clock speed and
 * to adjust interrupt priorities.
 *
 * Here is an example of how to read the MCU serial number:
 *
 * \code
 * #include <applib/Core.hpp>
 * #include <cstdio>
 *
 * int i;
 * Core core(0);
 * core_attr_t attr;
 * core.open();
 * core.getattr(&attr);
 * printf("Serial Number is:");
 * for(i=3; i >= 0; i--){
 * 	printf("%X", attr.serial_number[i]);
 * }
 * printf("\n");
 *
 * core.close(); //core is never "powered-down" but this frees the file descriptor
 * \endcode
 *
 */
class Core : public Periph {
public:
	Core(port_t port);
	static int getports();
	int open(int flags = 0);
	/*! \brief get attributes of the Core peripheral and stores them in \a attr.
	 */
	int getattr(core_attr_t * attr);
	/*! \brief sets the attributes for the Core device */
	int setattr(const core_attr_t * attr);
	/*! \brief executes a request to set the functionality of a pin.
	 */
	int setpinfunc(const core_pinfunc_t * req);
	/*! \brief causes the device to sleep at the specified sleep level.
	 */
	int sleep(core_sleep_t level);
	/*! \brief resets the device. */
	void reset(void);
	/*! \brief invokes the device's bootloader. */
	void invokebootloader(void);
	/*! \brief sets the interrupt request priority according to \a req. */
	int setirqprio(const core_irqprio_t * req);
	/*! \brief configures the clock output pin. */
	int setclkout(const core_clkout_t * clkout);

#ifdef __HWPL_ONLY__
	int close();
#endif


private:
#ifndef __HWPL_ONLY__
	static int fd[HWPL_CORE_PORTS];
	static bool isinitialized;
#endif
};

#endif /* APPLIB_CORE_HPP_ */
