#include "myevic.h"
#include "myprintf.h"
#include "screens.h"
#include "atomizer.h"
#include "display.h"
#include "battery.h"
#include "miscs.h"
#include "events.h"
#include "dataflash.h"
#include "timers.h"
#include "meadc.h"
#include "atomizer.h"
#include "flappy.h"

//=========================================================================
// Globals

volatile uint8_t Event;
uint8_t	LastEvent;

uint8_t	WattsInc;


//=========================================================================

__myevic__ void PowerPlus( uint16_t *pwr, uint16_t min, uint16_t max )
{
	max = max - ( max % WattsInc );

	if ( *pwr >= max )
	{
		if ( KeyTicks < 5 )
		{
			*pwr = min;
		}
	}
	else if ( KeyTicks < 105 )
	{
		if ( *pwr < 1000 )
		{
			*pwr += WattsInc;
		}
		else
		{
			*pwr += 10;
		}

		if ( *pwr > max )
		{
			*pwr = max;
		}
	}
	else
	{
		if ( *pwr % 10 )
		{
			*pwr -= *pwr % 10;
		}
		*pwr += 10;
	}
}


//-------------------------------------------------------------------------

__myevic__ void PowerMinus( uint16_t *pwr, uint16_t min, uint16_t max )
{
	if ( *pwr <= min )
	{
		if ( KeyTicks < 5 )
		{
			*pwr = max - ( max % WattsInc );
		}
	}
	else if ( KeyTicks < 105 )
	{
		if ( *pwr <= 1000 )
		{
			*pwr -= WattsInc;
		}
		else
		{
			*pwr -= 10;
		}

		if ( *pwr < min )
		{
			*pwr = min;
		}
	}
	else
	{
		if ( *pwr % 10 )
		{
			*pwr -= *pwr % 10;
		}
		else
		{
			*pwr -= 10;
		}
	}
}


//-------------------------------------------------------------------------

__myevic__ void TempPlus()
{
	if ( dfIsCelsius )
	{
		dfTemp += dfStatus.onedegree ? 1 : 5;
		if ( dfTemp > 315 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 100 : 315;
		}
	}
	else
	{
		dfTemp += dfStatus.onedegree ? 5 : 10;
		if ( dfTemp > 600 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 200 : 600;
		}
	}
}


//-------------------------------------------------------------------------

__myevic__ void TempMinus()
{
	if ( dfIsCelsius )
	{
		dfTemp -= dfStatus.onedegree ? 1 : 5;
		if ( dfTemp < 100 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 315 : 100;
		}
	}
	else
	{
		dfTemp -= dfStatus.onedegree ? 5 : 10;
		if ( dfTemp < 200 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 600 : 200;
		}
	}
}


