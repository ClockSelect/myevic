#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "meadc.h"
#include "atomizer.h"


//=============================================================================

const uint16_t BatteryVoltsTable[] =
	{ 310, 330, 342, 350, 358, 363, 368, 379, 389, 400, 410 };

uint16_t	LowBatVolts;
uint32_t	PowerScale;
uint16_t	BatteryVoltage;
uint16_t	SavedBatVoltage;
uint8_t		BatteryPercent;
uint8_t		BatteryTenth;
uint8_t		BatRefreshTmr;
uint8_t		BatReadTimer;

uint8_t		byte_20000048;


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
	static uint8_t SavedBatPercent = 0;
	static uint8_t BatPCCmpCnt = 0;

	BatteryPercent = BatteryVoltsToPercent( BatteryVoltage );

	if ( !( gFlags.battery_charging ) && ( gFlags.usb_attached ) )
	{
		BatteryPercent = 100;
	}

	if ( (( SavedBatPercent != BatteryPercent ) && ( ++BatPCCmpCnt >= 3 ))
		 || gFlags.read_battery )
	{
		BatPCCmpCnt = 0;
		SavedBatVoltage = BatteryVoltage;
		SavedBatPercent = BatteryPercent;

		if ( ( BatteryVoltage > 300 ) && ( Screen == 1 ) )
		{
			gFlags.refresh_display = 1;
		}
	}
	else
	{
		BatteryPercent = SavedBatPercent;
	}

	if ( BatteryPercent >= 10 )
	{
		gFlags.battery_10pc = 0;
	}
	else
	{
		gFlags.battery_10pc = 1;
	}

	BatteryTenth = BatteryPercent / 10;

	return;
}


//=============================================================================
//----- (000006E0) --------------------------------------------------------
__myevic__ void ReadBatteryVoltage()
{
	static uint32_t VbatSampleSum = 0;
	static uint8_t	VbatSampleCnt = 0;

	unsigned int newbv; // r0@5

	if ( !(gFlags.firing) )
	{
		while ( VbatSampleCnt < 16 )
		{
			VbatSampleSum += ADC_Read( 18 );
			++VbatSampleCnt;

			if ( !(gFlags.sample_vbat) )
				return;
		}

		gFlags.sample_vbat = 0;
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

	if ( !( gFlags.firing )
		|| ( !ISMODEVW(dfMode) && ( !ISMODETC(dfMode) || gFlags.check_mode) ) )
	{
		gFlags.decrease_voltage = 0;
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

	if ( v0 || gFlags.limit_power || bv < 290 )
	{
		gFlags.decrease_voltage = 1;
		ShowWeakBatFlag = 5;
		if ( ISMODEVW(dfMode) && ( PowerScale > 5 ))
			--PowerScale;
		gFlags.limit_power = 0;
	}
	else
	{
		gFlags.decrease_voltage = 0;
	}
	return 0;
}
