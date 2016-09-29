#include "myevic.h"
#include "myprintf.h"
#include "myrtc.h"
#include "screens.h"
#include "atomizer.h"
#include "display.h"
#include "events.h"
#include "battery.h"

#include "dataflash.h"

//=============================================================================
// DataFlash parameters global structure

dfStruct_t DataFlash;

uint8_t ParamsBackup[DATAFLASH_PARAMS_SIZE];


//-------------------------------------------------------------------------
// Global variables

uint8_t		UpdateDFTimer;
uint8_t		UpdatePTTimer;
uint8_t		DFMagicNumber;


//-------------------------------------------------------------------------
// Internal variables

uint16_t	fmcCntrsIndex;


//=============================================================================
//----- (00002064) --------------------------------------------------------
__myevic__ void SetProductID()
{
	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	dfProductID = PID_VTCMINI;
	dfMaxHWVersion = 0x00010101;
	DFMagicNumber = 0x36;
	gFlags.is_mini  = 1;
	gFlags.is_vtwo  = 0;
	gFlags.is_presa = 0;

	for ( uint32_t offset = 0 ; offset < LDROM_SIZE ; offset += 4 )
	{
		uint32_t u32Data = FMC_Read( LDROM_BASE + offset );
		if ( u32Data == PID_VTWOMINI )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00000001;
			DFMagicNumber = 0x10;
			gFlags.is_vtwo = 1;
			break;
		}
		else if ( u32Data == PID_VTWO )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00010001;
			DFMagicNumber = 0x40;
			gFlags.is_vtwo = 1;
			gFlags.is_mini = 0;
			break;
		}
		else if ( u32Data == PID_PRESA75W )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00030001;
			DFMagicNumber = 0x30;
			gFlags.is_presa = 1;
			break;
		}
		else if ( u32Data == PID_EVICAIO )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00010001;
			DFMagicNumber = 0x50;
			gFlags.is_evicaio = 1;
			break;
		}
		else if ( u32Data == PID_EGRIPII )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00000001;
			DFMagicNumber = 0x15;
			gFlags.is_egrip2 = 1;
			break;
		}
		else if ( u32Data == PID_CUBOMINI )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00020001;
			DFMagicNumber = 0x50;
			gFlags.is_cuboid = 1;
			break;
		}
		else if ( u32Data == PID_EVICBASIC )
		{
			dfProductID = u32Data;
			dfMaxHWVersion = 0x00010001;
			DFMagicNumber = 0x13;
			gFlags.is_evicbasic = 1;
			break;
		}
	}

	FMC_DISABLE_ISP();
	SYS_LockReg();
}


//=============================================================================
//----- (00002080) --------------------------------------------------------
__myevic__ void FMCReadCounters()
{
	uint32_t pc, v;
	uint32_t idx;

	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	for ( idx = 0 ; idx < FMC_FLASH_PAGE_SIZE ; idx += 4 )
	{
		v = FMC_Read( DATAFLASH_PUFFCNTR_BASE + idx );
		if ( v == ~0 ) break;
		pc = v;
	}

	fmcCntrsIndex = idx;

	if ( idx )
	{
		dfPuffCount = pc;
		dfTimeCount = FMC_Read( DATAFLASH_TIMECNTR_BASE + idx - 4 );
	}
	else
	{
		dfPuffCount = 0;
		dfTimeCount = 0;
	}

	FMC_DISABLE_ISP();
	SYS_LockReg();
}


//=============================================================================
//----- (000020CC) --------------------------------------------------------
__myevic__ void UpdatePTCounters()
{
	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	if ( fmcCntrsIndex >= FMC_FLASH_PAGE_SIZE )
	{
		FMC_Erase( DATAFLASH_PUFFCNTR_BASE );
		FMC_Erase( DATAFLASH_TIMECNTR_BASE );
		fmcCntrsIndex = 0;
	}

	FMC_Write( DATAFLASH_PUFFCNTR_BASE + fmcCntrsIndex, dfPuffCount );
	FMC_Write( DATAFLASH_TIMECNTR_BASE + fmcCntrsIndex, dfTimeCount );
	fmcCntrsIndex += 4;

	FMC_DISABLE_ISP();
	SYS_LockReg();
}


