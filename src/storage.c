#include "storage.h"
#include "dataflash.h"
#include "dtmacros.h"

//=========================================================================
// Mass Storage
//-------------------------------------------------------------------------

#define MSC_BULK_IN_BUF		((uint8_t*)(USBD_BUF_BASE+USBD_GET_EP_BUF_ADDR(EP5)))
#define MSC_BULK_OUT_BUF	((uint8_t*)(USBD_BUF_BASE+USBD_GET_EP_BUF_ADDR(EP6)))

//-------------------------------------------------------------------------
// USB Descriptors
//-------------------------------------------------------------------------

#define DESC_LEN_WITH_MSC \
	(LEN_CONFIG + LEN_INTERFACE * 2 + LEN_HID + LEN_ENDPOINT * 4)

const uint8_t usbdMSCConfigDesc[] =
{
	LEN_CONFIG,     /* bLength */
	DESC_CONFIG,    /* bDescriptorType */
	/* wTotalLength */
	DESC_LEN_WITH_MSC & 0x00FF,
	(DESC_LEN_WITH_MSC & 0xFF00) >> 8,
	0x02,           /* bNumInterfaces */
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

/* I/F descr: Mass Storage */
	LEN_INTERFACE,  // bLength
	DESC_INTERFACE, // bDescriptorType
	0x02,     // bInterfaceNumber
	0x00,     // bAlternateSetting
	0x02,     // bNumEndpoints
	0x08,     // bInterfaceClass:		Mass Storage
	0x06,     // bInterfaceSubClass:	SCSI transparent command set
	0x50,     // bInterfaceProtocol:	BBB (Bulk-Only)
	0x00,     // iInterface

	/* const BYTE cbyEndpointDescriptor1[LEN_ENDPOINT] = */
	LEN_ENDPOINT,           // bLength
	DESC_ENDPOINT,          // bDescriptorType
	(MSC_BULK_IN_EP_NUM | EP_INPUT), // bEndpointAddress
	EP_BULK,                // bmAttributes
	EP5_MAX_PKT_SIZE, 0x00, // wMaxPacketSize
	0x00,                   // bInterval

	/* const BYTE cbyEndpointDescriptor2[LEN_ENDPOINT] = */
	LEN_ENDPOINT,           // bLength
	DESC_ENDPOINT,          // bDescriptorType
	(MSC_BULK_OUT_EP_NUM | EP_OUTPUT), // bEndpointAddress
	EP_BULK,                // bmAttributes
	EP6_MAX_PKT_SIZE, 0x00, // wMaxPacketSize
	0x00                    // bInterval
};


//-------------------------------------------------------------------------
// Device size in blocks (sectors).
// This value should be set accordingly with the FAT cluster size
// so that the number of data clusters is between 4085 and 65524.
// A value of 32768 w/ a cluster size of 4 blocks yields to approx
// 8000 data clusters, which is a not-so-bad value.
// The goal is to be in the domain of FAT16, which is the simplest
// and easy-to-code file system.
//-------------------------------------------------------------------------
#define MSC_DEVICE_BLOCKS		0x8000
#define MSC_LAST_BLOCK			(MSC_DEVICE_BLOCKS-1)
//-------------------------------------------------------------------------
// Block Size in bytes.
// 512 is an arbitrary but consensual value.
// This is also the minimal value. Code will malfunction below, and many
// other codes will, too. This value must also be a power of two.
//-------------------------------------------------------------------------
#define MSC_BLOCK_SIZE			0x200
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// SCSI/UFI Requests and responses structures
//-------------------------------------------------------------------------

typedef struct
{
	uint8_t OPCode;
	uint8_t	LUN;
	union
	{
		struct
		{
			uint8_t	PageCode;
			uint8_t	r1[1];
			uint8_t	AllocationLength;
			uint8_t r2[7];
		}
		Inquiry;
		
		struct
		{
			uint8_t r1[5];
			uint8_t AllocationLengthH;
			uint8_t AllocationLengthL;
			uint8_t r2[3];
		}
		ReadFormatCapacities;
		
		struct
		{
			uint8_t LBA[4];
			uint8_t r1[1];
			uint8_t TransferLengthH;
			uint8_t TransferLengthL;
			uint8_t r2[3];
		}
		Read10;
		
		struct
		{
			uint8_t r1[2];
			uint8_t	Prevent;
			uint8_t r2[7];
		}
		PreventAllowMR;

		uint8_t	Data[14];
	};
}
CB_t;

