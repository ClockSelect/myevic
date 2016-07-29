#include "myevic.h"
#include "dataflash.h"
#include "M451Series.h"


void usbdEP2Handler();
void usbdEP3Handler();


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

/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    8
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
#define EP2_MAX_PKT_SIZE    64
#define EP3_MAX_PKT_SIZE    64

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

/* Define the EP number */
#define INT_IN_EP_NUM       0x01
#define INT_OUT_EP_NUM      0x02

/* Define Descriptor information */
#define HID_DEFAULT_INT_IN_INTERVAL     1
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */

#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE+LEN_HID+LEN_ENDPOINT)


//=========================================================================

const uint8_t usbdHIDReport[] =
{
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x00, // USAGE (0)
	0xA1, 0x01, // COLLECTION (Application)
	0x15, 0x00, //     LOGICAL_MINIMUM (0)
	0x25, 0xFF, //     LOGICAL_MAXIMUM (255)
	0x19, 0x01, //     USAGE_MINIMUM (1)
	0x29, 0x08, //     USAGE_MAXIMUM (8)
	0x95, 0x40, //     REPORT_COUNT (8)
	0x75, 0x08, //     REPORT_SIZE (8)
	0x81, 0x02, //     INPUT (Data,Var,Abs)
	0x19, 0x01, //     USAGE_MINIMUM (1)
	0x29, 0x08, //     USAGE_MAXIMUM (8)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0xC0        // END_COLLECTION
};

const uint8_t usbdDevice[] =
{
	LEN_DEVICE,     /* bLength */
	DESC_DEVICE,    /* bDescriptorType */
	0x10, 0x01,     /* bcdUSB */
	0x00,           /* bDeviceClass */
	0x00,           /* bDeviceSubClass */
	0x00,           /* bDeviceProtocol */
	EP0_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
	/* idVendor */
	USBD_VID & 0x00FF,
	(USBD_VID & 0xFF00) >> 8,
	/* idProduct */
	USBD_PID & 0x00FF,
	(USBD_PID & 0xFF00) >> 8,
	0x00, 0x00,     /* bcdDevice */
	0x01,           /* iManufacture */
	0x02,           /* iProduct */
	0x03,           /* iSerialNumber */
	0x01            /* bNumConfigurations */
};

