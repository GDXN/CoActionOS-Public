#include "config.h"

#include <errno.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <link_protocol.h>
#include <hwpl.h>

#include <applib/usb.h>
#include <applib/pio.h>
#include <applib/debug.h>

#include "hwpl/flash.h"
#include <hwpl/debug.h>

#include "link.h"
#include "link_local.h"


#define USB_PORT 0
#define USB_BULKIN 0x82
#define USB_BULKOUT 0x2
#define ENDPOINT_SIZE 64

#ifndef LINK_USB_VID
/*! \details This defines the USB vendor ID used by the Link protocol.
 *
 */
#define LINK_USB_VID 0x20A0
#endif

#ifndef LINK_USB_PID
/*! \details This define the USB product ID used by the Link protocol.
 *
 */
#define LINK_USB_PID 0x41D5
#endif

#ifndef LINK_USBDEV_PORT
#define LINK_USBDEV_PORT 0
#endif



#define LINK_USB_DESC_MANUFACTURER_SIZE 15
#define LINK_USB_DESC_PRODUCT_SIZE 10
#define LINK_USB_DESC_SERIAL_SIZE 16
#define LINK_USB_DESC_MANUFACTUER_STRING 'C','o','A','c','t','i','o','n','O','S',',',' ','I','n','c'
#define LINK_USB_DESC_PRODUCT_STRING 'C','o','A','c','t','i','o','n','O','S'
#define LINK_USB_DESC_SERIAL_STRING '0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'


#define LINK_REQD_CURRENT 500


#define BUF_SIZE 256
static char rd_buffer[BUF_SIZE];
static int rd_tail;
static int rd_head;


/*! \brief USB Link String Data
 * \details This structure defines the USB strings structure which includes
 * a string for the manufacturer, product, and serial number.
 */
struct HWPL_PACK link_usb_string_t {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wLANGID;
	usb_declare_string(LINK_USB_DESC_MANUFACTURER_SIZE) manufacturer;
	usb_declare_string(LINK_USB_DESC_PRODUCT_SIZE) product;
	usb_declare_string(LINK_USB_DESC_SERIAL_SIZE) serial;
};



/*! \details This variable stores the device descriptor.  It has a weak attribute and can be
 * overridden by using a user specific value
 * in the file \a devices.c.  This allows the user to change the USB vendor and product IDs.
 * All other values should be unchanged.
 *
 */
const usb_dev_desc_t link_dev_desc HWPL_WEAK = {
		.bLength = sizeof(usb_dev_desc_t),
		.bDescriptorType = USB_DEVICE_DESCRIPTOR_TYPE,
		.bcdUSB = 0x0200,
		.bDeviceClass = USB_DEVICE_CLASS_COMMUNICATIONS,
		.bDeviceSubClass = 0,
		.bDeviceProtocol = 0,
		.bMaxPacketSize = USB_MAX_PACKET0,
		.idVendor = LINK_USB_VID,
		.idProduct = LINK_USB_PID,
		.bcdDevice = BCDVERSION,
		.iManufacturer = 1,
		.iProduct = 2,
		.iSerialNumber = 3,
		.bNumConfigurations = 1
};