//=============================================================================
//----- (0000388C) --------------------------------------------------------
const uint16_t TempCoefsNI[] =
		{	10, 20, 35, 60, 80, 110, 120, 130, 140, 150, 160,
			170, 170, 170, 170, 170, 170, 170, 170, 170, 170	};

__myevic__ void CpyTmpCoefsNI()
{
	MemCpy( dfTempCoefsNI, TempCoefsNI, sizeof( TempCoefsNI ) );
}


//=============================================================================
//----- (000038D8) --------------------------------------------------------
const uint16_t TempCoefsTI[] =
		{	53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54,
			55, 55, 55, 55, 56, 56, 56, 56, 56, 56		};

__myevic__ void CpyTmpCoefsTI()
{
	MemCpy( dfTempCoefsTI, TempCoefsTI, sizeof( TempCoefsTI ) );
}


//=============================================================================
//----- (00001C30) --------------------------------------------------------
__myevic__ void ResetDataFlash()
{
	int hwv;

	hwv = dfHWVersion;
	MemClear( DataFlash.params, DATAFLASH_PARAMS_SIZE );
	dfHWVersion = hwv;

	// Parameters whose reset value is zero are commented out
	// since we start by clearing the memory.

	dfMagic = DFMagicNumber;
	dfMode = 4;
	dfProtec = 100;
	dfVWVolts = 330;
	dfPower = 200;
	dfTCPower = 200;
	dfIsCelsius = 1;
	dfRezType = 1;
	dfTempAlgo = 1;
	dfTemp = 210;
//	dfResistance = 0;
	dfUIVersion = 2;
//	dfAPT = 0;
//	dfRezTI = 0;
//	dfRezNI = 0;
//	dfRezLockedTI = 0;
//	dfRezLockedNI = 0;
	dfTiOn = 1;
//	dfStealthOn = 0;
	CpyTmpCoefsNI();
	CpyTmpCoefsTI();
//	dfStatus.off = 0;
//	dfStatus.keylock = 0;
//	dfStatus.flipped = 0;
//	dfStatus.nologo = 0;
//	dfStatus.clock = 0;
//	dfStatus.vcom = 0;
//	dfStatus.storage = 0;
//	dfStatus.dbgena = 0;
//	dfStatus.x32off = 0;
//	dfStatus.onewatt = 0;
//	dfStatus.digclk = 0;
//	dfStatus.battpc = 0;
//	dfStatus.phpct = 0;
//	dfStatus.wakeonpm = 0;
//	dfStatus.font = 0;
//	dfStatus.nfeoff = 0;
//	dfRezSS = 0;
//	dfRezLockedSS = 0;
//	dfRezTCR = 0;
//	dfRezLockedTCR = 0;
//	dfTCRIndex = 0;
//	dfScrMainTime = 0;
	dfTCRM[0] = 120;
	dfTCRM[1] = 120;
	dfTCRM[2] = 120;
	dfScreenSaver = 1;
//	dfTCMode = 0;
//	dfScreenProt = 0;
//	MemClear( dfSavedCfgRez, sizeof(dfSavedCfgRez) );
//	MemClear( dfSavedCfgPwr, sizeof(dfSavedCfgPwr) );
//	dfFBBest = 0;
//	dfFBSpeed = 0;
//	dfbyte_2000033D = 0;
	dfContrast = 45;
//	dfModesSel = 0;
	dfClkRatio = RTC_DEF_CLK_RATIO;
	dfPreheatPwr = 200;
//	dfPreheatTime = 0;
	dfClick[0] = CLICK_ACTION_CLOCK;
	dfClick[1] = CLICK_ACTION_EDIT;
//	dfClick[2] = CLICK_ACTION_NONE;
	dfDimTimeout = 30;
//	dfBatteryModel = 0;
	UpdateDataFlash();

	dfPuffCount = 0;
	dfTimeCount = 0;
	UpdatePTCounters();
}


//=============================================================================