typedef struct
{
	uint32_t	Signature;
	uint32_t	Tag;
	uint32_t	DataTransferLength;
	uint8_t		Flags;
	uint8_t		LUN;
	uint8_t		CBLength;
	CB_t		CB;
}
CBW_t;

#define CBW_SIGNATURE 0x43425355
#define CBW_SIZE 31

typedef struct
{
	uint32_t	Signature;
	uint32_t	Tag;
	uint32_t	DataResidue;
	uint8_t		Status;
}
CSW_t;

#define CSW_SIGNATURE 0x53425355
#define CSW_SIZE 13

#define CSW_STATUS_SUCCESS		0x00
#define CSW_STATUS_FAILED		0x01
#define CSW_STATUS_PHASE_ERROR	0x02


typedef struct
{
	uint8_t Peripheral;
	uint8_t Removable;
	uint8_t Version;
	uint8_t Response_Data_Format;
	uint8_t AdditionalLength;
	uint8_t Sccstp;
	uint8_t bqueetc;
	uint8_t CmdQue;
	char vendorID[8];
	char productID[16];
	char productRev[4];
} UFI_InquiryResponse_t;

const UFI_InquiryResponse_t MSC_InquiryResp =
{
	0x00, // direct access block device, connected
	0x80, // device is removable
	0x04, // SPC-2 compliance
	0x02, // response data format
	0x20, // response has 20h + 4 bytes
	0x00, // additional fields, none set
	0x00, // additional fields, none set
	0x00, // additional fields, none set
	"Joyetech", // 8-byte T10-assigned Vendor ID
	"eVic VTC/VTwo[M]", // 16-byte product identification
	"0001" // 4-byte product revision level
};

typedef struct
{
	uint8_t	PeripheralType;
	uint8_t	PageCode;
	uint8_t reserved1;
	uint8_t	PageLength;
	char	Serial[12];
	char	FirstStorageAddr[4];
} UFI_Inquiry80_t;

const UFI_Inquiry80_t MSC_Inquiry80 =
{
	0x08,
	0x80,
	0x00,
	0x10,
	"0123456789AB",
	"0000"
};


const uint8_t UFI_RFCResponse[12] =
{
	0, 0, 0,	// reserved
	8,			// size of descriptors
	// capacity descriptor 0: 16MB device
		// number of blocks
		(MSC_DEVICE_BLOCKS >> 24) & 0xFF,
		(MSC_DEVICE_BLOCKS >> 16) & 0xFF,
		(MSC_DEVICE_BLOCKS >> 8 ) & 0xFF,
		(MSC_DEVICE_BLOCKS      ) & 0xFF,
	2,	// formatted media
		// bytes per block
		(MSC_BLOCK_SIZE >> 16) & 0xFF,
		(MSC_BLOCK_SIZE >>  8) & 0xFF,
		(MSC_BLOCK_SIZE      ) & 0xFF
};

const uint8_t UFI_RCResponse[8] =
{
	// Last addressable block
	(MSC_LAST_BLOCK >> 24) & 0xFF,
	(MSC_LAST_BLOCK >> 16) & 0xFF,
	(MSC_LAST_BLOCK >>  8) & 0xFF,
	(MSC_LAST_BLOCK      ) & 0xFF,
	// Block size
	(MSC_BLOCK_SIZE >> 24) & 0xFF,
	(MSC_BLOCK_SIZE >> 16) & 0xFF,
	(MSC_BLOCK_SIZE >>  8) & 0xFF,
	(MSC_BLOCK_SIZE      ) & 0xFF
};


typedef struct
{
	uint8_t	ErrorCode;
	uint8_t	r1[1];
	uint8_t	SenseKey;
	uint8_t	Information[4];
	uint8_t	AddlSenseLength;
	uint8_t	r2[4];
	uint8_t AddlSenseCode;
	uint8_t	AddlSenseCodeQualifier;
	uint8_t r3[3];
}
UFI_RQSenseData_t;

UFI_RQSenseData_t UFI_RQSenseData =
{
	0x80 | 0x70,
	{ 0 },
	0,
	{ 0 },
	10,
	{ 0 },
	0,
	0,
	{ 0 }
};


//-------------------------------------------------------------------------
// Globals
//-------------------------------------------------------------------------

static volatile CBW_t gCBW;
static volatile CSW_t gCSW;

enum {
	MSC_WAITING_CBW,
	MSC_WAITING_CSW,
	MSC_WAITING_DATA,
	MSC_WAITING_NONE
};

