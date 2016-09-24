#include "myevic.h"
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

uint8_t	MenuPage;
uint8_t	WattsInc;


//=========================================================================

__myevic__ void PowerPlus( uint16_t *pwr, uint16_t min, uint16_t max )
{
	if ( *pwr >= max )
	{
		if ( KeyTicks < 5 )
		{
			*pwr = min;
		}
	}
	else if ( KeyTicks < 105 )
	{
		*pwr += WattsInc;

		if ( *pwr >= max )
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
			*pwr = max;
		}
	}
	else if ( KeyTicks < 105 )
	{
		*pwr -= WattsInc;

		if ( *pwr <= min )
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
		dfTemp += 1;
		if ( dfTemp > 315 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 100 : 315;
		}
	}
	else
	{
		dfTemp += 5;
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
		dfTemp -= 1;
		if ( dfTemp < 100 )
		{
			dfTemp = ( KeyTicks < 5 ) ? 315 : 100;
		}
	}
	else
	{
		dfTemp -= 5;
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

	int v12;
	unsigned int v21;
	int v22;
	int v23;
	int tempf;
	unsigned int v44;
	unsigned int v45;
	unsigned int v46;
	unsigned int v47;
	unsigned int v57;
	unsigned int v58;
	unsigned int v59;
	int v60;

	if ( Event == 0 )
		return;

	BatRefreshTmr = 200;
	LastEvent = Event;
	Event = 0;

	if ( CustomEvents() )
		return;

	switch ( LastEvent )
	{

		case 20:	// ?
		case 21:	// ?
			gFlags.refresh_display = 1;
			return;

		case 1:		// Fire
		{
			if ( Screen == 59 )
			{
				gFlags.edit_value ^= 1;
				gFlags.refresh_display = 1;
				ScreenDuration = 10;
				return;
			}

			if ( dfStatus.off )
			{
				return;
			}

			if ( dfMode == 6 )
			{
				pwr = dfSavedCfgPwr[ConfigIndex];
			}
			else
			{
				pwr = dfPower;
			}

			if ( Screen == 83 )
			{
				fbStartGame();
				return;
			}

			if ( Screen == 82 )
			{
				UpdateDataFlash();
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

			if ( gFlags.low_battery )
			{
				if ( BatteryVoltage < 330 )
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

			if ( BatteryVoltage <= 310 )
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
				word_200000C0 = AtoRez;

				v12 = 0;
				if 		( dfMode == 0 ) v12 = dfRezLockedNI;
				else if ( dfMode == 1 ) v12 = dfRezLockedTI;
				else if ( dfMode == 2 ) v12 = dfRezLockedSS;
				else if ( dfMode == 3 ) v12 = dfRezLockedTCR;

				if ( !v12 || dfMode == 4 || dfMode == 5 || dfMode == 6 )
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

					if (  3 * dfRezNI >= word_200000C0 )
					{
						if (	dfRezNI + dfRezNI / 20 < word_200000C0
							&&	dfRezNI + 1 < word_200000C0
							&&	!dfRezLockedNI )
						{
							gFlags.check_rez_ni = 1;
							Event = 32;
							return;
						}

						if
						(
							(	(dfRezNI - dfRezNI / 20 <= word_200000C0 || dfRezNI - 1 <= word_200000C0)
							&&	(dfRezNI + dfRezNI / 20 >= word_200000C0 || dfRezNI + 1 >= word_200000C0))
						||
							(	dfRezLockedNI
							&&	(dfRezNI - dfRezNI / 10 <= word_200000C0 || dfRezNI - 4 <= word_200000C0))
						)
						{
							dfResistance = dfRezNI;
						}
						else
						{
							if ( dfRezNI - dfRezNI / 20 > word_200000C0 && dfRezNI - 1 > word_200000C0 )
							{
								dfResistance = word_200000C0;
								dfRezNI = word_200000C0;
							}
						}
					}
					else
					{
						dfResistance = word_200000C0;
						dfRezNI = word_200000C0;
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

					if (  2 * dfRezTI >= word_200000C0 )
					{
						if (	dfRezTI + dfRezTI / 20 < word_200000C0
							&&	dfRezTI + 1 < word_200000C0
							&&	!dfRezLockedTI )
						{
							gFlags.check_rez_ti = 1;
							Event = 32;
							return;
						}

						if
						(
							(	(dfRezTI - dfRezTI / 20 <= word_200000C0 || dfRezTI - 1 <= word_200000C0)
							&&	(dfRezTI + dfRezTI / 20 >= word_200000C0 || dfRezTI + 1 >= word_200000C0))
						||
							(	dfRezLockedTI
							&&	(dfRezTI - dfRezTI / 10 <= word_200000C0 || dfRezTI - 4 <= word_200000C0))
						)
						{
							dfResistance = dfRezTI;
						}
						else
						{
							if ( dfRezTI - dfRezTI / 20 > word_200000C0 && dfRezTI - 1 > word_200000C0 )
							{
								dfResistance = word_200000C0;
								dfRezTI = word_200000C0;
							}
						}
					}
					else
					{
						dfResistance = word_200000C0;
						dfRezTI = word_200000C0;
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

					if ( 3 * dfRezSS >= 2 * word_200000C0 )
					{
						if (	dfRezSS + dfRezSS / 20 < word_200000C0
							&&	dfRezSS + 1 < word_200000C0
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
								(	( dfRezSS - dfRezSS / 20 <= word_200000C0 || dfRezSS - 1 <= word_200000C0 )
								&&	( dfRezSS + dfRezSS / 20 >= word_200000C0 || dfRezSS + 1 >= word_200000C0 ) )
							||
								(	  dfRezLockedSS
								&&	( dfRezSS - dfRezSS / 10 <= word_200000C0 || dfRezSS - 4 <= word_200000C0 ) )
							)
							{
								dfResistance = dfRezSS;
							}
							else if ( dfRezSS - dfRezSS / 20 > word_200000C0 && dfRezSS - 1 > word_200000C0 )
							{
								dfResistance = word_200000C0;
								dfRezSS = word_200000C0;
							}
						}
					}
					else
					{
						dfResistance = word_200000C0;
						dfRezSS = word_200000C0;
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

					if ( 3 * dfRezTCR >= 2 * word_200000C0 )
					{
						if (	dfRezTCR + dfRezTCR / 20 < word_200000C0
							&&	dfRezTCR + 1 < word_200000C0
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
								(	( dfRezTCR - dfRezTCR / 20 <= word_200000C0 || dfRezTCR - 1 <= word_200000C0 )
								&&	( dfRezTCR + dfRezTCR / 20 >= word_200000C0 || dfRezTCR + 1 >= word_200000C0 ) )
							||
								(	  dfRezLockedTCR
								&&	( dfRezTCR - dfRezTCR / 10 <= word_200000C0 || dfRezTCR - 4 <= word_200000C0 ) )
							)
							{
								dfResistance = dfRezTCR;
							}
							else if ( dfRezTCR - dfRezTCR / 20 > word_200000C0 && dfRezTCR - 1 > word_200000C0 )
							{
								dfResistance = word_200000C0;
								dfRezTCR = word_200000C0;
							}
						}
					}
					else
					{
						dfResistance = word_200000C0;
						dfRezTCR = word_200000C0;
					}
				}
			}

			UpdateDFTimer = 50;

//------------------------------------------------------------------------------

			GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );
			PD7 = 0;

			gFlags.firing = 1;
			FireDuration = 0;

			if ( byte_20000048 == 1 ) byte_20000048 = 2;

			switch ( dfTempAlgo )
			{
				case 1:
					GetTempCoef( dfTempCoefsNI );
					break;
				case 2:
					GetTempCoef( dfTempCoefsTI );
					break;
				case 3:
					// Hard to find a consensusal value for the TCR of SS316L.
					// Anything from 88- to 100+ can be found depending on the source.
					// The original value of 120 is obviously way too high.
					// steam-engine.org gives a value of 87.9, seems reliable to me.
					TCR = 88;
					break;
				case 4:
					TCR = dfTCRM[dfTCRIndex];
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
					if ( !(gFlags.check_mode) )
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
				if ( pwr > MAXPWRLIMIT && BatteryVoltage <= 340 )
				{
					gFlags.limit_power = 1;
					PowerScale = 100 * MAXPWRLIMIT / pwr;
				}

				LowBatVolts = ( BatteryVoltage > 380 ) ? 0 : BatteryVoltage;
			}

			if ( ISMODEBY(dfMode) )
			{
				TargetVolts = 450;
			}

			SetADCState( 1, 1 );
			SetADCState( 2, 1 );
			AtoWarmUp();
			if ( !(gFlags.firing) || LastInputs != 1 )
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

		case 41:	// Game menu select
			if ( dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 83;
			ScreenDuration = 10;
			return;

		case 40:	// LOGO menu select
			if ( dfStatus.off )
				return;
			gFlags.refresh_display = 1;
			Screen = 82;
			ScreenDuration = 10;
			return;

		case 39:	// TCR Set menu select
			gFlags.edit_value = 0;
			EditTCRIndex = 0;
			gFlags.refresh_display = 1;
			Screen = 59;
			ScreenDuration = 10;
			return;

		case 38:	// Change interface version (unused)
			if ( !(dfStatus.off) )
				return;
			if ( ++dfUIVersion > 2 ) dfUIVersion = 0;
			UpdateDataFlash();
			gFlags.refresh_display = 1;
			Screen = 1;
			ScreenDuration = 3;
			return;

		case 34:	// Show battery voltage
			if ( !(dfStatus.off) )
				return;
			gFlags.refresh_display = 1;
			Screen = 54;
			ScreenDuration = 5;
			return;

		case 33:	// Ti ON/OFF (unused)
			if ( !(dfStatus.off) )
				return;
			dfTiOn = ( dfTiOn == 0 );
			gFlags.refresh_display = 1;
			Screen = 41;
			ScreenDuration = 3;
			UpdateDFTimer = 50;
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
			ScreenDuration = 2;
			return;

		case 23:	// Reset Time counter
			dfTimeCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 0;
			gFlags.refresh_display = 1;
			gFlags.draw_edited_item = 1;
			return;

		case 22:	// Reset Puff counter
			dfPuffCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 0;
			gFlags.refresh_display = 1;
			gFlags.draw_edited_item = 1;
			return;

		case 18:	// Flip display
			if ( !(dfStatus.off) )
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
				if ( BatteryVoltage > 300 || gFlags.usb_attached )
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
					Screen = 5;
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
			if ( Screen == 59 )
				return;
			if ( Screen == 82 )
				UpdateDataFlash();
			if ( Screen == 83 )
				UpdateDataFlash();
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
			if ( Screen == 5 )
			{
				gFlags.refresh_display = 1;
				Screen = 0;
				if ( dfStatus.off )
					SleepTimer = 0;
				else
					SleepTimer = 18000;
			}
			gFlags.battery_charging = 0;
			return;

		case 12:	// Battery charging
			gFlags.battery_charging = 1;
			gFlags.refresh_display = 1;
			if ( dfStatus.off )
			{
				Screen = 5;
				BatAnimLevel = BatteryTenth;
			}
			else
			{
				if ( Screen != 5 )
					MainView();
			}
			return;

		case 11:	// USB cable detach
			gFlags.usb_attached = 0;
			gFlags.battery_charging = 0;
			if ( Screen == 5 )
			{
				if ( dfStatus.off )
				{
					gFlags.refresh_display = 1;
					Screen = 0;
				}
				else
				{
					MainView();
				}
			}
			return;


		case 10:	// USB cable attach
			gFlags.low_battery = 0;
			gFlags.usb_attached = 1;
			if ( !(dfStatus.off) )
				MainView();
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
			if ( Screen == 59 )
			{
				if ( gFlags.edit_value )
				{
					if ( EditTCRIndex >= 3 ) EditTCRIndex = 0;

					if ( dfTCRM[EditTCRIndex] > 1 )
					{
						--dfTCRM[EditTCRIndex];
					}
				}
				else
				{
					if ( EditTCRIndex )
						--EditTCRIndex;
					else
						EditTCRIndex = 2;
				}
				gFlags.refresh_display = 1;
				ScreenDuration = 10;
				return;
			}

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

			if ( Screen == 82 )
			{
				dfStatus.nologo ^= 1;
			}
			else if ( Screen == 83 )
			{
				if ( ++dfFBSpeed > 3 ) dfFBSpeed = 0;
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
				return;
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
							v58 = dfSavedCfgPwr[ConfigIndex];
							if ( v58 % 10 )
							{
								v59 = 5 * ( v58 / 10 );
								v58 = 10 * ( v58 / 10 );
								dfSavedCfgPwr[ConfigIndex] = 2 * v59;
							}

							v60 = v58 - 10;

							if ( v60 <= 0 )
								dfSavedCfgPwr[ConfigIndex] = AtoMinPower;
							else
								dfSavedCfgPwr[ConfigIndex] = v60;

							if ( dfSavedCfgPwr[ConfigIndex] <= AtoMinPower )
								dfSavedCfgPwr[ConfigIndex] = AtoMinPower;
						}
						v57 = dfSavedCfgPwr[ConfigIndex];
						dfVWVolts = GetAtoVWVolts(v57);
					}
					else if ( dfMode == 4 )
					{
						PowerMinus( &dfPower, AtoMinPower, AtoMaxPower );
						dfVWVolts = GetAtoVWVolts( dfPower );
					}
				}

	            MainView();
	            if ( KeyTicks >= 5 )
	            {
					gFlags.draw_edited_item = 1;
	                DrawScreen();
	            }
	            UpdateDFTimer = 50;
	            return;
			}

			gFlags.refresh_display = 1;
			ScreenDuration = 10;
			return;
		}

