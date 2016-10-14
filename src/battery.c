#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "meadc.h"
#include "atomizer.h"
#include "myprintf.h"
#include "timers.h"
#include "battery.h"


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
uint16_t	BattVolts[3];
uint16_t	BatteryCutOff;
uint16_t	BatteryIntRez;
uint16_t	BatteryMaxPwr;
uint16_t	SavedBatVoltage;
uint8_t		BatteryPercent;
uint8_t		BatteryTenth;
uint8_t		NoEventTimer;
uint8_t		BatReadTimer;
uint8_t		NumBatteries;
uint16_t	ChargerDuty;
uint16_t	RTBatVolts;

uint8_t		BattProbeCount;

const Battery_t	*Battery;

uint8_t		byte_20000055;
uint8_t		byte_20000056;
uint8_t		BatteryStatus;

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
		if ( ISVTCDUAL || ISCUBOID )
		{
			sample = ADC_Read( 0 );
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

//	sample = 0;
//	for ( i = 0 ; i < 16 ; ++i )
//		sample += ReadBatterySample( 0 );
//	// V * 100
//	vbat = ( sample >> 7 ) + dfBVOffset[0];
//
//	if ( ISVTCDUAL ) vbat += 2;

	vbat = RTBatVolts;

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
		if ( BatteryStatus != 2 )
		{
			BatteryPercent = 100;
		}
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
	static uint32_t VbatSample1 = 0;
	static uint32_t VbatSample2 = 0;
	static uint8_t	VbatSampleCnt = 0;

	if ( !(gFlags.firing) )
	{
		while ( VbatSampleCnt < 16 )
		{
			VbatSample2 += ReadBatterySample( 1 );
			VbatSample1 += ReadBatterySample( 0 );

			++VbatSampleCnt;

			if ( !(gFlags.sample_vbat) )
				return;
		}

		gFlags.sample_vbat = 0;

		if ( ISVTCDUAL )
		{
			VbatSample1 = ( VbatSample1 >> 7 ) + 2;
			VbatSample2 = 139 * ( VbatSample2 >> 4 ) / 624;

		//	myprintf( "S1=%d S2=%d\n", VbatSample1, VbatSample2 );

			BattVolts[0] = VbatSample1 + dfBVOffset[0];

			if ( VbatSample1 + VbatSample1 / 5 < VbatSample2 )
			{
				BattVolts[1] = VbatSample2 - VbatSample1 + dfBVOffset[1];
				NumBatteries = 2;
			}
			else if (( VbatSample1 - VbatSample1 / 10 < VbatSample2 ) && ( VbatSample2 >= 100 ))
			{
				BattVolts[1] = BattVolts[0];
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
				BattVolts[0] = 20 * (VbatSample1 - 250) / 154 + VbatSample1 + dfBVOffset[0];

			if ( VbatSample2 <= BattVolts[0] )
				BattVolts[1] = 0;
			else
				BattVolts[1] = VbatSample2 - BattVolts[0] + dfBVOffset[1];

		//	myprintf( "S1=%d S2=%d V1=%d V2=%d\n",
		//		VbatSample1, VbatSample2, BattVolts[0], BattVolts[1] );

		//	//***
		//	BattVolts[0] = ( BattVolts[0] + BattVolts[1] ) / 2;
		//	BattVolts[1] = BattVolts[0];
		//	//***
		}
		else
		{
			VbatSample1 >>= 7;
			BattVolts[0] = VbatSample1 + dfBVOffset[0];
		}

		gFlags.batteries_ooe = 0;

		if ( NumBatteries == 2 )
		{
			if ( BattVolts[0] < BattVolts[1] )
			{
				BatteryVoltage = BattVolts[0];
			}
			else
			{
				BatteryVoltage = BattVolts[1];
				BattVolts[1] = BattVolts[0];
				BattVolts[0] = BatteryVoltage;
			}

			if ( BattVolts[1] - BattVolts[0] > 30 )
			{
				gFlags.batteries_ooe = 1;
			}
		}
		else
		{
			BatteryVoltage = BattVolts[0];
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
	}
}


//=========================================================================
//----- (0000074C) --------------------------------------------------------

__myevic__ int CheckBattery()
{
	int v0;
	int pwr;
	int i;
	int bvtot, bv, bv2, bs;

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
		if ( ISVTCDUAL )
		{
			bv = ( ReadBatterySample( 0 ) >> 3 ) + 2;

			if ( NumBatteries == 2 )
			{
				bs = 139 * ReadBatterySample( 1 ) / 624u;
				bv2 = ( bs <= bv ) ? 0 : ( bs - bv );

				bv  += dfBVOffset[0];
				bv2 += dfBVOffset[1];

				if ( bv2 < bv ) bv = bv2;
			}
		}
		else if ( ISCUBOID )
		{
			bvtot = 139 * ReadBatterySample( 1 ) / 624u;
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

			if ( bv2 < bv ) bv = bv2;
		}
		else
		{
			bv = ( ReadBatterySample( 0 ) >> 3 ) + dfBVOffset[0];
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


//=========================================================================
// Battery Charging (VTC Dual)
//-------------------------------------------------------------------------
__myevic__ void BatteryChargeDual()
{
	uint32_t sample13, adc13, sample3, USBVolts;

	static uint32_t dword_200000B4 = 0;
	static uint32_t dword_200000C4 = 0;

	sample13 = ADC_Read( 13 );
	adc13 = sample13 >> 2;

	sample3 = ADC_Read( 3 );
	USBVolts = 147 * sample3 / 752 + 5;

//	myprintf( "nb=%d, adc13=%d USBVolts=%d, b55=%d, b56=%d, BS=%d PD1=%d\n",
//			NumBatteries, adc13, USBVolts,
//			byte_20000055, byte_20000056, BatteryStatus, PD1&1 );

	if ( NumBatteries == 0 )
	{
		BatteryStatus = 2;
		PA3 = 0;
		PA2 = 0;
		PF2 = 0;
		PC3 = 0;
		goto LABEL_29;
	}

	if ( NumBatteries == 1 )
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

LABEL_14:
		if ( BatteryStatus == 2 || BatteryStatus == 3 || BatteryStatus == 4 )
			goto LABEL_29;

		goto LABEL_17;
	}

	if ( NumBatteries == 2 )
	{
		if ( BatteryVoltage < 250u )
		{
			BatteryStatus = 2;
			PA3 = 0;
			PC3 = 0;
			PA2 = 0;
			PF2 = 0;
			goto LABEL_29;
		}

		if ( !gFlags.batt_unk )
		{
			PC3 = 0;
			gFlags.batt_unk = 1;

			MaxPower = 1500;
			MaxTCPower = 1500;
			SetAtoLimits();

			goto LABEL_14;
		}
	}
	if ( gFlags.usb_attached && ( USBVolts > 580 ) )
	{
		BatteryStatus = 3;
		PA3 = 0;
		PC3 = 0;
		PA2 = 0;
		PF2 = 0;
LABEL_47:
		if ( byte_20000056 != 6 )
		{
			gFlags.refresh_display = 1;
			Screen = 57;
			ScreenDuration = 2;
		}
		byte_20000056 = 6;
LABEL_53:
		gFlags.battery_charging = 0;
		goto LABEL_60;
	}

	if ( BatteryStatus == 4 )
		goto LABEL_29;

	if ( gFlags.batteries_ooe )
	{
		BatteryStatus = 1;
	}
	else if ( BatteryStatus )
	{
		BatteryStatus = 0;
	}

LABEL_17:

	PA3 = 1;

	if ( NumBatteries == 2 )
	{
		PF2 = 0;
		PA2 = 1;
	}
	else if ( NumBatteries == 1 )
	{
		PF2 = (( BatteryVoltage < 430 ) || !gFlags.battery_charging );
		PA2 = 0;
	}
	else
	{
		PA2 = 0;
		PF2 = 0;
	}

LABEL_29:

	if ( !gFlags.usb_attached )
		goto LABEL_60;

	if ( BatteryStatus == 2 )
	{
		byte_20000056 = 6;

		if ( gFlags.battery_charging )
			gFlags.battery_charging = 0;

		if ( adc13 > 250 )
		{
			gFlags.refresh_display = 1;
			Screen = 58;
			ScreenDuration = 2;
		}

		goto LABEL_60;
	}

	if ( BatteryStatus == 3 )
		goto LABEL_47;

	if ( BatteryStatus == 4 )
	{
		if ( byte_20000056 != 6 )
		{
			gFlags.refresh_display = 1;
			Screen = 58;
			ScreenDuration = 2;
		}
		byte_20000056 = 6;
		goto LABEL_53;
	}

	if ( gFlags.battery_charging )
	{
		if ( byte_20000056 == 4 && BattVolts[1] > 422 )
		{
			if ( NumBatteries != 2 )
				goto LABEL_61;

			Event = 13;
			byte_20000056 = 5;
			goto LABEL_64;
		}
LABEL_60:
		if ( NumBatteries != 2 )
			goto LABEL_61;
LABEL_64:
		PF2 = 0;
		if ( !gFlags.battery_charging || byte_20000056 == 5 || byte_20000056 == 6 )
		{
			PD7 = 0;
			BBC_Configure( BBC_PWMCH_CHARGER, 0 );
			PD7 = 0;
			ChargerDuty = 0;
			return;
		}

		if ( BatteryVoltage >= 290 )
		{
			if ( BattVolts[1] < 416 )
			{
				if ( byte_20000056 != 4 )
				{
					if ( byte_20000055 )
					{
						if ( USBVolts <= 420 )
						{
							dword_200000B4 = 500;
							byte_20000055 = 0;
						}
						else
						{
							dword_200000B4 = 1000;
						}
					}
					byte_20000056 = 3;
				}
			}
			else
			{
				dword_200000B4 = 400;
				byte_20000056 = 4;
			}
		}
		else
		{
			dword_200000B4 = 300;
			byte_20000056 = 2;
		}

		BBC_Configure( 5, 1 );

		if ( adc13 <= dword_200000B4 + 10 && USBVolts >= 400 )
		{
			if ( adc13 >= dword_200000B4 )
				goto LABEL_90;

			if ( ++dword_200000C4 <= 10 )
				goto LABEL_90;

			dword_200000C4 = 0;

			if ( adc13 >= 180 )
			{
				if ( ChargerDuty > 255 )
					goto LABEL_90;
			}
			else if ( ChargerDuty > 255 )
			{
				BatteryStatus = 4;
				PA3 = 0;
				PC3 = 0;
				PA2 = 0;
				PF2 = 0;
				goto LABEL_90;
			}
			if ( sample13 < 4020 )
			{
				++ChargerDuty;
				goto LABEL_90;
			}
		}
		else if ( ChargerDuty )
		{
			--ChargerDuty;
			goto LABEL_90;
		}
LABEL_90:
		PWM_SET_CMR( PWM0, BBC_PWMCH_CHARGER, ChargerDuty );
		return;
	}

	if ( BattVolts[1] >= 420 || byte_20000056 == 5 )
		goto LABEL_60;

	if ( NumBatteries == 2 )
	{
		Event = 12;
		byte_20000056 = 0;
		goto LABEL_64;
	}

LABEL_61:
	if ( NumBatteries == 1 )
	{
		PD7 = 0;
		BBC_Configure( BBC_PWMCH_CHARGER, 0 );
		PD7 = 0;
		ChargerDuty = 0;
		PA2 = 0;
	}
}


//=========================================================================
// Battery Charging (Cuboid)
//-------------------------------------------------------------------------
__myevic__ void BatteryChargeCuboid()
{
	uint32_t USBVolts, adc13, sample3, sample13;

	static uint32_t dword_200000B4 = 0;
	static uint32_t dword_200000C4 = 0;

	sample13 = ADC_Read(13);
	adc13 = sample13 >> 2;

	sample3 = ADC_Read(3);
	USBVolts = 147 * sample3 / 752 + 5;

//	myprintf( "adc13=%d usbv=%d, b55=%d, b56=%d, BS=%d PF0=%d CD=%d\n",
//		adc13, USBVolts,
//		byte_20000055, byte_20000056, BatteryStatus,
//		PF0, ChargerDuty );

	if ( BatteryVoltage >= 250 )
	{
		if ( gFlags.usb_attached && USBVolts > 580 )
		{
			BatteryStatus = 3;
			PF0 = 0;
			goto LABEL_24;
		}
		if ( BatteryStatus != 4 )
		{
			if ( gFlags.batteries_ooe )
			{
				BatteryStatus = 1;
			}
			else if ( BatteryStatus )
			{
				BatteryStatus = 0;
			}
			PF0 = 1;
		}
	}
	else
	{
		BatteryStatus = 2;
		PF0 = 0;
	}

	if ( !gFlags.usb_attached )
		goto LABEL_28;

	if ( BatteryStatus == 2 )
	{
		byte_20000056 = 6;
		gFlags.battery_charging = 0;

		if ( adc13 > 250 )
		{
			gFlags.refresh_display = 1;
			Screen = 58;
			ScreenDuration = 2;
		}
	}

	if ( BatteryStatus == 3 )
	{
LABEL_24:
		if ( byte_20000056 != 6 )
		{
			gFlags.refresh_display = 1;
			Screen = 57;
LABEL_35:
			ScreenDuration = 2;
		}

LABEL_28:
		if ( gFlags.battery_charging )
			goto LABEL_29;

LABEL_26:
		byte_20000056 = 6;
		gFlags.battery_charging = 0;

LABEL_39:
		PD7 = 0;
		BBC_Configure( BBC_PWMCH_CHARGER, 0 );
		PD7 = 0;
		ChargerDuty = 0;

		return;
	}

	if ( BatteryStatus == 4 )
	{
		if ( byte_20000056 != 6 )
		{
			gFlags.refresh_display = 1;
			Screen = 58;
			goto LABEL_35;
		}
		goto LABEL_26;
	}

	if ( !gFlags.battery_charging )
	{
		if ( BattVolts[1] < 420 && byte_20000056 != 5 )
		{
			Event = 12;
			byte_20000056 = 0;
		}
		goto LABEL_39;
	}
	if ( byte_20000056 != 4 )
	{
LABEL_29:
		if ( byte_20000056 == 5 || byte_20000056 == 6 )
			goto LABEL_39;
		goto LABEL_31;
	}
	if ( BattVolts[1] > 422 )
	{
		Event = 13;
		byte_20000056 = 5;
		goto LABEL_39;
	}

LABEL_31:
	if ( BatteryVoltage >= 290 )
	{
		if ( BattVolts[1] < 416 )
		{
			if ( byte_20000056 != 4 )
			{
				if ( byte_20000055 )
				{
					if ( USBVolts <= 420 )
					{
						dword_200000B4 = 500;
						byte_20000055 = 0;
					}
					else
					{
						dword_200000B4 = 1000;
					}
				}
				byte_20000056 = 3;
			}
		}
		else
		{
			dword_200000B4 = 400;
			byte_20000056 = 4;
		}
	}
	else
	{
		dword_200000B4 = 300;
		byte_20000056 = 2;
	}

	BBC_Configure( 5, 1 );

	if ( adc13 <= dword_200000B4 + 10 && USBVolts >= 400 )
	{
		if ( adc13 < dword_200000B4 )
		{
			if ( ++dword_200000C4 > 10 )
			{
				dword_200000C4 = 0;

				if ( adc13 >= 180 )
				{
					if ( ChargerDuty > 255 )
						goto LABEL_62;
				}
				else if ( ChargerDuty > 255 )
				{
					BatteryStatus = 4;
					PF0 = 0;
					goto LABEL_62;
				}
				if ( sample13 < 4020 )
				{
					++ChargerDuty;
				}
			}
		}
	}
	else
	{
		if ( ChargerDuty )
		{
			--ChargerDuty;
		}
	}

LABEL_62:
	PWM_SET_CMR( PWM0, BBC_PWMCH_CHARGER, ChargerDuty );
}

