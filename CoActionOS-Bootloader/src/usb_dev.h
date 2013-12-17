/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV USB Device
 *
 *
 * @{
 * \ingroup CAOSLIB
 *
 */

#ifndef USB_DEV_H_
#define USB_DEV_H_


#include <hwpl.h>
#include <applib/pio.h>


#include "usb_dev_defs.h"
#include "usb_dev_typedefs.h"
#include "usb_dev_std.h"

/*! \addtogroup USB_DEV_CONF Configuration
 * @{
 */


#ifndef USB_DEV_ALT_SETTING_SIZE
/*! \details
 *
 */
#define USB_DEV_ALT_SETTING_SIZE 4
#endif



#ifndef USB_DEV_USE_DFU_RUNTIME_INTERFACE
/*! details This value is defined as non-zero if the USB runtime class
 * is used.
 *
 */
#define USB_DEV_USE_DFU_RUNTIME_INTERFACE 0
#endif


/*! @} */



int usb_dev_init(const void * dev_desc, const void * cfg_desc, const void * string_desc);



static inline void usb_dev_assert_connect(void) HWPL_ALWAYS_INLINE;
/*! \details This function asserts the USB connect.  It
 * is used to pull the D+ line high.
 */
void usb_dev_assert_connect(void){
	if( USBDEV_CONNECT_ACTIVE == 1 ){
		pio_setmask(USBDEV_CONNECT_PORT, (1<<USBDEV_CONNECT_PIN) );
	} else {
		pio_clrmask(USBDEV_CONNECT_PORT, (1<<USBDEV_CONNECT_PIN) );
	}
}


static inline void usb_dev_deassert_connect(void) HWPL_ALWAYS_INLINE;
/*! \details This function deasserts the USB connect.  It
 * is used to pull the D+ line low.
 */
void usb_dev_deassert_connect(void){
	if( USBDEV_CONNECT_ACTIVE == 1 ){
		pio_clrmask(USBDEV_CONNECT_PORT, (1<<USBDEV_CONNECT_PIN) );
	} else {
		pio_setmask(USBDEV_CONNECT_PORT, (1<<USBDEV_CONNECT_PIN) );
	}
}


void usb_feature_event(void);
void usb_configure_event(void);
void usb_interface_event(void);


#endif /* USB_DEVICE_H_ */

/*! @} */