//----- (000039E0) --------------------------------------------------------
__myevic__ void EventHandler()
{
	unsigned int pwr;
	unsigned int spwr;

	unsigned int v21;
	int v22;
	int v23;
	int tempf;

	if ( Event == 0 )
		return;

	NoEventTimer = 200;
	LastEvent = Event;
	Event = 0;

	if ( CustomEvents() )
		return;

	switch ( LastEvent )
	{

		case 1:		// Fire
		{
			if ( dfStatus.off )
			{
				return;
			}

			if ( Screen == 1 )
			{
				if ( EditModeTimer )
				{
					EditModeTimer = 0;
					gFlags.edit_capture_evt = 0;
					gFlags.draw_edited_item = 1;
					UpdateDFTimer = 50;
				}
			}

			if ( gFlags.firing )
			{
				KeyPressTime |= 0x8000;
				return;
			}

			if ( BatteryStatus == 2 )
			{
				Screen = 56;
				ScreenDuration = 2;
				gFlags.refresh_display = 1;
				return;
			}

			if ( BatteryStatus == 3 )
			{
				Screen = 57;
				ScreenDuration = 2;
				gFlags.refresh_display = 1;
				return;
			}

			if ( BatteryStatus == 4 )
			{
				Screen = 58;
				ScreenDuration = 2;
				gFlags.refresh_display = 1;
				return;
			}

			if ( gFlags.low_battery )
			{
				if ( BatteryVoltage < BatteryCutOff + 50 )
				{
					gFlags.refresh_display = 1;
					Screen = 25;
					ScreenDuration = 2;
					KeyPressTime |= 0x8000;
					return;
				}
				else
				{
					gFlags.low_battery = 0;
				}
			}

			if ( BatteryVoltage <= BatteryCutOff + 30 )
			{
				Event = 28;
				gFlags.low_battery = 1;
				return;
			}

			if ( BoardTemp >= 70 )
			{
				Overtemp();
				return;
			}

			while ( AtoStatus == 4 && AtoProbeCount < 12 )
			{
				ProbeAtomizer();
				WaitOnTMR2( 10 );
			}

			switch ( AtoStatus )
			{
				case 1:
					Event = 25;
					break;
				case 2:
					Event = 27;
					break;
				case 0:
				case 3:
					Event = 26;
					break;
				case 4:
				default:
					break;
			}

			if ( AtoError )
				return;

			if ( byte_200000B3 == 1 )
			{
				byte_200000B3 = 0;
				NewRez = AtoRez;

				uint16_t rez = 0;
				if 		( dfMode == 0 ) rez = dfRezLockedNI;
				else if ( dfMode == 1 ) rez = dfRezLockedTI;
				else if ( dfMode == 2 ) rez = dfRezLockedSS;
				else if ( dfMode == 3 ) rez = dfRezLockedTCR;

				if ( !rez || dfMode == 4 || dfMode == 5 || dfMode == 6 )
				{
					dfResistance = AtoRez;
					UpdateDFTimer = 50;
				}
			}

//------------------------------------------------------------------------------

			if ( (gFlags.check_rez_ni) && dfMode == 0 )
			{
				gFlags.check_rez_ni = 0;

				if ( !dfRezNI )
				{
					dfRezNI = dfResistance;
				}
				else
				{
					word_200000BA = dfRezNI;

					if (  3 * dfRezNI >= NewRez )
					{
						if (	dfRezNI + dfRezNI / 20 < NewRez
							&&	dfRezNI + 1 < NewRez
							&&	!dfRezLockedNI )
						{
							gFlags.check_rez_ni = 1;
							Event = 32;
							return;
						}

						if
						(
							(	(dfRezNI - dfRezNI / 20 <= NewRez || dfRezNI - 1 <= NewRez)
							&&	(dfRezNI + dfRezNI / 20 >= NewRez || dfRezNI + 1 >= NewRez))
						||
							(	dfRezLockedNI
							&&	(dfRezNI - dfRezNI / 10 <= NewRez || dfRezNI - 4 <= NewRez))
						)
						{
							dfResistance = dfRezNI;
						}
						else
						{
							if ( dfRezNI - dfRezNI / 20 > NewRez && dfRezNI - 1 > NewRez )
							{
								dfResistance = NewRez;
								dfRezNI = NewRez;
							}
						}
					}
					else
					{
						dfResistance = NewRez;
						dfRezNI = NewRez;
					}
				}
			}

			if ( (gFlags.check_rez_ti) && dfMode == 1 )
			{
				gFlags.check_rez_ti = 0;

				if ( !dfRezTI )
				{
					dfRezTI = dfResistance;
				}
				else
				{
					word_200000B8 = dfRezTI;

					if (  2 * dfRezTI >= NewRez )
					{
						if (	dfRezTI + dfRezTI / 20 < NewRez
							&&	dfRezTI + 1 < NewRez
							&&	!dfRezLockedTI )
						{
							gFlags.check_rez_ti = 1;
							Event = 32;
							return;
						}

						if
						(
							(	(dfRezTI - dfRezTI / 20 <= NewRez || dfRezTI - 1 <= NewRez)
							&&	(dfRezTI + dfRezTI / 20 >= NewRez || dfRezTI + 1 >= NewRez))
						||
							(	dfRezLockedTI
							&&	(dfRezTI - dfRezTI / 10 <= NewRez || dfRezTI - 4 <= NewRez))
						)
						{
							dfResistance = dfRezTI;
						}
						else
						{
							if ( dfRezTI - dfRezTI / 20 > NewRez && dfRezTI - 1 > NewRez )
							{
								dfResistance = NewRez;
								dfRezTI = NewRez;
							}
						}
					}
					else
					{
						dfResistance = NewRez;
						dfRezTI = NewRez;
					}
				}
			}

			if ( (gFlags.check_rez_ss) && dfMode == 2 )
			{
				gFlags.check_rez_ss = 0;

				if ( !dfRezSS )
				{
					dfRezSS = dfResistance;
				}
				else
				{
					word_200000BC = dfRezSS;

					if ( 3 * dfRezSS >= 2 * NewRez )
					{
						if (	dfRezSS + dfRezSS / 20 < NewRez
							&&	dfRezSS + 1 < NewRez
							&&	!dfRezLockedSS )
						{
							gFlags.check_rez_ss = 1;
							Event = 32;
							return;
						}
						else
						{
							if
							(
								(	( dfRezSS - dfRezSS / 20 <= NewRez || dfRezSS - 1 <= NewRez )
								&&	( dfRezSS + dfRezSS / 20 >= NewRez || dfRezSS + 1 >= NewRez ) )
							||
								(	  dfRezLockedSS
								&&	( dfRezSS - dfRezSS / 10 <= NewRez || dfRezSS - 4 <= NewRez ) )
							)
							{
								dfResistance = dfRezSS;
							}
							else if ( dfRezSS - dfRezSS / 20 > NewRez && dfRezSS - 1 > NewRez )
							{
								dfResistance = NewRez;
								dfRezSS = NewRez;
							}
						}
					}
					else
					{
						dfResistance = NewRez;
						dfRezSS = NewRez;
					}
				}
			}

			if ( (gFlags.check_rez_tcr) && dfMode == 3 )
			{
				gFlags.check_rez_tcr = 0;

				if ( !dfRezTCR )
				{
					dfRezTCR = dfResistance;
				}
				else
				{
					word_200000BE = dfRezTCR;

					if ( 3 * dfRezTCR >= 2 * NewRez )
					{
						if (	dfRezTCR + dfRezTCR / 20 < NewRez
							&&	dfRezTCR + 1 < NewRez
							&&	!dfRezLockedTCR )
						{
							gFlags.check_rez_tcr = 1;
							Event = 32;
							return;
						}
						else
						{
							if
							(
								(	( dfRezTCR - dfRezTCR / 20 <= NewRez || dfRezTCR - 1 <= NewRez )
								&&	( dfRezTCR + dfRezTCR / 20 >= NewRez || dfRezTCR + 1 >= NewRez ) )
							||
								(	  dfRezLockedTCR
								&&	( dfRezTCR - dfRezTCR / 10 <= NewRez || dfRezTCR - 4 <= NewRez ) )
							)
							{
								dfResistance = dfRezTCR;
							}
							else if ( dfRezTCR - dfRezTCR / 20 > NewRez && dfRezTCR - 1 > NewRez )
							{
								dfResistance = NewRez;
								dfRezTCR = NewRez;
							}
						}
					}
					else
					{
						dfResistance = NewRez;
						dfRezTCR = NewRez;
					}
				}
			}

			UpdateDFTimer = 50;

//------------------------------------------------------------------------------

			if ( ISVTCDUAL )
			{
				GPIO_SetMode( PD, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
				PD1 = 0;
			}
			else if ( !ISCUBOID )
			{
				GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );
				PD7 = 0;
			}

		//	myprintf( "StartFire\n" );

			gFlags.firing = 1;
			FireDuration = 0;

			if ( BattProbeCount == 1 ) BattProbeCount = 2;

			switch ( dfTempAlgo )
			{
				case 1:
					GetTempCoef( dfTempCoefsNI );
					break;

				case 2:
					GetTempCoef( dfTempCoefsTI );
					break;

				case 3:
					TCR = 120;
					break;

				case 4:
					if ( dfMode == 3 )
					{
						TCR = dfTCRM[dfTCRIndex];
					}
					else if ( dfMode < 3 )
					{
						TCR = dfTCRP[dfMode];
					}
					break;

				default:
					break;
			}

			TargetVolts = 100;
			PowerScale = 100;

			if ( ISMODETC(dfMode) )
			{
				if ( dfResistance <= 150 )
				{
					if ( !gFlags.check_mode )
					{
						if ( dfRezType != 1 )
						{
							tempf = dfTemp;
							if ( dfIsCelsius == 1 ) tempf = CelsiusToF( dfTemp );

							int a  = ( 100 * ( tempf - 200 ) / 400 );
							int b = 300 * a;

							TargetVolts = GetAtoVWVolts( b / 100 + 100 );
						}
						else
						{
							dfMode = 4;
						}
					}
					else
					{
						v21 = dfTCPower;
						if ( v21 > 400 ) v21 = 400;
						if ( v21 < 300 ) v21 = 300;

						v22 = AtoPowerLimit( v21 );
						v23 = GetVoltsForPower( v22 );

						TargetVolts = v23;
					}
				}
				else
				{
					dfMode = 4;
				}
			}

			if ( !ISMODETC(dfMode) )
			{
				if ( AtoRez < 10 )
				{
					StopFire();
					Event = 27;
					return;
				}
			}

			if ( dfMode == 6 )
			{
				pwr = dfSavedCfgPwr[ConfigIndex];
			}
			else
			{
				pwr = dfPower;
			}

			if ( ISMODEVW(dfMode) )
			{
				if ( dfPreheatTime )
				{
					PreheatTimer = dfPreheatTime;

					if ( dfStatus.phpct )
					{
						PreheatPower = dfPower * dfPreheatPwr / 100;
					}
					else
					{
						PreheatPower = dfPreheatPwr;
					}

					if ( PreheatPower > AtoMaxPower )
					{
						PreheatPower = AtoMaxPower;
					}

					pwr = PreheatPower;

					TargetVolts = GetVoltsForPower( PreheatPower );
				}
				else if ( pwr <= 300 )
				{
					TargetVolts = dfVWVolts;
				}
				else
				{
					TargetVolts = GetVoltsForPower( 300 );
				}

				gFlags.limit_power = 0;
				if ( pwr > BatteryMaxPwr )
				{
					gFlags.limit_power = 1;
					PowerScale = 100 * BatteryMaxPwr / pwr;
					TargetVolts = GetVoltsForPower( BatteryMaxPwr );
				}

				LowBatVolts = ( BatteryVoltage > BatteryCutOff + 100 ) ? 0 : BatteryVoltage;
			}

			if ( ISMODEBY(dfMode) )
			{
				if ( ( NumBatteries > 1 ) && ( ISVTCDUAL || ISCUBOID ) )
				{
					TargetVolts = 900;
				}
				else
				{
					TargetVolts = 450;
				}
			}

			SetADCState( 1, 1 );
			SetADCState( 2, 1 );
			if ( ISRX200S )
			{
				SetADCState( 15, 1 );
			}
			AtoWarmUp();
			if ( !gFlags.firing || LastInputs != 1 )
				StopFire();
			gFlags.refresh_display = 1;
			Screen = 2;
			ScreenDuration = 1;
			return;
		}

//------------------------------------------------------------------------------

		default:
			return;

//------------------------------------------------------------------------------

		case 34:	// Show battery voltage
			if ( !dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 54;
			ScreenDuration = 5;
			return;

		case 32:	// New coil
			StopFire();
			gFlags.refresh_display = 1;
			Screen = 51;
			ScreenDuration = 10;
			return;

		case 31:	// Show board temperature
			if ( dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 37;
			ScreenDuration = 5;
			return;

		case 30:	// Key lock violation
			if ( dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 28;
			ScreenDuration = 2;
			return;

		case 29:	// FW Version screen
			if ( dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 50;
			ScreenDuration = 5;
			return;

		case 28:	// Battery < 3.1V idle or < 2.8V firing
			StopFire();
			KeyPressTime |= 0x8000;
			gFlags.refresh_display = 1;
			Screen = 24;
			ScreenDuration = 2;
			return;

		case 27:	// Atomizer Low
			StopFire();
			gFlags.refresh_display = 1;
			Screen = 22;
			ScreenDuration = 1;
			KeyPressTime |= 0x8000;
			return;

		case 26:	// No Atomizer Found
			StopFire();
			gFlags.refresh_display = 1;
			Screen = 20;
			ScreenDuration = 1;
			return;

		case 25:	// Atomizer short
			gFlags.refresh_display = 1;
			Screen = 21;
			ScreenDuration = 1;
			return;

		case 24:	// 10s Fire protection
			StopFire();
			if ( AtoError )
				return;
			gFlags.refresh_display = 1;
			Screen = 23;
			ScreenDuration = 10;
			return;

		case 23:	// Reset Time counter
			dfTimeCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 1000;
			gFlags.refresh_display = 1;
			gFlags.draw_edited_item = 1;
			return;

		case 22:	// Reset Puff counter
			dfPuffCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 1000;
			gFlags.refresh_display = 1;
			gFlags.draw_edited_item = 1;
			return;

		case 18:	// Flip display
			if ( !dfStatus.off )
				return;
			dfStatus.flipped ^= 1;
			InitDisplay();
			gFlags.refresh_display = 1;
			Screen = 1;
			ScreenDuration = 2;
			UpdateDataFlash();
			return;

		case 17:	// Switch On/Off
			if ( gFlags.firing )
				return;
			if ( dfStatus.off )
			{
				gFlags.sample_vbat = 1;
				ReadBatteryVoltage();
				if ( ( BatteryVoltage > BatteryCutOff + 20 ) || gFlags.usb_attached )
				{
					dfStatus.off = 0;
					MainView();
				}
			}
			else
			{
				dfStatus.off = 1;
				gFlags.refresh_display = 1;
				if ( gFlags.battery_charging )
				{
					ChargeView();
					BatAnimLevel = BatteryTenth;
				}
				else
				{
					Screen = 0;
					SleepTimer = 0;
				}
			}
			return;

		case 16:	// Edit mode
			if ( dfStatus.off )
				return;
			gFlags.draw_edited_item = 1;
			EditItemIndex = 0;
			EditModeTimer = 1000;
			MainView();
			return;

		case 15:	// Single Fire click
			if ( dfStatus.off || gFlags.firing )
				return;
			if ( gFlags.refresh_battery )
			{
				gFlags.refresh_battery = 0;
				gFlags.sample_vbat = 1;
				ReadBatteryVoltage();
			}
			if ( Screen == 1 )
			{
				if ( EditModeTimer )
				{
					EditModeTimer = 0;
					gFlags.edit_capture_evt = 0;
					gFlags.draw_edited_item = 1;
					UpdateDFTimer = 50;
				}
			}
			MainView();
			return;

		case 13:	// Battery removed
			gFlags.battery_charging = 0;
			if ( Screen == 5 )
			{
				gFlags.refresh_display = 1;
				Screen = 0;
				if ( dfStatus.off )
					SleepTimer = 0;
				else
					SleepTimer = 18000;
			}
			return;

		case 12:	// Battery charging
			gFlags.battery_charging = 1;
			gFlags.refresh_display = 1;
			BatAnimLevel = BatteryTenth;
			if ( BatteryStatus == 1 )
			{
				Screen = 55;
				ScreenDuration = 2;
			}
			else if ( dfStatus.off )
			{
				ChargeView();
			}
			else
			{
				if ( Screen != 5 )
					MainView();
			}
			return;

		case 11:	// USB cable detach
			if ( ISVTCDUAL )
			{
				PF2 = 0;
				PA2 = 0;
			}
			byte_20000056 = 0;
			if ( BatteryStatus == 3 || BatteryStatus == 4 )
			{
				BatteryStatus = 0;
			}
			gFlags.usb_attached = 0;
			gFlags.battery_charging = 0;
			gFlags.monitoring = 0;
			if ( Screen == 5 )
			{
				if ( dfStatus.off || dfStealthOn )
				{
					gFlags.refresh_display = 1;
					Screen = 0;
					if ( dfStatus.off )
						SleepTimer = 0;
					else
						SleepTimer = 18000;
				}
				else
				{
					MainView();
				}
			}
			return;

		case 10:	// USB cable attach
			byte_20000056 = 1;
			byte_20000055 = 1;
			gFlags.low_battery = 0;
			gFlags.usb_attached = 1;
			if ( !dfStatus.off )
			{
				if ( Screen == 0 )
				{
					ChargeView();
				}
				else
				{
					MainView();
				}
			}
			return;

		case 6:		// Stealth On/Off
			if ( dfStatus.off )
				return;
			dfStealthOn = ( dfStealthOn == 0 );
			gFlags.refresh_display = 1;
			Screen = 40;
			ScreenDuration = 3;
			return;

		case 4:		// Key (Un)Lock
			if ( dfStatus.off )
				return;
			dfStatus.keylock ^= 1;
			if ( dfStatus.keylock )
				Screen = 28;
			else
				Screen = 31;
			ScreenDuration = 2;
			gFlags.refresh_display = 1;
			UpdateDFTimer = 50;
			return;

//------------------------------------------------------------------------------

		case 3:		// - (minus or left) button
		{
			if ( dfStatus.off )
			{
				return;
			}

			if ( Screen == 0 || Screen == 60 )
			{
				if ( dfStatus.wakeonpm )
				{
					MainView();
				}
			}

			if ( Screen == 2 )
			{
				MainView();
			}

			else if ( Screen == 51 )
			{
				switch ( dfMode )
				{
					case 0:
						dfResistance = dfRezNI;
						gFlags.check_rez_ni = 0;
						break;
					case 1:
						dfResistance = dfRezTI;
						gFlags.check_rez_ti = 0;
						break;
					case 2:
						dfResistance = dfRezSS;
						gFlags.check_rez_ss = 0;
						break;
					case 3:
						dfResistance = dfRezTCR;
						gFlags.check_rez_tcr = 0;
						break;
					default:
						break;
				}
				MainView();
			}
			else if ( Screen == 1 )
			{
				KeyUpTimer = 5;

				if ( EditModeTimer )
				{
					EditModeTimer = 1000;

					if ( EditItemIndex == 2 && gFlags.edit_capture_evt )
					{
						dfStatus.priopwr ? TempMinus() : PowerMinus( &dfTCPower, 10, MaxTCPower );
					}
					else if ( !ISMODETC(dfMode) )
					{
						if ( dfMode == 6 )
						{
							EditItemIndex = 0;
						}
						else
						{
							if ( EditItemIndex == 0 )
							{
								EditItemIndex = 4;
							}
							else
							{
								EditItemIndex = 0;
							}
						}
					}
					else
					{
						if ( ++EditItemIndex > 4 )
							EditItemIndex = 0;
					}
				}
				else
				{
					if ( ISMODETC(dfMode) )
					{
						dfStatus.priopwr ? PowerMinus( &dfTCPower, 10, MaxTCPower ) : TempMinus();
					}
					else if ( dfMode == 6 )
					{
						if ( ConfigIndex < 10 )
						{
							spwr = dfSavedCfgPwr[ConfigIndex];
							spwr -= spwr % 10;
							spwr -= 10;
							if ( spwr < AtoMinPower ) spwr = AtoMinPower;

							dfSavedCfgPwr[ConfigIndex] = spwr;
							dfVWVolts = GetAtoVWVolts( spwr );
						}
					}
					else if ( dfMode == 4 )
					{
						PowerMinus( &dfPower, AtoMinPower, AtoMaxPower );
						dfVWVolts = GetAtoVWVolts( dfPower );
						if ( ConfigIndex < 10 && !AtoError && AtoRez )
						{
							dfSavedCfgPwr[ConfigIndex] = dfPower;
						}
					}
				}

	            MainView();
	            if ( KeyTicks >= 5 )
	            {
					gFlags.draw_edited_item = 1;
	                DrawScreen();
	            }
	            UpdateDFTimer = 50;
			}

			break;
		}

//------------------------------------------------------------------------------

		case 2:		// + (plus or right) button
		{
			if ( dfStatus.off )
			{
				return;
			}

			if ( Screen == 0 || Screen == 60 )
			{
				if ( dfStatus.wakeonpm )
				{
					MainView();
				}
			}

			if ( Screen == 2 )
			{
				MainView();
			}

			if ( Screen == 51 )
			{
				switch ( dfMode )
				{
					case 0:
						dfRezNI = NewRez;
						gFlags.check_rez_ni = 0;
						break;
					case 1:
						dfRezTI = NewRez;
						gFlags.check_rez_ti = 0;
						break;
					case 2:
						dfRezSS = NewRez;
						gFlags.check_rez_ss = 0;
						break;
					case 3:
						dfRezTCR = NewRez;
						gFlags.check_rez_tcr = 0;
						break;
					default:
						break;
				}
				MainView();
			}
			else if ( Screen == 1 )
			{
				KeyUpTimer = 5;

				if ( EditModeTimer )
				{
					EditModeTimer = 1000;

					switch ( EditItemIndex )
					{
						case 0:
							NextMode();
							break;

						case 1:
							if ( dfMode == 3 )
							{
								if ( ++dfTCRIndex >= 3 )
									dfTCRIndex = 0;
							}
							else if ( dfMode == 0 || dfMode == 1 || dfMode == 2 )
							{
								++dfTCMode;
								if ( dfTCMode > 2 )
									dfTCMode = 0;
								dfMode = dfTCMode;
								ModeChange();
							}
							break;

						case 2:
							dfStatus.priopwr ? TempPlus() : PowerPlus( &dfTCPower, 10, MaxTCPower );
							gFlags.edit_capture_evt = 1;
							break;

						case 3:
							SwitchRezLock();
							break;

						case 4:
							if ( ++dfAPT > 8 ) dfAPT = 0;
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
							dfStatus.priopwr ? PowerPlus( &dfTCPower, 10, MaxTCPower ) : TempPlus();
							break;

						case 4:
							PowerPlus( &dfPower, AtoMinPower, AtoMaxPower );
							dfVWVolts = GetAtoVWVolts( dfPower );
							if ( ConfigIndex < 10 && !AtoError && AtoRez )
							{
								dfSavedCfgPwr[ConfigIndex] = dfPower;
							}
							break;

						case 6:
							if ( ConfigIndex < 10 && !AtoError && AtoRez )
							{
								spwr = dfSavedCfgPwr[ConfigIndex];
								spwr -= spwr % 10;
								spwr += 10;
								if ( spwr > AtoMaxPower ) spwr = AtoMaxPower;

								dfSavedCfgPwr[ConfigIndex] = spwr;
								dfVWVolts = GetAtoVWVolts( spwr );
							}
							break;

						case 5:
						default:
							break;
					}

				}

	            MainView();
	            if ( KeyTicks >= 5 )
	            {
					gFlags.draw_edited_item = 1;
	                DrawScreen();
	            }
	            UpdateDFTimer = 50;
			}

			break;
		}
	}
}