static volatile uint8_t gRxMode = MSC_WAITING_CBW;

typedef struct
{
	int bulk_in_ready:1;
	int csw_ready:1;
	int stalled:1;
}
MSC_Flags_t;

static volatile MSC_Flags_t gMSC_Flags = { 0 };

static volatile uint32_t gTxLBA;
static volatile uint32_t gTxNumBlocks = 0;
static volatile uint32_t gTxDataLength = 0;
static volatile uint32_t gTxDataIndex = 0;
static uint8_t gTxDataBlock[MSC_BLOCK_SIZE];


//=========================================================================
// FAT16
//-------------------------------------------------------------------------
// 32768 sectors
// -1 reserved = 32767
//  /4 = 8191 clusters
//   *2 = 16382 bytes in FAT
//    /512 = 32 sectors per FAT
//         = 16 clusters for 2 FATs
//  -16 = 8175 data clusters
// => FAT type 16
//-------------------------------------------------------------------------

// MBR block address.
// Must be 0 or -1.
// A value of -1 disables the MBR. If MBR is disabled, the FAT16 partition
// starting block should be set to zero.
#define MBR_BLOCK			-1

// FAT16 partition starting block.
// If MBR address is set to -1, this value should be set to zero.
// Else, it should be set to any non-zero arbitrary value (e.g., 1).
#define FAT16_PART0_START	0
#define FAT16_PART0_SIZE	(MSC_DEVICE_BLOCKS-FAT16_PART0_START)

// Cluster size in blocks and bytes.
// Should be set accordingly with MSC_DEVICE_BLOCKS so that the number of
// data clusters is in the FAT16 domain (See above).
// 4 * 0x200 is the size of a dataflash page, which is irrelevant but cool.
#define FAT16_CLUSTER_SIZE	4
#define FAT16_CLUSTER_BYTES	(FAT16_CLUSTER_SIZE*MSC_BLOCK_SIZE)

// Number of blocks before the FAT16 partition.
// Since there's only one partition, the number of hidden blocks is equal
// to the partition starting block.
#define FAT16_HIDDEN		FAT16_PART0_START
// FAT16 partition boot block #
// Must be the first block of the partition.
#define FAT16_BOOT_BLOCK	FAT16_PART0_START
// Reserved blocks zone starts at boot block
#define FAT16_RESERVED		FAT16_BOOT_BLOCK
// Reserved blocks
// This can be set to any arbitrary value (min 1 for the boot block);
// but we don't need any reserved space for this device, so we reserve
// only the boot block.
#define FAT16_RESERVED_SIZE	1

// Max number of entries in the root directory.
// This value * 32 must be an integer multiple of the block size.
// Set to 512 for maximum compatibility.
#define FAT16_ROOT_ENTRIES	512
// Size of the root directory, in blocks
#define FAT16_ROOT_SIZE		((32*FAT16_ROOT_ENTRIES)/MSC_BLOCK_SIZE)

// Size of the FAT
// The FAT size depends on the number of data clusters, which depends on
// the size of the FAT. We don't want to spend memory in mathematics, so
// we compute an over-estimated value, which is fine since our virtual
// disk space costs absolutly nothing.
#define FAT16_FAT_SIZE		(((((FAT16_PART0_SIZE-(FAT16_RESERVED_SIZE+FAT16_ROOT_SIZE))/FAT16_CLUSTER_SIZE)*2)+(MSC_BLOCK_SIZE-1))/MSC_BLOCK_SIZE)

// Start of the file data zone (data clusters) in blocks
// Offset from start of partition and absolute block value
#define FAT16_DATA_OFFSET	(FAT16_RESERVED_SIZE+(2*FAT16_FAT_SIZE)+FAT16_ROOT_SIZE)
#define FAT16_DATA_START	(FAT16_RESERVED+FAT16_DATA_OFFSET)

// Total number of data clusters on the partition
// Since we estimated the FAT size up, and the division rounds down, we are
// sure that all clusters fit into the FAT.
#define FAT16_NUM_CLUSTERS	((FAT16_PART0_SIZE-FAT16_DATA_OFFSET)/FAT16_CLUSTER_SIZE)

// Start addresses (blocks) of the two FATs
#define FAT16_FAT1			(FAT16_RESERVED+FAT16_RESERVED_SIZE)
#define FAT16_FAT2			(FAT16_FAT1+FAT16_FAT_SIZE)

// Start address (block) of the root directory
#define FAT16_ROOT_BLOCK	(FAT16_FAT2+FAT16_FAT_SIZE)