__myevic__ void DFCheckValuesValidity()
{
	int i,v;

	if ( dfMagic == DATAFLASH_NFE_MAGIC )
	{
		dfMagic = DFMagicNumber;
	}

	if ( dfMode >= 7 )
		dfMode = 4;

	if (( dfProtec < 20 ) || ( dfProtec > 100 ))
		dfProtec = 100;

	if ( dfVWVolts > MaxVolts || dfVWVolts < 50 )
		dfVWVolts = 330;

	if ( dfPower > MaxPower || dfPower < 10 )
		dfPower = MaxPower;

	if ( dfTCPower > MaxTCPower || dfTCPower < 10 )
		dfTCPower = MaxTCPower;

	if ( dfPuffCount > 99999 || dfTimeCount > 999999 )
	{
		dfPuffCount = 0;
		dfTimeCount = 0;
		UpdatePTCounters();
	}

	if ( dfUIVersion != 2 )
		dfUIVersion = 2;

	if ( dfAPT > 8 )
		dfAPT = 0;

	if ( dfTempAlgo != 1 && dfTempAlgo != 2 && dfTempAlgo != 3 && dfTempAlgo != 4 )
		dfTempAlgo = 1;

	if ( dfIsCelsius > 1 )
	{
		dfIsCelsius = 1;
		dfTemp = 200;
	}
	else if ( dfIsCelsius )
	{
		if ( dfTemp < 100 || dfTemp > 315 )
			dfTemp = 200;
	}
	else
	{
		if ( dfTemp < 200 || dfTemp > 600 )
			dfTemp = 450;
	}

	if ( dfRezTI > 150 )
		dfRezTI = 0;

	if ( dfRezNI > 150 )
		dfRezNI = 0;

	if ( dfRezLockedTI > 1 )
		dfRezLockedTI = 0;

	if ( dfRezLockedNI > 1 )
		dfRezLockedNI = 0;

	if ( dfStealthOn > 1 )
		dfStealthOn = 0;

	if ( dfTiOn > 1 )
		dfTiOn = 1;

	if ( dfRezSS > 150 )
		dfRezSS = 0;

	if ( dfRezLockedSS > 1 )
		dfRezLockedSS = 0;

	if ( dfScrMainTime > 5 )
		dfScrMainTime = 0;
	
	if ( dfRezTCR > 150 )
		dfRezTCR = 0;

	if ( dfScreenSaver >= SSAVER_MAX )
		dfScreenSaver = SSAVER_CLOCK;

	if ( dfRezLockedTCR > 1 )
		dfRezLockedTCR = 0;

	if ( dfScreenProt > 7 )
		dfScreenProt = 0;

	for ( i = 0 ; i < 21 ; ++i )
	{
		if ( dfTempCoefsNI[i] > 200 )
			break;
	}
	if ( i != 21 )
		CpyTmpCoefsNI();

	for ( i = 0 ; i < 21 ; ++i )
	{
		if ( dfTempCoefsTI[i] > 100 )
			break;
	}
	if ( i != 21 )
		CpyTmpCoefsTI();

	if ( dfTCRIndex > 2 )
		dfTCRIndex = 0;

	for ( i = 0 ; i < 3 ; ++i )
	{
		if ( dfTCRM[i] > 999 )
			dfTCRM[i] = 120;
	}

	if ( dfbyte_2000033D > 1 )
		dfbyte_2000033D = 0;

	if ( dfFBSpeed > 2 )
		dfFBSpeed = 0;

	for ( i = 0 ; i < 10 ; ++i )
	{
		v = dfSavedCfgRez[i];
		if ( v > 350 || ( v < 5 && v ) )
			break;
		v = dfSavedCfgPwr[i];
		if ( v > MaxPower || ( v < 10 && v ) )
			break;
	}
	if ( i < 10 )
	{
		MemClear( dfSavedCfgRez, sizeof(dfSavedCfgRez) );
		MemClear( dfSavedCfgPwr, sizeof(dfSavedCfgPwr) );
	}

	if ( dfModesSel & 0x80 || ( dfModesSel & 0x7F ) == 0x7F )
		dfModesSel = 0;

	if ( dfClkRatio < RTC_MIN_CLOCK_RATIO || dfClkRatio > RTC_MAX_CLOCK_RATIO )
		dfClkRatio = RTC_DEF_CLK_RATIO;

	if ( dfStatus.phpct )
	{
		if ( dfPreheatPwr > 200 )
			dfPreheatPwr = 100;
	}
	else
	{
		if ( dfPreheatPwr > MaxPower )
			dfPreheatPwr = 200;
	}

	if ( dfPreheatTime > 200 )
		dfPreheatTime = 0;

	v = 0;
	for ( i = 0 ; i < 3 ; ++i )
	{
		if ( dfClick[i] >= CLICK_ACTION_MAX )
			break;
		if ( dfClick[i] == CLICK_ACTION_EDIT )
			v = 1;
	}
	if ( i < 3 )
	{
		dfClick[0] = CLICK_ACTION_CLOCK;
		dfClick[1] = CLICK_ACTION_EDIT;
		dfClick[2] = CLICK_ACTION_NONE;
	}
	else if ( v == 0 )
	{
		dfClick[1] = CLICK_ACTION_EDIT;
	}

	if ( dfDimTimeout < 5 || dfDimTimeout > 60 )
		dfDimTimeout = ScrMainTimes[dfScrMainTime];

	if ( dfBatteryModel >= GetNBatteries() )
		dfBatteryModel = 0;
}


