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
#include "M451Series.h"


//----- (000039E0) --------------------------------------------------------
__myevic__ void EventHandler()
{
	unsigned int pwr;

	int v12;
	unsigned int v21;
	int v22;
	int v23;
	int tempf;
	unsigned int v35;
	short v36;
	unsigned int v41;
	short v42;
	short v43;
	unsigned int v44;
	unsigned int v45;
	unsigned int v46;
	unsigned int v47;
	signed short v48;
	unsigned int v49;
	unsigned int v53;
	short v54;
	unsigned int v57;
	unsigned int v58;
	unsigned int v59;
	int v60;
	signed short v61;

	if ( (signed char)Event == (signed char)-1 )
		return;

	BatRefreshTmr = 200;
	LastEvent = Event;
	Event = -1;

	if ( CustomEvents() )
		return;

	switch ( LastEvent )
	{

		case 20:	// ?
		case 21:	// ?
			Flags64 |= 0x20000;
			return;

		case 1:		// Fire
		{
			if ( dfStatus & 1 )
			{
				if ( Screen == 59 )
				{
					Flags68 ^= 1;
					Flags64 |= 0x20000u;
					ScreenDuration = 10;
				}
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
				UpdateDataFlash();

				if ( dfFBSpeed > 2u )
					return;

				if ( !( Flags68 & 0x200 ) )
				{
					Flags68 |= 0x200;
					Screen = 0;
					Flags64 |= 0x4000u;
					BatRefreshTmr = 0;
					SleepTimer = 3000;
					InitTimeouts();
					DisplayRefresh();
					fbBirdAnim( 24 );
					CreateTimeout( fbStartScreen + 1 );
				}
				return;
			}

			if ( Screen == 82 )
			{
				UpdateDataFlash();
			}

			if ( EditModeTimer )
			{
				EditModeTimer = 0;
				Flags64 |= 0x40000u;
				UpdateDFTimer = 50;
			}

			Flags68 &= ~0x10u;

			if ( Flags64 & 0x100 )
			{
				KeyPressTime |= 0x8000u;
				return;
			}

			if ( Flags64 & 0x200 )
			{
				if ( BatteryVoltage < 330u )
				{
					Flags64 |= 0x20000u;
					Screen = 25;
					ScreenDuration = 2;
					KeyPressTime |= 0x8000u;
					return;
				}
				else
				{
					Flags64 &= ~0x200u;
				}
			}

			if ( BatteryVoltage <= 310u )
			{
				Event = 28;
				Flags64 |= 0x200;
				return;
			}

			if ( BoardTemp >= 70u )
			{
				Overtemp();
				return;
			}

			while ( AtoStatus == 4 && AtoProbeCount < 12 )
			{
				ProbeAtomizer();
				WaitOnTMR2(10);
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

			if ( (Flags64 & 0x10000000) && dfMode == 0 )
			{
				Flags64 &= ~0x10000000;

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
							Flags64 |= 0x10000000;
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

			if ( (Flags64 & 0x8000000) && dfMode == 1 )
			{
				Flags64 &= ~0x8000000;

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
							Flags64 |= 0x8000000;
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

			if ( (Flags64 & 0x80000000u) && dfMode == 2 )
			{
				Flags64 &= ~0x80000000u;

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
							Flags64 |= 0x80000000u;
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

			if ( (Flags68 & 2) && dfMode == 3 )
			{
				Flags68 &= ~2u;

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
							Flags68 |= 2;
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

			GPIO_SetMode( PD, 1<<7, 1 );
			PD7 = 0;

			Flags64 |= 0x100u;

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
					TCR = 120;
					break;
				case 4:
					TCR = ((unsigned short *)&dfTCRM1)[dfTCRIndex];
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
					if ( !(Flags64 & 0x800000) )
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
				if ( pwr <= 300 )
					TargetVolts = dfVWVolts;
				else
					TargetVolts = GetVoltsForPower( 300 );

				Flags64 &= ~0x20000000;
				if ( pwr > 600 && BatteryVoltage <= 340 )
				{
					Flags64 |= 0x20000000;
					PowerScale = 60000 / pwr;
				}

				LowBatVolts = ( BatteryVoltage > 380 ) ? 0 : BatteryVoltage;
			}

			if ( ISMODEBY(dfMode) )
			{
				TargetVolts = 450;
			}

			BBCNextMode = 2;
			BBCMode = 0;
			SetADCState( 1, 1 );
			SetADCState( 2, 1 );
			ReachTargetVoltage();
			if ( !(Flags64 & 0x100) || LastInputs != 1 )
				StopFire();
			Flags64 |= 0x20000;
			Screen = 2;
			ScreenDuration = 1;
			return;
		}

//------------------------------------------------------------------------------

		default:
			return;

//------------------------------------------------------------------------------

		case 41:	// Game menu select
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x20000u;
			Screen = 83;
			ScreenDuration = 10;
			return;

		case 40:	// LOGO menu select
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x20000u;
			Screen = 82;
			ScreenDuration = 10;
			return;

		case 39:	// TCR Set menu select
			if ( !(dfStatus & 1) )
				return;
			Flags68 &= ~1u;
			EditTCRIndex = 0;
			Flags64 |= 0x20000u;
			Screen = 59;
			ScreenDuration = 10;
			return;

		case 38:	// Change interface version
			if ( !(dfStatus & 1) )
				return;
			if ( ++dfUIVersion > 2 ) dfUIVersion = 0;
			UpdateDataFlash();
			Flags64 |= 0x20000u;
			Screen = 1;
			ScreenDuration = 3;
			return;

		case 34:	// Show battery voltage
			if ( !(dfStatus & 1) )
				return;
			Flags64 |= 0x20000u;
			Screen = 54;
			ScreenDuration = 5;
			return;

		case 33:	// Ti ON/OFF
			if ( !(dfStatus & 1) )
				return;
			dfTiOn = ( dfTiOn == 0 );
			Flags64 |= 0x20000u;
			Screen = 41;
			ScreenDuration = 3;
			UpdateDFTimer = 50;
			return;

		case 32:	// New coil
			StopFire();
			Flags64 |= 0x20000u;
			Screen = 51;
			ScreenDuration = 10;
			return;

		case 31:	// Show board temperature
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x20000u;
			Screen = 37;
			ScreenDuration = 5;
			return;

		case 30:	// Key lock violation
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x20000u;
			Screen = 28;
			ScreenDuration = 2;
			return;

		case 29:	// FW Version screen
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x20000u;
			Screen = 50;
			ScreenDuration = 5;
			return;

		case 28:	// Battery < 3.1V idle or < 2.8V firing
			StopFire();
			KeyPressTime |= 0x8000u;
			Flags64 |= 0x20000u;
			Screen = 24;
			ScreenDuration = 2;
			return;

		case 27:	// Atomizer Low
			StopFire();
			Flags64 |= 0x20000u;
			Screen = 22;
			ScreenDuration = 1;
			KeyPressTime |= 0x8000u;
			return;

		case 26:	// No Atomizer Found
			StopFire();
			Flags64 |= 0x20000u;
			Screen = 20;
			ScreenDuration = 1;
			return;

		case 25:	// Atomizer short
			Flags64 |= 0x20000u;
			Screen = 21;
			ScreenDuration = 1;
			return;

		case 24:	// 10s Fire protection
			StopFire();
			if ( AtoError )
				return;
			Flags64 |= 0x20000u;
			Screen = 23;
			ScreenDuration = 2;
			return;

		case 23:	// Reset Time counter
			dfTimeCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 0;
			Flags64 |= 0x60000u;
			return;

		case 22:	// Reset Puff counter
			dfPuffCount = 0;
			UpdatePTTimer = 80;
			EditModeTimer = 0;
			Flags64 |= 0x60000u;
			return;

		case 18:	// Flip display
			if ( !(dfStatus & 1) )
				return;
			dfStatus ^= 4;
			InitDisplay();
			Flags64 |= 0x20000u;
			Screen = 1;
			ScreenDuration = 2;
			UpdateDataFlash();
			return;

		case 17:	// Switch On/Off
			if ( Flags64 & 0x100 )
				return;
			if ( dfStatus & 1 )
			{
				Flags64 |= 0x8000u;
				ReadBatteryVoltage();
				if ( BatteryVoltage > 300u || Flags64 & 0x400 )
				{
					dfStatus &= ~1u;
					MainView();
				}
			}
			else
			{
				dfStatus |= 1u;
				Flags64 |= 0x20000u;
				if ( Flags64 & 0x1000 )
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
			if ( dfStatus & 1 )
				return;
			Flags64 |= 0x40000u;
			EditItemIndex = 0;
			EditModeTimer = 1000;
			MainView();
			return;

		case 15:	// Single Fire click
			if ( dfStatus & 1 || Flags64 & 0x100 )
				return;
			if ( Screen == 82 )
				UpdateDataFlash();
			if ( Screen == 83 )
				UpdateDataFlash();
			if ( Flags64 & 0x800 )
			{
				Flags64 &= ~0x800;
				Flags64 |= 0x8000;
				ReadBatteryVoltage();
			}
			if ( EditModeTimer )
			{
				EditModeTimer = 0;
				Flags68 &= ~0x10;
				Flags64 |= 0x40000;
				UpdateDFTimer = 50;
			}
			MainView();
			ScreenDuration = 30;
			return;

		case 13:	// Battery removed
			if ( Screen == 5 || Screen == 4 )
			{
				Flags64 |= 0x20000u;
				Screen = 0;
				if ( dfStatus & 1 )
					SleepTimer = 0;
				else
					SleepTimer = 18000;
			}
			Flags64 &= ~0x1000;
			return;

		case 12:	// Battery inserted
			Flags64 |= 0x21000u;
			if ( dfStatus & 1 )
			{
				Screen = 5;
				BatAnimLevel = BatteryTenth;
				return;
			}
			if ( Screen != 5 )
				Screen = 3;
			ScreenDuration = 30;
			return;

		case 11:	// USB cable detach
			Flags64 &= ~0x1400;
			if ( Screen == 5 )
			{
				if ( dfStatus & 1 )
				{
					Flags64 |= 0x20000;
					Screen = 0;
					return;
				}
				MainView();
			}
			else if ( Screen == 3 )
			{
				MainView();
			}
			return;


		case 10:	// USB cable attach
			Flags64 &= ~0x200;
			Flags64 |= 0x400;
			if ( !(dfStatus & 1) )
				MainView();
			return;

		case 6:		// Stealth On/Off
			if ( dfStatus & 1 )
				return;
			dfStealthOn = ( dfStealthOn == 0 );
			Flags64 |= 0x20000u;
			Screen = 40;
			ScreenDuration = 3;
			return;

		case 4:		// Key (Un)Lock
			if ( dfStatus & 1 )
				return;
			dfStatus ^= 2;
			if ( dfStatus & 2 )
				Screen = 28;
			else
				Screen = 31;
			ScreenDuration = 2;
			Flags64 |= 0x20000u;
			UpdateDFTimer = 50;
			return;

//------------------------------------------------------------------------------

		case 3:		// - (minus or left) button
		{
			if ( dfStatus & 1 )
			{
				if ( Screen == 59 )
				{
					if ( Flags68 & 1 )
					{
						if ( EditTCRIndex >= 3 ) EditTCRIndex = 0;

						v49 = ((unsigned short*)(&dfTCRM1))[EditTCRIndex];
						if ( v49 > 1 )
						{
							v36 = v49 - 1;
							((unsigned short*)(&dfTCRM1))[EditTCRIndex] = v36;
						}
					}
					else
					{
						if ( EditTCRIndex )
							--EditTCRIndex;
						else
							EditTCRIndex = 2;
					}
				}
			}
			else if ( Screen == 82 )
			{
				dfStatus ^= 8;
			}
			else if ( Screen == 83 )
			{
				if ( ++dfFBSpeed > 3u ) dfFBSpeed = 0;
			}
			else if ( Screen == 51 )
			{
				switch ( dfMode )
				{
					case 0:
						dfResistance = dfRezNI;
						Flags64 &= ~0x10000000;
						break;
					case 1:
						dfResistance = dfRezTI;
						Flags64 &= ~0x8000000;
						break;
					case 2:
						dfResistance = dfRezSS;
						Flags64 &= ~0x80000000;
						break;
					case 3:
						dfResistance = dfRezTCR;
						Flags68 &= ~0x2;
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

					if ( EditItemIndex == 2 && Flags68 & 0x10 )
					{
						if ( dfTCPower <= 10u )
						{
							dfTCPower = MaxTCPower;
						}
						else if ( word_20000054 < 105u )
						{
							--dfTCPower;
						}
						else
						{
							if ( dfTCPower % 10u )
							{
								dfTCPower -= dfTCPower % 10u;
							}
							else
							{
								dfTCPower -= 10;
							}
						}
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
						if ( ++EditItemIndex > 4u )
							EditItemIndex = 0;
					}
				}
				else
				{
					v53 = AtoMinPower;
					v54 = AtoMinPower;

					if ( ISMODETC(dfMode) )
					{
						if ( dfIsCelsius )
						{
							v61 = dfTemp - 5;
							if ( dfTemp - 5 < 100 )
							{
								dfIsCelsius = 0;
								v61 = 600;
							}
						}
						else
						{
							v61 = dfTemp - 10;
							if ( dfTemp - 10 < 200 )
							{
								dfIsCelsius = 1;
								v61 = 315;
							}
						}
						dfTemp = v61;
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
								dfSavedCfgPwr[ConfigIndex] = v54;
							else
								dfSavedCfgPwr[ConfigIndex] = v60;

							if ( dfSavedCfgPwr[ConfigIndex] <= v53 )
								dfSavedCfgPwr[ConfigIndex] = v54;
						}
						v57 = dfSavedCfgPwr[ConfigIndex];
						dfVWVolts = GetAtoVWVolts(v57);
					}
					else if ( dfMode == 4 )
					{
						if ( word_20000054 < 105u )
						{
							if ( dfPower ) --dfPower;
						}
						else
						{
							if ( dfPower % 10u )
								dfPower -= dfPower % 10u;
							else
								if ( dfPower >= 10 ) dfPower -= 10;
						}

						if ( dfPower <= AtoMinPower )
							dfPower = AtoMinPower;

						dfVWVolts = GetAtoVWVolts( dfPower );
					}
				}

	            MainView();
	            if ( word_20000054 >= 5 )
	            {
					Flags64 |= 0x40000u;
	                DrawScreen();
	            }
	            UpdateDFTimer = 50;
	            return;
			}

			Flags64 |= 0x20000u;
			ScreenDuration = 10;
			return;
		}

//------------------------------------------------------------------------------

		case 2:		// + (plus or right) button
		{
			if ( dfStatus & 1 )
			{
				if ( Screen == 59 )
				{
					if ( Flags68 & 0x1 )
					{
						if ( EditTCRIndex > 2u ) EditTCRIndex = 0;

						v35 = ((unsigned short*)(&dfTCRM1))[EditTCRIndex];
						if ( ++v35 < 1000 )
						{
							((unsigned short*)(&dfTCRM1))[EditTCRIndex] = v35;
						}
					}
					else
					{
						++EditTCRIndex;
						if ( ( EditTCRIndex ) > 2u ) EditTCRIndex = 0;
					}
					Flags64 |= 0x20000u;
					ScreenDuration = 10;
				}
			}
			else if (( Screen >= 80 ) && ( Screen < 100u ))
			{
				if ( ++MenuPage > 2u )
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
						Flags64 &= ~0x10000000;
						break;
					case 1:
						dfRezTI = word_200000C0;
						Flags64 &= ~0x8000000;
						break;
					case 2:
						dfRezSS = word_200000C0;
						Flags64 &= ~0x80000000;
						break;
					case 3:
						dfRezTCR = word_200000C0;
						Flags68 &= ~0x2;
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
								if ( ++dfTCRIndex >= 3u )
									dfTCRIndex = 0;
							}
							else if ( dfMode == 0 || dfMode == 1 || dfMode == 2 )
							{
								++dfLastTCMode;
								if ( dfLastTCMode > 2u )
									dfLastTCMode = 0;
								dfMode = dfLastTCMode;
								ModeChange();
							}
							break;
						
						case 2:
							if ( dfTCPower >= MaxTCPower )
							{
								dfTCPower = 10;
							}
							else if ( word_20000054 < 105u )
							{
								++dfTCPower;
							}
							else
							{
								if ( dfTCPower % 10u )
								{
									dfTCPower -= dfTCPower % 10u;
								}
								dfTCPower += 10;
							}
							Flags68 |= 0x10u;
							break;

						case 3:
							SwitchRezLock();
							break;
						
						case 4:
							if ( ++dfAPT > 6 ) dfAPT = 0;
							break;
					}

				}
				else
				{
					v41 = AtoMaxPower;
					v42 = AtoMaxPower;
		
					switch ( dfMode )
					{
						case 0:
						case 1:
						case 2:
						case 3:
							if ( dfIsCelsius )
							{
								v48 = dfTemp + 5;
								if ( dfTemp + 5 >= 316 )
								{
									dfIsCelsius = 0;
									v48 = 200;
								}
							}
							else
							{
								v48 = dfTemp + 10;
								if ( dfTemp + 10 > 600 )
								{
									dfIsCelsius = 1;
									v48 = 100;
								}
							}
							dfTemp = v48;
							break;

						case 4:
							if ( word_20000054 < 105u )
							{
								v43 = dfPower + 1;
							}
							else
							{
								if ( dfPower % 10u )
									dfPower = 10 * (dfPower / 10u);
								v43 = dfPower + 10;
							}
							dfPower = v43;
							if ( v43 > AtoMaxPower )
								dfPower = AtoMaxPower;
							dfVWVolts = GetAtoVWVolts( dfPower );
							break;

						case 6:
							if ( ConfigIndex < 10u && !AtoError && AtoRez )
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
								if ( v47 > v41 )
									dfSavedCfgPwr[ConfigIndex] = v42;
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
	            if ( word_20000054 >= 5 )
	            {
					Flags64 |= 0x40000u;
	                DrawScreen();
	            }
	            UpdateDFTimer = 50;
	            return;
			}
		}
	}
}
