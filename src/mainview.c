#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "display.h"
#include "battery.h"
#include "myrtc.h"
#include "atomizer.h"
#include "miscs.h"
#include "timers.h"

//=============================================================================

uint16_t	BypassVolts;
uint8_t		HideLogo;
uint8_t		ShowProfNum;
uint8_t		SplashTimer;


//=============================================================================
//----- (00001654) --------------------------------------------------------
__myevic__ void MainView()
{
	if ( !dfStatus.off )
	{
		if ( Screen != 1 || !PD3 || !PD2 || !PE0 )
		{
			HideLogo = 3;
		}
		Screen = 1;
		ScreenDuration = GetMainScreenDuration();
	}
	else
	{
		if ( gFlags.battery_charging && !dfStealthOn )
		{
			ChargeView();
		}
		else
		{
			Screen = 0;
			SleepTimer = 0;
		}
	}
	gFlags.refresh_display = 1;
}


//=============================================================================
__myevic__ void DrawMode()
{
	if ( !BLINKITEM(0) )
	{
		switch ( dfMode )
		{
			case 0:
				DrawString( String_TEMP, 0, 2 );
				break;
			case 1:
				DrawString( String_TEMP, 0, 2 );
				break;
			case 2:
				DrawString( String_TEMP, 0, 2 );
				break;
			case 3:
				DrawString( String_TCR, 0, 2 );
				break;
			case 4:
				DrawString( String_POWER, 0, 2 );
				break;
			case 5:
				DrawString( String_BYPASS, 0, 2 );
				break;
			case 6:
				DrawString( String_SMART, 13, 4 );
				break;
			default:
				break;
		}
	}

	if ( !BLINKITEM(1) )
	{
		switch ( dfMode )
		{
			case 0:
				DrawString( String_NI, 32, 2 );
				break;
			case 1:
				DrawString( String_TI, 32, 2 );
				break;
			case 2:
				DrawString( String_SS, 32, 2 );
				DrawImage( 48, 3, 0x04 );
				DrawImage( 54, 3, 0x02 );
				DrawImage( 59, 3, 0x07 );
				break;
			case 3:
				DrawValue( 25, 2, dfTCRM[dfTCRIndex], 0, 0x0B, 3 );
				DrawImage( 49, 2, 0xA8 );
				DrawValue( 57, 2, dfTCRIndex + 1, 0, 0x0B, 1 );
				break;
			default:
				break;
		}
	}
}

//=============================================================================

__myevic__ void DrawPwrLine( int pwr, int line )
{
	if ( BLINKITEM(2) && PD2 && PD3 )
		return;

    DrawString( String_PWR_s, 0, line+2 );

	if ( pwr < 1000 )
	{
		DrawValueRight( 55, line, pwr, 1, 0x1F, 0 );
		DrawImage( 56, line+2, 0x98 );
	}
	else
	{
		if ( EditModeTimer && ( EditItemIndex == 2 ) && ( !PD2 || !PD3 ) && !dfStatus.onewatt )
		{
			DrawValueRight( 64, line, pwr, 1, 0x1F, 0 );
		}
		else
		{
			DrawValueRight( 55, line, pwr / 10, 0, 0x1F, 3 );
			DrawImage( 56, line+2, 0x98 );
		}
	}
}


//=============================================================================

__myevic__ void DrawTempLine( int line )
{
	if ( BLINKITEM(2) && PD2 && PD3 )
		return;

    DrawString( String_TEMP_s, 0, line+2 );

	DrawImage( 56, line+2, dfIsCelsius ? 0xC9 : 0xC8 );

	if ( Screen == 2 )
	{
		if ( dfIsCelsius )
		{
			DrawValue( 31, line, FarenheitToC( AtoTemp ), 0, 0x1F, 3 );
		}
		else
		{
			DrawValue( 31, line, AtoTemp, 0, 0x1F, 3 );
		}
	}
	else
	{
		DrawValue( 31, line, dfTemp, 0, 0x1F, 3 );
	}
}


//=============================================================================

__myevic__ void DrawVoltsLine( int volts, int line )
{
	DrawString( String_VOLT_s, 0, line+2 );
	if ( volts > MaxVolts ) volts = MaxVolts;
	DrawValue( 27, line, volts, 2, 0x1F, 3 );
	DrawImage( 57, line+2, 0x97 );
}


//=============================================================================