const link_cfg_desc_t link_cfg_desc HWPL_WEAK = {

		.cfg = {
				.bLength = sizeof(usb_cfg_desc_t),
				.bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE,
				.wTotalLength = sizeof(link_cfg_desc_t)-1, //exclude the zero terminator
				.bNumInterfaces = 0x02,
				.bConfigurationValue = 0x01,
				.iConfiguration = 0x03,
				.bmAttributes = USB_CONFIG_BUS_POWERED,
				.bMaxPower = USB_CONFIG_POWER_MA( LINK_REQD_CURRENT )
		},

		//This is the assocation interface -- still testing to see if it is necessary
		/*
		.ifasso = {
				.bLength = sizeof(usb_dev_interface_assocation_t),
				.bDescriptorType = 11,
				.bFirstInterface = 0,
				.bInterfaceCount = 2,
				.bFunctionClass = 2,
				.bFunctionSubClass = 2,
				.bFunctionProtocol = 0,
				.iFunction = 0x03,
		},
		 */

		.ifcontrol = {
				.bLength = sizeof(usb_interface_desc_t),
				.bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE,
				.bInterfaceNumber = 0x00,
				.bAlternateSetting = 0x00,
				.bNumEndpoints = 0x01,
				.bInterfaceClass = USB_INTERFACE_CLASS_COMMUNICATIONS,
				.bInterfaceSubClass = USB_INTERFACE_SUBCLASS_ACM,
				.bInterfaceProtocol = USB_INTERFACE_PROTOCOL_V25TER,
				.iInterface = 0x00
		},

		.acm = {
				.header.bLength = sizeof(usb_dev_cdc_header_t),
				.header.bDescriptorType = 0x24,
				.header.bDescriptorSubType = 0x00,
				.header.bcdCDC = 0x0110,
				.acm.bFunctionLength = sizeof(usb_dev_cdc_acm_t),
				.acm.bDescriptorType = 0x24,
				.acm.bDescriptorSubType = 0x02,
				.acm.bmCapabilities = 0x00,
				.union_descriptor.bFunctionLength = sizeof(usb_dev_cdc_uniondescriptor_t),
				.union_descriptor.bDescriptorType = 0x24,
				.union_descriptor.bDescriptorSubType = 0x06,
				.union_descriptor.bMasterInterface = 0x00,
				.union_descriptor.bSlaveInterface = 0x01,
				.call_management.bFunctionLength = sizeof(usb_dev_cdc_callmanagement_t),
				.call_management.bDescriptorType = 0x24,
				.call_management.bDescriptorSubType = 0x01,
				.call_management.bmCapabilities = 0x00,
				.call_management.bDataInterface = 0x01
		},

		.control = {
				.bLength= sizeof(usb_ep_desc_t),
				.bDescriptorType=USB_ENDPOINT_DESCRIPTOR_TYPE,
				.bEndpointAddress=LINK_INT_EP|0x80,
				.bmAttributes=USB_ENDPOINT_TYPE_INTERRUPT,
				.wMaxPacketSize=LINK_INTERRUPT_ENDPOINT_SIZE,
				.bInterval=1
		},

		.ifdata = {
				.bLength = sizeof(usb_interface_desc_t),
				.bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE,
				.bInterfaceNumber = 0x01,
				.bAlternateSetting = 0x00,
				.bNumEndpoints = 0x02,
				.bInterfaceClass = USB_INTERFACE_CLASS_COMMUNICATIONS_DATA,
				.bInterfaceSubClass = 0x00,
				.bInterfaceProtocol = 0x00,
				.iInterface = 0x00
		},

		.data_out = {
				.bLength= sizeof(usb_ep_desc_t),
				.bDescriptorType=USB_ENDPOINT_DESCRIPTOR_TYPE,
				.bEndpointAddress=LINK_BULK_EP,
				.bmAttributes=USB_ENDPOINT_TYPE_BULK,
				.wMaxPacketSize=LINK_BULK_ENDPOINT_SIZE,
				.bInterval=1
		},

		.data_in = {
				.bLength= sizeof(usb_ep_desc_t),
				.bDescriptorType=USB_ENDPOINT_DESCRIPTOR_TYPE,
				.bEndpointAddress=LINK_BULK_EP|0x80,
				.bmAttributes=USB_ENDPOINT_TYPE_BULK,
				.wMaxPacketSize=LINK_BULK_ENDPOINT_SIZE,
				.bInterval=1
		},



		.terminator = 0
};

/*! \details This variable stores the USB strings as the defaults listed below:
 * - Manufacturer: "CoActionOS, Inc"
 * - Product: "CoActionOS"
 * - Serial Number: "00000000"
 *
 * This variable has a weak attribute.  It can be overridden by using a user specific value
 * is the file \a devices.c.
 *
 */
const struct link_usb_string_t link_string_desc HWPL_WEAK = {
		.bLength = 4,
		.bDescriptorType = USB_STRING_DESCRIPTOR_TYPE,
		.wLANGID = 0x0409, //English
		.manufacturer = usb_assign_string(LINK_USB_DESC_MANUFACTURER_SIZE, LINK_USB_DESC_MANUFACTUER_STRING),
		.product = usb_assign_string(LINK_USB_DESC_PRODUCT_SIZE, LINK_USB_DESC_PRODUCT_STRING),
		.serial = usb_assign_string(LINK_USB_DESC_SERIAL_SIZE, 0) //dynamically load SN based on silicon
};


