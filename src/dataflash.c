#include "myevic.h"
#include "myrtc.h"


//=============================================================================
//----- (00002064) --------------------------------------------------------
__myevic__ void GetProductID()
{
	dfProductID = *(uint32_t*)"E052";
	dfMaxHWVersion = 0x00010101;
}


//=============================================================================
//----- (00002080) --------------------------------------------------------
__myevic__ void FMCReadCounters()
{
	uint32_t pc, v;
	uint32_t idx;

	idx = 0;
	do
	{
		v = FMC_Read( 0x1F000 + idx );
		if ( v == -1 ) break;
		idx += 4;
		pc = v;
	}
	while ( idx < 0x800 );

	fmcCntrsIndex = idx;

	if ( idx )
	{
		dfPuffCount = pc;
		dfTimeCount = FMC_Read( 0x1F800 + idx - 4 );
	}
	else
	{
		dfPuffCount = 0;
		dfTimeCount = 0;
	}
}


//=============================================================================
//----- (000020CC) --------------------------------------------------------
__myevic__ void FMCWriteCounters()
{
	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	if ( fmcCntrsIndex >= 0x800u )
	{
		FMC_Erase( 0x1F000 );
		FMC_Erase( 0x1F800 );
		fmcCntrsIndex = 0;
	}

	FMC_Write( 0x1F000 + fmcCntrsIndex, dfPuffCount );
	FMC_Write( 0x1F800 + fmcCntrsIndex, dfTimeCount );
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
	MemClear( &dfCRC, 0x100 );
	dfHWVersion = hwv;
	dfMagic = 54;
	dfMode = 4;
	dfVWVolts = 330;
	dfPower = MaxPower;
	dfTCPower = MaxTCPower;
	dfPuffCount = 0;
	dfTimeCount = 0;
	dfIsCelsius = 0;
	dfRezType = 1;
	dfTempAlgo = 1;
	dfTemp = 450;
	dfResistance = 0;
	dfStealthOn = 0;
	dfUIVersion = 2;
	dfAPT = 0;
	dfRezTI = 0;
	dfRezNI = 0;
	dfRezLockedTI = 0;
	dfRezLockedNI = 0;
	dfTiOn = 1;
	dfRezSS = 0;
	dfRezLockedSS = 0;
	dfRezTCR = 0;
	dfRezLockedTCR = 0;
	dfTCRM1 = 120;
	dfTCRM2 = 120;
	dfTCRM3 = 120;
	byte_2000033D = 0;
	dfFBBest = 0;
	dfFBSpeed = 0;
	CpyTmpCoefsNI();
	CpyTmpCoefsTI();
	dfStatus = 0;
	MemClear( dfSavedCfgRez, 20 );
	MemClear( dfSavedCfgPwr, 20 );
	FMCWriteCounters();
	dfContrast = 45;
	dfModesSel = 0;
	dfClkRatio = DEF_RTC_CLK_RATIO;
}


//=============================================================================

__myevic__ void DFCheckValuesValidity()
{
	int i,v;

	if ( dfMode >= 7u )
		dfMode = 4;

	if ( dfVWVolts > MaxVWVolts || dfVWVolts < 50u )
		dfVWVolts = 330;

	if ( dfPower > MaxPower || dfPower < 10u )
		dfPower = MaxPower;

	if ( dfTCPower > MaxTCPower || dfTCPower < 10u )
		dfTCPower = MaxTCPower;

	if ( dfPuffCount > 99999 || dfTimeCount > 999999 )
	{
		dfPuffCount = 0;
		dfTimeCount = 0;
		FMCWriteCounters();
	}

	if ( dfUIVersion != 2 )
		dfUIVersion = 2;

	if ( dfAPT > 6 )
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


	if ( dfRezTI > 150u )
		dfRezTI = 0;

	if ( dfRezNI > 150u )
		dfRezNI = 0;

	if ( dfRezLockedTI > 1u )
		dfRezLockedTI = 0;

	if ( dfRezLockedNI > 1u )
		dfRezLockedNI = 0;

	if ( dfStealthOn > 1u )
		dfStealthOn = 0;

	if ( dfTiOn > 1u )
		dfTiOn = 1;

	if ( dfRezSS > 150u )
		dfRezSS = 0;

	if ( dfRezLockedSS > 1u )
		dfRezLockedSS = 0;

	if ( dfRezTCR > 150u )
		dfRezTCR = 0;

	if ( dfRezLockedTCR > 1u )
		dfRezLockedTCR = 0;

	i = 0;
	do
	{
		if ( dfTempCoefsNI[i] > 200 )
			break;
		++i;
	}
	while ( i < 21 );
	if ( i != 21 )
		CpyTmpCoefsNI();

	i = 0;
	do
	{
		if ( dfTempCoefsTI[i] > 100 )
			break;
		++i;
	}
	while ( i < 21 );
	if ( i != 21 )
		CpyTmpCoefsTI();

	i = 0;
	do
	{
		if ( (&dfTCRM1)[i] > 999 )
			(&dfTCRM1)[i] = 120;
		++i;
	}
	while ( i < 3 );

	if ( byte_2000033D >= 2u )
		byte_2000033D = 0;

	if ( dfFBSpeed > 2u )
		dfFBSpeed = 0;

	i = 0;
	while ( i < 10 )
	{
		v = dfSavedCfgRez[i];
		if ( v > 350 || ( v < 5 && v ) )
			break;
		v = dfSavedCfgPwr[i];
		if ( v > MaxPower || ( v < 10 && v ) )
			break;
		++i;
	}
	if ( i < 10 )
	{
		MemClear(dfSavedCfgRez, 0x14u);
		MemClear(dfSavedCfgPwr, 0x14u);
	}

	if ( dfModesSel & 0x80 || ( dfModesSel & 0x7F ) == 0x7F )
		dfModesSel = 0;

	if ( dfClkRatio < 10000 || dfClkRatio > 50000 )
		dfClkRatio = DEF_RTC_CLK_RATIO;
}


