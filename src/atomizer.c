#include "myevic.h"
#include "dataflash.h"
#include "battery.h"
#include "timers.h"
#include "meadc.h"
#include "megpio.h"
#include "M451Series.h"


//=============================================================================
//----- (00008F00) --------------------------------------------------------
__myevic__ void ClampPowers()
{
	if ( AtoMinPower < 10 ) AtoMinPower = 10;
	if ( AtoMaxPower > MaxPower ) AtoMaxPower = MaxPower;
}


//=============================================================================
//----- (0000121C) --------------------------------------------------------
__myevic__ uint16_t ClampPower( uint16_t pwr, int clampmax )
{
	if ( AtoError || !AtoRez )
	{
		pwr = MaxPower;
	}
	else
	{
		ClampPowers();

		if ( pwr < AtoMinPower )
			pwr = AtoMinPower;

		if ( clampmax && pwr > AtoMaxPower )
			pwr = AtoMaxPower;
	}
	return pwr;
}


//=============================================================================
//----- (00008F28) --------------------------------------------------------
__myevic__ void ClampAtoVolts()
{
	if ( AtoMinVolts < 50 ) AtoMinVolts = 50;
	if ( AtoMaxVolts > MaxVWVolts ) AtoMaxVolts = MaxVWVolts;
}


//=============================================================================
//----- (00001274) --------------------------------------------------------
__myevic__ uint16_t GetAtoVWVolts( uint16_t pwr )
{
	uint16_t volts; // r0@9

	if ( AtoError || !AtoRez )
	{
		volts = 330;
	}
	else
	{
		volts = sqrtul( 10 * pwr * AtoRez );

		ClampAtoVolts();

		if ( volts < AtoMinVolts ) volts = AtoMinVolts;
		if ( volts > AtoMaxVolts ) volts = AtoMaxVolts;
	}

	return volts;
}


//=============================================================================
//----- (00001334) --------------------------------------------------------
__myevic__ uint16_t CelsiusToF( uint16_t tc )
{
  return ( tc * 9 ) / 5 + 32;
}


//=============================================================================
//----- (000022A8) --------------------------------------------------------
__myevic__ uint16_t FarenheitToC( uint16_t tf )
{
  return ( 5 * ( tf - 32 )) / 9;
}


//=============================================================================
//----- (0000344C) --------------------------------------------------------
__myevic__ void StopFire()
{
	GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_INPUT );

	if ( Flags64 & 0x100 ) Flags64 &= ~0x100;

	if ( FireDuration > 5 )
	{
		dfTimeCount += FireDuration;
		if ( dfTimeCount > 999999 ) dfTimeCount = 0;
		if ( ++dfPuffCount > 99999 ) dfPuffCount = 0;
		UpdatePTTimer = 80;
	}

	FireDuration = 0;

	PC1 = 0;
	PC3 = 0;

	BuckDuty = 0;
	PC0 = 0;
	BBC_Configure( 0, 0 );

	BoostDuty = 0;
	PC2 = 0;
	BBC_Configure( 2, 0 );

	SetADCState( 1, 0 );
	SetADCState( 2, 0 );

	LowBatVolts = 0;
	BatReadTimer = 200;
}


//=============================================================================
//----- (00003508) --------------------------------------------------------
uint16_t LowestRezMeasure()
{
	uint16_t rez;

	rez = dfResistance;
	if ( dfResistance > AtoRez && AtoRez )
		rez = AtoRez;
	if ( AtoRezMilli / 10 < rez && AtoRezMilli >= 10 )
		rez = ( AtoRezMilli / 10 );

	return rez;
}


//=============================================================================
//----- (00003540) --------------------------------------------------------
__myevic__ uint16_t AtoPowerLimit( uint16_t pwr )
{
	uint16_t p;

	if ( AtoRezMilli < 120 )
	{
		p = 625 * AtoRezMilli / 100;
		if ( pwr > p ) pwr = p;
	}
	return pwr;
}


