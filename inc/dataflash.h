#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

#include "M451Series.h"

//=========================================================================
// Configuration constants
//-------------------------------------------------------------------------

// End of APROM address (on a 128k chip)
#define DATAFLASH_APROM_END			0x00020000

// Time & Puffs counters space
#define DATAFLASH_TIMECNTR_BASE		(DATAFLASH_APROM_END-FMC_FLASH_PAGE_SIZE)
#define DATAFLASH_PUFFCNTR_BASE		(DATAFLASH_TIMECNTR_BASE-FMC_FLASH_PAGE_SIZE)

// Available space for parameters
#define DATAFLASH_PARAMS_SPACE		(2*FMC_FLASH_PAGE_SIZE)
#define DATAFLASH_PARAMS_END		DATAFLASH_PUFFCNTR_BASE
#define DATAFLASH_PARAMS_BASE		(DATAFLASH_PARAMS_END-DATAFLASH_PARAMS_SPACE)

// Profiles
#define DATAFLASH_PROFILES_SPACE	FMC_FLASH_PAGE_SIZE
#define DATAFLASH_PROFILES_END		DATAFLASH_PARAMS_BASE
#define DATAFLASH_PROFILES_BASE		(DATAFLASH_PROFILES_END-DATAFLASH_PROFILES_SPACE)

#define DATAFLASH_PROFILES_MAX		7

// Custom Logo space
// Those addresses are hardcoded in Joyetech's custom logo utility so
//  we have to hardcode them here too.
#define DATAFLASH_LOGO_SIZE			0x200
#define DATAFLASH_LOGO_1327_BASE	0x19000
#define DATAFLASH_LOGO_1306_BASE	0x19200

// To read product ID
#define LDROM_BASE 0x100000
#define LDROM_SIZE 0x1000

// NFE Magic Number
#define DATAFLASH_NFE_MAGIC	0xFE


//=========================================================================
// DataFlash content structure
// Padding bytes added by compiler to ensure correct alignements are
//  indicated and can eventually be used to store usefull parameters
//  without increasing the total data size.
// Useless fields may also be recycled.
//-------------------------------------------------------------------------

typedef struct
{
/* 00000001 */	unsigned int off:1;
/* 00000002 */	unsigned int keylock:1;
/* 00000004 */	unsigned int flipped:1;
/* 00000008 */	unsigned int nologo:1;
/* 00000010 */	unsigned int clock:1;
/* 00000020 */	unsigned int vcom:1;
/* 00000040 */	unsigned int storage:1;
/* 00000080 */	unsigned int dbgena:1;

/* 00000100 */	unsigned int x32off:1;
/* 00000200 */	unsigned int priopwr:1;
/* 00000400 */	unsigned int onewatt:1;
/* 00000800 */	unsigned int digclk:1;
/* 00001000 */	unsigned int battpc:1;
/* 00002000 */	unsigned int phpct:1;
/* 00004000 */	unsigned int wakeonpm:1;
/* 00008000 */	unsigned int font:1;

/* 00010000 */	unsigned int nfe:1;
/* 00020000 */	unsigned int dfmt1:1;
/* 00040000 */	unsigned int invert:1;
/* 00080000 */	unsigned int vapedml:1;
/* 00100000 */	unsigned int onedegree:1;
/* 00200000 */	unsigned int battv:1;
/* 00400000 */	unsigned int lsloff:1;
/* 00800000 */	unsigned int logomid:1;

/* 01000000 */	unsigned int timebig:1;
/* 02000000 */	unsigned int usbchgoff:1;
/* 04000000 */	unsigned int chkmodeoff:1;
/* 08000000 */	unsigned int dfmt2:1;
/* 10000000 */	unsigned int pcurve:1;

// Do not exceed 32 bits;
// if you may do so, create another bitfield.
}
dfStatus_t;

typedef struct
{
	uint16_t P;
	uint16_t I;
	uint16_t D;
}
dfPID_t;

typedef struct
{
	uint8_t	percent[9];
	uint8_t voltage[11];
	uint8_t	cutoff;
	uint8_t	maxamp;
}
dfBattery_t;

#define PWR_CURVE_PTS	10

typedef struct
{
	uint8_t time;
	uint8_t power;
}
dfPCPoint_t;