__myevic__ void DrawCoilLine( int line )
{
	unsigned int rez;

	if ( BLINKITEM(3) )
		return;

	DrawString( String_COIL_s, 0, line+2 );

	if ( gFlags.firing )
	{
		rez = AtoRezMilli;
	}
	else if ( ISMODETC(dfMode) )
	{
		if ( byte_200000B3 || !AtoRez )
		{
			rez = AtoRez * 10 + AtoMillis;
		}
		else
		{
			rez = dfResistance * 10 + RezMillis;
		}
	}
	else
	{
		rez = AtoRez * 10 + AtoMillis;
	}

	if ( rez < 1000 )
		DrawValueRight( 55, line, rez, 3, 0x1F, 3 );
	else
		DrawValueRight( 55, line, rez / 10, 2, 0x1F, 3 );

	if ((( dfMode == 0 ) && ( dfRezLockedNI ))
	||	(( dfMode == 1 ) && ( dfRezLockedTI ))
	||	(( dfMode == 2 ) && ( dfRezLockedSS ))
	||	(( dfMode == 3 ) && ( dfRezLockedTCR )))
	{
		DrawImage( 56, line+2, 0xC3 );
	}
	else
	{
		DrawImage( 56, line+2, 0xC0 );
	}

	if ( rez )
	{
		if (   ( ISMODETC(dfMode) && ( rez > 1500 ) )
			|| ( ISMODEVW(dfMode) && ( rez <  100 ) ) )
		{
			if ( gFlags.osc_1hz )
			{
				DrawFillRect( 26, line-1, 63, line+10, 2 );
			}
			ScreenRefreshTimer = 5;
		}
	}
}

//=============================================================================

__myevic__ void DrawAPTLine( int line )
{
	if ( BLINKITEM(4) )
		return;

	switch ( dfAPT )
	{
		default:
		case 0:	// Current
		{
			DrawString( String_AMP_s, 0, line+2 );
			DrawValue( 27, line, ( gFlags.firing ) ? AtoCurrent : 0, 1, 0x1F, 3 );
			DrawImage( 56, line+2, 0x9C );
			break;
		}

		case 1:	// Puff counter
		{
			DrawString( String_PUFF_s, 0, line+2 );
			DrawValueRight( 24+8*5, line, dfPuffCount, 0, 0x1F, 0 );
			break;
		}

		case 2:	// Time counter
		{
		//	DrawString( String_TIME_s, 0, line+2 );
		//	DrawValue( 24, line, dfTimeCount / 10, 0, 0x1F, 5 );
			DrawString( String_PUFF_s, 0, line+2 );
			DrawValueRight( 34, line+2, dfTimeCount / 36000, 0, 0x0B, 0 );
			DrawImage( 34, line+2, 0xD7 );
			DrawValue( 37, line+2, dfTimeCount / 600 % 60, 0, 0x0B, 2 );
			DrawImage( 49, line+2, 0xD7 );
			DrawValue( 52, line+2, dfTimeCount / 10 % 60, 0, 0x0B, 2 );
			break;
		}

		case 3:	// Vape Velocity
		{
			uint32_t vv, t;
			// Elasped seconds since last VV reset
			t = RTCGetEpoch( 0 );
			t -= RTCReadRegister( RTCSPARE_VV_BASE );

			vv = dfVVRatio * ( MilliJoules / 1000 ) / 1000;
			vv /= 10;
			if ( vv > 9999 ) vv = 9999;
			if ( dfStatus.vapedml )
			{
				DrawString( String_LIQ_s, 0, line+2 );
				DrawString( String_ml, 52, line+2 );
				DrawValueRight( 50, line, vv, 2, 0x1F, 0 );
			}
			else
			{
				vv = vv * 86400 / ( t ? : 1 );
				DrawString( String_mld, 42, line+2 );
				DrawValueRight( 40, line, vv, 2, 0x1F, 0 );
			}
			break;
		}

		case 4:	// Atomizer voltage
		{
			DrawString( String_VOUT_s, 0, line+2 );
			DrawValue( 27, line, gFlags.firing?AtoVolts:0, 2, 0x1F, 3 );
			DrawImage( 57, line+2, 0x97 );
			break;
		}

		case 5:	// Battery voltage
		{
			DrawString( String_BATT_s, 0, line+2 );
			DrawValue( 27, line, gFlags.firing?RTBattVolts:BatteryVoltage, 2, 0x1F, 3 );
			DrawImage( 57, line+2, 0x97 );
			break;
		}

		case 6:	// Board temperature
		{
			DrawString( String_BOARD_s, 0, line+2 );

			int t = dfIsCelsius ? BoardTemp : CelsiusToF( BoardTemp );

			DrawValue( t>99?31:39, line, t, 0, 0x1F, t>99?3:2 );
			DrawImage( 56, line+2, dfIsCelsius ? 0xC9 : 0xC8 );
			break;
		}

		case 7:	// Real-time atomizer resistance
		{
			int rez = AtoError ? 0 : AtoRezMilli;
			int nd = ( rez < 1000 ) ? 3 : 4;
			DrawString( String_RES_s, 0, line+2 );
			DrawValueRight( 55, line, rez, 3, 0x1F, nd );
			DrawImage( 56, line+2, 0xC0 );
			// Refresh every second
			ScreenRefreshTimer = 10;
			break;
		}

		case 8:	// Real-time clock
		{
			S_RTC_TIME_DATA_T rtd;
			GetRTC( &rtd );
			DrawTime( 5, line, &rtd, 0x1F );
			break;
		}
	}
}