//=============================================================================
//----- (000018D0) --------------------------------------------------------
__myevic__ int FMCCheckConfig( unsigned long cfg[] )
{
	if ( cfg[0] & 1 || cfg[1] != DATAFLASH_PARAMS_BASE )
	{
		FMC_EnableConfigUpdate();
		FMC_Erase( FMC_CONFIG_BASE );

		cfg[0] &= ~1;
		cfg[1] = DATAFLASH_PARAMS_BASE;

		if ( FMC_WriteConfig( cfg, 2 ) < 0 )
			return 0;

		FMC_ReadConfig( cfg, 2 );

		if ( cfg[0] & 1 || cfg[1] != DATAFLASH_PARAMS_BASE )
			return 0;

		SYS_ResetChip();
	}
	return 1;
}


//=============================================================================
//----- (00001926) --------------------------------------------------------
__myevic__ void FMCRead256( uint32_t u32Addr, uint32_t *pu32Buf )
{
	for ( uint32_t offset = 0 ; offset < 0x100 ; offset += 4 )
	{
		*pu32Buf = FMC_Read( u32Addr + offset );
		++pu32Buf;
	}
}


//=============================================================================
//----- (00001CEC) --------------------------------------------------------
__myevic__ uint32_t ReadDataFlash( uint32_t u32Addr, uint32_t *pu32Buf )
{
	uint32_t offset;

	for ( offset = 0 ; offset < DATAFLASH_PARAMS_SPACE ; offset += DATAFLASH_PARAMS_SIZE )
	{
		if ( FMC_Read( u32Addr + offset ) == ~0 && FMC_Read( u32Addr + offset + 4 ) == ~0 )
		{
			break;
		}
		offset += DATAFLASH_PARAMS_SIZE;
	}

	if ( offset )
	{
		u32Addr += offset - DATAFLASH_PARAMS_SIZE;
	}

	for ( offset = 0 ; offset < DATAFLASH_PARAMS_SIZE ; offset += 0x100 )
	{
		FMCRead256( u32Addr + offset, pu32Buf + offset / 4 );
	}

	return u32Addr;
}


//=============================================================================
//----- (0000119C) --------------------------------------------------------
__myevic__ uint32_t CalcPageCRC( uint32_t *pu32Addr )
{
	uint32_t idx;
	uint16_t *addr;
	uint32_t crc;

	CRC_Open( CRC_CCITT, 0, 0xFFFF, CRC_CPU_WDATA_16 );

	idx = 0;
	addr = (uint16_t*)(pu32Addr+1);

	do
	{
		CRC_WRITE_DATA( addr[idx] );
	}
	while ( ++idx < ( DATAFLASH_PARAMS_SIZE - 4 ) / 2 );

	crc = CRC_GetChecksum();

	CRC->CTL &= ~1;

	return crc;
}


//=============================================================================
//----- (00001FD0) --------------------------------------------------------
// Writes 256 bytes from address from pu32Data to first free page
// in DF after u32Addr
__myevic__ void WriteDataFlash( uint32_t u32Addr, const uint32_t *pu32Data )
{
	uint32_t offset;

	for ( offset = 0 ; offset < DATAFLASH_PARAMS_SPACE ; offset += DATAFLASH_PARAMS_SIZE )
	{
		if ( FMC_Read( u32Addr + offset ) == ~0 && FMC_Read( u32Addr + offset + 4 ) == ~0 )
		{
			break;
		}
	}

	if ( offset >= DATAFLASH_PARAMS_SPACE )
	{
		offset = 0;
		FMC_Erase( u32Addr );
	}
	else if ( offset < DATAFLASH_PARAMS_SPACE - FMC_FLASH_PAGE_SIZE )
	{
		if ( offset % FMC_FLASH_PAGE_SIZE == DATAFLASH_PARAMS_SIZE )
		{
			FMC_Erase( u32Addr + offset - DATAFLASH_PARAMS_SIZE + FMC_FLASH_PAGE_SIZE );
		}
	}

	u32Addr += offset;

	for ( offset = 0 ; offset < DATAFLASH_PARAMS_SIZE ; offset += 4 )
	{
		FMC_Write( u32Addr + offset, pu32Data[ offset / 4 ] );
	}
}


