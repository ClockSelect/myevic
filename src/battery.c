#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "meadc.h"
#include "atomizer.h"
#include "timers.h"
#include "miscs.h"
#include "battery.h"


//=========================================================================

// Cuboid Mini / eVic Basic / eGrip II
const Battery_t BatteryCuboMini =
{
	String_GEN,
	{
		{   0, 340 },
		{  10, 357 },
		{  20, 367 },
		{  30, 370 },
		{  40, 373 },
		{  50, 377 },
		{  60, 382 },
		{  70, 389 },
		{  80, 395 },
		{  90, 402 },
		{ 100, 410 }
	},
	280,
	25,
	20
};

// Wismec RX Mini
const Battery_t BatteryRXMini =
{
	String_GEN,
	{
		{   0, 310 },
		{  10, 330 },
		{  20, 367 },
		{  30, 370 },
		{  40, 373 },
		{  50, 377 },
		{  60, 382 },
		{  70, 389 },
		{  80, 395 },
		{  90, 402 },
		{ 100, 410 }
	},
	280,
	25,
	20
};

const Battery_t const Batteries[] =
{
	// Generic 18650
	{
		String_GEN,
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
		},
		280,
		25,
		20
	},

	// Samsung 25R
	{
		String_25R,
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
		},
		280,
		20,
		25
	},

	// LG HG2
	{
		String_HG2,
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
		},
		280,
		20,
		20
	},

	// LG HE4
	{
		String_HE4,
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
		},
		280,
		20,
		25
	},

	// Samsung 30Q
	{
		String_30Q,
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
		},
		280,
		20,
		25
	},

	// Sony VTC4
	{
		String_VT4,
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
		},
		280,
		20,
		30
	},

	// Sony VTC5
	{
		String_VT5,
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
		},
		280,
		20,
		30
	},

	// Sony VTC6
	{
		String_VT6,
		{
			{   0, 324 },
			{  11, 342 },
			{  14, 348 },
			{  35, 366 },
			{  36, 368 },
			{  53, 383 },
			{  63, 389 },
			{  78, 403 },
			{  91, 409 },
			{  99, 416 },
			{ 100, 420 }
		},
		280,
		20,
		25
	}
};

#define NBATTERIES (sizeof(Batteries)/sizeof(Battery_t))

//-------------------------------------------------------------------------

uint16_t	LowBatVolts;
uint32_t	PowerScale;
uint16_t	BatteryVoltage;
uint16_t	BattVoltsHighest;
uint16_t	BattVoltsTotal;
uint16_t	BattVolts[4];
uint16_t	BatteryCutOff;
uint16_t	BatteryIntRez;
uint16_t	BatteryMaxAmp;
uint16_t	BatteryMaxPwr;
uint16_t	SavedBatVoltage;
uint8_t		BatteryPercent;
uint8_t		BatteryTenth;
uint8_t		NoEventTimer;
uint8_t		BatReadTimer;
uint8_t		NumBatteries;
uint8_t		MaxBatteries;
uint16_t	ChargerDuty;
uint16_t	MaxChargerDuty;
uint16_t	ChBalTimer;
uint16_t	RTBattVolts;
uint16_t	RTBVolts[4];
uint16_t	RTBVTotal;

uint8_t		BattProbeCount;

Battery_t	CustomBattery;

const Battery_t	*Battery;

uint8_t		USBMaxLoad;
uint8_t		ChargeStatus;
uint8_t		BatteryStatus;
uint8_t		BBBits;
uint8_t		ChargeMode;
uint8_t		ChargeStep;


//=========================================================================

__myevic__ int GetNBatteries()
{
	return NBATTERIES;
}


__myevic__ void SetBatteryModel()
{
	if ( dfBatteryModel == BATTERY_CUSTOM )
	{
		LoadCustomBattery();
		Battery = &CustomBattery;
	}
	else
	{
		Battery = &Batteries[dfBatteryModel];

		if ( dfBatteryModel == 0 )
		{
			if ( ISCUBOMINI || ISEVICBASIC || ISEGRIPII || ISVTWO )
			{
				Battery = &BatteryCuboMini;
			}
			else if ( ISRXMINI )
			{
				Battery = &BatteryRXMini;
			}
		}
	}

	BatteryCutOff = Battery->cutoff;

	if ( !BatteryIntRez )
	{
		BatteryIntRez = 20;

		if ( Battery->intrez > BatteryIntRez )
		{
			BatteryIntRez = Battery->intrez;
		}

		BatteryMaxAmp = Battery->maxamp * 100;
	}

	NewBatteryVoltage();
	SetBatMaxPower();

	gFlags.read_battery = 1;
}