//=============================================================================
__myevic__ void ShowFireDuration( int line )
{
	int x;
	DrawFillRect( 0, line, 63, line+15, 1 );
	DrawFillRect( 1, line+1, 62, line+14, 0 );
	x = ( FireDuration > dfProtec / 2 ) ? 5 : 38;
	DrawValue( x, line+4, FireDuration, 1, 0xB, 0 );
	DrawImage( x + 15 + 6 * ( FireDuration > 99 ), line+4, 0x94 );
	InvertRect( 2, line+2, 2 + 59 * FireDuration / dfProtec, line+13 );
}


//=============================================================================
__myevic__ void DrawInfoLines()
{
	if (( gFlags.debug & 1 ) && ( !gFlags.firing ) && ( !EditModeTimer ))
	{
		uint32_t flags;
		MemCpy( &flags, (uint8_t*)&gFlags + 4, sizeof( uint32_t ) );
		DrawHexLong( 0, 52, flags, 1 );

		MemCpy( &flags, (void*)&gFlags, sizeof( uint32_t ) );
		DrawHexLong( 0, 71, flags, 1 );

		DrawValue( 0, 90, BatteryIntRez, 0, 0x1F, 0 );
		DrawValueRight( 64, 90, BatteryMaxPwr / 10, 0, 0x1F, 0 );
		return;
	}

	if ( Screen == 2 )
	{
		switch ( dfMode )
		{
			case 0:
			case 1:
			case 2:
			case 3:
				if ( dfStatus.priopwr )
				{
					DrawTempLine( 52 );
				}
				else
				{
					DrawPwrLine( AtoPower( AtoVolts ), 52 );
				}
				break;
			case 4:
			case 5:
			{
				ShowFireDuration( 49 );
				break;
			}
			default:
				break;
		}
	}
	else
	{
		switch ( dfMode )
		{
			case 0:
			case 1:
			case 2:
			case 3:
				if ( dfStatus.priopwr )
				{
					DrawTempLine( 52 );
				}
				else
				{
					DrawPwrLine( dfTCPower, 52 );
				}
				break;
			case 4:
				DrawVoltsLine( dfVWVolts, 52 );
				break;
			case 5:
				DrawVoltsLine( BypassVolts, 52 );
				break;
			default:
				break;
		}
	}

	DrawCoilLine( 71 );
	DrawAPTLine( 90 );
}


//=============================================================================
//----- (000068A0) --------------------------------------------------------
__myevic__ void DrawBFLine( int y )
{
	for ( int v = 0 ; v < 13 ; ++v )
	{
		DrawHLine( 5 * v, y, 5 * v + 2, 1 );
		DrawHLine( 5 * v, y + 1, 5 * v + 2, 1 );
	}
}


//=============================================================================

__myevic__ void DrawTemp()
{
	if ( Screen == 2 )
	{
		if ( dfIsCelsius )
		{
			int tempc = FarenheitToC( AtoTemp );

			DrawValue( 0, 13, tempc, 0, 0x48, 3 );
			DrawImage( 48, 20, 0xE0 );
		}
		else
		{
			DrawValue( 0, 13, AtoTemp, 0, 0x48, 3 );
			DrawImage( 48, 20, 0xE1 );
		}
	}
	else
	{
		DrawValue( 0, 13, dfTemp, 0, 0x48, 3 );
		DrawImage( 48, 20, dfIsCelsius ? 0xE0 : 0xE1 );
	}
}