//=============================================================================
//----- (00003564) --------------------------------------------------------
__myevic__ void GetAtoCurrent()
{
	unsigned int avolts;
	unsigned int ashunt;
	unsigned int arez;
	int s;

	if ( Flags64 & 0x100 || Flags64 & 0x2000 )
	{
		avolts = ADC_Read( 2 );
		CLK_SysTickDelay( 10 );
		ashunt = ADC_Read( 1 );

		AtoCurrent = ( (25600 * avolts) >> 12 ) / AtoShuntValue;

		arez = LowestRezMeasure();

		if	(	Flags64 & 0x100
			&&	2080 * AtoShuntValue / 100 * ashunt < 30 * avolts * arez
			&& AtoCurrent > 50
			&& TargetVolts >= 100	)
		{
			s = 2;
		}
		else if ( AtoCurrent > 256 && !ISMODEBY(dfMode) )
		{
			s = 3;
		}
		else
		{
			return;
		}

		AtoStatus = 1;

		if ( Flags64 & 0x100 ) Event = 25;

		StopFire();

		PutTextf(	"\n"
					" Short %d! u32ADValue_Res_temp(%d) u32ADValue_CurVol_temp(%d)"
					" g_u16DetRes_I(%d.%d) u16Res(%d) g_u32Set_OutVol(%d).\n",
					s,
					avolts,
					ashunt,
					AtoCurrent / 10,
					AtoCurrent % 10,
					arez,
					TargetVolts	);
	}
}


//=============================================================================
//----- (00002F44) --------------------------------------------------------
__myevic__ void GetAtoTemp()
{
	long v;

	if (	AtoRezMilli
		&&	AtoRezMilli <= 3000
		&&	dfResistance <= 150
		&&	AtoRezMilli <= 50 * dfResistance	)
	{
		if ( AtoRezMilli / 10 <= dfResistance )
		{
			AtoTemp = 70;
		}
		else if ( dfTempAlgo == 1 )
		{
			AtoTemp = 100 * ( AtoRezMilli - 10 * dfResistance ) / TCR + 140;
		}
		else if ( dfTempAlgo == 2 )
		{
			AtoTemp = AtoRezMilli * TCR / dfResistance - 460;
		}
		else if ( dfTempAlgo == 3 || dfTempAlgo == 4 )
		{
			v = dfResistance * TCR;
			v = 10000 * ( AtoRezMilli -10 * dfResistance ) / v + 20;
			AtoTemp = CelsiusToF( v );
		}
	}
}


//=============================================================================
//----- (00002FF0) --------------------------------------------------------
__myevic__ void GetTempCoef( const uint16_t tc[] )
{
	int r; // r2@4
	int v; // r4@6

	if ( dfResistance > 100 )
	{
		TCR = tc[20];
		return;
	}

	r = dfResistance / 5;
	TCR = tc[r];

	if ( dfResistance % 5u || r < 20 )
	{
		v = tc[r + 1];
		if ( v <= TCR )
		{
			TCR = ( TCR - ( TCR - v ) / 5 * ( dfResistance % 5 ) );
			if ( v > TCR ) TCR = v;
		}
		else
		{
			TCR = ( TCR + ( v - TCR ) / 5 * ( dfResistance % 5 ) );
			if ( v < TCR ) TCR = v;
		}
	}
}


//=============================================================================
//----- (00003078) --------------------------------------------------------
__myevic__ void CheckMode()
{
	unsigned int v0; // r0@2
	unsigned int v1; // r1@4

	if ( AtoRezMilli / 10 <= AtoRez )
		v0 = 0;
	else
		v0 = ( AtoRezMilli / 10 - AtoRez );

	v1 = 10 * AtoRezMilli / AtoRez;

	if (	AtoRez > 150
		||	( v1 <= 115 && ( ( dfMode != 2 && dfMode != 3 ) || v1 <= 105 || v0 <= 1 ) ) )
	{
		if ( FireDuration < 20 )
		{
			CheckModeCounter = 0;
			return;
		}

		dfRezType = 1;
		dfMode = 4;

		if ( dfPower > 200 )
		{
			dfPower = 200;
			dfVWVolts = GetAtoVWVolts( 200 );
		}

		if ( !(Flags64 &  0x8000000) ) dfRezTI  = word_200000B8;
		if ( !(Flags64 & 0x10000000) ) dfRezNI  = word_200000BA;
		if ( !(Flags64 & 0x80000000) ) dfRezSS  = word_200000BC;
		if ( !(Flags68 &          2) ) dfRezTCR = word_200000BE;

		if ( AtoRez < 10 )
		{
			StopFire();
			Event = 27;
		}
	}
	else
	{
		if ( ++CheckModeCounter <= 3 )
			return;

		dfRezType = 2;
	}

	UpdateDFTimer = 50;

	Flags64 &= ~0x800000u;
	CheckModeCounter = 0;
}