__myevic__ const uint8_t *GetBatteryName()
{
	return Battery->name;
}


//=========================================================================
__myevic__ uint32_t ReadBatterySample( int nbat )
{
	uint32_t sample;

	if ( nbat < MaxBatteries )
	{
		if ( nbat == 0 )
		{
			if ( ISPRIMOMINI || ISVTWOMINI || ISEVICAIO )
			{
				sample = ADC_Read( 0 );
			}
			else if ( ISVTCDUAL )
			{
				sample = ADC_Read( 4 );
			}
			else
			{
				sample = ADC_Read( 18 );
			}
		}
		else if ( nbat == 1 )
		{
			sample = ADC_Read( 0 );
		}
		else if ( nbat == 2 )
		{
			sample = ADC_Read( 4 );
		}
		else if ( nbat == 3 )
		{
			sample = ADC_Read( 14 );
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
	// 100th of an Amp
	int Imax = 1000	* ( BatteryVoltage - ( BatteryCutOff + 10 ) )
					/ BatteryIntRez;

	if ( Imax > BatteryMaxAmp )
	{
		Imax = BatteryMaxAmp;
	}

	// milliWatts
	// Assume 90% efficiency of the circuitry
	int Pmax = 90	* ( BatteryVoltage - BatteryIntRez * Imax / 1000 )
					* Imax / 1000;

	BatteryMaxPwr = NumBatteries * Pmax / 100;
}


//=========================================================================
// Read battery internal resistance
//-------------------------------------------------------------------------
__myevic__ void ReadInternalResistance()
{
	static filter_t filter;

	uint32_t ibat, rez;

	// mA
	// Assume 90% efficiency of the circuitry
	ibat = ( 1000 * AtoVolts * AtoCurrent ) / ( 9 * RTBVTotal );

	// mOhm
	rez = ( 10000 * ( BattVoltsTotal - RTBVTotal ) ) / ibat;

	rez = FilterMedian( &filter, rez );

	if ( filter.count >= FILTER_SIZE )
	{
		// Mean rez per battery, rounded up
		rez = ( rez + NumBatteries / 2 ) / NumBatteries;

		// There's no battery with internal resistance
		// less than 20mOhm.
		if ( rez >= 20 /* BatteryIntRez */ )
		{
			BatteryIntRez = rez;
			SetBatMaxPower();
		}

		InitFilter( &filter );
		gFlags.read_bir = 0;
	}
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

	if ( BatteryStatus == 2 )
	{
		BatteryPercent = 0;
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
// Setup battery globals after voltage reading
//-------------------------------------------------------------------------
__myevic__ void NewBatteryData()
{
	gFlags.batteries_ooe = 0;

	BattVoltsHighest = 0;
	BattVoltsTotal = 0;

	for ( int i = 0 ; i < NumBatteries ; ++i )
	{
		BattVoltsTotal += BattVolts[i];

		if ( BattVolts[i] > BattVoltsHighest )
			BattVoltsHighest = BattVolts[i];

		if ( !i || BattVolts[i] < BatteryVoltage )
			BatteryVoltage = BattVolts[i];
	}

	if ( BattVoltsHighest - BatteryVoltage > 30 )
	{
		gFlags.batteries_ooe = 1;
	}
}


//=========================================================================
// RX300 Charge Balancing (maybe)
//-------------------------------------------------------------------------
__myevic__ void ChargeBalance()
{
	if ( gFlags.usb_attached )
	{
		if ( ChBalTimer > 1000 )
			ChBalTimer = 0;

		if ( ChBalTimer <= 50 )
		{
			BBBits = 0;

			for ( int i = 0 ; i < NumBatteries ; ++i )
			{
				uint16_t bv = BattVolts[i];

				if ( bv <= 250 || bv < BatteryVoltage + 3 )
					BBBits &= ~( 1 << i );
				else
					BBBits |= ( 1 << i );
			}
		}
		else if ( gFlags.battery_charging )
		{
			PF5 = ( BBBits & 1 ) != 0;
			PF6 = ( BBBits & 2 ) != 0;
			PA3 = ( BBBits & 4 ) != 0;
			PA2 = ( BBBits & 8 ) != 0;
		}
	}

	if ( !gFlags.usb_attached || !gFlags.battery_charging || ChBalTimer <= 50 )
	{
		PF5 = 0;
		PF6 = 0;
		PA3 = 0;
		PA2 = 0;
	}
}


//=========================================================================
//----- (000006E0) --------------------------------------------------------
__myevic__ void ReadBatteryVoltage()
{
	static uint32_t VbatSample1 = 0;
	static uint32_t VbatSample2 = 0;
	static uint32_t VbatSample3 = 0;
	static uint32_t VbatSample4 = 0;
	static uint8_t	VbatSampleCnt = 0;

	if ( !gFlags.firing )
	{
		while ( VbatSampleCnt < 16 )
		{
			VbatSample4 += ReadBatterySample( 3 );
			VbatSample3 += ReadBatterySample( 2 );
			VbatSample1 += ReadBatterySample( 0 );
			VbatSample2 += ReadBatterySample( 1 );

			++VbatSampleCnt;

			if ( !gFlags.sample_vbat )
				return;
		}

		gFlags.sample_vbat = 0;

		if ( ISVTCDUAL )
		{
			VbatSample1 = ( VbatSample1 >> 7 ) + 2;
			VbatSample2 = 139 * ( VbatSample2 >> 4 ) / 624;

			BattVolts[0] = VbatSample1;

			if ( VbatSample1 + VbatSample1 / 5 < VbatSample2 )
			{
				BattVolts[1] = VbatSample2 - VbatSample1;
				NumBatteries = 2;
			}
			else if (( VbatSample1 - VbatSample1 / 10 < VbatSample2 ) && ( VbatSample2 >= 100 ))
			{
				NumBatteries = 1;
			}
			else
			{
				BatteryVoltage = 0;
				BattVolts[0] = 0;
				BattVolts[1] = 0;
				NumBatteries = 0;
			}
		}
		else if ( ISCUBOID )
		{
			VbatSample2 = 139 * ( VbatSample2 >> 4 ) / 624;;
			if ( VbatSample2 ) VbatSample2 += 4;

			VbatSample1 = ( VbatSample1 >> 7 ) + 32;
			BattVolts[0] = VbatSample1;

			if ( VbatSample1 > 250 )
				BattVolts[0] = 20 * (VbatSample1 - 250) / 154 + VbatSample1;

			if ( VbatSample2 <= BattVolts[0] )
				BattVolts[1] = 0;
			else
				BattVolts[1] = VbatSample2 - BattVolts[0];
		}
		else if ( ISCUBO200 )
		{
			VbatSample1 = 125 * ( VbatSample1 >> 7 ) / 100;
			VbatSample2 =  13 * ( VbatSample2 >> 4 ) / 48;
			VbatSample3 =   3 * ( VbatSample3 >> 4 ) >> 3;

			if ( VbatSample3 ) VbatSample3 += 3;

			BattVolts[0] = VbatSample1;

			if ( VbatSample2 <= VbatSample1 )
				BattVolts[1] = 0;
			else
				BattVolts[1] = VbatSample2 - VbatSample1;

			if ( VbatSample2 >= VbatSample3 )
				BattVolts[2] = 0;
			else
				BattVolts[2] = VbatSample3 - VbatSample1 - BattVolts[1];
		}
		else if ( ISRX200S || ISRX23 )
		{
		//	myprintf( "S1=%d S2=%d S3=%d\n",
		//		VbatSample1, VbatSample2, VbatSample3 );

			VbatSample1 = 125 * ( VbatSample1 >> 7 ) / 100;
			VbatSample2 = 139 * ( VbatSample2 >> 4 ) / 624;
			VbatSample3 =   3 * ( VbatSample3 >> 4 ) >> 3;

			BattVolts[0] = VbatSample1;

			if ( VbatSample2 <= VbatSample1 + 20 )
				BattVolts[1] = 0;
			else
				BattVolts[1] = VbatSample2 - BattVolts[0];

			if ( VbatSample3 <= VbatSample2 + 20 )
				BattVolts[2] = 0;
			else
				BattVolts[2] = VbatSample3 - BattVolts[0] - BattVolts[1];

			if ( BattVolts[0] )
			{
				gFlags.bad_cell = ( VbatSample1 > VbatSample2 + 100 );

				if ( BattVolts[1] )
				{
					gFlags.bad_cell = ( VbatSample2 > VbatSample3 + 100 );
				}
			}

		//	myprintf( "S1=%d S2=%d S3=%d V1=%d V2=%d V3=%d\n",
		//		VbatSample1, VbatSample2, VbatSample3,
		//		BattVolts[0], BattVolts[1], BattVolts[2] );
		}
		else if ( ISRX300 )
		{
			VbatSample1 >>= 7;
			VbatSample2 = 139 * ( VbatSample2 >> 4 ) / 624;
			VbatSample3 =   3 * ( VbatSample3 >> 4 ) >> 3;
			VbatSample4 =  29 * ( VbatSample4 >> 4 ) >> 6;

			BattVolts[0] = VbatSample1;

			if ( VbatSample2 < VbatSample1 )
				BattVolts[1] = 0;
			else
				BattVolts[1] = VbatSample2 - VbatSample1;

			if ( VbatSample3 < VbatSample2 )
				BattVolts[2] = 0;
			else
				BattVolts[2] = VbatSample3 - VbatSample2;

			if ( VbatSample4 < VbatSample3 )
				BattVolts[3] = 0;
			else
				BattVolts[3] = VbatSample4 - VbatSample3;
		}
		else
		{
			VbatSample1 >>= 7;
			BattVolts[0] = VbatSample1;
		}

		if ( ISVTCDUAL )
		{
			if ( NumBatteries == 1 )
			{
				BattVolts[0] += dfBVOffset[0];
			}
			else if ( NumBatteries == 2 )
			{
				BattVolts[0] += dfBVOffset[1];
				BattVolts[1] += dfBVOffset[2];
			}
		}
		else
		{
			for ( int i = 0 ; i < NumBatteries ; ++i )
				if ( BattVolts[i] )
					BattVolts[i] += dfBVOffset[i];
		}

		NewBatteryData();

		if ( ISRX300 )
		{
			ChargeBalance();
		}

		if ( ( NoEventTimer <= 100 )
			&& (	( SavedBatVoltage > BatteryVoltage && SavedBatVoltage - BatteryVoltage > 3 )
				||	( BatteryVoltage > SavedBatVoltage && BatteryVoltage - SavedBatVoltage > 3 ) ) )
		{
			gFlags.read_bir = 1;
			SetBatMaxPower();
			NewBatteryVoltage();
		}

		VbatSampleCnt = 0;
		VbatSample1 = 0;
		VbatSample2 = 0;
		VbatSample3 = 0;
		VbatSample4 = 0;
	}
}


//=========================================================================
//----- (0000074C) --------------------------------------------------------

__myevic__ int CheckBattery()
{
	int v0;
	int pwr;
	int i;
	int bvtot, bv, bv2, bv3, bv4, bs;

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

	RTBattVolts = 0;
	RTBVTotal = 0;

	i = 0;
	do
	{
		if ( ISVTCDUAL )
		{
			bv = ( ReadBatterySample( 0 ) >> 3 ) + 2;

			if ( NumBatteries == 2 )
			{
				bs = 139 * ReadBatterySample( 1 ) / 624;
				bv2 = ( bs <= bv ) ? 0 : ( bs - bv );

				bv  += dfBVOffset[1];
				bv2 += dfBVOffset[2];

				RTBVolts[0] = bv;
				RTBVolts[1] = bv2;

				RTBVTotal = bv + bv2;

				if ( bv2 < bv ) bv = bv2;
			}
			else
			{
				bv  += dfBVOffset[0];

				RTBVolts[0] = bv;

				RTBVTotal = bv;
			}
		}
		else if ( ISCUBOID )
		{
			bvtot = 139 * ReadBatterySample( 1 ) / 624;
			if ( bvtot ) bvtot += 4;

			bv = ( ReadBatterySample( 0 ) >> 3 ) + 32;

			if ( bv > 250 )
				bv += 20 * ( bv - 250 ) / 154;

			if ( bvtot <= bv )
				bv2 = 0;
			else
				bv2 = bvtot - bv;

			bv  += dfBVOffset[0];
			bv2 += dfBVOffset[1];

			RTBVolts[0] = bv;
			RTBVolts[1] = bv2;

			RTBVTotal = bv + bv2;

			if ( bv2 < bv ) bv = bv2;
		}
		else if ( ISCUBO200 )
		{
			bv3 =   3 * ReadBatterySample( 2 ) >> 3;
			bv  = 125 * ( ReadBatterySample( 0 ) >> 3 ) / 100;
			bv2 =  13 * ReadBatterySample( 1 ) / 48u;

			bv2 = ( bv >= bv2 ) ? 0 : bv2 - bv;
			bv3 = ( bv + bv2 >= bv3 ) ? 0 : bv3 - bv - bv2;

			bv  += dfBVOffset[0];
			bv2 += dfBVOffset[1];
			bv3 += dfBVOffset[2];

			RTBVolts[0] = bv;
			RTBVolts[1] = bv2;
			RTBVolts[2] = bv3;

			RTBVTotal = bv + bv2 + bv3;

			if ( bv3 < bv ) bv = bv3;
			if ( bv2 < bv ) bv = bv2;
		}
		else if ( ISRX200S || ISRX23 )
		{
			bv  = 125 * ( ReadBatterySample( 0 ) >> 3 ) / 100;

			if ( NumBatteries > 1 )
			{
				bv2 = 139 * ReadBatterySample( 1 ) / 624;

				if ( bv2 <= bv )
					bv2 = 0;
				else
					bv2 = bv2 - bv;

				if ( NumBatteries == 2 )
				{
					bv  += dfBVOffset[0];
					bv2 += dfBVOffset[1];

					RTBVolts[0] = bv;
					RTBVolts[1] = bv2;

					RTBVTotal = bv + bv2;
				}
				else
				{
					bvtot = 3 * ReadBatterySample( 2 ) >> 3;

					if ( bv + bv2 >= bvtot )
						bv3 = 0;
					else
						bv3 = bvtot - bv - bv2;

					bv  += dfBVOffset[0];
					bv2 += dfBVOffset[1];
					bv3 += dfBVOffset[2];

					RTBVolts[0] = bv;
					RTBVolts[1] = bv2;
					RTBVolts[2] = bv3;

					RTBVTotal = bv + bv2 + bv3;

					if ( bv3 < bv ) bv = bv3;
				}

				if ( bv2 < bv ) bv = bv2;
			}
			else if ( ISRX300 )
			{
				bv4 = 29 * ReadBatterySample( 3 ) >> 6;
				bv3 = 3 * ReadBatterySample( 2 ) >> 3;
				bv  = ReadBatterySample( 0 ) >> 3;
				bv2 = 139 * ReadBatterySample( 1 ) / 624;

				if ( bv4 < bv3 )
					bv4 = 0;
				else
					bv4 = bv4 - bv3;

				if ( bv3 < bv2 )
					bv3 = 0;
				else
					bv3 = bv3 - bv2;

				if ( bv2 < bv )
					bv2 = 0;
				else
					bv2 = bv2 - bv;

				bv  += dfBVOffset[0];
				bv2 += dfBVOffset[1];
				bv3 += dfBVOffset[2];
				bv4 += dfBVOffset[3];

				RTBVolts[0] = bv;
				RTBVolts[1] = bv2;
				RTBVolts[2] = bv3;
				RTBVolts[3] = bv4;

				RTBVTotal = bv + bv2 + bv3 + bv4;

				if ( bv2 < bv ) bv = bv2;
				if ( bv3 < bv ) bv = bv3;
				if ( bv4 < bv ) bv = bv4;
			}
			else
			{
				bv  += dfBVOffset[0];

				RTBVolts[0] = bv;

				RTBVTotal = bv;
			}
		}
		else
		{
			bv = ( ReadBatterySample( 0 ) >> 3 ) + dfBVOffset[0];

			RTBVolts[0] = bv;

			RTBVTotal = bv;
		}

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

	RTBattVolts = bv;

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


//=========================================================================
// Battery Charging (VTC Dual)
//-------------------------------------------------------------------------
__myevic__ void BatteryChargeDual()
{
	uint32_t ChargeCurrent, USBVolts;

	static uint32_t ChargerTarget = 0;
	static uint32_t ChargerTempo = 0;

	ChargeCurrent = ADC_Read( 13 ) >> 2;
	USBVolts = 147 * ADC_Read( 3 ) / 752 + 5;

//	myprintf( "nb=%d, CC=%d USBVolts=%d, 1A=%d, CS=%d, BS=%d PD1=%d\n",
//			NumBatteries, ChargeCurrent, USBVolts,
//			USBMaxLoad, ChargeStatus, BatteryStatus, PD1 );

	if (( NumBatteries == 0 ) || (( NumBatteries == 2 ) && ( BatteryVoltage < 250u )) )
	{
		BatteryStatus = 2;

		PA3 = 0;
		PA2 = 0;
		PF2 = 0;
		PC3 = 0;
	}
	else if ( NumBatteries == 1 )
	{
		PC3 = 1;
		PA2 = 0;

		if ( gFlags.batt_unk )
		{
			gFlags.batt_unk = 0;

			MaxPower = 750;
			MaxTCPower = 750;
			SetAtoLimits();
		}

		if ( BatteryStatus != 2 && BatteryStatus != 3 && BatteryStatus != 4 )
		{
			PA3 = 1;

			if ( dfStatus.usbchgoff )
			{
				PF2 = 0;
				gFlags.battery_charging = 0;
			}
			else
			{
				PF2 = !( gFlags.battery_charging && ( BatteryVoltage >= 430 ) );
			}
		}
	}
	else if (( NumBatteries == 2 ) && ( !gFlags.batt_unk ))
	{
		PC3 = 0;

		gFlags.batt_unk = 1;

		MaxPower = 1500;
		MaxTCPower = 1500;
		SetAtoLimits();

		if ( BatteryStatus != 2 && BatteryStatus != 3 && BatteryStatus != 4 )
		{
			PA3 = 1;
			PF2 = 0;
			PA2 = 1;
		}
	}
	else if ( gFlags.usb_attached &&  USBVolts > 580 )
	{
		BatteryStatus = 3;

		PA3 = 0;
		PC3 = 0;
		PA2 = 0;
		PF2 = 0;

		if ( ChargeStatus != 6 )
		{
			gFlags.refresh_display = 1;
			Screen = 57;
			ScreenDuration = 2;
		}

		ChargeStatus = 6;
	}
	else if ( BatteryStatus != 4 )
	{
		if ( gFlags.batteries_ooe )
		{
			BatteryStatus = 1;
		}
		else if ( BatteryStatus )
		{
			BatteryStatus = 0;
		}

		PA3 = 1;
		PF2 = 0;
		PA2 = 1;
	}

	if ( gFlags.usb_attached )
	{
		if ( dfStatus.usbchgoff )
		{
			if ( ChargeStatus != 5 && ChargeStatus != 6 )
			{
				Event = 13;
				ChargeStatus = 6;
			}
		}
		else if ( BatteryStatus == 2 )
		{
			if ( ChargeCurrent > 250 )
			{
				gFlags.refresh_display = 1;
				Screen = 58;
				ScreenDuration = 2;
			}

			ChargeStatus = 6;
		}
		else if ( BatteryStatus == 3 )
		{
			if ( ChargeStatus != 6 )
			{
				gFlags.refresh_display = 1;
				Screen = 57;
				ScreenDuration = 2;
			}

			ChargeStatus = 6;
		}
		else if ( BatteryStatus == 4 )
		{
			if ( ChargeStatus != 6 )
			{
				gFlags.refresh_display = 1;
				Screen = 58;
				ScreenDuration = 2;
			}

			ChargeStatus = 6;
		}
		else if ( !gFlags.battery_charging )
		{
			if ( BattVoltsHighest < 420 && ChargeStatus != 5 )
			{
				if ( NumBatteries == 2 )
				{
					Event = 12;
					ChargeStatus = 0;
				}
			}
		}
		else
		{
			if ( NumBatteries == 2 )
			{
				if ( ( ChargeStatus == 4 ) && ( BattVoltsHighest > 422 ) )
				{
					Event = 13;
					ChargeStatus = 6;
				}

				PF2 = 0;

				if ( ChargeStatus != 5 && ChargeStatus != 6 )
				{
					if ( gFlags.battery_charging )
					{
						if ( BatteryVoltage < 290 )
						{
							ChargerTarget = 300;
							ChargeStatus = 2;
						}
						else if ( BattVoltsHighest < 416 )
						{
							if ( ChargeStatus != 4 )
							{
								if ( USBMaxLoad )
								{
									if ( USBVolts > 420 )
									{
										ChargerTarget = 1000;
									}
									else
									{
										ChargerTarget = 500;
										USBMaxLoad = 0;
									}
								}
								ChargeStatus = 3;
							}
						}
						else
						{
							ChargerTarget = 400;
							ChargeStatus = 4;
						}

						BBC_Configure( BBC_PWMCH_CHARGER, 1 );

						if ( ChargeCurrent > ChargerTarget + 10 || USBVolts < 400 )
						{
							if ( ChargerDuty )
							{
								--ChargerDuty;
							}
						}
						else if ( ChargeCurrent < ChargerTarget )
						{
							if ( ++ChargerTempo > 10 )
							{
								ChargerTempo = 0;

								if ( ChargeCurrent < 180 )
								{
									if ( ChargerDuty < MaxChargerDuty )
									{
										++ChargerDuty;
									}
									else
									{
										BatteryStatus = 4;

										PA3 = 0;
										PC3 = 0;
										PA2 = 0;
										PF2 = 0;
									}
								}
								else if ( ChargeCurrent < 1005 )
								{
									if ( ChargerDuty < MaxChargerDuty )
									{
										++ChargerDuty;
									}
								}
							}
						}

						PWM_SET_CMR( PWM0, BBC_PWMCH_CHARGER, ChargerDuty );

						return;
					}
				}
			}
		}
	}

	if ( NumBatteries == 2 )
	{
		PF2 = 0;
		gFlags.battery_charging = 0;
	}
	else if ( NumBatteries == 1 )
	{
		PA2 = 0;
	}

	PD7 = 0;
	BBC_Configure( BBC_PWMCH_CHARGER, 0 );
	PD7 = 0;
	ChargerDuty = 0;
}


//=========================================================================
// Battery Charging (Cuboid|RX200S|RX2/3)
//-------------------------------------------------------------------------
__myevic__ void BatteryCharge()
{
	uint32_t USBVolts, ChargeCurrent;

	static uint32_t ChargerTarget = 0;
	static uint32_t ChargerTempo = 0;
	static uint8_t	EOCTempo = 0;

	if ( ISRX300 )
	{
		ChargeCurrent = 135 * ADC_Read( 13 ) / 360;
	}
	else
	{
		ChargeCurrent = ADC_Read( 13 ) >> 2;
	}

	USBVolts = 147 * ADC_Read( 3 ) / 752 + 5;

//	myprintf( "ChargeCurrent=%d usbv=%d, b55=%d, b56=%d, BS=%d PF0=%d CD=%d\n",
//		ChargeCurrent, USBVolts,
//		USBMaxLoad, ChargeStatus, BatteryStatus,
//		PF0, ChargerDuty );

	if ( gFlags.bad_cell )
	{
		BatteryStatus = 2;
		PF0 = 0;
	}
	else if ( BatteryVoltage >= 250 )
	{
		if ( gFlags.usb_attached && USBVolts > 580 )
		{
			BatteryStatus = 3;
			PF0 = 0;
		}
		else
		{
			if ( BatteryStatus != 4 )
			{
				if ( gFlags.batteries_ooe )
				{
					BatteryStatus = 1;
				}
				else
				{
					BatteryStatus = 0;
				}

				PF0 = 1;
			}
		}
	}
	else if ( ISRX23 && BattVolts[0] >= 250 && BattVolts[1] >= 250 && BattVolts[2] < 20 )
	{
		NumBatteries = 2;
		NewBatteryData();

		MaxVolts = 600;
		MaxPower = 2000;
		MaxTCPower = 2000;
		SetAtoLimits();

		if ( BatteryStatus != 2 && BatteryStatus != 3 && BatteryStatus != 4 )
		{
			PF1 = 1;
		}
	}
	else
	{
		BatteryStatus = 2;
		PF0 = 0;
	}

	if ( gFlags.usb_attached )
	{
		if ( dfStatus.usbchgoff )
		{
			if ( ChargeStatus != 5 && ChargeStatus != 6 )
			{
				Event = 13;
				ChargeStatus = 6;
			}
		}
		else if ( BatteryStatus == 2 )
		{
			if ( ChargeCurrent > 250 )
			{
				gFlags.refresh_display = 1;
				Screen = 58;	// Charge Error
				ScreenDuration = 2;
			}
			ChargeStatus = 6;
		}
		else if ( BatteryStatus == 3 )
		{
			if ( ChargeStatus != 6 )
			{
				gFlags.refresh_display = 1;
				Screen = 57;	// USB Adapter Error
				ScreenDuration = 2;
			}
			ChargeStatus = 6;
		}
		else if ( BatteryStatus == 4 )
		{
			if ( ChargeStatus != 6 )
			{
				gFlags.refresh_display = 1;
				Screen = 58;	// Charge Error
				ScreenDuration = 2;
			}
			ChargeStatus = 6;
		}
		else if ( ChargeMode == 2 && ChargeStatus != 5 )
		{
			if ( BBBits )
			{
				if ( BattVoltsHighest < 412 || BatteryVoltage <= 300 )
				{
					EOCTempo = 0;

					ChargeMode = 1;
					Event = 12;
					ChargeStatus = 0;
				}
			}
			else
			{
				if ( BattVoltsHighest > 418 )
				{
					ChargeMode = 0;
					Event = 13;
					ChargeStatus = 5;
				}
				else
				{
					EOCTempo = 0;

					ChargeMode = 1;
					Event = 12;
					ChargeStatus = 0;
				}
			}
		}
		else if ( !gFlags.battery_charging )
		{
			EOCTempo = 0;

			if ( ChargeStatus != 5 )
			{
				if ( ( BattVoltsHighest < 420 ) || ( BBBits && BatteryVoltage < 415 ) )
				{
					Event = 12;
					ChargeStatus = 0;
				}
			}
		}
		else
		{
			if ( ( ChargeStatus == 4 ) && ( BattVoltsHighest > 422 ) )
			{
				if ( EOCTempo < 10 ) ++EOCTempo;

				if ( EOCTempo >= 10 )
				{
					if ( BBBits
						 && ( BattVoltsHighest > BatteryVoltage + 5 )
						 && ( ChargeStep < 2 ) )
					{
						++ChargeStep;
						ChargeMode = 2;
						ChargeStatus = 1;
					}
					else
					{
						ChargeMode = 0;
						Event = 13;
						ChargeStatus = 5;
					}
				}
			}
			else
			{
				EOCTempo = 0;
			}
		}

		if ( gFlags.battery_charging && ChargeStatus != 5 && ChargeStatus != 6 )
		{
			EOCTempo = 0;

			if ( ChargeMode != 2 )
			{
				if ( BatteryVoltage < 290 )
				{
					ChargeStatus = 2;
					ChargerTarget = 300;
				}
				else if ( BattVoltsHighest < 416 )
				{
					if ( ChargeStatus != 4 )
					{
						ChargeStatus = 3;

						if ( USBMaxLoad == 2 )
						{
							if ( USBVolts <= 420 )
							{
								USBMaxLoad = 1;
								ChargerTarget = 1000;
							}
							else
							{
								ChargerTarget = 1500;
							}
						}
						else if ( USBMaxLoad )
						{
							if ( USBVolts <= 420 )
							{
								USBMaxLoad = 0;
								ChargerTarget = 500;
							}
							else
							{
								ChargerTarget = 1000;
							}
						}
					}
				}
				else
				{
					ChargeStatus = 4;
					ChargerTarget = 600;
				}

				BBC_Configure( BBC_PWMCH_CHARGER, 1 );

				if ( ( ChargeCurrent > ChargerTarget + 10 ) || ( USBVolts < 400 ) )
				{
					if ( ChargerDuty )
					{
						--ChargerDuty;
					}
				}
				else if ( ChargeCurrent < ChargerTarget )
				{
					if (( ++ChargerTempo >  10 && ChargeCurrent <  1300 )
					||	(	ChargerTempo > 100 && ChargeCurrent >= 1300 ))
					{
						ChargerTempo = 0;

						if ( ChargerDuty >= MaxChargerDuty )
						{
							if ( ChargeCurrent < 180 )
							{
								BatteryStatus = 4;
								PF0 = 0;
							}
						}
						else if ( ChargeCurrent < 1507 )
						{
							++ChargerDuty;
						}
					}
				}

				PWM_SET_CMR( PWM0, BBC_PWMCH_CHARGER, ChargerDuty );

				return;
			}
		}
	}

	PD7 = 0;
	BBC_Configure( BBC_PWMCH_CHARGER, 0 );
	PD7 = 0;
	ChargerDuty = 0;
}


//=========================================================================
// Custom Battery Profile management
//-------------------------------------------------------------------------
// Write Custom Battery data to the dataflash
//-------------------------------------------------------------------------
__myevic__ void SaveCustomBattery( const Battery_t *b )
{
	int i;

	for ( i = 0 ; i < 11 ; ++i )
	{
		// Voltages are offset by 250 so values run from 60 to 170
		// and hold into a single byte.
		dfBattery.voltage[i] = b->V2P[i].voltage - 250;

		// First and last percents are mandatorily 0 and 100,
		// so it's useless to save them. 2 bytes saved.
		if ( i < 9 )
			dfBattery.percent[i] = b->V2P[i+1].percent;
	}

	dfBattery.cutoff = b->cutoff - 250;
	dfBattery.maxamp = b->maxamp;

	UpdateDFTimer = 1;
}


//-------------------------------------------------------------------------
// Read Custom Battery data from the dataflash
//-------------------------------------------------------------------------
__myevic__ void LoadCustomBattery()
{
	int i;

	CustomBattery.name = String_CUS;

	CustomBattery.V2P[ 0].percent = 0;
	CustomBattery.V2P[10].percent = 100;

	for ( i = 0 ; i < 11 ; ++i )
	{
		if ( i < 9 )
			CustomBattery.V2P[i+1].percent = dfBattery.percent[i];

		CustomBattery.V2P[i].voltage = dfBattery.voltage[i] + 250;
	}

	CustomBattery.cutoff = dfBattery.cutoff + 250;
	CustomBattery.maxamp = dfBattery.maxamp;
	CustomBattery.intrez = 20;
}


//-------------------------------------------------------------------------
// Reset Custom Battery data from Generic battery profile
//-------------------------------------------------------------------------
__myevic__ void ResetCustomBattery()
{
	SaveCustomBattery( &Batteries[0] );
	LoadCustomBattery();
}


//-------------------------------------------------------------------------
// Check Custom Battery data consistency
//-------------------------------------------------------------------------
__myevic__ int CheckCustomBattery()
{
	int i;

	if ( CustomBattery.V2P[0].percent != 0 ) return 0;
	if ( CustomBattery.V2P[10].percent != 100 ) return 0;

	for ( i = 0 ; i < 11 ; ++i )
	{
		if ( CustomBattery.V2P[i].percent > 100 )
			return 0;

		if ( CustomBattery.V2P[i].voltage < 310 || CustomBattery.V2P[i].voltage > 420 )
			return 0;

		if (( i > 0 ) && ( CustomBattery.V2P[i].percent <= CustomBattery.V2P[i-1].percent ))
			return 0;
	}

	if ( CustomBattery.cutoff < 275 || CustomBattery.cutoff > 350 ) return 0;

	return 1;
}