const uint8_t usbdDescSet[] =
{
	LEN_CONFIG,     /* bLength */
	DESC_CONFIG,    /* bDescriptorType */
	/* wTotalLength */
	(LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0x00FF,
	((LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2) & 0xFF00) >> 8,
	0x01,           /* bNumInterfaces */
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
	(INT_IN_EP_NUM | EP_INPUT), /* bEndpointAddress */
	EP_INT,         /* bmAttributes */
	/* wMaxPacketSize */
	EP2_MAX_PKT_SIZE & 0x00FF,
	(EP2_MAX_PKT_SIZE & 0xFF00) >> 8,
	HID_DEFAULT_INT_IN_INTERVAL,        /* bInterval */

	/* EP Descriptor: interrupt out. */
	LEN_ENDPOINT,   /* bLength */
	DESC_ENDPOINT,  /* bDescriptorType */
	(INT_OUT_EP_NUM | EP_OUTPUT),   /* bEndpointAddress */
	EP_INT,         /* bmAttributes */
	/* wMaxPacketSize */
	EP3_MAX_PKT_SIZE & 0x00FF,
	(EP3_MAX_PKT_SIZE & 0xFF00) >> 8,
	HID_DEFAULT_INT_IN_INTERVAL     /* bInterval */
};

/*!<USB Language String Descriptor */
const uint8_t gu8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
const uint8_t gu8VendorStringDesc[] =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

/*!<USB Product String Descriptor */
const uint8_t gu8ProductStringDesc[] =
{
    26,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'H', 0, 'I', 0, 'D', 0, ' ', 0, 'T', 0, 'r', 0, 'a', 0, 'n', 0, 's', 0, 'f', 0, 'e', 0, 'r', 0
};

/*!<USB Product Serial Number Descriptor */
const uint8_t gu8StringSerial[26] =
{
    26,             // bLength
    DESC_STRING,    // bDescriptorType
    'A', 0, '0', 0, '2', 0, '0', 0, '1', 0, '5', 0, '0', 0, '8', 0, '1', 0, '3', 0, '0', 0, '2', 0
};

const uint8_t *usbdStrings[] =
{
	gu8StringLang,
	gu8VendorStringDesc,
	gu8ProductStringDesc,
	gu8StringSerial
};

const uint8_t*	usbdHIDReportDesc[] =
{
	usbdHIDReport,
	0
};

const uint32_t	usbdHIDReportSize[] =
{
	sizeof( usbdHIDReport ),
	0
};

const uint32_t	usbdConfigHidDescIdx[] =
{
	(LEN_CONFIG+LEN_INTERFACE),
	0
};

const S_USBD_INFO_T usbdDescriptors =
{
	usbdDevice,
	usbdDescSet,
	usbdStrings,
	usbdHIDReportDesc,
	usbdHIDReportSize,
	usbdConfigHidDescIdx
};


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ void USBD_IRQHandler(void)
{
	uint32_t u32IntSts = USBD_GET_INT_FLAG();
	uint32_t u32State = USBD_GET_BUS_STATE();

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_FLDET)
	{
		// Floating detect
		USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

		if(USBD_IS_ATTACHED())
		{
			/* USB Plug In */
			USBD_ENABLE_USB();
		}
		else
		{
			/* USB Un-plug */
			USBD_DISABLE_USB();
		}
	}

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_BUS)
	{
		/* Clear event flag */
		USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

		if(u32State & USBD_STATE_USBRST)
		{
			/* Bus reset */
			USBD_ENABLE_USB();
			USBD_SwReset();
		}
		if(u32State & USBD_STATE_SUSPEND)
		{
			/* Enable USB but disable PHY */
			USBD_DISABLE_PHY();
		}
		if(u32State & USBD_STATE_RESUME)
		{
			/* Enable USB and enable PHY */
			USBD_ENABLE_USB();
		}
	}

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_USB)
	{
		// USB event
		if(u32IntSts & USBD_INTSTS_SETUP)
		{
			// Setup packet
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

			/* Clear the data IN/OUT ready flag of control end-points */
			USBD_STOP_TRANSACTION(EP0);
			USBD_STOP_TRANSACTION(EP1);

			USBD_ProcessSetupPacket();
		}

		// EP events
		if(u32IntSts & USBD_INTSTS_EP0)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
			// control IN
			USBD_CtrlIn();
		}

		if(u32IntSts & USBD_INTSTS_EP1)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

			// control OUT
			USBD_CtrlOut();
		}

		if(u32IntSts & USBD_INTSTS_EP2)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);
			// Interrupt IN
			usbdEP2Handler();
		}

		if(u32IntSts & USBD_INTSTS_EP3)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);
			// Interrupt OUT
			usbdEP3Handler();
		}

		if(u32IntSts & USBD_INTSTS_EP4)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
		}

		if(u32IntSts & USBD_INTSTS_EP5)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
		}

		if(u32IntSts & USBD_INTSTS_EP6)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP6);
		}

		if(u32IntSts & USBD_INTSTS_EP7)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP7);
		}
	}
	/* clear unknown event */
	USBD_CLR_INT_FLAG(u32IntSts);
}