//=============================================================================
//----- (000018D0) --------------------------------------------------------
__myevic__ int FMCCheckConfig( unsigned long cfg[] )
{
	if ( cfg[0] & 1 || cfg[1] != 0x1E000 )
	{
		FMC_EnableConfigUpdate();
		FMC_Erase( FMC_CONFIG_BASE );

		cfg[0] &= ~1;
		cfg[1] = 0x1E000;

		if ( FMC_WriteConfig( cfg, 2 ) < 0 )
			return 0;

		FMC_ReadConfig( cfg, 2 );

		if ( cfg[0] & 1 || cfg[1] != 0x1E000 )
			return 0;

		SYS_ResetChip();
	}
	return 1;
}


//=============================================================================
//----- (00001926) --------------------------------------------------------
__myevic__ void FMCRead100( uint32_t u32Addr, uint32_t *pu32Buf )
{
	for ( uint32_t offset = 0 ; offset < 0x100 ; offset += 4 )
	{
		*pu32Buf = FMC_Read( u32Addr + offset );
		++pu32Buf;
	}
}


//=============================================================================
//----- (00001CEC) --------------------------------------------------------
__myevic__ uint32_t FMCLoadDFFirstPage( uint32_t u32Addr, uint32_t *pu32Buf )
{
	uint32_t offset = 0;
	do
	{
		if ( FMC_Read( u32Addr + offset ) == ~0 && FMC_Read( u32Addr + offset + 4 ) == ~0 )
			break;
		offset += 0x100;
	}
	while ( offset < 0x1000 );

	if ( offset )
	{
		u32Addr += offset - 0x100;
	}

	FMCRead100( u32Addr, pu32Buf );
	return u32Addr;
}


//=============================================================================
//----- (0000119C) --------------------------------------------------------
__myevic__ uint32_t CalcPageCRC( uint32_t *pu32Addr )
{
	uint32_t idx; // r1@1
	uint16_t *addr; // r4@1
	uint32_t crc; // r0@3

	CRC_Open( CRC_CCITT, 0, 0xFFFF, CRC_CPU_WDATA_16 );

	idx = 0;
	addr = (uint16_t*)(pu32Addr+1);

	do
	{
		CRC_WRITE_DATA( addr[idx] );
	}
	while ( ++idx < (0x100-4)/2 );

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
	uint32_t offset; // r4@1

	for ( offset = 0 ; offset < 0x1000 ; offset += 0x100 )
	{
		if ( FMC_Read( u32Addr + offset ) == ~0 && FMC_Read( u32Addr + offset + 4 ) == ~0 )
			break;
	}

	if ( offset == 0x1000 )
	{
		offset = 0;
		FMC_Erase( u32Addr );
	}
	else if ( offset == 0x100 )
	{
		FMC_Erase( u32Addr + 0x200 );
	}

	u32Addr = u32Addr + offset;

	for ( int i = 0 ; i < 0x100 / 4 ; ++i )
	{
		FMC_Write( u32Addr + 4 * i, pu32Data[i] );
	}
}


