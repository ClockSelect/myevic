#include "myevic.h"
#include "screens.h"
#include "display.h"
#include "menus.h"
#include "myrtc.h"
#include "dataflash.h"
#include "atomizer.h"
#include "timers.h"
#include "battery.h"
#include "miscs.h"
#include "events.h"


//=========================================================================

uint8_t		Screen;
uint16_t	ScreenDuration;
uint16_t	ScreenRefreshTimer;

const uint8_t ScrSaveTimes[8] = { 1, 2, 5, 10, 15, 20, 30, 0 };
const uint8_t ScrMainTimes[5] = { 30, 60, 5, 10, 20 };

uint8_t		EditItemIndex;
uint16_t	EditModeTimer;
uint8_t		EditTCRIndex;

uint8_t		ShowWeakBatFlag;
uint8_t		BatAnimLevel;


//=========================================================================
// Called at a frequency of 10Hz except when firing in TC modes.
// Called at a frequency of 2Hz when firing in TC modes.

__myevic__ void DrawScreen()
{
	static uint8_t	ShowFDTimer = 0;
	static uint16_t	CurrentFD = 0;

	if ( Screen == 2 && FireDuration && FireDuration != CurrentFD )
	{
		CurrentFD = FireDuration;
		ScreenDuration = 1;
		ShowFDTimer = 0;
		gFlags.refresh_display = 1;
	}
	else
	{
		if ( ScreenRefreshTimer && !--ScreenRefreshTimer )
			gFlags.refresh_display = 1;
	}

	if ( gFlags.refresh_display )
	{
		gFlags.refresh_display = 0;
		ClearScreenBuffer();

		switch ( Screen )
		{
			case  0: // Black
				break;

			case  1: // Main view
			case  3: // Main view (?)
			case  4: // (unused?)
				ShowMainView();
				break;

			case  2: // Firing
				if ( !dfStealthOn )
				{
					ShowMainView();
				}
				break;

			case  5: // Black w/ Battery
				ShowBatCharging();
				break;

			case 20: // No Atomizer Found
				ShowNoAtoFound();
				break;

			case 21: // Atomizer Short
				ShowAtoShort();
				break;

				case 22: // Atomizer Low
				ShowAtoLow();
				break;

			case 23: // 10s Protection
				Show10sProtec();
				break;

			case 24: // Battery Low
				ShowBatLow();
				break;

			case 25: // Battery Low Lock
				ShowBatLowLock();
				break;

			case 28: // Key Lock
				ShowKeyLock();
				break;

			case 29: // Device too hot
				ShowDevTooHot();
				break;

			case 31: // Key UnLock
				ShowKeyUnLock();
				break;

			case 37: // Board Temp
				ShowBoardTemp();
				break;

			case 40: // Stealth ON/OFF
				ShowStealthMode();
				break;

			case 41: // Ti ON/OFF
				ShowTiOnOff();
				break;

			case 50: // FW Version
				ShowVersion();
				break;

			case 51: // New Coil
				ShowNewCoil();
				break;

			case 54: // Battery Voltage
				ShowBattVolts();
				break;

			case 59: // TCR Set Menu
				ShowTCRSet();
				break;

			case 60: // Screen Saver
				ShowScreenSaver();
				break;
				
			case 82: // LOGO Menu
				ShowLOGOMenu();
				break;

			case 83: // Game Menu
				ShowGameMenu();
				break;

			case 100:
				ShowInfos();
				break;

			case 101:
				ShowContrast();
				break;

			case 102:
				ShowMenus();
				break;

			case 103:
				ShowRTCSpeed();
				break;

			case 104:
				ShowRTCAdjust();
				break;

			case 105:
				ShowSetTime();
				break;

			case 106:
				ShowSetDate();
				break;

			default:
				break;
		}

		if ( gFlags.debug & 1 )
		{
			int nd = (Screen<100?Screen<10?1:2:3);
			DrawValue( 64-6*nd, 120, Screen, 0, 0x01, nd );
			DrawValue( 0, 120, ScreenDuration, 0, 0x01, 0 );
		}

		DisplayRefresh();
	}

	if (( gFlags.firing ) && ISMODETC(dfMode))
	{
		ShowFDTimer += 5;
	}
	else
	{
		ShowFDTimer += 1;
	}

	if ( ShowFDTimer < 10 )
		return;

	ShowFDTimer = 0;

	if (  10 * ScreenDuration < EditModeTimer )
	{
		ScreenDuration = EditModeTimer / 10 + 1;
	}

	if ( ScreenDuration && --ScreenDuration )
		return;

	switch ( Screen )
	{
		case   0: // Black
			if ( dfStatus.off )
			{
				SleepTimer = 0;
			}
			else
			{
				if (( !gFlags.firing )
				&&	( !dfStealthOn )
				&&	( SleepTimer > 0 )
				&&	( dfScreenSaver > 0 )
				&&	( GetScreenProtection() > 0 ))
				{
					Screen = 60;
					ScreenDuration = GetScreenProtection();
					gFlags.refresh_display = 1;
				}
			}
			break;

		case   2: // Firing
		case  28: // Key Lock
		case  40: // Stealth ON/OFF
			if ( dfStealthOn )
			{
				gFlags.refresh_display = 1;
				if ( !(gFlags.battery_charging) )
				{
					Screen = 0;
					SleepTimer = 18000;
				}
				else
				{
					Screen = 5;
				}
			}
			else
			{
				MainView();
			}
			break;

		case   5: // Black w/ Battery
		case  22: // Atomizer Low
		case  23: // 10s Protection
		case  24: // Battery Low
		case  25: // Battery Low Lock
		case  50: // FW Version
			break;

		case  20: // No Atomizer Found
		case  21: // Atomizer Short
		case  29: // Device too hot
		case  31: // Key UnLock
		case  51: // New Coil
			MainView();
			break;

		case 101: // Contrast Menu
		case 102: // Menus
		case 103: // RTC Speed
		case 104: // Adjust Clock
		case 105: // Set Time
		case 106: // Set Date
			gFlags.edit_capture_evt = 0;
			// NOBREAK
		case  59: // TCR Set Menu
		case  82: // LOGO Menu
		case  83: // Game Menu
			UpdateDataFlash();
			// NOBREAK
		case   1: // Main view
		case  37: // Board Temp
		case  41: // Ti ON/OFF
		case  54: // Battery Voltage
		case 100: // Infos page
			if ( !(gFlags.battery_charging) )
			{
				Screen = 0;
				SleepTimer = 18000;
			}
			else
			{
				Screen = 5;
			}
			gFlags.refresh_display = 1;
			break;

		case  60: // Screen Saver
			Screen = 0;
			ScreenDuration = 0;
			SleepTimer = 0;
			gFlags.refresh_display = 1;
			break;

		default:
			break;
	}

	return;
}