//=============================================================================
//----- (00003250) --------------------------------------------------------
__myevic__ void ReadAtoTemp()
{
	if ( TargetVolts )
	{
		if ( Flags64 & 0x100 || AtoProbeCount != 10 )
		{
			byte_20000098 = 1;
		}
		else
		{
			byte_20000098 = 50;
		}

		ADCAtoSum = 0;
		ADCShuntSum = 0;
		for ( int count = 0 ; count < byte_20000098 ; ++count )
		{
			CLK_SysTickDelay( 10 );
			ADCShuntSum += ADC_Read( 2 );
			CLK_SysTickDelay( 10 );
			ADCAtoSum += ADC_Read( 1 );
		}

		if ( !ADCShuntSum ) ADCShuntSum = 1;
		AtoRezMilli = 1300 * AtoShuntValue / 100 * ADCAtoSum / ( 3 * ADCShuntSum );

		GetAtoCurrent();

		if ( AtoRezMilli >= 5 )
		{
			if ( AtoRezMilli <= 20 )
			{
				AtoStatus = 1;
				PutTextf( "RL_GND %d(%d,%d,%d)\n",
						  AtoRezMilli, ADCAtoSum, ADCShuntSum, dword_200000A4 );
				if ( Flags64 & 0x100 )
				{
					StopFire();
					Event = 25;
				}
				return;
			}
			if ( ( AtoProbeCount <= 10u && AtoRezMilli < 50 ) || AtoRezMilli < 40 )
			{
				PutTextf( "RL_LOW %d\n", AtoRezMilli );
				AtoStatus = 2;
				if ( Flags64 & 0x100)
				{
					Event = 27;
				}
				return;
			}
			if ( AtoRezMilli > 50000 )
			{
				AtoStatus = 0;
				AtoRezMilli = 0;
				if ( Flags64 & 0x100)
				{
					Event = 26;
				}
				return;
			}
			if ( AtoProbeCount <= 10u && AtoRezMilli > 3500 )
			{
				AtoStatus = 3;
				PutTextf( "RL_LARGE %d\n", AtoRezMilli );
				return;
			}
			if ( Flags64 & 0x100 && AtoRezMilli / 10u <= AtoRez >> 2 )
			{
				StopFire();
				Event = 25;
				PutTextf( "RL_GND2 %d\n", AtoRezMilli );
				return;
			}
			if ( (AtoStatus == 4 || !AtoStatus || AtoStatus == 3)
					&& AtoRezMilli >= 50
					&& AtoRezMilli <= 3500
					&& (Flags64 & 0x100 || AtoProbeCount <= 10u) )
			{
				AtoStatus = 4;
				if ( Flags64 & 0x100 ) GetAtoTemp();
			}
		}
	}
}


