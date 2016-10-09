#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "meadc.h"
#include "atomizer.h"
#include "myprintf.h"
#include "timers.h"


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

const BatV2P_t LHE4_VTable[] =
{
	{   0, 323 },
	{   3, 332 },
	{  11, 347 },
	{  19, 352 },
	{  27, 359 },
	{  48, 369 },
	{  62, 379 },
	{  84, 400 },
	{  96, 413 },
	{  99, 418 },
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
		25
	},

	{
		String_25R,
		S25R_VTable,
		280,
		20
	},

	{
		String_HG2,
		LHG2_VTable,
		280,
		20
	},

	{
		String_HE4,
		LHE4_VTable,
		280,
		20
	},

	{
		String_30Q,
		S30Q_VTable,
		280,
		20
	},

	{
		String_VT4,
		VTC4_VTable,
		280,
		20
	},

	{
		String_VT5,
		VTC5_VTable,
		280,
		20
	}
};

#define NBATTERIES (sizeof(Batteries)/sizeof(Battery_t))

//-------------------------------------------------------------------------

uint16_t	LowBatVolts;
uint32_t	PowerScale;
uint16_t	BatteryVoltage;
uint16_t	BatteryCutOff;
uint16_t	BatteryIntRez;
uint16_t	BatteryMaxPwr;
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
	BatteryCutOff = Battery->cutoff;
	if ( !BatteryIntRez )
	{
		BatteryIntRez = 100;

		if ( Battery->intrez > BatteryIntRez )
		{
			BatteryIntRez = Battery->intrez;
		}
	}
}


__myevic__ const uint16_t *GetBatteryName()
{
	return Batteries[dfBatteryModel].name;
}


//=========================================================================
__myevic__ int ReadBatterySample( int nbat )
{
	int sample;

	if ( nbat == 0 )
	{
		if ( ISVTWOMINI || ISEVICAIO )
		{
			sample = ADC_Read( 0 );
		}
		else
		{
			sample = ADC_Read( 18 );
		}
	}
	else if ( nbat == 1 )
	{
		if ( ISVTCDUAL )
		{
			sample = ADC_Read( 4 );
		}
		else
		{
			sample = 0;
		}
	}
	else
	{
		sample = 0;
	}

	return sample;
}


//=========================================================================
__myevic__ void SetBatMaxPower()
{
	// Assume 90% efficiency of the circuitry
	int Pmax = 90	* ( BatteryCutOff + 10 )
					* ( BatteryVoltage - ( BatteryCutOff + 10 ) )
					/ BatteryIntRez;

	BatteryMaxPwr = Pmax / 100;

//	int Imax = 10000 * ( BatteryVoltage - ( BatteryCutOff + 10 ) ) / BatteryIntRez;
//	myprintf( "Imax = %dA, Pmax = %dW\n", Imax / 1000, Pmax / 1000 );
}


//=========================================================================
// Read battery internal resistance
//-------------------------------------------------------------------------
__myevic__ void ReadInternalResistance()
{
	int sample, i;
	int iato, ibat, vato, vbat, rez;

	sample = 0;
	for ( i = 0 ; i < 16 ; ++i )
		sample += ADC_Read( 1 );
	// V * 100
	vato = ( 13 * sample / 48 ) >> 4;

	sample = 0;
	for ( i = 0 ; i < 16 ; ++i )
		sample += ADC_Read( 2 );
	// A * 1000
	iato = ( 625 * sample / AtoShuntRez ) >> 4;

	sample = 0;
	for ( i = 0 ; i < 16 ; ++i )
		sample += ReadBatterySample( 0 );
	// V * 100
	vbat = sample >> 7;

	// Assume 90% efficiency of the circuitry
	ibat = ( 10 * vato * iato ) / ( 9 * vbat );

	rez = ( 10000 * ( BatteryVoltage - vbat ) ) / ibat;

	// There's no battery with internal resistance
	// less than 20mOhm.
	if ( rez >= 20 /* BatteryIntRez */ )
	{
		BatteryIntRez = rez;
		SetBatMaxPower();
	}

	gFlags.read_bir = 0;
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

		if ( ( BatteryVoltage > Battery->V2P[0].voltage ) && ( Screen == 1 ) )
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
			VbatSampleSum += ReadBatterySample( 0 );
			++VbatSampleCnt;

			if ( !(gFlags.sample_vbat) )
				return;
		}

		gFlags.sample_vbat = 0;
		newbv = ( VbatSampleSum >> 7 ) + dfBVOffset;

		VbatSampleSum = newbv;
		BatteryVoltage = newbv;

		if ( ( NoEventTimer <= 100 )
			&& (	( SavedBatVoltage > newbv && SavedBatVoltage - newbv > 3 )
				||	( newbv > SavedBatVoltage && newbv - SavedBatVoltage > 3 ) ) )
		{
			gFlags.read_bir = 1;
			SetBatMaxPower();
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

	if ( PreheatTimer )
	{
		pwr = PreheatPower;
	}
	else if ( dfMode == 6 )
	{
		pwr = dfSavedCfgPwr[ConfigIndex];
	}
	else
	{
		pwr = dfPower;
	}

	RTBatVolts = 0;

	i = 0;
	do
	{
		bv = ( ReadBatterySample( 0 ) >> 3 ) + dfBVOffset;
		if ( bv > BatteryCutOff )
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
		|| ( !ISMODEVW(dfMode) && ( !ISMODETC(dfMode) || gFlags.check_mode ) ) )
	{
		gFlags.decrease_voltage = 0;
		return 0;
	}

	int limit_voltage = BatteryCutOff + 10;

	if ( LowBatVolts
		&& pwr > BatteryMaxPwr
		&& PowerScale == 100
		&& bv >= limit_voltage
		&& 100 * ( bv - limit_voltage ) / ( LowBatVolts - limit_voltage ) < 10
		)
	{
		v0 = 1;
		PowerScale = 100 * BatteryMaxPwr / pwr;
	}

	if ( v0 || gFlags.limit_power || bv < limit_voltage )
	{
		if ( !gFlags.limit_power || !gFlags.read_bir )
		{
			ShowWeakBatFlag = 5;
		}

		gFlags.limit_power = 0;
		gFlags.decrease_voltage = 1;

		if ( ISMODEVW(dfMode) && ( PowerScale > 5 ))
		{
			--PowerScale;
		}
	}
	else
	{
		gFlags.decrease_voltage = 0;

		if (( PowerScale < 100 ) && ( bv > limit_voltage ))
		{
			++PowerScale;
		}
	}
	return 0;
}