//=========================================================================

__myevic__ uint16_t GetScreenProtection()
{
	return ( 60 * ScrSaveTimes[dfScreenProt] );
}

__myevic__ uint16_t GetMainScreenDuration()
{
	return dfDimTimeout ? : ScrMainTimes[dfScrMainTime];
}


//=========================================================================

__myevic__ int convert_string1( uint16_t *strbuf, const char *s )
{
	int i;
	char c;

	i = 0;
	while (( c = *s++ ) && ( i < 20 ))
	{
		if (( c >= '0' ) && ( c <= '9' ))
		{
			strbuf[i++] = c - '0' + 0x0B;
		}
		else if (( c >= 'A' ) && ( c <= 'Z' ))
		{
			strbuf[i++] = c - 'A' + 0x9C;
		}
		else if (( c >= 'a' ) && ( c <= 'z' ))
		{
			strbuf[i++] = c - 'a' + 0x82;
		}
		else
		{
			strbuf[i++] = 0xBC;
		}
	}
	strbuf[i] = 0;
	return i;
}

//=========================================================================

__myevic__ void ShowInfos()
{
	uint16_t strbuf[20];

	// TODO : infos page
	convert_string1( strbuf, "Ferox" );
	DrawStringCentered( strbuf, 82 );
	convert_string1( strbuf, "was" );
	DrawStringCentered( strbuf, 92 );
	convert_string1( strbuf, "here" );
	DrawStringCentered( strbuf, 102 );

	return;
}

//=========================================================================