//=============================================================================
//----- (00002CD8) --------------------------------------------------------
__myevic__ void RegulateBuckBoost()
{
	if ( ( (Flags64 & 0x100) && CheckBattery() )
		|| ( !(Flags64 & 0x2000) && !(Flags64 & 0x100) ) )
	{
		return;
	}

	AtoVoltsADC = ADC_Read( 1 );
	AtoVolts = ( 1109 * AtoVoltsADC ) >> 12;


	switch ( BBCNextMode )
	{
		case 1:	// Transition mode Buck <-> Boost, or bypass mode
			{
				if ( BBCMode != 1 )
				{
					PC2 = 1;
					BBC_Configure( BBC_PWMCH_BOOST, 0 );
					PC3 = 1;

					PC0 = 1;
					BBC_Configure( BBC_PWMCH_BUCK, 0 );
					PC1 = 1;
				}

				if ( AtoVolts == TargetVolts )
				{
					BBCNumCmps = 0;
				}
				else if ( ++BBCNumCmps >= 5 )
				{
					if ( !ISMODEBY(dfMode) )
					{
						if ( BBCMode == 2 ) BBCNextMode = 3;
						else BBCNextMode = 2;
					}
					BBCMode = 1;
				}
			}
			break;

		case 2:	// Buck mode
			{
				if ( BBCMode != 2 )
				{
					PC2 = 1;
					BBC_Configure( BBC_PWMCH_BOOST, 0 );

					PC3 = 1;
					BBC_Configure( BBC_PWMCH_BUCK, 1 );
					BuckDuty = ( BBCMode == 0 ) ? 10 : 479;
					PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, BuckDuty );
					PC1 = 1;

					BBCMode = BBCNextMode;
				}

				if ( AtoVolts < TargetVolts )
				{
					if ( BuckDuty < 479 )
					{
						++BuckDuty;
					}
					else
					{
						if ( 12 * TargetVolts > 10 * BatteryVoltage )
						{
							BBCNextMode = 1;
						}
					}
				}
				else if ( AtoVolts > TargetVolts )
				{
					if ( BuckDuty > 10 ) --BuckDuty;
					else BuckDuty = 0;
				}

				if (	( AtoStatus == 0 || AtoStatus == 1 )
					||	( !(Flags64 & 0x100) && AtoProbeCount >= 12 ) )
				{
					if ( BuckDuty >= 45 ) BuckDuty = 45;
				}

				PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, BuckDuty );
			}
			break;

		case 3:	// Boost mode
			{
				if ( BBCMode != 3 )
				{
					BBCMode = 3;

					PC0 = 1;
					BBC_Configure( BBC_PWMCH_BUCK, 0 );

					PC1 = 1;
					BBC_Configure( BBC_PWMCH_BOOST, 1 );
					BoostDuty = 479;
				//	PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, BuckDuty );
					PWM_SET_CMR( PWM0, BBC_PWMCH_BOOST, BoostDuty );
					PC3 = 1;
				}

				if ( AtoVolts < TargetVolts && BoostDuty > 80 )
				{
					--BoostDuty;
				}
				else if ( AtoVolts > TargetVolts )
				{
					if ( BoostDuty < 479 )
					{
						++BoostDuty;
					}
					else
					{
						BBCNextMode = 1;
					}
				}

				PWM_SET_CMR( PWM0, BBC_PWMCH_BOOST, BoostDuty );
			}
			break;

		default:
			break;
	}
}


//=============================================================================
//----- (00002EBC) --------------------------------------------------------
__myevic__ void ReachTargetVoltage()
{
	BBCNextMode = 2;
	BBCMode = 0;
	TMR0Counter2 = 0;

	do
	{
		if ( !(Flags64 & 0x2000) && !(Flags64 & 0x100) )
			break;

		RegulateBuckBoost();
		GetAtoCurrent();

		if ( AtoVolts == TargetVolts )
			break;

		if ((AtoStatus == 0 || AtoStatus == 1 || (!(Flags64 & 0x100) && AtoProbeCount >= 12))
			&& BuckDuty >= 45u )
		{
			break;
		}

		if ( ISMODEBY(dfMode) && BBCMode == 1 ) break;

	}
	while ( TMR0Counter2 < 2000 );
}


//=============================================================================
//----- (000031DC) --------------------------------------------------------
__myevic__ uint16_t AtoPower( uint16_t volts )
{
	unsigned long result; // r0@1

	result = volts * volts / AtoRezMilli;
	if ( !result ) result = 1;
	return (uint16_t)result;
}


//=============================================================================
//----- (000031F4) --------------------------------------------------------
__myevic__ uint16_t GetVoltsForPower( uint16_t pwr )
{
	uint16_t v;

	v = sqrtul( (uint32_t)pwr * AtoRezMilli );

	if ( v > MaxVWVolts ) v = MaxVWVolts;

	return v;
}


