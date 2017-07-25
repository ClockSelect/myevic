#include "myevic.h"
#include "myrtc.h"
#include "dataflash.h"
#include "display.h"
#include "events.h"
#include "timers.h"
#include "atomizer.h"
#include "battery.h"
#include "meusbd.h"
#include "dtmacros.h"
#include "screens.h"

void usbdEP2Handler();
void usbdEP3Handler();
void usbdEP5Handler();
void usbdEP6Handler();


//=========================================================================

const uint8_t usbdHIDReport[27] =
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
	0xEF,           /* bDeviceClass */
	0x02,           /* bDeviceSubClass */
	0x01,           /* bDeviceProtocol */
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

#define DESC_LEN_WITHOUT_VCOM \
	(LEN_CONFIG + LEN_INTERFACE + LEN_HID + LEN_ENDPOINT * 2)

const uint8_t usbdConfigDesc[] =
{
	LEN_CONFIG,     /* bLength */
	DESC_CONFIG,    /* bDescriptorType */
	/* wTotalLength */
	DESC_LEN_WITHOUT_VCOM & 0x00FF,
	(DESC_LEN_WITHOUT_VCOM & 0xFF00) >> 8,
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
	usbdConfigDesc,
	usbdStrings,
	usbdHIDReportDesc,
	usbdHIDReportSize,
	usbdConfigHidDescIdx
};

const S_USBD_INFO_T usbdVCOMDescriptors =
{
	usbdDevice,
	usbdVCOMConfigDesc,
	usbdStrings,
	usbdHIDReportDesc,
	usbdHIDReportSize,
	usbdConfigHidDescIdx
};

