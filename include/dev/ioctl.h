/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#ifndef _DEV_IOCTL_H_
#define _DEV_IOCTL_H_

#define _IOCTLPARM_MASK 0x3FFF
#define _IOCTLTYPE_MASK 0xFFFF
#define _IOCTLIDENT_MASK 0xFF
#define _IOCTLNUM_MASK 0xFF
#define _IOCTL_IN 0x40000000
#define _IOCTL_OUT 0x80000000
#define _IOCTL_INOUT (_IOCTL_IN | _IOCTL_OUT)
#define _IOCTL_VOID 0

#define _IOCTL(x,y)	((x << 8) | (y) | ((0 & _IOCTLPARM_MASK) << 16) | _IOCTL_VOID)
#define _IOCTLR(x,y,t)	((x << 8) | (y) | ((sizeof(t) & _IOCTLPARM_MASK) << 16) | _IOCTL_OUT)
#define _IOCTLW(x,y,t)	((x << 8) | (y) | ((sizeof(t) & _IOCTLPARM_MASK) << 16) | _IOCTL_IN)
#define _IOCTLRW(x,y,t)	((x << 8) | (y) | ((sizeof(t) & _IOCTLPARM_MASK) << 16) | _IOCTL_INOUT)


#define _IOCTL_NUM(i) ((i) & _IOCTLNUM_MASK)
#define _IOCTL_IDENT(i) (((i) >> 8) & _IOCTLIDENT_MASK)
#define _IOCTL_SIZE(i) (((i) >> 16) & _IOCTLPARM_MASK)
#define _IOCTL_IOCTLR(i) ((i) & _IOCTL_OUT)
#define _IOCTL_IOCTLRW(i) ((i) & _IOCTL_INOUT)
#define _IOCTL_IOCTLW(i) ((i) & _IOCTL_IN)

#endif /* _DEV_IOCTL_H_ */
