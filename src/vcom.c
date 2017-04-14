#include "vcom.h"

//=========================================================================
// VCOM
//-------------------------------------------------------------------------

#define DESC_LEN_WITH_VCOM \
	(LEN_CONFIG + LEN_INTERFACE * 3 + LEN_HID + LEN_ENDPOINT * 5 + 8 + 19)

const uint8_t usbdVCOMConfigDesc[] =
{
	LEN_CONFIG,     /* bLength */
	DESC_CONFIG,    /* bDescriptorType */
	/* wTotalLength */
	DESC_LEN_WITH_VCOM & 0x00FF,
	(DESC_LEN_WITH_VCOM & 0xFF00) >> 8,
	0x03,           /* bNumInterfaces */
	0x01,           /* bConfigurationValue */
	0x00,           /* iConfiguration */
	0x80 | (USBD_SELF_POWERED << 6) | (USBD_REMOTE_WAKEUP << 5),/* bmAttributes */
	USBD_MAX_POWER,         /* MaxPower */

/* I/F descr: HID */
	LEN_INTERFACE,  /* bLength */
	DESC_INTERFACE, /* bDescriptorType */
	0x00,           /* bInterfaceNumber */
	0x00,           /* bAlternateSetting */
	0x02,           /* bNumEndpoints */
	0x03,           /* bInterfaceClass */
	0x00,           /* bInterfaceSubClass */
	0x00,           /* bInterfaceProtocol */
	0x00,           /* iInterface */

	/* HID Descriptor */
	LEN_HID,        /* Size of this descriptor in UINT8s. */
	DESC_HID,       /* HID descriptor type. */
	0x10, 0x01,     /* HID Class Spec. release number. */
	0x00,           /* H/W target country. */
	0x01,           /* Number of HID class descriptors to follow. */
	DESC_HID_RPT,   /* Descriptor type. */
	/* Total length of report descriptor. */
	sizeof(usbdHIDReport) & 0x00FF,
	(sizeof(usbdHIDReport) & 0xFF00) >> 8,

	/* EP Descriptor: interrupt in. */
	LEN_ENDPOINT,   /* bLength */
	DESC_ENDPOINT,  /* bDescriptorType */
	(HID_INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
	EP_INT,         /* bmAttributes */
	/* wMaxPacketSize */
	EP2_MAX_PKT_SIZE & 0x00FF,
	(EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
	HID_DEFAULT_INT_IN_INTERVAL,        /* bInterval */

	/* EP Descriptor: interrupt out. */
	LEN_ENDPOINT,   /* bLength */
	DESC_ENDPOINT,  /* bDescriptorType */
	(HID_INT_OUT_EP_NUM | EP_OUTPUT),   /* bEndpointAddress */
	EP_INT,         /* bmAttributes */
	/* wMaxPacketSize */
	EP3_MAX_PKT_SIZE & 0x00FF,
	(EP3_MAX_PKT_SIZE & 0xFF00) >> 8,
	HID_DEFAULT_INT_IN_INTERVAL,    /* bInterval */

/* I/F descr: VCOM */
    // IAD
    0x08,               // bLength: Interface Descriptor size
    0x0B,               // bDescriptorType: IAD
    0x01,               // bFirstInterface
    0x02,               // bInterfaceCount
    0x02,               // bFunctionClass: CDC
    0x02,               // bFunctionSubClass: Abstract Control Model
    0x01,               // bFunctionProtocol: AT Commands [V250] etc.
    0x00, /*0x02 */     // iFunction

    /* INTERFACE descriptor */
    LEN_INTERFACE,  /* bLength              */
    DESC_INTERFACE, /* bDescriptorType      */
    0x01,           /* bInterfaceNumber     */
    0x00,           /* bAlternateSetting    */
    0x01,           /* bNumEndpoints        */
    0x02,           /* bInterfaceClass      */
    0x02,           /* bInterfaceSubClass   */
    0x01,           /* bInterfaceProtocol   */
    0x00,           /* iInterface           */

    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x00,           /* Header functional descriptor subtype */
    0x10, 0x01,     /* Communication device compliant to the communication spec. ver. 1.10 */
    
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x01,           /* Call management functional descriptor */
    0x00,           /* BIT0: Whether device handle call management itself. */
                    /* BIT1: Whether device can send/receive call management information over a Data Class Interface 0 */
    0x02,           /* Interface number of data class interface optionally used for call management */

    /* Communication Class Specified INTERFACE descriptor */
    0x04,           /* Size of the descriptor, in bytes */
    0x24,           /* CS_INTERFACE descriptor type */
    0x02,           /* Abstract control management functional descriptor subtype */
    0x00,           /* bmCapabilities       */
    
    /* Communication Class Specified INTERFACE descriptor */
    0x05,           /* bLength              */
    0x24,           /* bDescriptorType: CS_INTERFACE descriptor type */
    0x06,           /* bDescriptorSubType   */
    0x01,           /* bMasterInterface     */
    0x02,           /* bSlaveInterface0     */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | VCOM_INT_IN_EP_NUM),/* bEndpointAddress */
    EP_INT,                         /* bmAttributes     */
    EP4_MAX_PKT_SIZE,               /* wMaxPacketSize   */
	(EP4_MAX_PKT_SIZE & 0xFF00) >> 8,
    0x01,                           /* bInterval        */

    /* INTERFACE descriptor */
    LEN_INTERFACE,                  /* bLength              */
    DESC_INTERFACE,                 /* bDescriptorType      */
    0x02,                           /* bInterfaceNumber     */
    0x00,                           /* bAlternateSetting    */
    0x02,                           /* bNumEndpoints        */
    0x0A,                           /* bInterfaceClass      */
    0x00,                           /* bInterfaceSubClass   */
    0x00,                           /* bInterfaceProtocol   */
    0x00,                           /* iInterface           */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_INPUT | VCOM_BULK_IN_EP_NUM),    /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    EP5_MAX_PKT_SIZE,               /* wMaxPacketSize   */
	(EP5_MAX_PKT_SIZE & 0xFF00) >> 8,
    0x00,                           /* bInterval        */

    /* ENDPOINT descriptor */
    LEN_ENDPOINT,                   /* bLength          */
    DESC_ENDPOINT,                  /* bDescriptorType  */
    (EP_OUTPUT | VCOM_BULK_OUT_EP_NUM),  /* bEndpointAddress */
    EP_BULK,                        /* bmAttributes     */
    EP6_MAX_PKT_SIZE,               /* wMaxPacketSize   */
	(EP6_MAX_PKT_SIZE & 0xFF00) >> 8,
    0x00,                           /* bInterval        */
};


