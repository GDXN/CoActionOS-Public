/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

/*! \addtogroup USB_DEV_MSC Mass Storage Class
 *
 *
 * @{
 * \ingroup USB_DEV
 *
 * \details This is not yet implemented.
 *
 */


/*! \file */

#ifndef USB_DEV_MSC_H_
#define USB_DEV_MSC_H_


//! \todo add an init function

//Subclasses
#define MSC_SUBCLASS_RBC 0x01
#define MSC_SUBCLASS_SFF 0x02
#define MSC_SUBCLASS_QIC 0x03
#define MSC_SUBCLASS_UFI 0x04
#define MSC_SUBCLASS_SFF 0x05
#define MSC_SUBCLASS_SCSI 0x06

//Protocol Codes
#define MSC_CBI_PROTOCOL 0x00
#define MSC_CBI_PROTOCOL_COMMAND_COMPLETION 0x01
#define MSC_BULK_ONLY_TRANSPORT 0x50




#endif /* USB_DEV_MSC_H_ */

/*! @} */