__myevic__ void ShowContrast()
{
	int pc, nd, x;

	DrawString( String_Contrast, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	pc = ( ( 100 * dfContrast ) / 255 );
	nd = ( pc < 100 ? pc < 10 ? 1 : 2 : 3 );
	x = ( 64 - ( 6 * nd + 9 )) / 2;
	DrawValue( x, 20, pc, 0, 0x0B, 0 );
	DrawImage( x + 6 * nd, 20, 0xC2 );

	DrawFillRect( 0, 32, 63, 44, 1 );
	DrawFillRect( 1, 33, 62, 43, 0 );
	if ( dfContrast )
	{
		DrawFillRect( 2, 34, 2 + ( ( 59 * dfContrast ) / 255 ), 42, 1 );
	}

	DrawStringCentered( String_Fireto, 57 );
	DrawStringCentered( ( gFlags.edit_capture_evt ) ? String_Exit : String_Edit, 67 );

	DrawLOGO( 0, 88 );
}


//=========================================================================

__myevic__ void ShowMenus()
{
	DrawMenu();
}


//=========================================================================

__myevic__ void ShowRTCSpeed()
{
	unsigned int cs;
	S_RTC_TIME_DATA_T rtd;

	DrawString( String_ClkSpeed, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	GetRTC( &rtd );
	DrawTimeSmall( 12, 20, &rtd, 0x1F );

	if ( gFlags.has_x32 )
	{
		DrawString( String_X32, 11, 40 );
		DrawString( String_ON, 37, 40 );
	}
	else
	{
		cs = RTCGetClockSpeed();
		DrawValue( 12, 40, cs, 0, 0x1F, 5 );
	}
}


//=========================================================================

__myevic__ int IsClockOnScreen()
{
	return (  ((( Screen == 1 ) || ( Screen == 2 )) && ( dfAPT == 6 ))
			|| (( Screen == 1 ) && ( dfStatus.clock ))
			|| (( Screen == 60 ) && ( dfScreenSaver == 1 ))
			||  ( Screen == 103 )
			||  ( Screen == 104 )
			);
}


//=========================================================================
//----- (000067C8) --------------------------------------------------------
__myevic__ void ShowBattery()
{
	if ( dfStatus.battpc )
	{
		DrawValue(	BatteryPercent < 10  ? 12 :
					BatteryPercent < 100 ? 6 :
					0, 118, BatteryTenth * 10, 0, 0x0B, 0 );
		DrawImage( 19, 118, 0xC2 );
	}

	if ( gFlags.battery_10pc && !(gFlags.battery_charging) )
	{
		if ( gFlags.draw_battery )
		{
			if ( dfStatus.battpc )
			{
				DrawImage( 30, 114, 0xE2 );
			}
			else
			{
				DrawImage( 8, 115, 0xC4 );
			}
		}
	}
	else if ( gFlags.draw_battery_charging && gFlags.battery_charging )
	{
		if ( dfStatus.battpc )
		{
			DrawImage( 30, 114, 0xE3 );
		}
		else
		{
			DrawImage( 8, 115, 0xC5 );
		}
	}
	else
	{
		if ( dfStatus.battpc )
		{
			DrawImage( 30, 114, 0xE2 );
			if ( BatteryTenth )
			{
				DrawFillRect( 32, 118, (25 * BatteryTenth / 10 + 31), 124, 1 );
			}
		}
		else
		{
			DrawImage( 8, 115, 0xC4 );
			if ( BatteryTenth )
			{
				DrawFillRect( 10, 118, (4 * BatteryTenth + 9), 124, 1 );
			}
		}
	}
}


//=========================================================================
//----- (00006764) --------------------------------------------------------
__myevic__ void ShowBatCharging()
{
	ShowScreenSaver();

	if ( dfStatus.battpc )
	{
		DrawValue(	BatteryPercent < 10  ? 12 :
					BatteryPercent < 100 ? 6 :
					0, 118, BatteryTenth * 10, 0, 0x0B, 0 );
		DrawImage( 19, 118, 0xC2 );
		DrawImage( 30, 114, 0xE2 );
	}
	else
	{
		DrawImage( 8, 115, 0xC4 );
	}

	if ( BatteryTenth != 10 )
	{
		if ( BatAnimLevel )
		{
			if ( dfStatus.battpc )
			{
				DrawFillRect( 32, 118, (25 * BatAnimLevel / 10 + 31), 124, 1 );
			}
			else
			{
				DrawFillRect( 10, 118, (4 * BatAnimLevel + 9), 124, 1 );
			}
		}
	}
	else if ( gFlags.draw_battery_charging )
	{
		if ( dfStatus.battpc )
		{
			DrawFillRect( 32, 118, 56, 124, 1 );
		}
		else
		{
			DrawFillRect( 10, 118, 49, 124, 1 );
		}
	}

	DrawValue( 6, 105, BatteryVoltage, 2, 0x0B, 3 );
	DrawImage( 27, 106, 0x7D );

	int t = dfIsCelsius ? BoardTemp : CelsiusToF( BoardTemp );

	DrawValue( 38, 105, t, 0, 0x0B, t > 99 ? 3 : 2 );
	DrawImage( t > 99 ? 57 : 51, 104, dfIsCelsius ? 0x6A : 0x6D );
}


//=========================================================================
//----- (0000683C) --------------------------------------------------------
__myevic__ void ShowBattVolts()
{
	DrawStringCentered( String_Battery, 88 );
	DrawValue( 6, 102, BatteryVoltage, 2, 0x29, 3 );
	DrawImage( 46, 102, 0xB8 );
}


//=========================================================================
//----- (00006874) --------------------------------------------------------
__myevic__ void ShowBoardTemp()
{
	DrawStringCentered( String_Temp, 88 );
	DrawValue( 16, 102, BoardTemp, 0, 0x52, 2 );
}


//=========================================================================
//----- (00007684) --------------------------------------------------------
__myevic__ void ShowVersion()
{
	DrawStringCentered( String_Version, 88 );
	DrawValue( 6, 102, FWVERSION, 2, 0x52, 3 );
}


//=========================================================================
//----- (000068D4) --------------------------------------------------------
__myevic__ void ShowNewCoil()
{
	int rez;

	DrawStringCentered( String_NewCoil, 50 );
	DrawValue( 16, 62, dfResistance, 2, 0x0B, 3 );
	DrawImage( 40, 62, 0xC0 );
	DrawStringCentered( String_Right, 74 );
	DrawStringCentered( String_SameCoil, 90 );

	if ( ISMODETC( dfMode ) )
	{
		if ( dfMode == 0 )
		{
			rez = dfRezNI;
		}
		else if ( dfMode == 1 )
		{
			rez = dfRezTI;
		}
		else if ( dfMode == 2 )
		{
			rez = dfRezSS;
		}
		else if ( dfMode == 3 )
		{
			rez = dfRezTCR;
		}
		DrawValue( 16, 102, rez, 2, 11, 3 );
	}

	DrawImage( 40, 102, 0xC0 );
	DrawStringCentered( String_Left, 114 );
}


//=========================================================================
//----- (000071A4) --------------------------------------------------------
__myevic__ void ShowLOGOMenu()
{
	int l;

	DrawString( String_Logo, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	l = dfStatus.nologo;

	DrawFillRect( 0, 14 * l + 18, 63, 14 * l + 30, 1 );

	if ( l )
	{
		DrawString( String_On, 4, 20 );
		DrawStringInv( String_Off, 4, 34 );
	}
	else
	{
		DrawStringInv( String_On, 4, 20 );
		DrawString( String_Off, 4, 34 );
	}
}


//=========================================================================
//----- (00007234) --------------------------------------------------------
__myevic__ void ShowGameMenu()
{
	int line;
	const uint16_t *str;

	DrawString( String_Game, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	DrawString( String_Easy, 4, 20 );
	DrawString( String_Normal, 4, 34 );
	DrawString( String_Hard, 4, 48 );
	DrawString( String_Exit, 4, 62 );

	DrawFillRect( 0, ( 18 + 14 * dfFBSpeed ), 63, ( 30 + 14 * dfFBSpeed ), 1);

	line = 20 + 14 * dfFBSpeed;

	if ( dfFBSpeed == 0 )
	{
		str = String_Easy;
	}
	else if ( dfFBSpeed == 1 )
	{
		str = String_Normal;
	}
	else if ( dfFBSpeed == 2 )
	{
		str = String_Hard;
	}
	else if ( dfFBSpeed == 3 )
	{
		str = String_Exit;
	}
	else return;

	DrawStringInv( str, 4, line );
}


//=========================================================================
//----- (000072EC) --------------------------------------------------------
__myevic__ void ShowTCRSet()
{
	int i;
	int line;

	DrawString( String_TCRSet, 7, 6 );
	DrawHLine( 0, 22, 63, 1 );

	if ( gFlags.edit_value )
	{
		for ( i = 0 ; i < 3 ; ++i )
		{
			if ( EditTCRIndex == i )
			{
				DrawFillRect( 28, 25 * i + 30, 62, 25 * i + 52, 1 );
				DrawValueInv(30, 25 * i + 36, dfTCRM[i], 0, 0x1F, 4);
			}
			else
			{
				DrawValue( 30, 25 * i + 36, dfTCRM[i], 0, 0x1F, 4 );
			}
			line = 25 * i + 33;
			DrawImage(  0, line, 0xED );
			DrawImage( 15, line, 0xEE + i );
		}
	}
	else
	{
		for ( i = 0 ; i < 3 ; ++i )
		{
			line = 25 * i + 33;

			if ( EditTCRIndex == i )
			{
				DrawImageInv(  0, line, 0xED );
				DrawImageInv( 15, line, 0xEE + i );
				DrawFillRect( 25, line, 30, line + 16, 0);
			}
			else
			{
				DrawImage(  0, line, 0xED );
				DrawImage( 15, line, 0xEE + i );
			}
			DrawValue( 30, 25 * i + 36, dfTCRM[i], 0, 0x1F, 4 );
		}
	}
}


//=========================================================================
//----- (000076AC) --------------------------------------------------------
__myevic__ void ShowStealthMode()
{
	DrawStringCentered( String_Stealth, 88 );
	DrawStringCentered( dfStealthOn ? String_ON : String_OFF, 102 );
}


//=========================================================================
//----- (000076DC) --------------------------------------------------------
__myevic__ void ShowDevTooHot()
{
	DrawStringCentered( String_Device, 88 );
	DrawStringCentered( String_TooHot, 102 );
}


//=========================================================================
//----- (000076F8) --------------------------------------------------------
__myevic__ void ShowTiOnOff()
{
	DrawStringCentered( dfTiOn ? String_TiON : String_TiOFF, 88 );
}


//=========================================================================
//----- (00007718) --------------------------------------------------------
__myevic__ void ShowAtoLow()
{
	DrawStringCentered( String_Atomizer, 88 );
	DrawStringCentered( String_Low, 102 );
}


//=========================================================================
//----- (00007734) --------------------------------------------------------
__myevic__ void ShowAtoShort()
{
	DrawStringCentered( String_Atomizer, 88 );
	DrawStringCentered( String_Short, 102 );
}


//=========================================================================
//----- (00007750) --------------------------------------------------------
__myevic__ void ShowBatLow()
{
	DrawStringCentered( String_Battery, 88 );
	DrawStringCentered( String_Low, 102 );
}


//=========================================================================
//----- (0000776C) --------------------------------------------------------
__myevic__ void ShowBatLowLock()
{
	DrawStringCentered( String_Battery, 82 );
	DrawStringCentered( String_Low, 92 );
	DrawStringCentered( String_Lock, 102 );
}


//=========================================================================
//----- (00007794) --------------------------------------------------------
__myevic__ void ShowKeyLock()
{
	DrawStringCentered( String_Key, 88 );
	DrawStringCentered( String_Lock, 102 );
}


//=========================================================================
//----- (000077B0) --------------------------------------------------------
__myevic__ void ShowKeyUnLock()
{
	DrawStringCentered( String_Key, 88 );
	DrawStringCentered( String_UnLock, 102 );
}


//=========================================================================
//----- (000077CC) --------------------------------------------------------
__myevic__ void ShowNoAtoFound()
{
	DrawStringCentered( String_No, 82 );
	DrawStringCentered( String_Atomizer, 92 );
	DrawStringCentered( String_Found, 102 );
}


//=========================================================================
//----- (000077F4) --------------------------------------------------------
__myevic__ void Show10sProtec()
{
	DrawStringCentered( String_Over10s, 88 );
	DrawStringCentered( String_Protec, 102 );
}


//=========================================================================
//----- (00007810) --------------------------------------------------------
__myevic__ void ShowWeakBat()
{
	DrawStringCentered( String_Weak, 106 );
	DrawStringCentered( String_Battery, 118 );
}


//=========================================================================
__myevic__ void ShowRTCAdjust()
{
	S_RTC_TIME_DATA_T rtd;

	DrawString( String_ClkAdjust, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	GetRTC( &rtd );
	DrawTime( 5, 40, &rtd, 0x1F );
}


//=========================================================================
__myevic__ void ShowScreenSaver()
{
	switch ( dfScreenSaver )
	{
		case 1:
			if ( dfStatus.digclk )
			{
				DrawDigitClock( 40 );
			}
			else
			{
				DrawClock( 25 );
			}
			break;

		case 2:
			anim3d( 1 );
			break;

		case 3:
			DrawLOGO( 0, 32 );
			break;

		case 4:
			qix( 1 );
			break;

		default:
			break;
	}
}


//=========================================================================
__myevic__ void AnimateScreenSaver()
{
	switch ( dfScreenSaver )
	{
		case 2:
			anim3d( 0 );
			break;

		case 4:
			qix( 0 );
			break;

		default:
			break;
	}
}


//=========================================================================
__myevic__ void ShowSetTime()
{
	DrawString( String_SetTime, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	DrawTime( 6, 46, &SetTimeRTD, 0x1F & ~( 1 << ( EditItemIndex << 1 ) ) );
	DrawDate( 4, 64, &SetTimeRTD, 0x1F );
}


//=========================================================================
__myevic__ void ShowSetDate()
{
	S_RTC_TIME_DATA_T rtd;

	GetRTC( &rtd );

	DrawString( String_SetDate, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	DrawTime( 6, 46, &rtd, 0x1F );
	DrawDate( 4, 64, &SetTimeRTD, 0x1F & ~( 1 << ( EditItemIndex << 1 ) ) );
}