//=============================================================================
//----- (00003198) --------------------------------------------------------
__myevic__ void TweakTargetVoltsVW()
{
	unsigned int pwr;

	if ( dfMode == 6 )
	{
		pwr = dfSavedCfgPwr[ConfigIndex];
	}
	else
	{
		pwr = dfPower;
	}

	pwr = AtoPowerLimit( pwr );
	TargetVolts = GetVoltsForPower( pwr * PowerScale / 100 );
}


//=============================================================================
//----- (00003658) ------------------------------------------------------------

__myevic__ void TweakTargetVoltsTC()
{
	unsigned int pwr;
	unsigned int volts;
	unsigned int temp;

	if ( TargetVolts )
	{
		pwr = dfTCPower;

		if ( pwr < 10u ) pwr = 10;

		if ( Flags64 & 0x800000 )
		{
			if ( pwr > 400 ) pwr = 400;
			if ( pwr < 300 ) pwr = 300;
		}

		pwr = AtoPowerLimit( pwr );

		if ( FireDuration <= 2 && pwr > 300 ) pwr = 300;

		volts = GetVoltsForPower( pwr );

		if ( Flags64 & 0x800000 )
		{
			TargetVolts = volts;
			return;
		}

		temp = ( dfIsCelsius == 1 ) ? CelsiusToF( dfTemp ) : dfTemp;

		if ( Flags64 & 0x400000 )
		{
			if ( TargetVolts ) --TargetVolts;
		}
		else if ( AtoTemp < temp )
		{
			++TargetVolts;
			Flags64 &= ~0x4000000;
		}
		else if ( AtoTemp > temp )
		{
			if ( Flags64 & 0x4000000 )
			{
				Flags64 &= ~0x4000000;
				TargetVolts = 0;
				PC3 = 0;
				PC1 = 0;
				BuckDuty = 0;
				PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, 0 );
				BoostDuty = 0;
				PWM_SET_CMR( PWM0, BBC_PWMCH_BOOST, 0 );
			}
			else
			{
				if ( TargetVolts ) --TargetVolts;
			}
		}

		if ( TargetVolts > volts ) TargetVolts = volts;
	}
}


//=============================================================================
//----- (00008F90) --------------------------------------------------------
__myevic__ void SetMinMaxPower()
{
	unsigned int u;

	if ( AtoError || !AtoRez )
	{
		AtoMinPower = 10;
		AtoMaxPower = MaxPower;
	}
	else
	{
		u = 25 * AtoRez;
		if ( u > MaxVWVolts ) u = MaxVWVolts;
		AtoMaxPower = (( u * u ) / AtoRez + 5 ) / 10u;
		AtoMinPower = 2500u / AtoRez / 10;
		ClampPowers();
	}
}


//=============================================================================
//----- (00009000) --------------------------------------------------------
__myevic__ void SetMinMaxVolts()
{
	if ( AtoError || !AtoRez )
	{
		AtoMinVolts = 50;
		AtoMaxVolts = MaxVWVolts;
	}
	else
	{
		AtoMinVolts = sqrtul( 10ul * 10 * AtoRez );
		AtoMaxVolts = sqrtul( 10ul * MaxPower * AtoRez );
		ClampAtoVolts();
	}
}


//=============================================================================
// START mode resistances values
const uint16_t STARTRezValues[] =
		{	50, 60, 100, 150	};

// START mode power ranges
const uint16_t STARTPowers[] =
		{	250, 300,
			200, 280,
			180, 250,
			160, 200,
			200, 750	};


//=============================================================================
//----- (00008F5C) --------------------------------------------------------
__myevic__ int SearchSTARTRez( uint16_t rez )
{
	int i;
	uint16_t r;

	i = 0;
	while ( 1 )
	{
		r = STARTRezValues[i];
		if ( ( r + r / 10 >= rez ) && ( r - r / 10 <= rez ) ) break;
		if ( ++i == 4 ) break;
	}
	return i;
}