//=============================================================================
//----- (00001D30) --------------------------------------------------------
__myevic__ void UpdateDataFlash()
{
	uint8_t *df;
	uint32_t idx;

	dfAtoRez = AtoRez;
	dfAtoStatus = AtoStatus;

	df = (uint8_t*)&DataFlash.params;

	for ( idx = 0 ; idx < DATAFLASH_PARAMS_SIZE ; ++idx )
	{
		if ( df[idx] != ParamsBackup[idx] )
			break;
	}

	if ( idx != DATAFLASH_PARAMS_SIZE )
	{
		dfCRC = CalcPageCRC( DataFlash.params );
		MemCpy( ParamsBackup, DataFlash.params, DATAFLASH_PARAMS_SIZE );
		SYS_UnlockReg();
		FMC_ENABLE_ISP();
		WriteDataFlash( DATAFLASH_PARAMS_BASE, DataFlash.params );
		FMC_DISABLE_ISP();
		SYS_LockReg();
	}
}


//=============================================================================
//----- (00001940) --------------------------------------------------------
__myevic__ void InitDataFlash()
{
	int i;
	unsigned long cfg[2];
	unsigned long hwv;
	unsigned long addr;

	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	MemClear( &DataFlash, sizeof( DataFlash ) );

	dffmcCID = FMC_ReadCID();

	FMC->ISPCMD = FMC_ISPCMD_READ_DID;
	FMC->ISPADDR = 0;
	FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
	while( FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk )
		;
	dffmcDID = FMC->ISPDAT;

	dffmcPID = FMC_ReadPID();

	for ( i = 0 ; i < 3 ; ++i )
	{
		dffmcUID[i] = FMC_ReadUID( i );
	}

	for ( i = 0 ; i < 4 ; ++i )
	{
		dffmcUCID[i] = FMC_ReadUCID( i );
	}

	FMC_ReadConfig( cfg, 2 );

	addr = DATAFLASH_PARAMS_BASE;

	if ( FMCCheckConfig( cfg ) )
	{
		addr = ReadDataFlash( addr, DataFlash.params );
	}

	if ( CalcPageCRC( DataFlash.params ) != dfCRC )
	{
		if ( addr == DATAFLASH_PARAMS_BASE )
		{
			addr = DATAFLASH_PARAMS_END - DATAFLASH_PARAMS_SIZE;
		}
		else
		{
			addr -= DATAFLASH_PARAMS_SIZE;
		}

		hwv = dfHWVersion;

		for ( i = 0 ; i < DATAFLASH_PARAMS_SIZE ; i += 0x100 )
		{
			FMCRead256( addr + i, &DataFlash.params[ i / 4 ] );
		}

		if ( CalcPageCRC( DataFlash.params ) == dfCRC )
		{
			UpdateDataFlash();
		}
		else
		{
			dfHWVersion = hwv;
		}
	}

	FMC_DISABLE_ISP();
	SYS_LockReg();

	FMCReadCounters();

	SetProductID();

	if ( ISVTWO || ISEVICAIO || ISCUBOMINI || ISEVICBASIC )
	{
		switch ( dfHWVersion )
		{
			case 101:
				DisplayModel = 1;
				break;
			default:
				DisplayModel = 0;
				break;
		}
	}
	else if ( ISVTWOMINI || ISEGRIPII )
	{
		DisplayModel = 0;
	}
	else if ( ISPRESA75W )
	{
		switch ( dfHWVersion )
		{
			case 102:
			case 103:
				DisplayModel = 1;
				break;
			default:
				DisplayModel = 0;
				break;
		}
	}
	else
	{
		switch ( dfHWVersion )
		{
			case 102:
			case 103:
			case 106:
			case 108:
			case 109:
			case 111:
				DisplayModel = 1;
				break;
			default:
				DisplayModel = 0;
				break;
		}
	}

	if ( ISPRESA75W || ISEVICAIO )
	{
		AtoShuntRez = 100;
	}
	else if ( ISVTWOMINI || ISVTWO )
	{
		AtoShuntRez = 115;
	}
	else if ( ISEGRIPII || ISEVICBASIC )
	{
		AtoShuntRez = 120;
	}
	else if ( ISCUBOMINI )
	{
		switch ( dfHWVersion )
		{
			case 100:
			case 101:
			default:
				AtoShuntRez = 100;
				break;

			case 102:
				AtoShuntRez = 105;
				break;
		}
	}
	else
	{
		switch ( dfHWVersion )
		{
			case 100:
			case 102:
			default:
				AtoShuntRez = 115;
				break;
			case 101:
				AtoShuntRez = 125;
				break;
			case 108:
				AtoShuntRez = 125;
				break;
			case 103:
			case 104:
			case 105:
			case 106:
				AtoShuntRez = 110;
				break;
			case 107:
			case 109:
				AtoShuntRez = 120;
				break;
			case 110:
			case 111:
				AtoShuntRez = 105;
				break;
		}
	}

	dfFWVersion	= FWVERSION;

	MaxVolts	= 900;

	if ( ISEVICBASIC )
	{
		MaxPower	= 600;
		MaxTCPower	= 600;
	}
	else if ( gFlags.is_mini )
	{
		MaxPower	= 750;
		MaxTCPower	= 750;
	}
	else
	{
		MaxPower	= 800;
		MaxTCPower	= 800;
	}

	myprintf( "  APROM Version ......................... [%d.%d%d]\n",
				FWVERSION / 100,
				FWVERSION / 10 % 10,
				FWVERSION % 10 );
	myprintf( "  Hardware Version ...................... [%d.%d%d]\n",
				dfHWVersion / 100,
				dfHWVersion / 10 % 10,
				dfHWVersion % 10 );

	if ( dfMagic == DATAFLASH_NFE_MAGIC )
	{
		dfMagic = DFMagicNumber;
	}

	if ( dfMagic == DFMagicNumber && CalcPageCRC( DataFlash.params ) == dfCRC )
	{
		DFCheckValuesValidity();
	}
	else
	{
		myprintf( "Data Flash Re-Initialization\n" );
		ResetDataFlash();
	}

	dfStatus.off = 0;
	dfUIVersion = 2;

	MemCpy( ParamsBackup, DataFlash.params, DATAFLASH_PARAMS_SIZE );

	if ( dfBootFlag )
	{
		dfBootFlag = 0;
		UpdateDFTimer = 1;
	}
}


