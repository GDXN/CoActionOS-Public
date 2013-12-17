/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup TTY TTY (terminal devices)
 * @{
 *
 *
 * \ingroup SYS
 *
 * \details This is the interface to accessing terminal devices.  This driver is not implemented
 * in this version.
 *
 *
 */

/*! \file  */
#ifndef DEV_TTY_H_
#define DEV_TTY_H_

#include <stdint.h>

#include "dev/ioctl.h"
#include "termios.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TTY_IOC_CHAR 't'

/*! \brief This request sets the terminal attributes
 * \hideinitializer
 */
#define I_TTY_SETATTR _IOCTLW(TTY_IOC_CHAR, 0, struct termios)

/*! \brief This request gets the terminal attributes
 * \hideinitializer
 */
#define I_TTY_GETATTR _IOCTLR(TTY_IOC_CHAR, 1, struct termios)



#define TTY_FLAG_ICRNL 0
#define TTY_FLAG_IGNBRK 1
#define TTY_FLAG_IGNCR 2
#define TTY_FLAG_IGNPAR 3
#define TTY_FLAG_ECHO 4
#define TTY_FLAG_ECHOE 5
#define TTY_FLAG_ECHOK 6
#define TTY_FLAG_ECHONL 7
#define TTY_FLAG_ICANON 8
#define TTY_FLAG_IEXTEN 9
#define TTY_FLAG_ISIG 10
#define TTY_FLAG_NOFLSH 11
#define TTY_FLAG_TOSTOP 12


typedef struct {
	int flags;
} tty_state_t;

typedef struct {
	uint8_t write_ep;
	uint8_t read_ep;
} tty_cfg_t;

#ifdef __cplusplus
}
#endif


#endif /* DEV_TTY_H_ */

/*! @} */