// Data cluster # to block #
// The first data cluster is numbered 2 (FAT convention).
#define FAT16_CLUSTER_TO_BLOCK(c) \
	(FAT16_DATA_START+(((c)-2)*FAT16_CLUSTER_SIZE))

// Start addresses (cluster and block) of the firmware file
// If the first cluster is above 16, one *may* have to modify the FAT block
// generation in the MSC_NextBlock() function.
#define FIRMWARE_CLUSTER_START	2
#define FIRMWARE_BLOCK_START	FAT16_CLUSTER_TO_BLOCK(FIRMWARE_CLUSTER_START)


//=========================================================================
// Disk image structures
//-------------------------------------------------------------------------
// We can't afford a full disk image - lack of memory space.
// So we use minimalist file system structures and use them to construct
// the file system image on the fly on the block read requests.
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// MBR
//-------------------------------------------------------------------------
// Conditionaly compiled depending of the MBR_BLOCK parameter to save space
//-------------------------------------------------------------------------

#if ( MBR_BLOCK >= 0 )

typedef struct __attribute__((packed))
{
	uint8_t		boot_flag;
	uint8_t		start_head;
	uint16_t	start_cyl_sect;
	uint8_t		partition_type;
	uint8_t		end_head;
	uint16_t	end_cyl_sect;
	uint32_t	starting_block;
	uint32_t	total_blocks;
}
MBR_Entry_t;

const MBR_Entry_t MBR_Entry0 =
{
	0x00,	// non-bootable - maybe one day
	0x00,	// CHS data - useless
	0x0000,	// idem
	0x04,	// FAT16
	0x00,	// idem
	0x0000,	// idem
	FAT16_PART0_START,	
	FAT16_PART0_SIZE
};

#endif


//-------------------------------------------------------------------------
// BIOS Parameter block (Boot sector)
//-------------------------------------------------------------------------

typedef struct __attribute__((packed))
{
	uint8_t 	boot_jmp[3];
	char		format_os[8];
	uint16_t	bytes_per_sector;
	uint8_t		sectors_per_cluster;
	uint16_t	reserved_sectors;
	uint8_t		number_of_fats;
	uint16_t	root_max_entries;
	uint16_t	num_sectors_16;
	uint8_t		media_desc;
	uint16_t	sectors_per_fat_16;
	uint16_t	sectors_per_track;
	uint16_t	number_of_heads;
	uint32_t	hidden_sectors;
	uint32_t	num_sectors_32;
	uint8_t		drive_number;
	uint8_t		reserved2[1];
	uint8_t		ext_boot_sig;
	uint32_t	volume_serial;
	char		volume_label[11];
	char		fs_type[8];
}
FAT16_Block0_t;

const FAT16_Block0_t FAT16_Block0 =
{
	{ 0xEB, 0x3C, 0x90 },
	"MSWIN4.1",
	MSC_BLOCK_SIZE,
	FAT16_CLUSTER_SIZE,
	FAT16_RESERVED_SIZE,
	2,
	FAT16_ROOT_ENTRIES,
	FAT16_PART0_SIZE,
	0xF8,
	FAT16_FAT_SIZE,
	0,
	0,
	FAT16_HIDDEN,
	0,
	0x80,
	{ 0 },
	0x29,
	0x12345678,
	"MYEVIC     ",
	"FAT16   "
};


//-------------------------------------------------------------------------
// File entries
//-------------------------------------------------------------------------

typedef struct __attribute__((packed))
{
	char		FileName[8];
	char		Extension[3];
	uint8_t		Attributes;
	uint8_t		reserved1;
	uint8_t		CreationMs;
	uint16_t	CreationTime;
	uint16_t	CreationDate;
	uint16_t	LastAccesDate;
	uint16_t	reserved2;
	uint16_t	ModifTime;
	uint16_t	ModifDate;
	uint16_t	StartCluster;
	uint32_t	FileSize;
}
FAT16_FileEntry_t;

// Volume name file entry