//=============================================================================
//----- (0000169C) --------------------------------------------------------
// Writes 2kB from RAM R1 to DF R0
__myevic__ void FMCWritePage( uint32_t u32Addr, uint32_t *pu32Data )
{
	for ( uint32_t idx = 0 ; idx < FMC_FLASH_PAGE_SIZE / 4 ; ++idx )
	{
		FMC_Write( u32Addr + 4 * idx, pu32Data[ idx ] );
	}
}

//=============================================================================
//----- (000016D0) --------------------------------------------------------
// Compares 2kB (0x800) DF @R0 with RAM @R1
__myevic__ uint32_t FMCVerifyPage( uint32_t u32Addr, uint32_t *pu32Data )
{
	for ( uint32_t idx = 0 ; idx < FMC_FLASH_PAGE_SIZE / 4 ; ++idx )
	{
		if ( FMC_Read( u32Addr + 4 * idx ) != pu32Data[ idx ] )
		{
			return idx + 1;
		}
	}
	return 0;
}


//=============================================================================
//----- (0000170C) --------------------------------------------------------
// Erase & writes 2kB from RAM R1 to DF R0
__myevic__ int FMCEraseWritePage( uint32_t u32Addr, uint32_t *src )
{
	if ( FMC_Erase( u32Addr ) == -1 )
	{
		return 1;
	}
	else
	{
		FMCWritePage( u32Addr, src );
		return 0;
	}
}


//=============================================================================
//----- (00002030) --------------------------------------------------------
__myevic__ void DataFlashUpdateTick()
{
	if ( UpdateDFTimer )
	{
		if ( !--UpdateDFTimer )
		UpdateDataFlash();
	}
	if ( UpdatePTTimer )
	{
		if ( !--UpdatePTTimer )
		UpdatePTCounters();
	}
}
