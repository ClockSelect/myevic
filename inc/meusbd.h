#ifndef __MEUSBD_H__
#define __MEUSBD_H__

#include "myprintf.h"
#include "vcom.h"
#include "storage.h"

//=========================================================================
// Many parts are from Nuvoton sample code

/* Define the vendor id and product id */
#define USBD_VID        0x0416
#define USBD_PID        0x5020

/*!<Define HID Class Specific Request */
#define GET_REPORT          0x01
#define GET_IDLE            0x02
#define GET_PROTOCOL        0x03
#define SET_REPORT          0x09
#define SET_IDLE            0x0A
#define SET_PROTOCOL        0x0B

/*!<USB HID Interface Class protocol */
#define HID_NONE            0x00
#define HID_KEYBOARD        0x01
#define HID_MOUSE           0x02

/*!<USB HID Class Report Type */
#define HID_RPT_TYPE_INPUT      0x01
#define HID_RPT_TYPE_OUTPUT     0x02
#define HID_RPT_TYPE_FEATURE    0x03

#define HID_REPORT_SIZE	0x1B

/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    8
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
#define EP2_MAX_PKT_SIZE    64
#define EP3_MAX_PKT_SIZE    64
#define EP4_MAX_PKT_SIZE    8
#define EP5_MAX_PKT_SIZE    64
#define EP6_MAX_PKT_SIZE    64

#define SETUP_BUF_BASE  0
#define SETUP_BUF_LEN   8
#define EP0_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN     EP0_MAX_PKT_SIZE
#define EP1_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN     EP1_MAX_PKT_SIZE
#define EP2_BUF_BASE    (EP1_BUF_BASE + EP1_BUF_LEN)
#define EP2_BUF_LEN     EP2_MAX_PKT_SIZE
#define EP3_BUF_BASE    (EP2_BUF_BASE + EP2_BUF_LEN)
#define EP3_BUF_LEN     EP3_MAX_PKT_SIZE
#define EP4_BUF_BASE    (EP3_BUF_BASE + EP3_BUF_LEN)
#define EP4_BUF_LEN     EP4_MAX_PKT_SIZE
#define EP5_BUF_BASE    (EP4_BUF_BASE + EP4_BUF_LEN)
#define EP5_BUF_LEN     EP5_MAX_PKT_SIZE
#define EP6_BUF_BASE    (EP5_BUF_BASE + EP5_BUF_LEN)
#define EP6_BUF_LEN     EP6_MAX_PKT_SIZE

/* Define the EP number */
#define HID_INT_IN_EP_NUM   	0x01
#define HID_INT_OUT_EP_NUM  	0x02

/* Define Descriptor information */
#define HID_DEFAULT_INT_IN_INTERVAL     1
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */

//=========================================================================

extern const uint8_t usbdHIDReport[27];

//-------------------------------------------------------------------------

extern void InitUSB();

//=========================================================================

#endif /* __MEUSBD_H__ */