const S_USBD_INFO_T usbdMSCDescriptors =
{
	usbdDevice,
	usbdMSCConfigDesc,
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

		gCtrlSignal = 0;
	}

	//------------------------------------------------------------------
	if(u32IntSts & USBD_INTSTS_WAKEUP)
	{
		// Wake-Up
		USBD_CLR_INT_FLAG(USBD_INTSTS_WAKEUP);
		gFlags.wake_up = 1;
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

		gCtrlSignal = 0;
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
			// Interrupt IN
			usbdEP5Handler();
		}

		if(u32IntSts & USBD_INTSTS_EP6)
		{
			/* Clear event flag */
			USBD_CLR_INT_FLAG(USBD_INTSTS_EP6);
			// Interrupt OUT
			usbdEP6Handler();
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
__myevic__ void usbdClassRequest()
{
	uint8_t token[8];

	USBD_GetSetupPacket( token );

	if ( ( token[0] & 0x1F ) == 1 )
	{
		if (( dfStatus.vcom ) && ( token[4] == VCOM_INTERFACE ))
		{
			VCOM_ClassRequest( token );
			return;
		}
		if (( dfStatus.storage ) && ( token[4] == MSC_INTERFACE ))
		{
			MSC_ClassRequest( token );
			return;
		}
	}

	if( token[0] & 0x80 )    /* request data transfer direction */
	{
		// Device to host
		switch ( token[1] )
		{
			case GET_REPORT:
			case GET_IDLE:
			case GET_PROTOCOL:
			default:
			{
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
			case SET_REPORT:
			{
				if( token[3] == 3 )
				{
					/* Request Type = Feature */
					USBD_SET_DATA1( EP1 );
					USBD_SET_PAYLOAD_LEN( EP1, 0 );
				}
				break;
			}

			case SET_IDLE:
			{
				/* Status stage */
				USBD_SET_DATA1( EP0 );
				USBD_SET_PAYLOAD_LEN( EP0, 0 );
				break;
			}

			case SET_PROTOCOL:
			default:
			{
				// Stall
				/* Setup error, stall the device */
				USBD_SetStall( EP0 );
				USBD_SetStall( EP1 );
				break;
			}
		}
	}
}


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ void SetupEndpoints()
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
	USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | HID_INT_IN_EP_NUM);
	/* Buffer range for EP2 */
	USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

	/* EP3 ==> Interrupt OUT endpoint, address 2 */
	USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | HID_INT_OUT_EP_NUM);
	/* Buffer range for EP3 */
	USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
	/* trigger to receive OUT data */
	USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

	/*****************************************************/
	if ( dfStatus.vcom )
	{
		/* EP4 ==> Interrupt IN endpoint, address 3 */
		USBD_CONFIG_EP(EP4, USBD_CFG_EPMODE_IN | VCOM_INT_IN_EP_NUM);
		/* Buffer offset for EP4 ->  */
		USBD_SET_EP_BUF_ADDR(EP4, EP4_BUF_BASE);

		/* EP5 ==> Bulk IN endpoint, address 1 */
		USBD_CONFIG_EP(EP5, USBD_CFG_EPMODE_IN | VCOM_BULK_IN_EP_NUM);
		/* Buffer offset for EP5 */
		USBD_SET_EP_BUF_ADDR(EP5, EP5_BUF_BASE);

		/* EP6 ==> Bulk Out endpoint, address 2 */
		USBD_CONFIG_EP(EP6, USBD_CFG_EPMODE_OUT | VCOM_BULK_OUT_EP_NUM);
		/* Buffer offset for EP6 */
		USBD_SET_EP_BUF_ADDR(EP6, EP6_BUF_BASE);
		/* trigger receive OUT data */
		USBD_SET_PAYLOAD_LEN(EP6, EP6_MAX_PKT_SIZE);
	}
	else if ( dfStatus.storage )
	{
		/* EP5 ==> Bulk IN endpoint, address 1 */
		USBD_CONFIG_EP(EP5, USBD_CFG_EPMODE_IN | MSC_BULK_IN_EP_NUM);
		/* Buffer offset for EP5 */
		USBD_SET_EP_BUF_ADDR(EP5, EP5_BUF_BASE);

		/* EP6 ==> Bulk Out endpoint, address 2 */
		USBD_CONFIG_EP(EP6, USBD_CFG_EPMODE_OUT | MSC_BULK_OUT_EP_NUM);
		/* Buffer offset for EP6 */
		USBD_SET_EP_BUF_ADDR(EP6, EP6_BUF_BASE);
		/* trigger receive OUT data */
		USBD_SET_PAYLOAD_LEN(EP6, EP6_MAX_PKT_SIZE);
	}
}


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ void InitUSB()
{
	USBD_SwReset();

	if ( dfStatus.vcom )
	{
		USBD_Open( &usbdVCOMDescriptors, usbdClassRequest+1, 0 );
	}
	else if ( dfStatus.storage )
	{
		USBD_Open( &usbdMSCDescriptors, usbdClassRequest+1, 0 );
		USBD_SetConfigCallback( MSC_SetConfig );
		MSC_Init();
	}
	else
	{
		USBD_Open( &usbdDescriptors, usbdClassRequest+1, 0 );
	}

	SetupEndpoints();
	USBD_Start();
	NVIC_EnableIRQ( USBD_IRQn );

	if ( dfStatus.vcom )
	{
		myputc = VCOM_Putc;
	}

}


//=========================================================================
// HID Requests definitions

#define HID_CMD_SIGNATURE   0x43444948

/* HID Commands */
#define HID_CMD_NONE		0x00
#define HID_CMD_GETINFO		0x35
#define HID_CMD_LDUPDATE	0x3C
#define HID_CMD_FORCE_VCOM	0x42
#define HID_CMD_MONITORING	0x43
#define HID_CMD_AUTO_PUFF	0x44
#define HID_CMD_SETPARAMS	0x53
#define HID_CMD_READCONFIG	0x60
#define HID_CMD_WRITECONFIG	0x61
#define HID_CMD_SETDATETIME	0x64
#define HID_CMD_GETMONDATA	0x66
#define HID_CMD_GETPROFILE	0x70
#define HID_CMD_SETPROFILE	0x71
#define HID_CMD_RESETPARAMS	0x7C
#define HID_CMD_SETLOGO		0xA5
#define HID_CMD_RESET		0xB4
#define HID_CMD_FMCREAD		0xC0
#define HID_CMD_SCREENSHOT	0xC1
#define HID_CMD_APUPDATE	0xC3