//-------------------------------------------------------------------------

typedef struct {
    uint32_t  u32DTERate;     /* Baud rate    */
    uint8_t   u8CharFormat;   /* stop bit     */
    uint8_t   u8ParityType;   /* parity       */
    uint8_t   u8DataBits;     /* data bits    */
} STR_VCOM_LINE_CODING;

//-------------------------------------------------------------------------

static STR_VCOM_LINE_CODING gLineCoding = {115200, 0, 0, 8};
volatile uint16_t gCtrlSignal = 0;


//=========================================================================
// VCOM
//-------------------------------------------------------------------------
#define RXBUFSIZE	512 /* RX buffer size */

volatile uint8_t comRbuf[RXBUFSIZE];
volatile uint16_t comRbytes = 0;
volatile uint16_t comRhead = 0;
volatile uint16_t comRtail = 0;

uint8_t gTxBuf[64] = {0};
volatile uint8_t *gpu8RxBuf = 0;
volatile uint32_t gu32RxSize = 0;
volatile uint32_t gu32TxSize = 0;

volatile uint8_t VCOM_TxReady = 1;

//-------------------------------------------------------------------------
__myevic__ void VCOM_Poll()
{
    int32_t i, i32Len;

	if ( VCOM_TxReady )
	{
		/* Check whether we have new COM Rx data to send to USB or not */
		if ( comRbytes )
		{
			i32Len = comRbytes;
			if( i32Len > EP5_MAX_PKT_SIZE )
				i32Len = EP5_MAX_PKT_SIZE;

			for ( i = 0; i < i32Len; i++ )
			{
				gTxBuf[i] = comRbuf[comRhead++];
				if ( comRhead >= RXBUFSIZE )
					comRhead = 0;
			}

			__set_PRIMASK(1);
			comRbytes -= i32Len;
			__set_PRIMASK(0);

			VCOM_TxReady = 0;
			USBD_MemCopy( (uint8_t*)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP5)),
						  (uint8_t *)gTxBuf, i32Len );
			USBD_SET_PAYLOAD_LEN( EP5, i32Len );
		}
		else
		{
			/* Prepare a zero packet if previous packet size is EP2_MAX_PKT_SIZE and 
			   no more data to send at this moment to note Host the transfer has been done */
			i32Len = USBD_GET_PAYLOAD_LEN( EP5 );
			if( i32Len == EP5_MAX_PKT_SIZE )
			{
				USBD_SET_PAYLOAD_LEN( EP5, 0 );
			}
		}
	}
}