//=========================================================================
//----- (00002564) --------------------------------------------------------
__myevic__ void hidClassRequest()
{
	uint8_t buf[8];

	USBD_GetSetupPacket(buf);

	if(buf[0] & 0x80)    /* request data transfer direction */
	{
		// Device to host
		switch(buf[1])
		{
		case GET_REPORT:
//             {
//                 break;
//             }
		case GET_IDLE:
//             {
//                 break;
//             }
		case GET_PROTOCOL:
//            {
//                break;
//            }
		default:
			{
				/* Setup error, stall the device */
				USBD_SetStall(0);
				break;
			}
		}
	}
	else
	{
		// Host to device
		switch(buf[1])
		{
		case SET_REPORT:
			{
				if(buf[3] == 3)
				{
					/* Request Type = Feature */
					USBD_SET_DATA1(EP1);
					USBD_SET_PAYLOAD_LEN(EP1, 0);
				}
				break;
			}
		case SET_IDLE:
			{
				/* Status stage */
				USBD_SET_DATA1(EP0);
				USBD_SET_PAYLOAD_LEN(EP0, 0);
				break;
			}
		case SET_PROTOCOL:
//             {
//                 break;
//             }
		default:
			{
				// Stall
				/* Setup error, stall the device */
				USBD_SetStall(0);
				break;
			}
		}
	}
}


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ void hidInit()
{
	/* Init setup packet buffer */
	/* Buffer range for setup packet -> [0 ~ 0x7] */
	USBD->STBUFSEG = SETUP_BUF_BASE;

	/*****************************************************/
	/* EP0 ==> control IN endpoint, address 0 */
	USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
	/* Buffer range for EP0 */
	USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

	/* EP1 ==> control OUT endpoint, address 0 */
	USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
	/* Buffer range for EP1 */
	USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

	/*****************************************************/
	/* EP2 ==> Interrupt IN endpoint, address 1 */
	USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | INT_IN_EP_NUM);
	/* Buffer range for EP2 */
	USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

	/* EP3 ==> Interrupt OUT endpoint, address 2 */
	USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | INT_OUT_EP_NUM);
	/* Buffer range for EP3 */
	USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
	/* trigger to receive OUT data */
	USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
}


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ void InitUSB()
{
	USBD_Open( &usbdDescriptors, hidClassRequest+1, 0 );
	hidInit();
	USBD_Start();
	NVIC_EnableIRQ( USBD_IRQn );
}


//=========================================================================
// HID Requests definitions

#define HID_CMD_SIGNATURE   0x43444948

/* HID Commands */
#define HID_CMD_NONE		0x00
#define HID_CMD_GETINFO		0x35
#define HID_CMD_LDUPDATE	0x3C
#define HID_CMD_SETPARAMS	0x53
#define HID_CMD_RESETPARAMS	0x7C
#define HID_CMD_SETLOGO		0xA5
#define HID_CMD_RESET		0xB4
#define HID_CMD_FMCREAD		0xC0

#define PAGE_SIZE			2048

typedef struct __attribute__((packed))
{
    uint8_t u8Cmd;
    uint8_t u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
} CMD_T;

/*
extern CMD_T hidCmd;

extern uint8_t hidData[PAGE_SIZE];
extern uint32_t hidDFData[PAGE_SIZE/4];
extern uint32_t hidDataIndex;
*/

CMD_T hidCmd;

uint8_t hidData[PAGE_SIZE];
uint32_t hidDFData[PAGE_SIZE/4];
uint32_t hidDataIndex;

//=============================================================================
//----- (00001204) --------------------------------------------------------
// R0 = Sum of the R1 bytes at R0
__myevic__ uint32_t Checksum( const uint8_t *p, const uint32_t l )
{
	uint32_t sum = 0;
	for ( uint32_t i = 0 ; i < l ; ++i ) sum += p[i];
	return sum;
}


