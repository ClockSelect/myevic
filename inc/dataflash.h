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

// Custom Logo space
// Those addresses are hardcoded in Joyetech's custom logo utility so
//  we have to hardcode them here too.
#define DATAFLASH_LOGO_SIZE			0x200
#define DATAFLASH_LOGO_1327_BASE	0x19000
#define DATAFLASH_LOGO_1306_BASE	0x19200


//=========================================================================
// DataFlash content structure
// Padding bytes added by compiler to ensure correct alignements are
//  indicated and can eventually be used to store usefull parameters
//  without increasing the total data size.
// Useless fields may also be recycled.
//-------------------------------------------------------------------------

typedef struct
{
	int	off:1;
	int	keylock:1;
	int	flipped:1;
	int	nologo:1;
	int anaclk:1;
	int vcom:1;
}
dfStatus_t;

typedef struct dfParams
{
	uint32_t	PCRC;
	uint32_t	HWVersion;
	uint8_t		Magic;
	uint8_t		BootFlag;
	uint8_t		Mode;
//	1-byte pad
	uint16_t	Power;
	uint16_t	Temp;
	uint16_t	TCPower;
	uint16_t	VWVolts;
	uint8_t		APT;
	uint8_t		RezType;
	uint8_t		TempAlgo;
	uint8_t		IsCelsius;
	uint16_t	Resistance;
	uint16_t	RezTI;
	uint16_t	RezNI;
	uint8_t		RezLockedTI;
	uint8_t		RezLockedNI;
	uint8_t		TiOn;		// useless
	uint8_t		StealthOn;
	uint16_t	TempCoefsNI[21];
	uint16_t	TempCoefsTI[21];
//	2-bytes pad
	dfStatus_t	Status;
	uint16_t	AtoRez;
	uint8_t		AtoStatus;
//	1-byte pad
	uint16_t	RezSS;
	uint8_t		RezLockedSS;
	uint8_t		UIVersion;	// useless
	uint8_t		TCRIndex;
//	1-byte pad
	uint16_t	TCRM[3];
	uint16_t	RezTCR;
	uint8_t		RezLockedTCR;
//	1-byte pad
	uint8_t		LastTCMode;
	uint8_t		ScreenSave;		//	1-byte pad
	uint16_t	SavedCfgRez[10];
	uint16_t	SavedCfgPwr[10];
	uint16_t	FBBest;
	uint8_t		FBSpeed;
	uint8_t		byte_2000033D;	// unused
	uint8_t		Contrast;
	uint8_t		ModesSel;
	uint16_t	ClkRatio;
//	2-bytes pad
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
	uint32_t	FWVersion;
	uint32_t	LDVersion;
	uint32_t	fmcCID;
	uint32_t	fmcDID;
	uint32_t	fmcPID;
	uint32_t	fmcUID[3];
	uint32_t	fmcUCID[4];
	uint32_t	PuffCount;
	uint32_t	TimeCount;
	uint32_t	ProductID;
	uint32_t	MaxHWVersion;
	uint16_t	Year;
	uint8_t		Month;
	uint8_t		Day;
	uint8_t		Hour;
	uint8_t		Minute;
	uint8_t		Second;
}
dfInfos_t;


#define ALIGN256(x) (((x)&0xff)?((x)-((x)&0xff)+0x100):(x))

#define DATAFLASH_PARAMS_SIZE	ALIGN256(sizeof(dfParams_t))
#define DATAFLASH_INFOS_SIZE	ALIGN256(sizeof(dfInfos_t))
#define DATAFLASH_FREE_SIZE		(FMC_FLASH_PAGE_SIZE-DATAFLASH_PARAMS_SIZE-DATAFLASH_INFOS_SIZE)

//-------------------------------------------------------------------------
// It's important for DATAFLASH_PARAMS_SIZE to be a divider of
//	DATAFLASH_PARAMS_SPACE for all the functions of cycling to work
//	properly. Unfortunatly, we can't check this in the preprocessor
//	due to the sizeof() operation.
// It won't be a problem as long as the size of the parameters do not
//	exceed 0x200 bytes. Just be aware.
// In any case, parameters size should never exceed 0x600 bytes.
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

#define dfChecksum		DataFlash.Checksum
#define dfCRC			DFP(PCRC)
#define dfHWVersion		DFP(HWVersion)
#define dfMagic			DFP(Magic)
#define dfBootFlag		DFP(BootFlag)
#define dfMode			DFP(Mode)
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
#define dfTempCoefsTI	DFP(TempCoefsTI)
#define dfStatus		DFP(Status)
#define dfAtoRez		DFP(AtoRez)
#define dfAtoStatus		DFP(AtoStatus)
#define dfRezSS			DFP(RezSS)
#define dfRezLockedSS	DFP(RezLockedSS)
#define dfUIVersion		DFP(UIVersion)
#define dfTCRIndex		DFP(TCRIndex)
#define dfTCRM			DFP(TCRM)
#define dfRezTCR		DFP(RezTCR)
#define dfRezLockedTCR	DFP(RezLockedTCR)
#define dfScreenSave	DFP(ScreenSave)
#define dfLastTCMode	DFP(LastTCMode)
#define dfSavedCfgRez	DFP(SavedCfgRez)
#define dfSavedCfgPwr	DFP(SavedCfgPwr)
#define dfFBBest		DFP(FBBest)
#define dfFBSpeed		DFP(FBSpeed)
#define dfbyte_2000033D	DFP(byte_2000033D)
#define dfContrast		DFP(Contrast)
#define dfModesSel		DFP(ModesSel)
#define dfClkRatio		DFP(ClkRatio)

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

#define gPlayfield		DataFlash.playfield


//=========================================================================
// Functions

extern void InitDataFlash();
extern void ResetDataFlash();
extern void DFCheckValuesValidity();
extern void UpdateDataFlash();
extern void DataFlashUpdateTick();

//-------------------------------------------------------------------------

extern void FMCWritePage( uint32_t u32Addr, uint32_t *pu32Data );
extern int  FMCEraseWritePage( uint32_t u32Addr, uint32_t *pu32Data );
extern uint32_t FMCVerifyPage( uint32_t u32Addr, uint32_t *pu32Data );

extern void FMCRead256( uint32_t u32Addr, uint32_t *pu32Buf );

//=========================================================================

#endif /* __DATAFLASH_H__ */