#define HID_CONFIG_LENGTH	0x400
#define HID_CONFIG_FORMAT	0x03
#define HID_PROFILE_LENGTH	(DATAFLASH_PARAMS_SIZE+DATAFLASH_INFOS_SIZE)
#define HID_PROFILE_FORMAT	0x00


typedef struct __attribute__((packed))
{
    uint8_t u8Cmd;
    uint8_t u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
}
CMD_T;

CMD_T hidCmd;

typedef struct __attribute__((packed))
{
	uint32_t	Timestamp; // X * 100 (seconds)

	uint8_t		IsFiring;
	uint8_t		IsCharging;
	uint8_t		IsCelsius;

	uint8_t		BatteryVoltage[4]; // Offsetted by 275, 420 - 275 = value

	uint16_t	PowerSet; // X * 10
	uint16_t	TemperatureSet;
	uint16_t	Temperature;

	uint16_t	OutputVoltage; // X * 100
	uint16_t	OutputCurrent; // X * 100

	uint16_t	Resistance; // X * 1000
	uint16_t	RealResistance;   // X * 1000

	uint8_t		BoardTemperature;
}
HIDMonData_t;

typedef struct __attribute__((packed))
{
	uint16_t	Year;
	uint8_t		Month;
	uint8_t		Day;
	uint8_t		Hour;
	uint8_t		Minute;
	uint8_t		Second;
}
HIDDateTime_t;

uint8_t *hidInDataPtr;
uint8_t hidData[FMC_FLASH_PAGE_SIZE];
uint32_t hidDFData[FMC_FLASH_PAGE_SIZE/4];
uint32_t hidDataIndex;


//=========================================================================
//----- (00001204) --------------------------------------------------------
// R0 = Sum of the R1 bytes at R0
__myevic__ uint32_t Checksum( const uint8_t *p, const uint32_t l )
{
	uint32_t sum = 0;
	for ( uint32_t i = 0 ; i < l ; ++i ) sum += p[i];
	return sum;
}

//-------------------------------------------------------------------------
__myevic__ void hidStartInReport( uint32_t u32ParamLen )
{
	hidDataIndex = u32ParamLen;
	hidCmd.u32Signature = u32ParamLen;

	USBD_MemCopy(
		(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
		hidInDataPtr,
		EP2_MAX_PKT_SIZE
	);
	USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
	hidDataIndex -= EP2_MAX_PKT_SIZE;
}


//----- (00002C60) --------------------------------------------------------
__myevic__ void hidSetInReport()
{
	uint8_t cmd = hidCmd.u8Cmd;

	switch ( cmd )
	{
		case HID_CMD_GETINFO:
		case HID_CMD_SCREENSHOT:
		case HID_CMD_GETPROFILE:
		{
			if ( hidDataIndex )
			{
				USBD_MemCopy(
					(uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)),
					&hidInDataPtr[hidCmd.u32Signature - hidDataIndex],
					EP2_MAX_PKT_SIZE
				);
				USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
				hidDataIndex -= EP2_MAX_PKT_SIZE;
			}
			else
			{
				cmd = HID_CMD_NONE;
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
					if ( u32Size > FMC_FLASH_PAGE_SIZE ) u32Size = FMC_FLASH_PAGE_SIZE;

					SYS_UnlockReg();
					FMC_ENABLE_ISP();

					for ( int i = 0 ; i < u32Size ; i += 4 )
					{
						uint32_t data = FMC_Read( u32Addr + i );
						MemCpy( &hidData[ FMC_FLASH_PAGE_SIZE - u32Size + i ], &data, 4 );
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
					&hidData[ FMC_FLASH_PAGE_SIZE - hidDataIndex ],
					EP2_MAX_PKT_SIZE
				);
				USBD_SET_PAYLOAD_LEN( EP2, EP2_MAX_PKT_SIZE );
				hidDataIndex -= EP2_MAX_PKT_SIZE;
			}
			else
			{
				cmd = HID_CMD_NONE;
			}
			break;
		}

		default:
			cmd = HID_CMD_NONE;
			break;
	}

	hidCmd.u8Cmd = cmd;
}