//----- (00002C60) --------------------------------------------------------
__myevic__ void usbdEP2Handler()
{
	uint8_t cmd = hidCmd.u8Cmd;

	switch ( cmd )
	{
		case HID_CMD_GETINFO:
		{
			if ( hidDataIndex )
			{
				USBD_MemCopy(
					(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
					&hidData[hidCmd.u32Signature - hidDataIndex],
					EP2_MAX_PKT_SIZE
				);
				USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
				hidDataIndex -= EP2_MAX_PKT_SIZE;
			}
			else
			{
				cmd = 0;
			}
			break;
		}

		case HID_CMD_FMCREAD:
		{
			if ( !hidDataIndex )
			{
				uint32_t u32Addr = hidCmd.u32Arg1 + hidCmd.u32Signature;
				int32_t  u32Size = hidCmd.u32Arg2 - hidCmd.u32Signature;
				if ( u32Size > 0 )
				{
					if ( u32Size > PAGE_SIZE ) u32Size = PAGE_SIZE;

					SYS_UnlockReg();
					FMC_ENABLE_ISP();

					for ( int i = 0 ; i < u32Size ; i += 4 )
					{
						uint32_t data = FMC_Read( u32Addr + i );
						MemCpy( &hidData[ PAGE_SIZE - u32Size + i ], &data, 4 );
					}

					FMC_DISABLE_ISP();
					SYS_LockReg();

					hidDataIndex = u32Size;
					hidCmd.u32Signature += u32Size;
				}
			}
			if ( hidDataIndex )
			{
				USBD_MemCopy(
					(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
					&hidData[ PAGE_SIZE - hidDataIndex ],
					EP2_MAX_PKT_SIZE
				);
				USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
				hidDataIndex -= EP2_MAX_PKT_SIZE;
			}
			else
			{
				cmd = 0;
			}
			break;
		}

		default:
			cmd = 0;
			break;
	}

	hidCmd.u8Cmd = cmd;
}


//----- (00002C00) --------------------------------------------------------
__myevic__ uint32_t hidResetParamCmd( CMD_T *pCmd )
{
	PutTextf("Reset param\n");
	ResetDataFlash();
	UpdateDataFlash();
	Flags64 |= 0x20000u;
	pCmd->u8Cmd = 0;
	return 0;
}


//----- (00002C38) --------------------------------------------------------
__myevic__ uint32_t hidResetSysCmd( CMD_T *pCmd )
{
	PutTextf("Reset system command\n");
	SYS_UnlockReg();
	SYS_ResetChip();
	while ( 1 )
		;
}


//----- (000025B8) --------------------------------------------------------
__myevic__ uint32_t hidGetInfoCmd( CMD_T *pCmd )
{
	uint32_t u32StartAddr;
	uint32_t u32ParamLen;

	u32StartAddr = pCmd->u32Arg1;
	u32ParamLen = pCmd->u32Arg2;

	PutTextf( "Get Info command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );

	if ( u32ParamLen )
	{
		dfData = Checksum( (uint8_t *)&dfCRC, PAGE_SIZE - 4 );

		if ( u32StartAddr + u32ParamLen > PAGE_SIZE )
		{
			u32ParamLen = PAGE_SIZE - u32StartAddr;
		}

		MemCpy( hidData, ((uint8_t *)&dfData) + u32StartAddr, u32ParamLen );

		hidDataIndex = u32ParamLen;
		pCmd->u32Signature = u32ParamLen;

		USBD_MemCopy(
			(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
			hidData,
			EP2_MAX_PKT_SIZE
		);
		USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
		hidDataIndex -= EP2_MAX_PKT_SIZE;
	}

	return 0;
}


//----- (00002678) --------------------------------------------------------
__myevic__ uint32_t hidBootLogoCmd( CMD_T *pCmd )
{
	PutTextf( "Set Boot Logo command - Start page: %d\t\tLen: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	MemClear( hidData, PAGE_SIZE );
	hidDataIndex = 0;
	pCmd->u32Signature = 0;
	return 0;
}


//----- (000026D8) --------------------------------------------------------
__myevic__ uint32_t hidSetParamCmd( CMD_T *pCmd )
{
	PutTextf( "Set Param command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	hidDataIndex = 0;
	return 0;
}


//----- (0000272C) --------------------------------------------------------
__myevic__ uint32_t hidLDUpdateCmd( CMD_T *pCmd )
{
	PutTextf( "Update LDROM command - Start page: %d\t\tLen: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	MemClear( hidData, PAGE_SIZE );
	hidDataIndex = 0;
	pCmd->u32Signature = 0;
	return 0;
}


//----- (000025B8) --------------------------------------------------------
__myevic__ uint32_t hidFMCReadCmd( CMD_T *pCmd )
{
	uint32_t u32StartAddr;
	uint32_t u32ParamLen;

	u32StartAddr = pCmd->u32Arg1;
	u32ParamLen  = pCmd->u32Arg2;

	PutTextf( "FMC Read command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	
	if ( ! u32ParamLen % EP2_MAX_PKT_SIZE )
	{
		return -1;
	}
	
	if ( u32ParamLen )
	{
		if ( u32ParamLen > PAGE_SIZE )
		{
			u32ParamLen = PAGE_SIZE;
		}

		SYS_UnlockReg();
		FMC_ENABLE_ISP();

		for ( uint32_t i = 0 ; i < u32ParamLen ; i += 4 )
		{
			uint32_t data;
			data = FMC_Read( u32StartAddr + i );
			MemCpy( &hidData[i], &data, 4 );
		}

		FMC_DISABLE_ISP();
		SYS_LockReg();

		hidDataIndex = u32ParamLen;
		pCmd->u32Signature = u32ParamLen;

		USBD_MemCopy(
			(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
			hidData,
			EP2_MAX_PKT_SIZE
		);
		USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
		hidDataIndex -= EP2_MAX_PKT_SIZE;
	}

	return 0;
}


//-------------------------------------------------------------------------
int32_t hidProcessCommand( uint8_t *pu8Buffer, uint32_t u32BufferLen )
{
    uint32_t u32sum;

    USBD_MemCopy( (uint8_t*)&hidCmd, pu8Buffer, u32BufferLen );

    /* Check size */
    if( ( hidCmd.u8Size > sizeof(hidCmd) ) || ( hidCmd.u8Size > u32BufferLen ) )
        return -1;

    /* Check signature */
    if( hidCmd.u32Signature != HID_CMD_SIGNATURE )
        return -1;

    /* Calculate checksum & check it*/
    u32sum = Checksum((uint8_t *)&hidCmd, hidCmd.u8Size);
    if( u32sum != hidCmd.u32Checksum )
        return -1;

    switch( hidCmd.u8Cmd )
    {
		case HID_CMD_GETINFO:
		{
			hidGetInfoCmd( &hidCmd );
			break;
		}
		case HID_CMD_LDUPDATE:
		{
			hidLDUpdateCmd( &hidCmd );
			break;
		}
		case HID_CMD_SETPARAMS:
		{
			hidSetParamCmd( &hidCmd );
			break;
		}
		case HID_CMD_RESETPARAMS:
		{
			hidResetParamCmd( &hidCmd );
			break;
		}
		case HID_CMD_SETLOGO:
		{
			hidBootLogoCmd( &hidCmd );
			break;
		}
		case HID_CMD_RESET :
		{
			hidResetSysCmd( &hidCmd );
			break;
		}
		case HID_CMD_FMCREAD :
		{
			hidFMCReadCmd( &hidCmd );
			break;
		}
		default:
		{
            return -1;
		}
    }

    return 0;
}


//----- (00002788) --------------------------------------------------------
__myevic__ void hidGetOutReport( uint8_t *pu8Buffer, uint32_t u32BufferLen )
{
	uint8_t u8Cmd;
	uint8_t *hidDataPtr;
	uint32_t u32StartAddr;
	uint32_t u32DataSize;
	uint32_t u32ByteCount;
	uint32_t u32Page;

	uint32_t sz;
	uint32_t veo;

	u8Cmd			= hidCmd.u8Cmd;
	u32StartAddr	= hidCmd.u32Arg1;
	u32DataSize		= hidCmd.u32Arg2;
	u32ByteCount	= hidCmd.u32Signature;
	hidDataPtr		= &hidData[hidDataIndex];

	switch ( hidCmd.u8Cmd )
	{
		case HID_CMD_SETPARAMS:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex >= u32DataSize )
			{
				u8Cmd = 0;

				if ( u32StartAddr + u32DataSize > PAGE_SIZE )
				{
					sz = PAGE_SIZE - u32StartAddr;
				}
				else
				{
					sz = u32DataSize;
				}

				MemCpy( (uint8_t*)(&hidDFData) + u32StartAddr, hidData, sz );

				PutTextf( "Set Sys Param complete.\n" );

				if ( Checksum( (uint8_t*)&hidDFData[1], PAGE_SIZE - 4 ) == hidDFData[0] )
				{
					#define DFOFFSET(p) (int)(((uint8_t*)&(p))-(uint8_t*)&dfData)
					PutTextf( "\tCompany ID ............................ [0x%08x]\n", hidDFData[DFOFFSET(fmcCID)>>2] );
					PutTextf( "\tDevice ID ............................. [0x%08x]\n", hidDFData[DFOFFSET(fmcDID)>>2] );
					PutTextf( "\tProduct ID ............................ [0x%08x]\n", hidDFData[DFOFFSET(fmcPID)>>2] );
					PutTextf( "\tu8UpdateAPRom ......................... [0x%08x]\n", (((char*)hidDFData)+DFOFFSET(dfBootFlag)) );
					#undef DFOFFSET

					MemCpy( &dfCRC, &hidDFData[1], 0x100 );

					DFCheckValuesValidity();
					UpdateDataFlash();

					Flags64 |= 0x20000u;
				}
				else
				{
					PutTextf( "Sys Param Recive fail.\n" );
				}

				hidDataIndex = 0;
			}

			break;
		}

		case HID_CMD_SETLOGO:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex < PAGE_SIZE && hidDataIndex + u32ByteCount < u32DataSize )
			{
				break;
			}

			u32Page = u32StartAddr + u32ByteCount;

			PutTextf( "Writing page %d\n", u32Page );

			SYS_UnlockReg();
			FMC_ENABLE_ISP();

			if ( FMCEraseWrite800( u32Page, (uint32_t*)hidData ) )
			{
				PutTextf( "Data Flash Erase error!\n" );
			}

			veo = FMCVerif800( u32Page, (uint32_t*)hidData );
			if ( veo )
			{
				PutTextf( "Data Flash Verify error! 0x%x\n", 4 * veo - 4 );
			}

			MemClear( hidData, PAGE_SIZE );
			u32ByteCount += hidDataIndex;

			PutTextf( "g_u32BytesInPageBuf %d, u32LenCnt 0x%x\n", hidDataIndex, u32ByteCount );

			FMC_DISABLE_ISP();
			SYS_LockReg();

			if ( u32ByteCount < u32DataSize )
			{
				hidDataIndex = 0;
			}
			else
			{
				u8Cmd = 0;
				PutTextf( "set boot logo command complete.\n" );
			}

			break;
		}

		case HID_CMD_LDUPDATE:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex < PAGE_SIZE && hidDataIndex + u32ByteCount < u32DataSize )
			{
				break;
			}

			u32Page = u32StartAddr + u32ByteCount;
			PutTextf( "Writing page 0x%08X\n", u32Page );

			SYS_UnlockReg();
			FMC_ENABLE_ISP();
			FMC_EnableLDUpdate();

			if ( FMCEraseWrite800( u32Page, (uint32_t*)hidData ) )
			{
				PutTextf( "Data Flash Erase error!\n" );
			}

			veo = FMCVerif800( u32Page, (uint32_t*)hidData );
			if ( veo )
			{
				PutTextf( "Data Flash Verify error! 0x%x\n", 4 * veo - 4 );
			}

			MemClear( hidData, PAGE_SIZE );
			u32ByteCount += hidDataIndex;
			hidDataIndex = 0;

			PutTextf( "g_u32BytesInPageBuf %d, u32LenCnt 0x%x\n", hidDataIndex, u32ByteCount );

			FMC_DisableLDUpdate();
			FMC_DISABLE_ISP();
			SYS_LockReg();

			if ( u32ByteCount >= u32DataSize )
			{
				u8Cmd = 0;
				PutTextf( "Update LDROM command complete.\n" );
			}
			
			break;
		}

		default:
		{
			if ( hidProcessCommand( pu8Buffer, u32BufferLen ) )
			{
				PutTextf( "Unknown HID command!\n" );
			}
			return;
		}
	}

	hidCmd.u8Cmd = u8Cmd;
	hidCmd.u32Signature = u32ByteCount;
}


//-------------------------------------------------------------------------
__myevic__ void usbdEP3Handler()
{
    uint8_t *ptr;
    /* Interrupt OUT */
    ptr = (uint8_t *)( USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR( EP3 ) );
    hidGetOutReport( ptr, USBD_GET_PAYLOAD_LEN( EP3 ) );
    USBD_SET_PAYLOAD_LEN( EP3, EP3_MAX_PKT_SIZE );
}