//=============================================================================
//----- (000038FC) --------------------------------------------------------
__myevic__ void SetAtoSTARTParams()
{
	int i, j, k;

	uint16_t r;
	uint16_t p;

	for ( i = 0 ; i < 10 ; ++i )
	{
		r = dfSavedCfgRez[i];

		if ( 	(( r - r / 20 ) <= AtoRez || r - 1 <= AtoRez )
			&& 	(( r + r / 20 ) >= AtoRez || r + 1 >= AtoRez ) )
		{
			break;
		}
	}

	if ( i < 10 )
	{
		ConfigIndex = i;

		j = SearchSTARTRez( r );
		p = dfSavedCfgPwr[i];
		if ( p > MaxPower || p < 10 )
		{
			dfSavedCfgPwr[i] = STARTPowers[ j << 1 ];
		}
	}
	else
	{
		for ( j = 0 ; j < 10 ; ++j )
		{
			if ( !dfSavedCfgRez[j] ) break;
		}

		if ( j == 10 )
		{
			ConfigIndex = 9;

			for ( k = 0 ; k < 9 ; ++k )
			{
				dfSavedCfgRez[k] = dfSavedCfgRez[k+1];
				dfSavedCfgPwr[k] = dfSavedCfgPwr[k+1];
			}
		}
		else
		{
			ConfigIndex = j;
		}

		dfSavedCfgRez[ConfigIndex] = AtoRez;
		dfSavedCfgPwr[ConfigIndex] = STARTPowers[ SearchSTARTRez( AtoRez ) << 1 ];
	}

	UpdateDFTimer = 50;
}


//=============================================================================
//----- (000090DC) --------------------------------------------------------
__myevic__ void SetAtoLimits()
{
	unsigned int pwr;

	SetMinMaxVolts();
	SetMinMaxPower();
	if ( !AtoError && AtoRez )
	{
		SetAtoSTARTParams();

		if ( dfMode == 6 )
			pwr = dfSavedCfgPwr[ConfigIndex];
		else
			pwr = dfPower;

		if ( pwr < AtoMinPower ) pwr = AtoMinPower;
		if ( pwr > AtoMaxPower ) pwr = AtoMaxPower;
		dfVWVolts = GetAtoVWVolts( pwr );

		if ( dfMode == 6 )
			dfPower = ClampPower( dfVWVolts, 1 );
		else
			dfPower = pwr;
	}
}


//=============================================================================
//----- (00006038) --------------------------------------------------------
__myevic__ void ProbeAtomizer()
{
	SetADCState( 1, 1 );
	SetADCState( 2, 1 );
	WaitOnTMR2( 2 );

	if (( AtoProbeCount == 8 ) || ( Flags64 & 0x100 ))
	{
		if ( Flags64 & 0x100 )
		{
			Flags64 |= 0x4000000u;
		}
		TargetVolts = GetVoltsForPower( 50 );
		if ( !TargetVolts ) TargetVolts = 100;
	}
	else if ( AtoProbeCount == 9 )
	{
		TargetVolts = GetVoltsForPower( 100 );
	}
	else if ( AtoProbeCount == 10 )
	{
		TargetVolts = GetVoltsForPower( 150 );
	}
	else
	{
		TargetVolts = 100;
	}

	if ( TargetVolts > 600 ) TargetVolts = 600;

	Flags64 |= 0x2000u;
	ReachTargetVoltage();
	WaitOnTMR2(2);
	ReadAtoTemp();
	Flags64 &= ~0x2000u;

	if ( !(Flags64 & 0x100) )
	{
		PC1 = 0;
		PC3 = 0;
		SetADCState( 1, 0 );
		SetADCState( 2, 0 );
		BoostDuty = 0;
		PWM_SET_CMR( PWM0, BBC_PWMCH_BOOST, 0 );
		BuckDuty = 0;
		PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, 0 );
	}

	switch ( AtoStatus )
	{
		case 0:
		case 3:
			AtoError = 1;
			break;
		case 1:
			AtoError = 2;
			break;
		case 2:
			AtoError = 3;
			break;
		case 4:
		default:
			AtoError = 0;
			break;
	}

	if ( AtoProbeCount < 12u )
	{
		++AtoProbeCount;
	}

	if ( AtoStatus == 4 )
	{
		if ( AtoProbeCount != 11 )
			return;
		AtoRez = AtoRezMilli / 10u;
	}
	else
	{
		AtoRez = 0;
		if ( !AtoStatus ) AtoProbeCount = 0;
	}

	if ( AtoError == byte_20000082
			&& ( AtoRez + AtoRez / 20u ) >= word_200000B6
			&& ( AtoRez - AtoRez / 20u ) <= word_200000B6 )
	{
		AtoRez = word_200000B6;
	}

	if ( AtoRez != word_200000B6
			|| AtoError != byte_20000082 )
	{
		if ( !word_200000B6 )
		{
			byte_200000B3 = 1;
		}
		word_200000B6 = AtoRez;
		byte_20000082 = AtoError;
		SetAtoLimits();
		Flags64 |= 0x20000u;
		ScreenDuration = 30;
	}

	if ( byte_200000B3 )
	{
		if ( byte_200000B3 == 2 )
		{
			byte_200000B3 = 1;
		}
		if ( !dfResistance )
		{
			if ( AtoRez )
			{
				dfResistance = AtoRez;
				UpdateDFTimer = 50;
			}
		}
		Flags64 |= 0x98800000;
		Flags68 |= 2u;
	}
}