//=============================================================================
//----- (00001D30) --------------------------------------------------------
__myevic__ void UpdateDataFlash()
{
	uint8_t *df; // r4@1
	uint32_t idx; // r0@1

	dfAtoRez = AtoRez;
	dfAtoStatus = AtoStatus;

	df = (uint8_t*)&dfCRC;

	for ( idx = 0 ; idx < 0x100 ; ++idx )
	{
		if ( df[idx] != SavedDF[idx] )
			break;
	}

	if ( idx != 0x100 )
	{
		dfCRC = CalcPageCRC( &dfCRC );
		MemCpy( SavedDF, &dfCRC, 0x100u );
		SYS_UnlockReg();
		FMC_ENABLE_ISP();
		WriteDataFlash( 0x1E000, &dfCRC );
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

	MemClear( &dfData, 0x800u );

	fmcCID = FMC_ReadCID();

	FMC->ISPCMD = FMC_ISPCMD_READ_DID;
	FMC->ISPADDR = 0;
	FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
	while( FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk );
	fmcDID = FMC->ISPDAT;

	fmcPID = FMC_ReadPID();

	for ( i = 0 ; i < 3 ; ++i )
	{
		fmcUID[i] = FMC_ReadUID( i );
	}

	for ( i = 0 ; i < 4 ; ++i )
	{
		fmcUCID[i] = FMC_ReadUCID( i );
	}

	FMC_ReadConfig( cfg, 2 );

	addr = 0x1E000;

	if ( FMCCheckConfig( cfg ) )
	{
		addr = FMCLoadDFFirstPage( addr, &dfCRC );
	}

	if ( CalcPageCRC( &dfCRC ) != dfCRC )
	{
		if ( addr == 0x1E000 )
		{
			addr = 0x1EF00;
		}
		else
		{
			addr -= 0x100;
		}

		hwv = dfHWVersion;
		FMCRead100( addr, &dfCRC );

		if ( CalcPageCRC( &dfCRC ) == dfCRC )
		{
			UpdateDataFlash();
		}
		else
		{
			dfHWVersion = hwv;
		}
	}

	FMCReadCounters();

	FMC_DISABLE_ISP();
	SYS_LockReg();

	GetProductID();

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

	switch ( dfHWVersion )
	{
		case 100:
		case 102:
		default:
			AtoShuntValue = 115;
			break;
		case 101:
		case 108:
			AtoShuntValue = 125;
			break;
		case 103:
		case 104:
		case 105:
		case 106:
			AtoShuntValue = 110;
			break;
		case 107:
		case 109:
			AtoShuntValue = 120;
			break;
		case 110:
		case 111:
			AtoShuntValue = 105;
			break;
	}

	dfFWVersion	= 303;
	MaxVWVolts	= 900;
	MaxPower	= 750;
	MaxTCPower	= 750;

	PutTextf( "  APROM Version ......................... [%d.%d%d]\n", 3, 0, 3 );
	PutTextf( "  Hardware Version ...................... [%d.%d%d]\n",
				dfHWVersion / 100u,
				dfHWVersion / 10u % 10,
				dfHWVersion % 10u );

	if ( dfMagic == 0x36 && CalcPageCRC( &dfCRC ) == dfCRC )
	{
		DFCheckValuesValidity();
	}
	else
	{
		PutTextf( "Data Flash Re-Initialization\n" );
		ResetDataFlash();
		dfMode = 0;
		dfLastTCMode = 0;
		dfPower = 200;
		dfIsCelsius = 1;
		dfTemp = 235;
		UpdateDataFlash();
	}

	dfStatus &=  ~1;
	dfUIVersion = 2;
	MemCpy( SavedDF, &dfCRC, 0x100u );

	if ( dfBootFlag )
	{
		dfBootFlag = 0;
		UpdateDFTimer = 1;
	}
}


//=============================================================================
//----- (0000169C) --------------------------------------------------------
// Writes 2kB from RAM R1 to DF R0
__myevic__ void FMCWrite800( uint32_t u32Addr, uint32_t *pu32Data )
{
	for ( uint32_t idx = 0 ; idx < 0x800 / 4 ; ++idx )
	{
		FMC_Write( u32Addr + 4 * idx, pu32Data[ idx ] );
	}
}

//=============================================================================
//----- (000016D0) --------------------------------------------------------
// Compares 2kB (0x800) DF @R0 with RAM @R1
__myevic__ uint32_t FMCVerif800( uint32_t u32Addr, uint32_t *pu32Data )
{
	for ( uint32_t idx = 0 ; idx < 0x800 / 4 ; ++idx )
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
__myevic__ int FMCEraseWrite800( uint32_t u32Addr, uint32_t *src )
{
	if ( FMC_Erase( u32Addr ) == -1 )
	{
		return 1;
	}
	else
	{
		FMCWrite800( u32Addr, src );
		return 0;
	}
}


//=============================================================================
//----- (00002030) --------------------------------------------------------
__myevic__ void UpdateFlash()
{
	if ( UpdateDFTimer )
	{
		if ( !--UpdateDFTimer )
		UpdateDataFlash();
	}
	if ( UpdatePTTimer )
	{
		if ( !--UpdatePTTimer )
		FMCWriteCounters();
	}
}