volatile int usbdev_stat;

link_phy_t link_phy_open(const char * name, int baudrate){
	pio_attr_t attr;
	usb_attr_t usb_attr;
	const int port = USBDEV_PORT;
#ifdef DEBUG_BOOTLOADER
	int tmp;
#endif

	//Deassert the Connect line and enable the output
	pio_clrmask(USBDEV_CONNECT_PORT, (1<<USBDEV_CONNECT_PIN));
	attr.mask = (1<<USBDEV_CONNECT_PIN);
	attr.mode = PIO_MODE_OUTPUT | PIO_MODE_DIRONLY;
	pio_setattr(USBDEV_CONNECT_PORT, &attr);

	dstr("OPEN USB\n");
	//open USB
	usb_open(port);

	dstr("USB OPEN\n");

	//set USB attributes
	usb_attr.pin_assign = USBDEV_PINASSIGN;
	usb_attr.mode = USB_ATTR_MODE_DEVICE;
	usb_attr.crystal_freq = MICROCOMPUTER_OSC_FREQ;
	if( usb_setattr(port, &usb_attr) < 0 ){
		dstr("FAILED TO SET USB ATTR\n");
	}

	dstr("USB ATTR SET\n");
	//initialize USB device
	usb_dev_init(&link_dev_desc, &link_cfg_desc, &link_string_desc);

	dstr("WAIT INIT USB\n");
	while( usb_isconnected(USBDEV_PORT) == 0 ){
#ifdef DEBUG_BOOTLOADER
		if( usbdev_stat != 0 ){
			tmp = usbdev_stat;
			usbdev_stat = 0;
			dstr("STAT: 0x"); dhex(tmp); dstr("\n");
		}
#endif
		; //wait for the USB to be connected and initialized
	}

	dstr("DONE WAIT\n");

	rd_tail = 0;
	rd_head = 0;

	return 0;
}

int link_phy_write(link_phy_t handle, const void * buf, int nbyte){
	int ret;
	ret = usb_write(USBDEV_PORT, USB_BULKIN, buf, nbyte);
	return ret;
}

static int read_buffer(char * dest, int nbyte){
	int i;
	for(i=0; i < nbyte; i++){
		if ( rd_head == rd_tail ){ //check for data in the fifo buffer
			//there is no more data in the buffer
			break;
		} else {
			dest[i] = rd_buffer[rd_tail];
			rd_tail++;
			if ( rd_tail == BUF_SIZE){
				rd_tail = 0;
			}
		}
	}
	return i; //number of bytes read
}

static int write_buffer(const char * src, int nbyte){
	int i;
	for(i=0; i < nbyte; i++){
		if( ((rd_head+1) == rd_tail) ||
				((rd_tail == 0) && (rd_head == (BUF_SIZE-1))) ){
			break; //no more room
		}

		rd_buffer[rd_head] = src[i];
		rd_head++;
		if ( rd_head == BUF_SIZE ){
			rd_head = 0;
		}
	}
	return i; //number of bytes written
}


int link_phy_read(link_phy_t handle, void * buf, int nbyte){
	int ret;
	int bytes_read;
	char buffer[ENDPOINT_SIZE];
	bytes_read = 0;
	ret = read_buffer(buf, nbyte);
	bytes_read += ret;

	while( bytes_read < nbyte ){
		//need more data to service request
		ret = usb_read(USBDEV_PORT, USB_BULKOUT, buffer, ENDPOINT_SIZE);
		write_buffer(buffer, ret);
		ret = read_buffer(buf + bytes_read, nbyte - bytes_read);
		bytes_read += ret;
	}
	return nbyte;
}

int link_phy_close(link_phy_t handle){
	return usb_close(USBDEV_PORT);
}

void link_phy_wait(int msec){
	_delay_us(msec*1000);
}


void link_phy_flush(link_phy_t handle){
	;
}