//=============================================================================
//----- (000088B4) --------------------------------------------------------
__myevic__ void _SwitchRezLock( uint8_t *plock, uint16_t *prez )
{
	if ( *plock )
	{
		*plock = 0;
	}
	else if ( AtoRez )
	{
		*plock = 1;
		if ( !dfResistance ) dfResistance = AtoRez;
		*prez = dfResistance;
	}
}

__myevic__ void SwitchRezLock()
{
	switch ( dfMode )
	{
		case 0:
			_SwitchRezLock( &dfRezLockedNI, &dfRezNI );
			break;

		case 1:
			_SwitchRezLock( &dfRezLockedTI, &dfRezTI );
			break;
			
		case 2:
			_SwitchRezLock( &dfRezLockedSS, &dfRezSS );
			break;
			
		case 3:
			_SwitchRezLock( &dfRezLockedTCR, &dfRezTCR );
			break;
		
		default:
			break;
	}
}


//=============================================================================

const uint32_t BoardTempTable[] =
	{	34800, 26670, 20620, 16070, 12630, 10000, 
		7976, 6407, 5182, 4218, 3455, 2847, 2360, 
	1967, 1648, 1388, 1175, 999, 853, 732, 630	};
 

//=============================================================================
//----- (00007BE0) --------------------------------------------------------
__myevic__ void ReadBoardTemp()
{
	unsigned int sample;
	unsigned int tdr;
	int v3;
	int v4;
	int v6;
	int v7;

	while ( BTempSampleCnt < 16 )
	{
		BTempSampleSum += ADC_Read( 14 );
		++BTempSampleCnt;
		if ( !(Flags64 & 0x10000) )
		return;
	}
	Flags64 &= ~0x10000u;

	sample = BTempSampleSum >> 4;

	BoardTemp = 0;

	if ( sample != 4096 && sample )
	{
		tdr = 10 * ( 2000 * sample / ( 5280 - sample ));

		if ( tdr <= 630 )
		{
			BoardTemp = 99;
		}
		else if ( tdr < 34800 )
		{
			for ( v3 = 1 ; v3 < 20 ; ++v3 )
			{
				if ( BoardTempTable[v3] <= tdr )
					break;
			}

			v4 = BoardTempTable[v3];
			v6 = (( 10 * BoardTempTable[v3 - 1] - 10 * v4 ) / 5 );
			v7 = 0;
			do
			{
				if ( v6 * v7 + 10 * v4 <= 10 * tdr && (v7 + 1) * v6 + 10 * v4 > 10 * tdr )
					break;
				++v7;
			}
			while ( v7 < 5 );

			BoardTemp = 5 * v3 - v7;
		}
	}

	BTempSampleCnt = 0;
	BTempSampleSum = 0;
}


//=============================================================================
//----- (00006004) --------------------------------------------------------
__myevic__ void Overtemp()
{
	StopFire();
	Flags64 |= 0x20000;
	Screen = 29;
	ScreenDuration = 2;
	KeyPressTime |= 0x8000;
}

