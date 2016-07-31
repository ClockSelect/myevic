#include "myevic.h"
#include "dataflash.h"
#include "meadc.h"
#include "atomizer.h"


//=============================================================================

const uint16_t BatteryVoltsTable[] =
	{ 310, 330, 342, 350, 358, 363, 368, 379, 389, 400, 410 };
		
//=============================================================================
//----- (00000608) ------------------------------------------------------------
__myevic__ int BatteryVoltsToPercent( int bv )
{
	int bpc;
	int i;

	if ( bv > 310 )
	{
		if ( bv < 410 )
		{
			i = 1;
			do
			{
				if ( BatteryVoltsTable[i] >= bv )
					break;
				++i;
			}
			while ( i < 11 );

			bpc = 10 * ( i - 1 ) +
				  10 * ( bv - BatteryVoltsTable[i - 1] )
					 / ( BatteryVoltsTable[i] - BatteryVoltsTable[i - 1] );
		}
		else
		{
			bpc = 100;
		}
	}
	else
	{
		bpc = 0;
	}
	return bpc;
}

//=============================================================================
//----- (0000065C) ------------------------------------------------------------

__myevic__ void NewBatteryVoltage()
{
	BatteryPercent = BatteryVoltsToPercent( BatteryVoltage );

	if ( !( Flags64 & 0x1000 ) && ( Flags64 & 0x400 ) )
	{
		BatteryPercent = 100;
	}

	if ( (( SavedBatPercent != BatteryPercent ) && ( ++BatPCCmpCnt >= 3u ))
		 || Flags64 & 0x80 )
	{
		BatPCCmpCnt = 0;
		SavedBatVoltage = BatteryVoltage;
		SavedBatPercent = BatteryPercent;

		if ( ( BatteryVoltage > 300 ) && ( Screen == 1 || Screen == 3 ) )
		{
			Flags64 |= 0x20000u;
		}
	}
	else
	{
		BatteryPercent = SavedBatPercent;
	}

	if ( BatteryPercent >= 10u )
	{
		Flags64 &= ~0x80000;
	}
	else
	{
		Flags64 |= 0x80000;
	}

	BatteryTenth = BatteryPercent / 10u;

	return;
}


//=============================================================================
//----- (000006E0) --------------------------------------------------------
__myevic__ void ReadBatteryVoltage()
{
	unsigned int newbv; // r0@5

	if ( !(Flags64 & 0x100) )
	{
		while ( VbatSampleCnt < 16u )
		{
			VbatSampleSum += ADC_Read( 18 );
			++VbatSampleCnt;

			if ( !(Flags64 & 0x8000) )
				return;
		}

		Flags64 &= ~0x8000;
		newbv = VbatSampleSum >> 7;

		VbatSampleSum = newbv;
		BatteryVoltage = newbv;

		if ( ( BatRefreshTmr <= 100 )
			&& (	( SavedBatVoltage > newbv && SavedBatVoltage - newbv > 3 )
				||	( newbv > SavedBatVoltage && newbv - SavedBatVoltage > 3 ) ) )
		{
			NewBatteryVoltage();
		}

		VbatSampleCnt = 0;
		VbatSampleSum = 0;
	}
}


//=============================================================================
//----- (0000074C) ------------------------------------------------------------

unsigned short RTBatVolts = 0;

__myevic__ int CheckBattery()
{
	int v0;
	int pwr;
	int i;
	int bv;

	v0 = 0;

	if ( dfMode == 6 )
		pwr = dfSavedCfgPwr[ConfigIndex];
	else
		pwr = dfPower;

	RTBatVolts = 0;

	i = 0;
	do
	{
		bv = ADC_Read( 18 ) >> 3;
		if ( bv > 280 )
			break;
		++i;
	}
	while ( i < 3 );

	if ( i == 3 )
	{
		StopFire();
		Event = 28;
		return 1;
	}

	RTBatVolts = bv;

	if ( !( Flags64 & 0x100 )
		|| ( !ISMODEVW(dfMode) && ( !ISMODETC(dfMode) || Flags64 & 0x800000) ) )
	{
		Flags64 &= ~0x400000;
		return 0;
	}

	if ( LowBatVolts
		&& pwr > 600
		&& PowerScale == 100
		&& bv >= 290
		&& 100 * ( bv - 290 ) / ( LowBatVolts - 290 ) < 10 )
	{
		v0 = 1;
		PowerScale = 60000 / pwr;
	}

	if ( v0 || Flags64 & 0x20000000 || bv < 290 )
	{
		Flags64 |= 0x400000;
		ShowWeakBatFlag = 5;
		if ( ISMODEVW(dfMode) && ( PowerScale > 5 ))
			--PowerScale;
		Flags64 &= ~0x20000000;
	}
	else
	{
		Flags64 &= ~0x400000;
	}
	return 0;
}
