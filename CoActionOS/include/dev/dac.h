/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup DAC Digital to Analog Converter (DAC)
 * @{
 *
 * \ingroup PERIPHIO
 *
 * \details The DAC peripheral driver allows access to the MCU's built-in DAC. More information about accessing
 * peripheral IO is in the \ref PERIPHIO section.
 *
 * The following is an example of how to write the DAC in an OS environment:
 *
 * \code
 *
 * #include <unistd.h>
 * #include <fcntl.h>
 * #include <errno.h>
 * #include <stdio.h>
 * #include "hwpl.h"
 *
 * int write_dac(dac_sample_t * src, int samples){
 * 	int fd;
 * 	dac_attr_t ctl;
 *
 * 	fd = open("/dev/dac0", O_RDWR);
 * 	if ( fd < 0 ){
 * 		printf("Error opening peripheral (%d)\n", errno);
 * 	} else {
 *		ctl.enabled_channels = (1<<0); //enable channel zero
 * 		ctl.freq = 20000; //20KHz output
 * 		ctl.pin_assign = 0; //Use GPIO configuration zero (see device specific documentation for details)
 * 		if( ioctl(fd, I_SETATTR, &ctl) < 0 ){
 * 			printf("Failed to set peripheral configuration (%d)\n", errno);
 * 			return -1;
 *		}
 *
 *		//now write the samples of the DAC
 *		lseek(fd, 0, SEEK_SET); //this sets the channel to 0, 'c' device does not auto-increment on write
 *		if ( write(fd, src, sizeof(dac_sample_t)*samples) < 0 ){
 *			printf("Error writing peripheral (%d)\n", errno);
 *			return -1;
 *		}
 * 	}
 * 	close(fd);
 * 	return 0;
 * }
 *
 * \endcode
 *
 */

/*! \file
 * \brief DAC Header File
 */

#ifndef DEV_DAC_H_
#define DEV_DAC_H_

#include <stdint.h>
#include "ioctl.h"
#include "hwpl/arch.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DAC_IOC_IDENT_CHAR 'd'


#ifndef DAC_MAX_VALUE
/*! \brief This defines the maximum value that can be written
 * to the DAC.  It is MCU specific.
 * \hideinitializer
 */
#define DAC_MAX_VALUE 0xFFC0
#endif

#ifndef DAC_SAMPLE_T
#define DAC_SAMPLE_T
/*! \brief This defines the type of variable
 * used when writing the DAC.
 */
typedef uint32_t dac_sample_t;
#endif


typedef hwpl_action_t dac_action_t;

/*! \brief DAC Attribute Data Structure
 * \details This structure defines how the control structure
 * for configuring the DAC port.
 */
typedef struct HWPL_PACK {
	uint8_t pin_assign /*! \brief The GPIO configuration to use (see \ref LPC17XXDEV) */;
	uint8_t enabled_channels /*! \brief The DAC channels to enable */;
	uint32_t freq /*! \brief The output frequency */;
} dac_attr_t;

/*! \brief See below for details.
 * \details This requests reads the DAC attributes.
 *
 * Example:
 * \code
 * #include <dev/adc.h>
 * dac_attr_t attr;
 * int fd;
 * ...
 * ioctl(fd, I_DAC_GETATTR, &attr);
 * \endcode
 * \hideinitializer
 */
#define I_DAC_GETATTR _IOCTLR(DAC_IOC_IDENT_CHAR, I_GLOBAL_GETATTR, dac_attr_t)

/*! \brief See below for details.
 * \details This requests writes the DAC attributes.
 *
 * Example:
 * \code
 * #include <dev/adc.h>
 * dac_attr_t attr;
 * int fd;
 * ...
 * ioctl(fd, I_ADC_SETATTR, &attr);
 * \endcode
 * \hideinitializer
 */
#define I_DAC_SETATTR _IOCTLW(DAC_IOC_IDENT_CHAR, I_GLOBAL_SETATTR, dac_attr_t)
#define I_DAC_SETACTION _IOCTLW(DAC_IOC_IDENT_CHAR, I_GLOBAL_SETACTION, dac_action_t)


/*! \brief See below for details.
 * \details This IOCTL arg value causes
 * the ioctl call to return the current output
 * value of the DAC.
 *
 * Example:
 * \code
 * int value;
 * value = ioctl(dac_fd, I_DAC_GET);
 * \endcode
 * \hideinitializer
 */
#define I_DAC_GET _IOCTL(DAC_IOC_IDENT_CHAR, I_GLOBAL_TOTAL)

#define I_DAC_TOTAL 1

#ifdef __cplusplus
}
#endif

#endif // DEV_DAC_H_

/*! @} */