//=============================================================================

__myevic__ void DrawPower( int pwr )
{
	int xp, yp;

	if ( pwr < 100 )
	{
		xp = 45;
		yp = 13;

		DrawValue( 5, 13, pwr, 1, 0x48, 2 );
		DrawImage( 45, 18, 0xB9 );
	}
	else
	{
		xp = 54;
		yp = 13;

		if ( pwr < 1000 )
		{
			DrawValue( 0, 13, pwr, 1, 0x48, 3 );
		}
		else
		{
			DrawValue( 5, 13, pwr / 10, 0, 0x48, 3 );
		//	DrawValue( 0, 18, pwr, 1, 0x29, 4 );
		}

		DrawImage( 54, 26, 0x98 );
	}

	if ( ISMODEVW(dfMode) )
	{
		if ( dfStatus.pcurve )
		{
			if ( !PreheatDelay || gFlags.osc_1hz )
			{
				DrawImage( xp, yp, 0x6A );
			}
		}
		else if ( dfPreheatTime )
		{
			if ( !PreheatDelay || gFlags.osc_1hz )
			{
				DrawImage( xp, yp, 0x77 );
			}
		}
	}
}


//=============================================================================

__myevic__ void ShowMainView()
{
	unsigned int pwr, amps;
	unsigned int i, j;

	unsigned int v15; // r0@93
	unsigned int v17; // r8@98
	unsigned int v19; // r3@99
	unsigned int v20; // r1@99
	unsigned int v26; // r2@168
	int v27; // r3@169

	if ( !gFlags.firing )
	{
		if ( gFlags.splash && SplashTimer )
		{
			ShowSplash();
			return;
		}
	}

	DrawMode();

	pwr = dfPower;

	if ( gFlags.firing )
	{
		pwr = AtoPower( AtoVolts );
	}
	else if ( ISMODEBY(dfMode) )
	{
		amps = 10 * BattVoltsTotal
			/ ( 10 * AtoRez + NumBatteries * BatteryIntRez );
		BypassVolts = AtoRez * amps;
		if ( BypassVolts > AtoMaxVolts ) BypassVolts = AtoMaxVolts;
		pwr  = ClampPower( BypassVolts, 0 );
	}

	if ( ISMODETC(dfMode) )
	{
		if ( dfStatus.priopwr )
		{
			if ( Screen == 2 )
			{
				pwr = AtoPower( TargetVolts );
			}
			else
			{
				pwr = dfTCPower;
			}

			DrawPower( pwr );
		}
		else
		{
			DrawTemp();
		}
	}

	if (( dfMode == 4 ) || ( dfMode == 5 ))
	{
		DrawPower( pwr );
	}

	if ( dfMode == 6 )
	{
		DrawString( String_MAX_s, 23, 15 );
		DrawString( String_MIN_s, 23, 97 );
		DrawHLine( 0, 18, 21, 1 );
		DrawHLine( 43, 18, 63, 1 );
		DrawHLine( 0, 100, 21, 1 );
		DrawHLine( 43, 100, 63, 1 );

		v15 = SearchSMARTRez( dfSavedCfgRez[(int)ConfigIndex] );
		if ( v15 > 3 )
		{
			for ( i = 0;
				  dfSavedCfgPwr[(int)ConfigIndex] / ( MaxPower / 15 ) > i;
				  ++i )
			{
				DrawFillRect( 0, 94 - 5*i, 63, 95 - 5*i, 1 );
			}
			if ( !i ) DrawBFLine( 94 );
		}
		else
		{
			v20 = SMARTPowers[ 2 * v15 + 1 ] / (MaxPower / 15);
			v17 = 99 - 5 * v20;
			for ( j = 0; ; ++j )
			{
				v19 = dfSavedCfgPwr[(int)ConfigIndex] / (MaxPower / 15);
				if ( v19 <= j )
					break;
				if ( v19 >= v20 && v20 - 1 <= j )
					DrawBFLine( 94 - 5*j );
				else
					DrawFillRect( 0, 94 - 5*j, 63, 95 - 5*j, 1 );
			}
			if ( !j ) DrawBFLine(94);
			DrawBFLine( v17 );
			DrawHLine( 25, v17, 40, 0 );
			DrawHLine( 25, v17 + 1, 40, 0 );
			DrawString( String_BF_s, 29, v17 - 3 );
		}

		if ( !ShowWeakBatFlag )
		{
			if ( !( gFlags.firing ) )
			{
				if ( !PD2 || !PD3 )
				{
					v26 = dfSavedCfgPwr[(int)ConfigIndex];
					if ( v26 >= 1000 )
					{
						v26 /= 10;
						v27 = 0;
					}
					else
					{
						v27 = 1;
					}
					DrawValue( 11, 112, v26, v27, 0x1F, 3 );
					DrawImage( 42, 114, 0xB2 );
				}
				else
				{
					ShowBattery();
				}
			}
			else
			{
				DrawValue( 10, 110, FireDuration, 1, 0x29, 2 );
				DrawImage( 40, 110, 0xB7 );
			}
		}
	}

	if ( dfMode != 6 )
	{
		DrawHLine( 0, 43, 63, 1 );
		DrawHLine( 0, 107, 63, 1 );

		ShowBattery();

		int h = ( dfStatus.nologo || HideLogo ) ? 0 : GetLogoHeight();

		if ( Screen == 2 || EditModeTimer )
		{
			DrawInfoLines();
		}
		else
		{
			if ( gFlags.anim3d )
			{
				anim3d( 1 );
			}
			else if ( dfStatus.clock )
			{
				if ( dfStatus.digclk )
				{
					DrawDigitClock( 63 );
				}
				else
				{
					DrawFillRect( 0, 44, 63, 127, 0 );
					DrawClock( 54 );
				}
			}
			else if ( ( h > 0 ) && dfStatus.logomid )
			{
				DrawLOGO( 0, ( 63 - h ) / 2 + 44 );
			}
			else
			{
				DrawInfoLines();
			}
		}

		if (( Screen == 1 ) && ( h > 0 ) && !dfStatus.logomid )
		{
			if ( h > 43 )
			{
				DrawHLine( 0, 43, 63, 0 );

				if ( h > 50 )
				{
					DrawFillRect( 0, 52, 63, 70, 0 );
				}
			}

			DrawLOGO( 0, 0 );
		}
	}

	if ( ShowProfNum )
	{
		DrawFillRect( 0, 108, 63, 127, 0 );
		DrawString( String_Profile, 12, 114 );
		DrawImage( 47, 114, dfProfile + 0x0C );
	}
	else if ( ShowWeakBatFlag )
	{
		DrawFillRect( 0, 107, 63, 127, 0 );
		ShowWeakBat();
	}
}