//-------------------------------------------------------------------------
__myevic__ void VCOM_EP5Handler()
{
    /* Bulk IN */
	VCOM_TxReady = 1;
}

__myevic__ void VCOM_EP6Handler()
{
    /* Bulk OUT */
//	int32_t i, i32Len;
//
//	i32Len = USBD_GET_PAYLOAD_LEN( EP6 );
//	if ( i32Len )
//	{
//		USBD_MemCopy(	(uint8_t *)gRxBuf, (uint8_t*)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP6)),
//						i32Len );
//	}

    USBD_SET_PAYLOAD_LEN( EP6, EP6_MAX_PKT_SIZE );
}


//-------------------------------------------------------------------------
__myevic__ void VCOM_Cout( uint8_t c )
{
	__set_PRIMASK(1);

	comRbuf[comRtail++] = c;

	if ( comRtail >= RXBUFSIZE )
		comRtail = 0;

	++comRbytes;

	__set_PRIMASK(0);
}

__myevic__ char VCOM_Putc( char c, FILE *out )
{
	if ( !USBD_IS_ATTACHED()
	||	 !( gCtrlSignal & VCOM_LINESTATE_MASK_DTR ))
	{
		// Don't send if no one is listening
		return c;
	}

	VCOM_Cout( (uint8_t)c );

	if ( c == '\n' )
	{
		VCOM_Cout( (uint8_t)'\r' );
	}

	return c;
}


//-------------------------------------------------------------------------
__myevic__ void VCOM_ClassRequest( uint8_t *token )
{
	if( token[0] & 0x80 )    /* request data transfer direction */
	{
		// Device to host
		switch ( token[1] )
		{
			case CDC_GET_LINE_CODE:
			{
				USBD_MemCopy(	(uint8_t*)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR( EP0 )),
								(uint8_t*)&gLineCoding, 7 );

				/* Data stage */
				USBD_SET_DATA1( EP0 );
				USBD_SET_PAYLOAD_LEN( EP0, 7 );
				/* Status stage */
				USBD_PrepareCtrlOut( 0, 0 );
				break;
			}

			default:
			{
				myprintf( "CLASS GET RQ 0x%02X IF %d\n", token[1], token[4] );
				/* Setup error, stall the device */
				USBD_SetStall( EP0 );
				USBD_SetStall( EP1 );
				break;
			}
		}
	}
	else
	{
		// Host to device
		switch ( token[1] )
		{
			case CDC_SET_CONTROL_LINE_STATE:
			{
				if ( token[4] == VCOM_INTERFACE )
				{ /* VCOM-1 */
					gCtrlSignal = token[3];
					gCtrlSignal = ( gCtrlSignal << 8 ) | token[2];
					myprintf( "RTS=%d  DTR=%d\n", (gCtrlSignal >> 1) & 1, gCtrlSignal & 1 );
					if ( !(gCtrlSignal & 1) )
					{
						gFlags.monitoring = 0;
					}
				}

				/* Status stage */
				USBD_SET_DATA1( EP0 );
				USBD_SET_PAYLOAD_LEN( EP0, 0 );
				break;
			}

			case CDC_SET_LINE_CODE:
			{
				//g_usbd_UsbConfig = 0100;
				USBD_PrepareCtrlOut( (uint8_t *)&gLineCoding, 7 );

				/* Status stage */
				USBD_SET_DATA1( EP0 );
				USBD_SET_PAYLOAD_LEN( EP0, 0 );
				break;
			}

			default:
			{
				myprintf( "CLASS SET RQ 0x%02X IF %d\n", token[1], token[4] );
				// Stall
				/* Setup error, stall the device */
				USBD_SetStall( EP0 );
				USBD_SetStall( EP1 );
				break;
			}
		}
	}
}
