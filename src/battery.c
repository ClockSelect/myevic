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

typedef struct
{
	const uint16_t percent;
	const uint16_t voltage;
}
BatV2P_t;

// Cuboid Mini / eVic Basic
const BatV2P_t CMIB_VTable[] =
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
};

// Generic 18650
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

const BatV2P_t VTC6_VTable[] =
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
};


typedef struct
{
	const uint16_t	*name;
	const BatV2P_t	*V2P;
	const uint16_t	cutoff;
	const uint16_t	intrez;
	const uint16_t	maxamp;
}
Battery_t;

const Battery_t BatteryCuboMini =
{
	String_GEN,
	CMIB_VTable,
	280,
	25,
	20
};

const Battery_t Batteries[] =
{
	{
		String_GEN,
		GENB_VTable,
		280,
		25,
		20
	},

	{
		String_25R,
		S25R_VTable,
		280,
		20,
		25
	},

	{
		String_HG2,
		LHG2_VTable,
		280,
		20,
		20
	},

	{
		String_HE4,
		LHE4_VTable,
		280,
		20,
		25
	},

	{
		String_30Q,
		S30Q_VTable,
		280,
		20,
		25
	},

	{
		String_VT4,
		VTC4_VTable,
		280,
		20,
		30
	},

	{
		String_VT5,
		VTC5_VTable,
		280,
		20,
		30
	},

	{
		String_VT6,
		VTC6_VTable,
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
uint16_t	BattVolts[3];
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
uint16_t	RTBattVolts;
uint16_t	RTBVTotal;

uint8_t		BattProbeCount;

const Battery_t	*Battery;

uint8_t		byte_20000055;
uint8_t		ChargeStatus;
uint8_t		BatteryStatus;

//=========================================================================

__myevic__ int GetNBatteries()
{
	return NBATTERIES;
}


__myevic__ void SetBatteryModel()
{
	Battery = &Batteries[dfBatteryModel];

	if ( dfBatteryModel == 0 )
	{
		if ( ISCUBOMINI || ISEVICBASIC || ISEGRIPII )
		{
			Battery = &BatteryCuboMini;
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
}


__myevic__ const uint16_t *GetBatteryName()
{
	return Batteries[dfBatteryModel].name;
}


//=========================================================================
__myevic__ uint32_t ReadBatterySample( int nbat )
{
	uint32_t sample;

	if ( nbat < MaxBatteries )
	{
		if ( nbat == 0 )
		{
			if ( ISVTWOMINI || ISEVICAIO )
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
//----- (000006E0) --------------------------------------------------------
__myevic__ void ReadBatteryVoltage()
{
	static uint32_t VbatSample1 = 0;
	static uint32_t VbatSample2 = 0;
	static uint32_t VbatSample3 = 0;
	static uint8_t	VbatSampleCnt = 0;

	if ( !gFlags.firing )
	{
		while ( VbatSampleCnt < 16 )
		{
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
				BattVolts[0] += dfBVOffset[1];
				BattVolts[1] = VbatSample2 - VbatSample1 + dfBVOffset[2];
				NumBatteries = 2;
			}
			else if (( VbatSample1 - VbatSample1 / 10 < VbatSample2 ) && ( VbatSample2 >= 100 ))
			{
				BattVolts[0] += dfBVOffset[0];
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

			BattVolts[0] += dfBVOffset[0];
			BattVolts[1] += dfBVOffset[1];
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

			for ( int i = 0 ; i < NumBatteries ; ++i )
				if ( BattVolts[i] )
					BattVolts[i] += dfBVOffset[i];
		}
		else
		{
			VbatSample1 >>= 7;
			BattVolts[0] = VbatSample1 + dfBVOffset[0];
		}

		NewBatteryData();

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
	}
}


//=========================================================================
//----- (0000074C) --------------------------------------------------------

__myevic__ int CheckBattery()
{
	int v0;
	int pwr;
	int i;
	int bvtot, bv, bv2, bv3, bs;

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

				RTBVTotal = bv + bv2;

				if ( bv2 < bv ) bv = bv2;
			}
			else
			{
				bv  += dfBVOffset[0];

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

			RTBVTotal = bv + bv2;

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

					RTBVTotal = bv + bv2 + bv3;

					if ( bv3 < bv ) bv = bv3;
				}

				if ( bv2 < bv ) bv = bv2;
			}
		}
		else
		{
			bv = ( ReadBatterySample( 0 ) >> 3 ) + dfBVOffset[0];

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
//			byte_20000055, ChargeStatus, BatteryStatus, PD1 );

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
								if ( byte_20000055 )
								{
									if ( USBVolts > 420 )
									{
										ChargerTarget = 1000;
									}
									else
									{
										ChargerTarget = 500;
										byte_20000055 = 0;
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

	ChargeCurrent = ADC_Read( 13 ) >> 2;

	USBVolts = 147 * ADC_Read( 3 ) / 752 + 5;

//	myprintf( "ChargeCurrent=%d usbv=%d, b55=%d, b56=%d, BS=%d PF0=%d CD=%d\n",
//		ChargeCurrent, USBVolts,
//		byte_20000055, ChargeStatus, BatteryStatus,
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
	else if ( BattVolts[0] >= 250 && BattVolts[1] >= 250 && BattVolts[2] < 20 )
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
	else if ( BattVolts[0] >= 250 && BattVolts[1] < 20 && BattVolts[2] < 20 )
	{
		NumBatteries = 1;
		NewBatteryData();

		MaxVolts = 450;
		MaxPower = 750;
		MaxTCPower = 750;
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
		else if ( !gFlags.battery_charging )
		{
			if ( ( BattVoltsHighest < 420 ) && ( ChargeStatus != 5 ) )
			{
				Event = 12;
				ChargeStatus = 0;
			}
		}
		else if ( ( BattVoltsHighest > 422 ) && ( ChargeStatus == 4 ) )
		{
			Event = 13;
			ChargeStatus = 5;
		}
		else if ( ChargeStatus != 5 && ChargeStatus != 6 )
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

					if ( byte_20000055 )
					{
						if ( USBVolts <= 420 )
						{
							byte_20000055 = 0;
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
				ChargerTarget = 400;
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
							PF0 = 0;
						}
					}
					else
					{
						if ( ChargeCurrent < 1005 )
						{
							if ( ChargerDuty < MaxChargerDuty )
							{
								++ChargerDuty;
							}
						}
					}
				}
			}

			PWM_SET_CMR( PWM0, BBC_PWMCH_CHARGER, ChargerDuty );

			return;

		}
	}

	if ( gFlags.battery_charging )
	{
		gFlags.battery_charging = 0;

		PD7 = 0;
		BBC_Configure( BBC_PWMCH_CHARGER, 0 );
		PD7 = 0;
		ChargerDuty = 0;
	}
}