typedef struct dfParams
{
/* 0000 */	uint32_t	PCRC;
/* 0004 */	uint32_t	HWVersion;
/* 0008 */	uint8_t		Magic;
/* 0009 */	uint8_t		BootFlag;
/* 000A */	uint8_t		Mode;
/* 000B */	uint8_t		Protec;			//	former 1-byte pad
/* 000C */	uint16_t	Power;
/* 000E */	uint16_t	Temp;
/* 0010 */	uint16_t	TCPower;
/* 0012 */	uint16_t	VWVolts;
/* 0014 */	uint8_t		APT;
/* 0015 */	uint8_t		RezType;
/* 0016 */	uint8_t		TempAlgo;
/* 0017 */	uint8_t		IsCelsius;
/* 0018 */	uint16_t	Resistance;
/* 001A */	uint16_t	RezTI;
/* 001C */	uint16_t	RezNI;
/* 001E */	uint8_t		RezLockedTI;
/* 001F */	uint8_t		RezLockedNI;
/* 0020 */	uint8_t		TiOn;			//	useless
/* 0021 */	uint8_t		StealthOn;
/* 0022 */	uint16_t	TempCoefsNI;
/* 0024 */	dfBattery_t	Battery;
/* 003A */	dfPCPoint_t	PwrCurve[PWR_CURVE_PTS];
/* 004E */	uint8_t		Unused4E[38];
/* 0074 */	uint16_t	TempCoefsTI;
/* 0076 */	uint16_t	LEDColor;		//	former 2-bytes pad
/* 0078 */	dfStatus_t	Status;
/* 007C */	uint16_t	AtoRez;			//	useless
/* 007E */	uint8_t		AtoStatus;		//	useless
/* 007F */	uint8_t		ShuntRez;		//	former 1-byte pad
/* 0080 */	uint16_t	RezSS;
/* 0082 */	uint8_t		RezLockedSS;
/* 0083 */	uint8_t		UIVersion;		//	useless
/* 0084 */	uint8_t		TCRIndex;
/* 0085 */	uint8_t		ScrMainTime;	//	former 1-byte pad
/* 0086 */	uint16_t	TCRM[3];
/* 008C */	uint16_t	RezTCR;
/* 008E */	uint8_t		RezLockedTCR;
/* 008F */	uint8_t		ScreenSaver;	//	former LED color
/* 0090 */	uint8_t		TCMode;
/* 0091 */	uint8_t		ScreenProt;		//	former 1-byte pad
/* 0092 */	uint16_t	SavedCfgRez[10];
/* 00A6 */	uint16_t	SavedCfgPwr[10];
/* 00BA */	uint16_t	FBBest;
/* 00BC */	uint8_t		FBSpeed;
/* 00BD */	uint8_t		BattPC;			//	unused
/* 00BE */	uint8_t		Contrast;
/* 00BF */	uint8_t		ModesSel;
/* 00C0 */	uint16_t	ClkRatio;
/* 00C2 */	uint16_t	VVRatio;
/* 00C4 */	uint8_t		PHDelay;
/* 00C5 */	uint8_t		Clicks[3];
/* 00C8 */	uint8_t		DimTimeout;
/* 00C9 */	uint8_t		BatteryModel;
/* 00CA */	int8_t		UnusedCA[3];
/* 00CD */	uint8_t		PreheatTime;
/* 00CE */	uint16_t	PreheatPwr;
/* 00D0 */	uint16_t	AtoPreheatPwr;
/* 00D2 */	uint8_t		TCAlgo;
/* 00D3 */	uint8_t		TCBoost;
/* 00D4 */	uint16_t	TCRP[3];
/* 00DA */	dfPID_t		PID;
/* 00E0 */	uint16_t	Millis;
/* 00E2 */	uint8_t		Profile;
/* 00E3 */	int8_t		BVOffset[4];
}
dfParams_t;


//-------------------------------------------------------------------------
// Information part
// This part is stored in RAM but never written to the DataFlash.
// It contains usefull infos that can be retreived in plus of the
//  parameters part by using the HID_CMD_GETINFO usb command.
// Used by firwmare update software.
//-------------------------------------------------------------------------