const FAT16_FileEntry_t VNFileEntry =
{
	"MYEVIC  ",
	"   ",
	0x08,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

// Firmware file entry
// This is not const since it must be populated at run-time,
// for two reasons:
// - Firmware size is only known after linkage, and there is
//   no simple way to set the value in the structure.
// - __TIME__ and __DATE__ macros are *not* constant strings,
//   ans thus cannot initialize const struct members. (why???!)

FAT16_FileEntry_t FWFileEntry =
{
	"MYEVIC  ",
	"BIN",
	0x01,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	FIRMWARE_CLUSTER_START,
	0
};

__myevic__ void SetFileEntryDates( FAT16_FileEntry_t *pfe )
{
	pfe->CreationTime = FAT16_NOW;
	pfe->CreationDate = FAT16_TODAY;
	pfe->LastAccesDate = FAT16_TODAY;
	pfe->ModifTime = FAT16_NOW;
	pfe->ModifDate = FAT16_TODAY;
}


//=========================================================================
// Initialization
//-------------------------------------------------------------------------
__myevic__ void MSC_Init()
{
	gMSC_Flags.bulk_in_ready = 1;
	gMSC_Flags.csw_ready = 0;
	gMSC_Flags.stalled = 0;

	gRxMode = MSC_WAITING_CBW;

	gTxNumBlocks = 0;
	gTxDataLength = 0;
	gTxDataIndex = 0;
}


//=========================================================================
// Class-Specific Requests
//-------------------------------------------------------------------------
__myevic__ void MSC_ClassRequest( uint8_t *token )
{
	if ( token[0] & 0x80 )
	{
		switch ( token[1] )
		{
			case MSC_GET_MAX_LUN:
                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = 0;
                /* Data stage */
                USBD_SET_DATA1( EP0 );
                USBD_SET_PAYLOAD_LEN( EP0, 1 );
                /* Status stage */
                USBD_PrepareCtrlOut( 0, 0 );
				break;

			default:
				USBD_SetStall( EP0 );
				break;
		}
	}
	else
	{
		switch ( token[1] )
		{
			case MSC_BOMSR:
				gTxNumBlocks = 0;
				gTxDataLength = 0;

				USBD_LockEpStall( 0 );

				/* Clear ready */
				USBD->EP[EP5].CFGP |= USBD_CFGP_CLRRDY_Msk;
				USBD->EP[EP6].CFGP |= USBD_CFGP_CLRRDY_Msk;

				gMSC_Flags.stalled = 0;
				gMSC_Flags.bulk_in_ready = 1;

				/* Prepare to receive the CBW */
				gRxMode = MSC_WAITING_CBW;

				USBD_SET_DATA1( EP6 );
				USBD_SET_EP_BUF_ADDR( EP6, EP6_BUF_BASE );
				USBD_SET_PAYLOAD_LEN( EP6, EP6_MAX_PKT_SIZE );

                /* Status stage */
                USBD_SET_DATA1( EP0 );
                USBD_SET_PAYLOAD_LEN( EP0, 0 );
				break;

			default:
				USBD_SetStall( EP0 );
				break;
		}
	}
}


//=========================================================================
__myevic__ void MSC_Stall()
{
	USBD_SET_EP_STALL( EP5 );
	USBD_SET_EP_STALL( EP6 );
	USBD_LockEpStall( ( 1 << EP5 ) | ( 1 << EP6 ) );
	gMSC_Flags.stalled = 1;
	gMSC_Flags.csw_ready = 0;
	gTxNumBlocks = 0;
	gTxDataLength = 0;
	gCBW.CB.OPCode = 0;
}


//=========================================================================
__myevic__ void MSC_SendStatus()
{
	if ( gMSC_Flags.bulk_in_ready && gMSC_Flags.csw_ready )
	{
		gMSC_Flags.bulk_in_ready = 0;
		gMSC_Flags.csw_ready = 0;

		USBD_MemCopy( MSC_BULK_IN_BUF, (uint8_t*)&gCSW, CSW_SIZE );
		USBD_SET_PAYLOAD_LEN( EP5, CSW_SIZE );
	}
}


//=========================================================================
__myevic__ void MSC_AckCmd()
{
	gMSC_Flags.csw_ready = 1;
	MSC_SendStatus();
}


//=========================================================================
__myevic__ void MSC_NextBlock()
{
	if ( gTxNumBlocks )
	{
		gTxDataIndex = 0;
		gTxDataLength = MSC_BLOCK_SIZE;

		MemClear( gTxDataBlock, gTxDataLength );

		#if ( MBR_BLOCK >= 0 )
		if ( gTxLBA == MBR_BLOCK )
		{
			// MBR Block
			USBD_MemCopy( &gTxDataBlock[446], (uint8_t*)&MBR_Entry0, sizeof(MBR_Entry0) );
			gTxDataBlock[510] = 0x55;
			gTxDataBlock[511] = 0xAA;
		}
		else
		#endif
		if ( gTxLBA == FAT16_BOOT_BLOCK )
		{
			// Boot Block
			USBD_MemCopy( gTxDataBlock, (uint8_t*)&FAT16_Block0, sizeof(FAT16_Block0) );
			gTxDataBlock[510] = 0x55;
			gTxDataBlock[511] = 0xAA;
		}
		else if (( gTxLBA == FAT16_FAT1 ) || ( gTxLBA == FAT16_FAT2 ))
		{
			// FAT
			// In the worst case scenario, firmware is 0x1E000 bytes long
			// and uses 240 clusters (0x200 block size, 1 block per cluster).
			// With the clusters 0 & 1, this uses 488 bytes of FAT and
			// holds on the first FAT sector.
			// Thus, we handle only the first FAT sector and ignore the others.

			gTxDataBlock[0] = 0xF8;
			gTxDataBlock[1] = 0xFF;

			gTxDataBlock[2] = 0xFF;
			gTxDataBlock[3] = 0xFF;

			uint16_t CurrentCluster = FIRMWARE_CLUSTER_START;
			uint32_t AllocSize = GetFirmwareSize();
			while ( 1 )
			{
				if ( AllocSize > FAT16_CLUSTER_BYTES )
				{
					gTxDataBlock[CurrentCluster*2  ] = (CurrentCluster+1)&0xFF;
					gTxDataBlock[CurrentCluster*2+1] = ((CurrentCluster+1)>>8)&0xFF;
				}
				else
				{
					gTxDataBlock[CurrentCluster*2  ] = 0xFF;
					gTxDataBlock[CurrentCluster*2+1] = 0xFF;
					break;
				}
				++CurrentCluster;
				AllocSize -= FAT16_CLUSTER_BYTES;
			}
		}
		else if ( gTxLBA == FAT16_ROOT_BLOCK )
		{
			// Root directory
			// Volume name entry
			USBD_MemCopy( &gTxDataBlock[0], (uint8_t*)&VNFileEntry, sizeof(VNFileEntry) );
			// Firmware file entry
			SetFileEntryDates( &FWFileEntry );
			FWFileEntry.FileSize = GetFirmwareSize();
			USBD_MemCopy( &gTxDataBlock[32], (uint8_t*)&FWFileEntry, sizeof(FWFileEntry) );
		}
		else if ( gTxLBA >= FIRMWARE_BLOCK_START )
		{
			// Firmware file data
			uint32_t fwSize   = GetFirmwareSize();
			uint32_t fwOffset = ( gTxLBA - FIRMWARE_BLOCK_START ) * MSC_BLOCK_SIZE;
			if ( fwOffset < fwSize )
			{
				SYS_UnlockReg();
				FMC_ENABLE_ISP();

				for ( uint32_t rdLength = 0 ; rdLength < MSC_BLOCK_SIZE ; )
				{
					FMCRead256( fwOffset + rdLength, (uint32_t*)&gTxDataBlock[rdLength] );
					rdLength += 0x100;
				}

				FMC_DISABLE_ISP();
				SYS_LockReg();
				
				// Encrypt firmware
				for ( int i = 0 ; i < MSC_BLOCK_SIZE ; ++i )
				{
					uint8_t gun = fwSize + 408376 + fwOffset + i - fwSize / 408376;
					gTxDataBlock[ i ] ^= gun;
				}
			}
		}

		gCSW.DataResidue -= gTxDataLength;

		--gTxNumBlocks;
		++gTxLBA;
	}
	else
	{
		gTxDataIndex = 0;
		gTxDataLength = 0;
	}
}


//=========================================================================
__myevic__ void MSC_SendData()
{
	uint32_t ui32Len;

	if ( !gTxDataLength )
	{
		if ( gTxNumBlocks )
		{
			MSC_NextBlock();
		}
	}

	if ( gMSC_Flags.bulk_in_ready )
	{
		if ( gTxDataLength )
		{
			gMSC_Flags.bulk_in_ready = 0;
			ui32Len = ( gTxDataLength < EP5_MAX_PKT_SIZE ) ? gTxDataLength : EP5_MAX_PKT_SIZE;

			USBD_MemCopy( MSC_BULK_IN_BUF, &gTxDataBlock[gTxDataIndex], ui32Len );

			gTxDataIndex  += ui32Len;
			gTxDataLength -= ui32Len;

			USBD_SET_PAYLOAD_LEN( EP5, ui32Len );
		}
	}
}


//=========================================================================
__myevic__ void MSC_UnsupportedCommand()
{
	// INVALID COMMAND OPERATION CODE
	UFI_RQSenseData.SenseKey = 0x05;
	UFI_RQSenseData.AddlSenseCode = 0x20;
	UFI_RQSenseData.AddlSenseCodeQualifier = 0x00;

	gCSW.Status = CSW_STATUS_FAILED;
	MSC_AckCmd();
}


//=========================================================================
__myevic__ void MSC_SendSimpleResponse( uint8_t *data, uint32_t length )
{
	gTxDataIndex = 0;
	gTxDataLength = length;
	gCSW.DataResidue -= gTxDataLength;
	USBD_MemCopy( gTxDataBlock, data, gTxDataLength );
	MSC_SendData();
	MSC_AckCmd();
}


//=========================================================================
__myevic__ void MSC_RequestSense()
{
	MSC_SendSimpleResponse( (uint8_t*)&UFI_RQSenseData, sizeof(UFI_RQSenseData) );
}


//=========================================================================
__myevic__ void MSC_Inquiry()
{
	gTxDataIndex = 0;
	switch ( gCBW.CB.Inquiry.PageCode )
	{
		case 0x00:
			gTxDataLength = sizeof(MSC_InquiryResp);
			USBD_MemCopy( gTxDataBlock, (uint8_t*)&MSC_InquiryResp, gTxDataLength );
			break;
		case 0x80:
			gTxDataLength = sizeof(MSC_Inquiry80);
			USBD_MemCopy( gTxDataBlock, (uint8_t*)&MSC_Inquiry80, gTxDataLength );
			break;
		default:
			gTxDataLength = 0;
			USBD_SET_EP_STALL( EP6 );
			USBD_LockEpStall( 1 << EP6 );
			break;
	}
	if ( gTxDataLength > gCBW.DataTransferLength )
	{
		gTxDataLength = gCBW.DataTransferLength;
	}
	gCSW.DataResidue -= gTxDataLength;
	MSC_SendData();
	MSC_AckCmd();
}


//=========================================================================
__myevic__ void MSC_ModeSense()
{
	gTxDataIndex = 0;
	gTxDataLength = 4;

	gTxDataBlock[0] = 0x03;	// data length
	gTxDataBlock[1] = 0x00;	// medium type
	gTxDataBlock[2] = 0x80;	// write protection
	gTxDataBlock[3] = 0x00;	// block descriptor length

	gCSW.DataResidue -= gTxDataLength;
	MSC_SendData();
	MSC_AckCmd();
}


//=========================================================================
__myevic__ void MSC_PreventMediaRemoval()
{
	if ( gCBW.CB.PreventAllowMR.Prevent & 1 )
	{
		// INVALID FIELD IN PARAMETER LIST
		UFI_RQSenseData.SenseKey = 0x05;
		UFI_RQSenseData.AddlSenseCode = 0x26;
		UFI_RQSenseData.AddlSenseCodeQualifier = 0x00;

		gCSW.Status = CSW_STATUS_FAILED;
		MSC_AckCmd();
	}
	else
	{
		MSC_AckCmd();
	}
}


//=========================================================================
__myevic__ void MSC_ReadFormatCapacities()
{
	MSC_SendSimpleResponse( (uint8_t*)&UFI_RFCResponse, sizeof(UFI_RFCResponse) );
}


//=========================================================================
__myevic__ void MSC_ReadCapacity()
{
	MSC_SendSimpleResponse( (uint8_t*)&UFI_RCResponse, sizeof(UFI_RCResponse) );
}


//=========================================================================
__myevic__ void MSC_Read10()
{
	gTxDataIndex = 0;
	gTxNumBlocks  = (uint32_t)gCBW.CB.Read10.TransferLengthH << 8;
	gTxNumBlocks += (uint32_t)gCBW.CB.Read10.TransferLengthL;
	if ( !gTxNumBlocks )
	{
		MSC_AckCmd();
		return;
	}
	gTxLBA  = (uint32_t)gCBW.CB.Read10.LBA[0] << 24;
	gTxLBA += (uint32_t)gCBW.CB.Read10.LBA[1] << 16;
	gTxLBA += (uint32_t)gCBW.CB.Read10.LBA[2] << 8;
	gTxLBA += (uint32_t)gCBW.CB.Read10.LBA[3];
	MSC_NextBlock();
	MSC_SendData();
	if ( !gTxNumBlocks )
	{
		MSC_AckCmd();
	}
}


//=========================================================================
__myevic__ void MSC_ProcessCommand()
{
	if ( gCBW.Signature != CBW_SIGNATURE )
	{
		MSC_Stall();
		return;
	}

	gCSW.Signature = CSW_SIGNATURE;
	gCSW.Tag = gCBW.Tag;
	gCSW.DataResidue = gCBW.DataTransferLength;
	gCSW.Status = CSW_STATUS_SUCCESS;

	if ( !( gCBW.Flags & 0x80 ) && ( gCSW.DataResidue > 0 ) )
	{
		gRxMode = MSC_WAITING_DATA;
	}
	else
	{
		gRxMode = MSC_WAITING_NONE;
	}

	switch ( gCBW.CB.OPCode )
	{
		case UFI_TEST_UNIT_READY:
			MSC_AckCmd();
			break;

		case UFI_REQUEST_SENSE:
			MSC_RequestSense();
			break;
			
		case UFI_INQUIRY:
			MSC_Inquiry();
			break;
			
		case UFI_MODE_SENSE:
			MSC_ModeSense();
			break;

		case UFI_PREVENT_MEDIA_REMOVAL:
			MSC_PreventMediaRemoval();
			break;

		case UFI_READ_FORMAT_CAPACITIES:
			MSC_ReadFormatCapacities();
			break;
		
		case UFI_READ_CAPACITY:
			MSC_ReadCapacity();
			break;
			
		case UFI_READ_10:
			MSC_Read10();
			break;

		default:
			MSC_UnsupportedCommand();
			break;
	}
}


//=========================================================================
// Config Callback
//-------------------------------------------------------------------------
void MSC_SetConfig(void)
{
    // Clear stall status and ready
    USBD->EP[5].CFGP = 1;
    USBD->EP[6].CFGP = 1;
    /*****************************************************/
    /* EP4 ==> Bulk IN endpoint, address 3 */
    USBD_CONFIG_EP(EP5, USBD_CFG_EPMODE_IN | MSC_BULK_IN_EP_NUM);
    /* Buffer range for EP4 */
    USBD_SET_EP_BUF_ADDR(EP5, EP5_BUF_BASE);

    /* EP5 ==> Bulk Out endpoint, address 4 */
    USBD_CONFIG_EP(EP6, USBD_CFG_EPMODE_OUT | MSC_BULK_OUT_EP_NUM);
    /* Buffer range for EP5 */
    USBD_SET_EP_BUF_ADDR(EP6, EP6_BUF_BASE);

    /* trigger to receive OUT data */
    USBD_SET_PAYLOAD_LEN(EP6, EP6_MAX_PKT_SIZE);

    USBD_LockEpStall(0);

	MSC_Init();
}


//=========================================================================
// Event Handlers
//-------------------------------------------------------------------------
__myevic__ void MSC_EP5Handler()
{
    /* Bulk IN */
	gMSC_Flags.bulk_in_ready = 1;

	if ( gMSC_Flags.stalled )
		return;

	if ( !gTxDataLength && gTxNumBlocks )
	{
		MSC_NextBlock();
		MSC_SendData();

		if ( !gTxNumBlocks )
		{
			MSC_AckCmd();
		}
		return;
	}

	if ( gTxDataLength )
	{
		MSC_SendData();
	}
	else if ( gMSC_Flags.csw_ready )
	{
		MSC_SendStatus();
	}
	else
	{
		gRxMode = MSC_WAITING_CBW;
		USBD_SET_DATA1( EP6 );
		USBD_SET_PAYLOAD_LEN( EP6, EP6_MAX_PKT_SIZE );
	}
}

//-------------------------------------------------------------------------
__myevic__ void MSC_EP6Handler()
{
	/* Bulk OUT */
	uint32_t ui32Len = USBD_GET_PAYLOAD_LEN( EP6 );

	switch ( gRxMode )
	{
		case MSC_WAITING_CBW:
			if ( ui32Len >= CBW_SIZE )
			{
				ui32Len = CBW_SIZE;
				USBD_MemCopy( (uint8_t*)&gCBW, MSC_BULK_OUT_BUF, ui32Len );
				MSC_ProcessCommand();
			}
			else
			{
				MSC_Stall();
                return;
			}
			break;

		case MSC_WAITING_CSW:
		case MSC_WAITING_DATA:
			USBD_SET_PAYLOAD_LEN( EP6, EP6_MAX_PKT_SIZE );
			break;

		case MSC_WAITING_NONE:
			MSC_Stall();
			return;
	}
}