//=========================================================================
// Analog clock
//-------------------------------------------------------------------------
__myevic__ void DrawClock( int line )
{
	S_RTC_TIME_DATA_T rtd;
	GetRTC( &rtd );

	int c = line + 32;

	DrawImage( 0, line, 0xFE );
	DrawCircle( 32, c, 3, 1, 1 );

	int32_t h = ( rtd.u32Hour % 12 ) * 30 + ( rtd.u32Minute >> 1 );
	int32_t m = ( rtd.u32Minute ) * 6;
	int32_t s = ( rtd.u32Second ) * 6;

	DrawLine( 32, c, 32 + (( sine( h ) * 15 ) >> 16 ), c - (( cosine( h ) * 15 ) >> 16 ), 1, 3 );
	DrawLine( 32, c, 32 + (( sine( m ) * 21 ) >> 16 ), c - (( cosine( m ) * 21 ) >> 16 ), 1, 2 );
	DrawLine( 32, c, 32 + (( sine( s ) * 23 ) >> 16 ), c - (( cosine( s ) * 23 ) >> 16 ), 1, 1 );
}

__myevic__ void DrawDigitClock( int line )
{
	S_RTC_TIME_DATA_T rtd;
	GetRTC( &rtd );

	if ( dfStatus.timebig )
	{
		DrawValue( 5, line-3, rtd.u32Hour, 0, 0x29, 2 );
		DrawValue( 33, line-3, rtd.u32Minute, 0, 0x29, 2 );
		if ( !( rtd.u32Second & 1 ) )
		{
			DrawImage( 29, line-5, 0xDF );
			DrawImage( 29, line-13, 0xDF );
		}
		DrawDate( 4, line+19, &rtd, 0x1F );
	}
	else
	{
		DrawTime( 5, line,    &rtd, 0x1F );
		DrawDate( 4, line+16, &rtd, 0x1F );
	}
}