typedef struct
{
/* 0000 */	uint32_t	FWVersion;
/* 0004 */	uint32_t	LDVersion;
/* 0008 */	uint32_t	fmcCID;
/* 000C */	uint32_t	fmcDID;
/* 0010 */	uint32_t	fmcPID;
/* 0014 */	uint32_t	fmcUID[3];
/* 0020 */	uint32_t	fmcUCID[4];
/* 0030 */	uint32_t	PuffCount;
/* 0034 */	uint32_t	TimeCount;
/* 0038 */	uint32_t	ProductID;
/* 003C */	uint32_t	MaxHWVersion;
/* 0040 */	uint16_t	Year;
/* 0042 */	uint8_t		Month;
/* 0043 */	uint8_t		Day;
/* 0044 */	uint8_t		Hour;
/* 0045 */	uint8_t		Minute;
/* 0046 */	uint8_t		Second;
/* 0047 */	uint8_t		Format;
/* 0048 */	uint32_t	Build;
}
dfInfos_t;


//-------------------------------------------------------------------------
// NFE Block
//-------------------------------------------------------------------------

typedef struct
{
	uint32_t	Build;
}
dfNFEBlock_t;


#define ALIGN256(x) (((x)&0xff)?((x)-((x)&0xff)+0x100):(x))

#define DATAFLASH_PARAMS_SIZE	ALIGN256(sizeof(dfParams_t))
#define DATAFLASH_INFOS_SIZE	ALIGN256(sizeof(dfInfos_t))
#define DATAFLASH_NFEBLOCK_SIZE	ALIGN256(sizeof(dfNFEBlock_t))
#define DATAFLASH_FREE_SIZE		(FMC_FLASH_PAGE_SIZE-DATAFLASH_PARAMS_SIZE-DATAFLASH_INFOS_SIZE-DATAFLASH_NFEBLOCK_SIZE)

//-------------------------------------------------------------------------
// It's important for DATAFLASH_PARAMS_SIZE to be a divider of
//	DATAFLASH_PARAMS_SPACE for all the functions of cycling to work
//	properly. Unfortunatly, we can't check this in the preprocessor
//	due to the sizeof() operation.
// It won't be a problem as long as the size of the parameters do not
//	exceed 0x200 bytes. Just be aware.
// In any case, parameters size should never exceed 0x500 bytes.
//-------------------------------------------------------------------------


//=============================================================================

typedef union
{
	int8_t c[0x100];
	uint8_t uc[0x100];
	int16_t s[0x100/sizeof(int16_t)];
	uint16_t us[0x100/sizeof(uint16_t)];
	int32_t l[0x100/sizeof(int32_t)];
	uint32_t ul[0x100/sizeof(uint32_t)];
} Playfield_t;


typedef struct dfStruct
{
	uint32_t Checksum;
	union
	{
		dfParams_t	p;
		uint32_t	params[DATAFLASH_PARAMS_SIZE/4];
	};
	union
	{
		dfInfos_t	i;
		uint32_t	infos[DATAFLASH_INFOS_SIZE/4];
	};
	union
	{
		dfNFEBlock_t	n;
		uint32_t		nfe[DATAFLASH_NFEBLOCK_SIZE/4];
	};
	union
	{
		uint32_t	free_pages[DATAFLASH_FREE_SIZE/4];
		Playfield_t	playfield;
	};
}
dfStruct_t;


//-------------------------------------------------------------------------

extern dfStruct_t DataFlash;

//-------------------------------------------------------------------------
// Accessor macros
// Used for better code readability
//-------------------------------------------------------------------------


#define DFP(x) DataFlash.p.x
#define DFI(x) DataFlash.i.x
#define DFN(x) DataFlash.n.x