//------------------------------------------------------------------------------

		case 2:		// + (plus or right) button
		{
			if ( Screen == 59 )
			{
				if ( gFlags.edit_value )
				{
					if ( EditTCRIndex > 2 ) EditTCRIndex = 0;

					if ( dfTCRM[EditTCRIndex] < 999 )
					{
						++dfTCRM[EditTCRIndex];
					}
				}
				else
				{
					++EditTCRIndex;
					if ( ( EditTCRIndex ) > 2 ) EditTCRIndex = 0;
				}
				gFlags.refresh_display = 1;
				ScreenDuration = 10;
				return;
			}

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

			if (( Screen >= 80 ) && ( Screen < 100 ))
			{
				if ( ++MenuPage > 2 )
				{
					MenuPage = 1;
				}
	
				if ( MenuPage == 1 )
				{
					Event = 40;
				}
				else if ( MenuPage == 2 )
				{
					Event = 41;
				}
			}
			else if ( Screen == 51 )
			{
				switch ( dfMode )
				{
					case 0:
						dfRezNI = word_200000C0;
						gFlags.check_rez_ni = 0;
						break;
					case 1:
						dfRezTI = word_200000C0;
						gFlags.check_rez_ti = 0;
						break;
					case 2:
						dfRezSS = word_200000C0;
						gFlags.check_rez_ss = 0;
						break;
					case 3:
						dfRezTCR = word_200000C0;
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
							if ( ++dfAPT > 7 ) dfAPT = 0;
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
							break;

						case 6:
							if ( ConfigIndex < 10 && !AtoError && AtoRez )
							{
								v45 = dfSavedCfgPwr[ConfigIndex];
								if ( v45 % 10 )
								{
									v46 = 5 * ( v45 / 10 );
									v45 = 10 * ( v45 / 10 );
									dfSavedCfgPwr[ConfigIndex] = 2 * v46;
								}
								v47 = ( v45 + 10 );
								dfSavedCfgPwr[ConfigIndex] = v47;
								if ( v47 > AtoMaxPower )
									dfSavedCfgPwr[ConfigIndex] = AtoMaxPower;
							}
							v44 = dfSavedCfgPwr[ConfigIndex];
							dfVWVolts = GetAtoVWVolts(v44);
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
	            return;
			}
		}
	}
}
