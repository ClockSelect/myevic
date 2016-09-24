#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "meadc.h"
#include "atomizer.h"


//=========================================================================

typedef struct
{
	const uint16_t percent;
	const uint16_t voltage;
}
BatV2P_t;

const BatV2P_t GENB_VTable[] =
{
	{   0, 310 }, 
	{  10, 330 },
	{  20, 342 },
	{  30, 350 },
	{  40, 358 },
	{  50, 363 },
	{  60, 368 },
	{  70, 379 },
	{  80, 389 },
	{  90, 400 },
	{ 100, 410 }
};

const BatV2P_t S25R_VTable[] =
{
	{   0, 302 },
	{   1, 310 },
	{   5, 327 },
	{   9, 341 },
	{  25, 357 },
	{  39, 365 },
	{  70, 391 },
	{  84, 405 },
	{  93, 409 },
	{  97, 417 },
	{ 100, 419 }
};

const BatV2P_t LHG2_VTable[] =
{
	{   0, 312 },
	{   4, 326 },
	{   9, 338 },
	{  13, 345 },
	{  26, 356 },
	{  38, 364 },
	{  79, 401 },
	{  96, 411 },
	{  98, 414 },
	{  99, 417 },
	{ 100, 420 }
};

const BatV2P_t S30Q_VTable[] =
{
	{   0, 312 },
	{  15, 340 },
	{  23, 352 },
	{  54, 381 },
	{  68, 391 },
	{  75, 400 },
	{  81, 403 },
	{  94, 408 },
	{  97, 411 },
	{  99, 416 },
	{ 100, 420 }
};

const BatV2P_t VTC4_VTable[] =
{
	{   0, 314 },
	{   1, 321 },
	{   2, 331 },
	{   4, 339 },
	{   6, 343 },
	{  14, 351 },
	{  22, 356 },
	{  49, 367 },
	{  66, 380 },
	{  99, 419 },
	{ 100, 421 }
};

const BatV2P_t VTC5_VTable[] =
{
	{   0, 305 },
	{   1, 310 },
	{   2, 320 },
	{   4, 329 },
	{  14, 342 },
	{  23, 351 },
	{  45, 365 },
	{  79, 396 },
	{  95, 411 },
	{  99, 418 },
	{ 100, 420 }
};

typedef struct
{
	const uint16_t	*name;
	const BatV2P_t	*V2P;
	const uint16_t	cutoff;
	const uint16_t	intrez;
}
Battery_t;

const Battery_t Batteries[] =
{
	{
		String_GEN,
		GENB_VTable,
		280,
		10
	},

	{
		String_25R,
		S25R_VTable,
		250,
		15
	},

	{
		String_HG2,
		LHG2_VTable,
		200,
		25
	},

	{
		String_30Q,
		S30Q_VTable,
		250,
		20
	},

	{
		String_VT4,
		VTC4_VTable,
		250,
		25
	},

	{
		String_VT5,
		VTC5_VTable,
		250,
		25
	}
};

#define NBATTERIES (sizeof(Batteries)/sizeof(Battery_t))

//-------------------------------------------------------------------------

uint16_t	LowBatVolts;
uint32_t	PowerScale;
uint16_t	BatteryVoltage;
uint16_t	SavedBatVoltage;
uint8_t		BatteryPercent;
uint8_t		BatteryTenth;
uint8_t		NoEventTimer;
uint8_t		BatReadTimer;

uint8_t		byte_20000048;

const Battery_t	*Battery;


//=========================================================================

__myevic__ int GetNBatteries()
{
	return NBATTERIES;
}


__myevic__ void SetBatteryModel()
{
	Battery = &Batteries[dfBatteryModel];
	// TODO: more to come
}


__myevic__ const uint16_t *GetBatteryName()
{
	return Batteries[dfBatteryModel].name;
}


//=========================================================================
__myevic__ int BatteryVoltsToPercent( int bv )
{
	int bpc;
	int i;

	const BatV2P_t *v2p = Battery->V2P;

	if ( bv <= v2p[0].voltage )
		return 0;

	if ( bv >= v2p[10].voltage )
		return 100;

	i = 1;
	do
	{
		if ( v2p[i].voltage >= bv )
			break;
		++i;
	}
	while ( i < 11 );

	// Linear interpolation.
	// Will be pessimistic on the high-side, and optimistic on the low-side.
	bpc = ( v2p[i-1].percent )
		+	( bv - v2p[i-1].voltage )
		  * ( v2p[i].percent - v2p[i-1].percent ) / ( v2p[i].voltage - v2p[i-1].voltage );
	
	return bpc;
}


//=========================================================================
//----- (0000065C) --------------------------------------------------------

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


//=========================================================================
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
			if ( ISVTWOMINI || ISEVICAIO )
			{
				VbatSampleSum += ADC_Read( 0 );
			}
			else
			{
				VbatSampleSum += ADC_Read( 18 );
			}
			++VbatSampleCnt;

			if ( !(gFlags.sample_vbat) )
				return;
		}

		gFlags.sample_vbat = 0;
		newbv = VbatSampleSum >> 7;

		VbatSampleSum = newbv;
		BatteryVoltage = newbv;

		if ( ( NoEventTimer <= 100 )
			&& (	( SavedBatVoltage > newbv && SavedBatVoltage - newbv > 3 )
				||	( newbv > SavedBatVoltage && newbv - SavedBatVoltage > 3 ) ) )
		{
			NewBatteryVoltage();
		}

		VbatSampleCnt = 0;
		VbatSampleSum = 0;
	}
}


//=========================================================================
//----- (0000074C) --------------------------------------------------------

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
		if ( ISVTWOMINI )
		{
			bv = ADC_Read( 0 ) >> 3;
		}
		else
		{
			bv = ADC_Read( 18 ) >> 3;
		}
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
		&& pwr > MAXPWRLIMIT
		&& PowerScale == 100
		&& bv >= 290
		&& 100 * ( bv - 290 ) / ( LowBatVolts - 290 ) < 10 )
	{
		v0 = 1;
		PowerScale = 100 * MAXPWRLIMIT / pwr;
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