#define dfChecksum		DataFlash.Checksum
#define dfCRC			DFP(PCRC)
#define dfHWVersion		DFP(HWVersion)
#define dfMagic			DFP(Magic)
#define dfBootFlag		DFP(BootFlag)
#define dfMode			DFP(Mode)
#define dfProtec		DFP(Protec)
#define dfPower			DFP(Power)
#define dfTemp			DFP(Temp)
#define dfTCPower		DFP(TCPower)
#define dfVWVolts		DFP(VWVolts)
#define dfAPT			DFP(APT)
#define dfRezType		DFP(RezType)
#define dfTempAlgo		DFP(TempAlgo)
#define dfIsCelsius		DFP(IsCelsius)
#define dfResistance	DFP(Resistance)
#define dfRezTI			DFP(RezTI)
#define dfRezNI			DFP(RezNI)
#define dfRezLockedTI	DFP(RezLockedTI)
#define dfRezLockedNI	DFP(RezLockedNI)
#define dfTiOn			DFP(TiOn)
#define dfStealthOn		DFP(StealthOn)
#define dfTempCoefsNI	DFP(TempCoefsNI)
#define dfBattery		DFP(Battery)
#define dfPwrCurve		DFP(PwrCurve)
#define dfTempCoefsTI	DFP(TempCoefsTI)
#define dfLEDColor		DFP(LEDColor)
#define dfStatus		DFP(Status)
#define dfAtoRez		DFP(AtoRez)
#define dfAtoStatus		DFP(AtoStatus)
#define dfShuntRez		DFP(ShuntRez)
#define dfRezSS			DFP(RezSS)
#define dfRezLockedSS	DFP(RezLockedSS)
#define dfUIVersion		DFP(UIVersion)
#define dfTCRIndex		DFP(TCRIndex)
#define dfScrMainTime	DFP(ScrMainTime)
#define dfTCRM			DFP(TCRM)
#define dfRezTCR		DFP(RezTCR)
#define dfRezLockedTCR	DFP(RezLockedTCR)
#define dfScreenSaver	DFP(ScreenSaver)
#define dfTCMode		DFP(TCMode)
#define dfScreenProt	DFP(ScreenProt)
#define dfSavedCfgRez	DFP(SavedCfgRez)
#define dfSavedCfgPwr	DFP(SavedCfgPwr)
#define dfFBBest		DFP(FBBest)
#define dfFBSpeed		DFP(FBSpeed)
#define dfBattPC		DFP(BattPC)
#define dfContrast		DFP(Contrast)
#define dfModesSel		DFP(ModesSel)
#define dfClkRatio		DFP(ClkRatio)
#define dfVVRatio		DFP(VVRatio)
#define dfPHDelay		DFP(PHDelay)
#define dfClick			DFP(Clicks)
#define dfDimTimeout	DFP(DimTimeout)
#define dfBatteryModel	DFP(BatteryModel)
#define dfBVOffset		DFP(BVOffset)
#define dfPreheatTime	DFP(PreheatTime)
#define dfPreheatPwr	DFP(PreheatPwr)
#define dfAtoPreheatPwr	DFP(AtoPreheatPwr)
#define dfTCAlgo		DFP(TCAlgo)
#define dfTCBoost		DFP(TCBoost)
#define dfTCRP			DFP(TCRP)
#define dfPID			DFP(PID)
#define dfMillis		DFP(Millis)
#define dfProfile		DFP(Profile)

#define dfFWVersion		DFI(FWVersion)
#define dffmcCID        DFI(fmcCID)
#define dffmcDID        DFI(fmcDID)
#define dffmcPID        DFI(fmcPID)
#define dffmcUID        DFI(fmcUID)
#define dffmcUCID       DFI(fmcUCID)
#define dfPuffCount     DFI(PuffCount)
#define dfTimeCount     DFI(TimeCount)
#define dfProductID     DFI(ProductID)
#define dfMaxHWVersion  DFI(MaxHWVersion)
#define dfYear			DFI(Year)
#define dfMonth			DFI(Month)
#define dfDay			DFI(Day)
#define dfHour			DFI(Hour)
#define dfMinute		DFI(Minute)
#define dfSecond		DFI(Second)

#define dfBuild			DFN(Build)

#define gPlayfield		DataFlash.playfield


//=========================================================================
// Some globals

extern uint8_t	UpdateDFTimer;
extern uint8_t	UpdatePTTimer;
extern uint8_t	DFMagicNumber;

//=========================================================================
// Functions

extern void InitDataFlash();
extern void ResetDataFlash();
extern void DFCheckValuesValidity();
extern uint32_t CalcPageCRC( uint32_t *pu32Addr );
extern void UpdateDataFlash();
extern void UpdatePTCounters();
extern void DataFlashUpdateTick();
extern uint16_t GetShuntRezValue();

extern void LoadProfile( int p );
extern void SaveProfile();
extern void EraseProfile( int p );
extern int	IsProfileValid( int p );
extern void ApplyParameters();

extern void ResetPowerCurve();

//-------------------------------------------------------------------------

extern void FMCWritePage( uint32_t u32Addr, uint32_t *pu32Data );
extern int  FMCEraseWritePage( uint32_t u32Addr, uint32_t *pu32Data );
extern uint32_t FMCVerifyPage( uint32_t u32Addr, uint32_t *pu32Data );

extern void FMCRead256( uint32_t u32Addr, uint32_t *pu32Buf );

//=========================================================================

#endif /* __DATAFLASH_H__ */