//-------------------------------------------------------------------------
__myevic__ void usbdEP2Handler()
{
	hidSetInReport();
}


//----- (00002C00) --------------------------------------------------------
__myevic__ uint32_t hidResetParamCmd( CMD_T *pCmd )
{
	uint8_t p;
	myprintf("Reset param\n");
	p = dfProfile;
	ResetDataFlash();
	dfProfile = p;
	gFlags.refresh_display = 1;
	pCmd->u8Cmd = HID_CMD_NONE;
	return 0;
}


//----- (00002C38) --------------------------------------------------------
__myevic__ uint32_t hidResetSysCmd( CMD_T *pCmd )
{
	myprintf("Reset system command\n");

	if ( UpdateDFTimer ) UpdateDataFlash();
	if ( UpdatePTTimer ) UpdatePTCounters();

	if ( ISVTCDUAL || ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
	{
		PD7 = 0;
		BBC_Configure( BBC_PWMCH_CHARGER, 0 );
		PD7 = 0;
		ChargerDuty = 0;

		if ( ISVTCDUAL )
		{
			PA3 = 0;
			PC3 = 0;
			PA2 = 0;
		}
		else	// if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
				// (currently useless, restore if needed)
		{
			PF0 = 0;
		}
	}

	if ( !gFlags.has_x32 )
	{
		RTCAdjustClock( dfBootFlag ? 9 : 1 );
		RTCAdjustClock( 0 );
		CLK_SysTickDelay( 500 );
	}

	SYS_UnlockReg();

	FMC_SELECT_NEXT_BOOT( dfBootFlag );
	SCB->AIRCR = 0x05FA0004;

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

	myprintf( "Get Info command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );

	if ( u32ParamLen )
	{
		if ( u32StartAddr + u32ParamLen > FMC_FLASH_PAGE_SIZE )
		{
			u32ParamLen = FMC_FLASH_PAGE_SIZE - u32StartAddr;
		}

		MemCpy( hidData, ((uint8_t *)&DataFlash), FMC_FLASH_PAGE_SIZE );

		dfStruct_t * df = (dfStruct_t*)hidData;

		if ( dfStatus.nfe )
		{
			df->p.Magic = DATAFLASH_NFE_MAGIC;
			df->n.Build = __BUILD3;
			uint32_t o = offsetof( dfStruct_t, p.VVRatio );
			MemClear( &hidData[o], DATAFLASH_PARAMS_SIZE - o );
			df->p.ShuntRez = 100;
			df->p.VVRatio = 200;	// PreheatPwr for NFE
			df->p.DimTimeout = 30;

			LoadCustomBattery();
			o = offsetof( dfStruct_t, p ) + 0xCA;	// NFE Custom Battery offset
			MemCpy( &hidData[o], &CustomBattery.V2P, 48 );
		}

		df->Checksum = Checksum( (uint8_t *)df->params, FMC_FLASH_PAGE_SIZE - 4 );

		hidInDataPtr = &hidData[u32StartAddr];
		hidStartInReport( u32ParamLen );
	}

	return 0;
}


//----- (00002678) --------------------------------------------------------
__myevic__ uint32_t hidBootLogoCmd( CMD_T *pCmd )
{
	myprintf( "Set Boot Logo command - Start page: %d\t\tLen: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	MemClear( hidData, FMC_FLASH_PAGE_SIZE );
	hidDataIndex = 0;
	pCmd->u32Signature = 0;
	return 0;
}


//----- (000026D8) --------------------------------------------------------
__myevic__ uint32_t hidSetParamCmd( CMD_T *pCmd )
{
	myprintf( "Set Param command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	hidDataIndex = 0;
	return 0;
}


//-------------------------------------------------------------------------
// Read Configuration
//-------------------------------------------------------------------------
__myevic__ uint32_t hidGetProfile( CMD_T *pCmd )
{
	uint32_t u32ProfileNum;

	myprintf( "Get Profile command - Profile: %d\n", pCmd->u32Arg1 );

	u32ProfileNum = pCmd->u32Arg1;

	dfParams_t *p;

	if (( u32ProfileNum == 0 ) || ( u32ProfileNum == dfProfile + 1 ))
	{
		dfCRC = CalcPageCRC( DataFlash.params );
		p = (dfParams_t*)&DataFlash.p;
	}
	else
	{
		if ( u32ProfileNum > DATAFLASH_PROFILES_MAX )
		{
			myprintf( "Invalid profile #.\n" );
			return 1;
		}

		p = (dfParams_t*)(DATAFLASH_PROFILES_BASE+DATAFLASH_PARAMS_SIZE*(u32ProfileNum-1));
	}

	MemCpy( &hidData[0], (uint8_t *)p, DATAFLASH_PARAMS_SIZE );
	MemCpy( &hidData[DATAFLASH_PARAMS_SIZE], ((uint8_t *)&DataFlash.i), DATAFLASH_INFOS_SIZE );

	dfInfos_t * di = (dfInfos_t*)&hidData[DATAFLASH_PARAMS_SIZE];

	di->Format = HID_PROFILE_FORMAT;
	di->Build = __BUILD3;

	hidInDataPtr = &hidData[0];
	hidStartInReport( HID_PROFILE_LENGTH );

	return 0;
}


//-------------------------------------------------------------------------
// Read Configuration
//-------------------------------------------------------------------------
__myevic__ uint32_t hidSetProfile( CMD_T *pCmd )
{
	myprintf( "Set Profile command - Profile: %d\n", pCmd->u32Arg1 );
	hidDataIndex = 0;
	return 0;
}


//-------------------------------------------------------------------------
// Monitoring
//-------------------------------------------------------------------------
__myevic__ uint32_t hidGetMonData( CMD_T *pCmd )
{
	uint32_t u32StartAddr;
	uint32_t u32ParamLen;

	u32StartAddr = pCmd->u32Arg1;
	u32ParamLen = pCmd->u32Arg2;

//	myprintf( "Get Monitoring Data command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );

	if ( u32StartAddr != 0 || u32ParamLen != 0x40 )
	{
//		myprintf( "Invalid parameters\n" );
		return 1;
	}

	HIDMonData_t *mondata = (HIDMonData_t*)hidData;

	MemSet( mondata, 0, u32ParamLen );

	mondata->Timestamp = TMR2Counter / 10;

	mondata->IsFiring = gFlags.firing;
	mondata->IsCharging = gFlags.battery_charging;
	mondata->IsCelsius = dfIsCelsius;

	uint16_t temp = dfIsCelsius ? FarenheitToC( AtoTemp ) : AtoTemp;

	if ( gFlags.firing )
	{
		for ( int i = 0 ; i < NumBatteries ; ++i )
		{
			mondata->BatteryVoltage[i] = RTBVolts[i] - 275;
		}

		if ( ISMODETC(dfMode) )
		{
			mondata->Temperature = temp;
		}

		mondata->OutputVoltage = AtoVolts;
		mondata->OutputCurrent = AtoCurrent * 10;
	}
	else
	{
		for ( int i = 0 ; i < NumBatteries ; ++i )
		{
			mondata->BatteryVoltage[i] = BattVolts[i] - 275;
		}

		if ( ISMODETC(dfMode) )
		{
			ReadAtoTemp();
			mondata->Temperature = temp;
		}
	}

	if ( ISMODETC(dfMode) )
	{
		mondata->PowerSet = dfTCPower;
		mondata->TemperatureSet = dfTemp;
	}
	else
	{
		mondata->PowerSet = dfPower;
	}

	mondata->Resistance = dfResistance * 10 + RezMillis;
	mondata->RealResistance = AtoRezMilli;

	mondata->BoardTemperature = BoardTemp;

	hidInDataPtr = &hidData[u32StartAddr];
	hidStartInReport( u32ParamLen );

	return 0;
}


//----- (0000272C) --------------------------------------------------------
__myevic__ uint32_t hidLDUpdateCmd( CMD_T *pCmd )
{
	myprintf( "Update LDROM command - Start page: %d\t\tLen: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );
	MemClear( hidData, FMC_FLASH_PAGE_SIZE );
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

	myprintf( "FMC Read command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );

	if ( ! u32ParamLen % EP2_MAX_PKT_SIZE )
	{
		return -1;
	}

	if ( u32ParamLen )
	{
		if ( u32ParamLen > FMC_FLASH_PAGE_SIZE )
		{
			u32ParamLen = FMC_FLASH_PAGE_SIZE;
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

		hidInDataPtr = hidData;
		hidStartInReport( u32ParamLen );
	}

	return 0;
}


//-------------------------------------------------------------------------
__myevic__ uint32_t hidScreenshot( CMD_T *pCmd )
{
	uint32_t u32StartAddr;
	uint32_t u32ParamLen;

	u32StartAddr = pCmd->u32Arg1;
	u32ParamLen = pCmd->u32Arg2;

	myprintf( "Screenshot command - Start Addr: %d    Param Len: %d\n", pCmd->u32Arg1, pCmd->u32Arg2 );

	if ( u32ParamLen )
	{
		if ( u32StartAddr + u32ParamLen > SCREEN_BUFFER_SIZE )
		{
			u32ParamLen = SCREEN_BUFFER_SIZE - u32StartAddr;
		}

		Screen2Bitmap( hidData );

		if ( dfStatus.invert )
		{
			for ( int i = 0 ; i < SCREEN_BUFFER_SIZE ; ++i )
				hidData[i] ^= 0xFF;
		}

		hidInDataPtr = hidData;
		hidStartInReport( u32ParamLen );
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
		case HID_CMD_GETMONDATA:
		{
			hidGetMonData( &hidCmd );
			break;
		}
		case HID_CMD_GETPROFILE:
		{
			hidGetProfile( &hidCmd );
			break;
		}
		case HID_CMD_SETPROFILE:
		{
			hidSetProfile( &hidCmd );
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
		case HID_CMD_SCREENSHOT:
		{
			hidScreenshot( &hidCmd );
			break;
		}
		case HID_CMD_FORCE_VCOM:
		{
			Event = EVENT_FORCE_VCOM;
			break;
		}
		case HID_CMD_MONITORING:
		{
			gFlags.monitoring = hidCmd.u32Arg1 ? 1 : 0;
			break;
		}
		case HID_CMD_AUTO_PUFF:
		{
			if ( hidCmd.u32Arg1 < 10 )
			{
				AutoPuffTimer = hidCmd.u32Arg1 * 100;
				Event = EVENT_AUTO_PUFF;
			}
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
		case HID_CMD_SETPROFILE:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex >= u32DataSize )
			{
				u8Cmd = HID_CMD_NONE;

				dfParams_t *p = (dfParams_t*)&hidData[0];
				dfInfos_t  *i = (dfInfos_t*)&hidData[DATAFLASH_PARAMS_SIZE];

				if ( i->Format != HID_PROFILE_FORMAT || p->Magic != DFMagicNumber )
				{
					myprintf( "Incompatible parameters format.\n" );
					break;
				}

				if ( u32StartAddr > DATAFLASH_PROFILES_MAX )
				{
					myprintf( "Invalid profile #.\n" );
					break;
				}

				if ( u32StartAddr == 0 ) // profile #
				{
					if ( p->Profile != dfProfile )
					{
						// Profile selection change
						SaveProfile();
					}

					MemCpy( DataFlash.params, p, DATAFLASH_PARAMS_SIZE );

					DFCheckValuesValidity();
					UpdateDataFlash();
				}
				else
				{
					p->Profile = u32StartAddr - 1;
					p->PCRC = CalcPageCRC( (uint32_t*)p );

					uint8_t page[FMC_FLASH_PAGE_SIZE] __attribute__((aligned(4)));

					uint32_t offset = p->Profile * DATAFLASH_PARAMS_SIZE;

					MemCpy( page, (void*)DATAFLASH_PROFILES_BASE, FMC_FLASH_PAGE_SIZE );
					MemCpy( page + offset, p, DATAFLASH_PARAMS_SIZE );

					SYS_UnlockReg();
					FMC_ENABLE_ISP();
					FMC_ENABLE_AP_UPDATE();

					FMCEraseWritePage( DATAFLASH_PROFILES_BASE, (uint32_t*)page );

					FMC_DISABLE_AP_UPDATE();
					FMC_DISABLE_ISP();
					SYS_LockReg();
				}

				myprintf( "Set Profile command complete.\n" );
			}

			break;
		}

		case HID_CMD_SETPARAMS:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex >= u32DataSize )
			{
				u8Cmd = HID_CMD_NONE;

				if ( u32StartAddr + u32DataSize > FMC_FLASH_PAGE_SIZE )
				{
					sz = FMC_FLASH_PAGE_SIZE - u32StartAddr;
				}
				else
				{
					sz = u32DataSize;
				}

				MemCpy( (uint8_t*)(&hidDFData) + u32StartAddr, hidData, sz );

				myprintf( "Set Sys Param complete.\n" );

				dfStruct_t * df = (dfStruct_t*)hidDFData;

				if ( Checksum( (uint8_t*)df->params, FMC_FLASH_PAGE_SIZE - 4 ) == df->Checksum )
				{
					myprintf( "\tCompany ID ............................ [0x%08x]\n",
								df->i.fmcCID );
					myprintf( "\tDevice ID ............................. [0x%08x]\n",
								df->i.fmcDID  );
					myprintf( "\tProduct ID ............................ [0x%08x]\n",
								df->i.fmcPID );
					myprintf( "\tu8UpdateAPRom ......................... [0x%08x]\n",
								df->p.BootFlag );

					if ( df->p.Magic == DFMagicNumber )
					{
						MemCpy( DataFlash.params, df->params, DATAFLASH_PARAMS_SIZE );

						DFCheckValuesValidity();
						UpdateDataFlash();
					}
					else
					{
						myprintf( "Incompatible parameters format.\n" );

						if ( df->p.Magic == DATAFLASH_NFE_MAGIC )
						{
							uint32_t o;

							dfBootFlag = df->p.BootFlag;

							// Custom Battery
							o = offsetof( dfStruct_t, p ) + 0xCA;
							MemCpy( &CustomBattery.V2P, (uint8_t*)(&hidDFData) + o, 48 );

							if ( CheckCustomBattery() )
							{
								SaveCustomBattery( &CustomBattery );
								SetBatteryModel();
							}
							else
							{
								myprintf( "Invalid battery data.\n" );
								LoadCustomBattery();
							}

							UpdateDataFlash();
						}
					}

					if ( df->i.Year >= 2000 && df->i.Year <= 2099 )
					{
						S_RTC_TIME_DATA_T rtd;
						rtd.u32Year = df->i.Year;
						rtd.u32Month = df->i.Month;
						rtd.u32Day = df->i.Day;
						rtd.u32DayOfWeek = 0;
						rtd.u32Hour = df->i.Hour;
						rtd.u32Minute = df->i.Minute;
						rtd.u32Second = df->i.Second;
						rtd.u32TimeScale = RTC_CLOCK_24;
						SetRTC( &rtd );
					}

					gFlags.refresh_display = 1;
				}
				else
				{
					myprintf( "Sys Param Receive fail.\n" );
				}

				hidDataIndex = 0;
			}

			break;
		}

		case HID_CMD_SETLOGO:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex < FMC_FLASH_PAGE_SIZE && hidDataIndex + u32ByteCount < u32DataSize )
			{
				break;
			}

			u32Page = u32StartAddr + u32ByteCount;

			myprintf( "Writing page %d\n", u32Page );

			SYS_UnlockReg();
			FMC_ENABLE_ISP();
			FMC_ENABLE_AP_UPDATE();

			if ( FMCEraseWritePage( u32Page, (uint32_t*)hidData ) )
			{
				myprintf( "Data Flash Erase error!\n" );
			}

			veo = FMCVerifyPage( u32Page, (uint32_t*)hidData );
			if ( veo )
			{
				myprintf( "Data Flash Verify error! 0x%x\n", 4 * veo - 4 );
			}

			MemClear( hidData, FMC_FLASH_PAGE_SIZE );
			u32ByteCount += hidDataIndex;

			myprintf( "g_u32BytesInPageBuf %d, u32LenCnt 0x%x\n", hidDataIndex, u32ByteCount );

			FMC_DISABLE_AP_UPDATE();
			FMC_DISABLE_ISP();
			SYS_LockReg();

			if ( u32ByteCount < u32DataSize )
			{
				hidDataIndex = 0;
			}
			else
			{
				u8Cmd = HID_CMD_NONE;
				myprintf( "set boot logo command complete.\n" );
			}

			break;
		}

		case HID_CMD_LDUPDATE:
		{
			USBD_MemCopy( hidDataPtr, pu8Buffer, EP3_MAX_PKT_SIZE );
			hidDataIndex += EP3_MAX_PKT_SIZE;

			if ( hidDataIndex < FMC_FLASH_PAGE_SIZE && hidDataIndex + u32ByteCount < u32DataSize )
			{
				break;
			}

			u32Page = u32StartAddr + u32ByteCount;
			myprintf( "Writing page 0x%08X\n", u32Page );

			SYS_UnlockReg();
			FMC_ENABLE_ISP();
			FMC_EnableLDUpdate();

			if ( FMCEraseWritePage( u32Page, (uint32_t*)hidData ) )
			{
				myprintf( "Data Flash Erase error!\n" );
			}

			veo = FMCVerifyPage( u32Page, (uint32_t*)hidData );
			if ( veo )
			{
				myprintf( "Data Flash Verify error! 0x%x\n", 4 * veo - 4 );
			}

			MemClear( hidData, FMC_FLASH_PAGE_SIZE );
			u32ByteCount += hidDataIndex;
			hidDataIndex = 0;

			myprintf( "g_u32BytesInPageBuf %d, u32LenCnt 0x%x\n", hidDataIndex, u32ByteCount );

			FMC_DisableLDUpdate();
			FMC_DISABLE_ISP();
			SYS_LockReg();

			if ( u32ByteCount >= u32DataSize )
			{
				u8Cmd = HID_CMD_NONE;
				myprintf( "Update LDROM command complete.\n" );
			}

			break;
		}

		case HID_CMD_SETDATETIME:
		{
			HIDDateTime_t *dt = (HIDDateTime_t*)pu8Buffer;

			if ( dt->Year >= 2000 && dt->Year <= 2099 )
			{
				S_RTC_TIME_DATA_T rtd;
				rtd.u32Year = dt->Year;
				rtd.u32Month = dt->Month;
				rtd.u32Day = dt->Day;
				rtd.u32DayOfWeek = 0;
				rtd.u32Hour = dt->Hour;
				rtd.u32Minute = dt->Minute;
				rtd.u32Second = dt->Second;
				rtd.u32TimeScale = RTC_CLOCK_24;
				SetRTC( &rtd );
			}

			u8Cmd = HID_CMD_NONE;

			myprintf( "Set Date/Time command complete.\n" );
			break;
		}

		default:
		{
			if ( hidProcessCommand( pu8Buffer, u32BufferLen ) )
			{
				myprintf( "Unknown HID command %02X!\n", hidCmd.u8Cmd );
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


//-------------------------------------------------------------------------
__myevic__ void usbdEP5Handler()
{
	if ( dfStatus.vcom )
	{
		VCOM_EP5Handler();
	}
	else if ( dfStatus.storage )
	{
		MSC_EP5Handler();
	}
}

//-------------------------------------------------------------------------
__myevic__ void usbdEP6Handler()
{
	if ( dfStatus.vcom )
	{
		VCOM_EP6Handler();
	}
	else if ( dfStatus.storage )
	{
		MSC_EP6Handler();
	}
}
