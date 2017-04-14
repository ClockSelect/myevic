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
const uint8_t ScrMainTimes[6] = { 30, 60, 5, 10, 15, 20 };

uint8_t		EditItemIndex;
uint16_t	EditModeTimer;

uint8_t		ShowWeakBatFlag;
uint8_t		BatAnimLevel;


//=========================================================================
// Change Screen
void SetScreen( int screen, int duration )
{
	Screen = screen;
	ScreenDuration = duration;
	gFlags.refresh_display = 1;
}


//=========================================================================
// Called at a frequency of 10Hz except when firing in TC modes.
// Called at a frequency of 2Hz when firing in TC modes.

__myevic__ void DrawScreen()
{
	static uint8_t	TenthOfSecs = 0;
	static uint16_t	CurrentFD = 0;

	if ( Screen == 2 && FireDuration && FireDuration != CurrentFD )
	{
		CurrentFD = FireDuration;
		ScreenDuration = ISMODETC(dfMode) ? 1 : 3;
		TenthOfSecs = 0;
		gFlags.refresh_display = 1;
	}
	else if ( ScreenRefreshTimer && !--ScreenRefreshTimer )
	{
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

			case 50: // FW Version
				ShowVersion();
				break;

			case 51: // New Coil
				ShowNewCoil();
				break;

			case 54: // Battery Voltage
				ShowBattVolts();
				break;

			case 55: // Imbalanced Batteries
				ShowImbBatts();
				break;

			case 56: // Check Battery
				ShowCheckBattery();
				break;

			case 57: // Check USB Adapter
				ShowCheckUSB();
				break;

			case 58: // Charge Error
				ShowChargeError();
				break;

			case 60: // Screen Saver
				ShowScreenSaver();
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

			case 107:
				ShowPowerCurve();
				break;
			
			default:
				break;
		}

		if ( gFlags.debug & 1 )
		{
			DrawValueRight( 64, 120, Screen, 0, 0x01, 0 );
			DrawValue( 0, 120, ScreenDuration, 0, 0x01, 0 );
		}

		DisplayRefresh();
	}

	if ( ( Screen == 1 || Screen == 60 ) && ( ScreenDuration <= 4 ) )
	{
		if ( !gFlags.fading  )
		{
			FadeOutTimer = 300;
			gFlags.fading = 1;
		}
	}
	else if ( gFlags.fading )
	{
		FadeOutTimer = 0;
		DisplaySetContrast( dfContrast );
		gFlags.fading = 0;
	}

	if (( gFlags.firing ) && ISMODETC(dfMode))
		TenthOfSecs += 5;
	else
		TenthOfSecs += 1;

	if ( TenthOfSecs < 10 )
		return;

	TenthOfSecs = 0;

	if (  100 * ScreenDuration < EditModeTimer )
		ScreenDuration = EditModeTimer / 100 + 1;

	if ( ScreenDuration && --ScreenDuration )
		return;

	switch ( Screen )
	{
		case   0: // Black
			if ( dfStatus.off )
				SleepTimer = 0;
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
				Screen = 0;
				SleepTimer = 18000;
				gFlags.refresh_display = 1;
			}
			else
			{
				MainView();
			}
			break;

		case   5: // Black w/ Battery
			break;

		case  22: // Atomizer Low
		case  24: // Battery Low
		case  25: // Battery Low Lock
		case  50: // FW Version
			break;

		case  23: // 10s Protection
			if ( !dfStatus.off )
			{
				// Switch box off after 10s if no response
				// have been given to a long fire.
				if ( !PE0 && gFlags.user_idle )
				{
					Event = 17;
				}
				else
				{
					Screen = 0;
					SleepTimer = 18000;
					gFlags.refresh_display = 1;
				}
			}
			break;

		case  20: // No Atomizer Found
		case  21: // Atomizer Short
		case  29: // Device too hot
		case  31: // Key UnLock
		case  51: // New Coil
		case  55: // Imbalanced Batteries
		case  56: // Check Battery
		case  57: // Check USB Adapter
		case  58: // Charge Error
			MainView();
			break;

		case 101: // Contrast Menu
		case 102: // Menus
		case 103: // RTC Speed
		case 104: // Adjust Clock
		case 105: // Set Time
		case 106: // Set Date
		case 107: // Power Curve
			EditModeTimer = 0;
			gFlags.edit_capture_evt = 0;
			gFlags.edit_value = 0;
			LEDOff();
			UpdateDataFlash();
			// NOBREAK
		case   1: // Main view
		case  37: // Board Temp
		case  54: // Battery Voltage
		case 100: // Infos page
			if ( gFlags.battery_charging )
			{
				ChargeView();

				if ( dfStealthOn )
				{
					ScreenDuration = 0;
				}
			}
			else
			{
				Screen = 0;
				SleepTimer = 18000;
				gFlags.refresh_display = 1;
			}
			break;

		case  60: // Screen Saver
			if ( gFlags.battery_charging )
			{
				ChargeView();

				if ( dfStealthOn )
				{
					ScreenDuration = 0;
				}
			}
			else
			{
				Screen = 0;
				SleepTimer = 0;
				gFlags.refresh_display = 1;
			}
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

__myevic__ int convert_string1( uint8_t *strbuf, const char *s )
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

__myevic__ void ChargeView()
{
	Screen = 5;
	gFlags.refresh_display = 1;
	ScreenDuration = 5;
}


//=========================================================================

__myevic__ void ShowInfos()
{
	uint8_t strbuf[20];

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
	DrawStringCentered( String_Exit, 67 );

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
	return (  ((( Screen == 1 ) || ( Screen == 2 )) && ( dfAPT == 8 ))
			|| (( Screen == 1 ) && ( dfStatus.clock ))
			|| (( Screen == 60 ) && ( dfScreenSaver == SSAVER_CLOCK ))
			||  ( Screen == 103 )
			||  ( Screen == 104 )
			);
}


//=========================================================================
//----- (000067C8) --------------------------------------------------------
__myevic__ void ShowBattery()
{
	if ( BLINKITEM(5) ) return;

	if ( dfStatus.battpc )
	{
		if ( dfStatus.battv )
		{
			uint16_t bv = gFlags.firing ? RTBattVolts : BatteryVoltage;
			DrawValueRight(	20, 118, bv, 2, 0x0B, 0 );
			DrawImage( 21, 118, 0x7D );
		}
		else
		{
			DrawValueRight(	18, 118, BatteryPercent, 0, 0x0B, 0 );
			DrawImage( 19, 118, 0xC2 );
		}
	}

	if ( gFlags.battery_10pc && !gFlags.battery_charging )
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
			if ( gFlags.batteries_ooe && gFlags.draw_battery )
			{
				DrawString( String_BAL_s, 35, 117 );
			}
			else if ( BatteryTenth )
			{
				DrawFillRect( 32, 118, (25 * BatteryTenth / 10 + 31), 124, 1 );
			}
		}
		else
		{
			DrawImage( 8, 115, 0xC4 );
			if ( gFlags.batteries_ooe && gFlags.draw_battery )
			{
				DrawString( String_BALANCE_s, 10, 117 );
			}
			else if ( BatteryTenth )
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
	if ( dfStealthOn && ScreenDuration == 0 )
	{
		return;
	}

	switch ( dfScreenSaver )
	{
		case SSAVER_CLOCK:
		case SSAVER_LOGO:
			ShowScreenSaver();
			break;

		default:
			break;
	}

	if ( dfStatus.battpc )
	{
		DrawValueRight(	18, 118, BatteryPercent, 0, 0x0B, 0 );
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

	if (( dfScreenSaver == SSAVER_CLOCK ) || ( dfScreenSaver == SSAVER_LOGO ))
	{
		DrawValue(  6, 104, BatteryVoltage, 2, 0x0B, 3 );
		DrawImage( 27, 104, 0x7D );
	}
	else
	{
		for ( int i = 0 ; i < NumBatteries ; ++i )
		{
			DrawValue(  6, 104 - i * 12, BattVolts[i], 2, 0x0B, 3 );
			DrawImage( 27, 104 - i * 12, 0x7D );
		}
	}

	int t = dfIsCelsius ? BoardTemp : CelsiusToF( BoardTemp );

	DrawValueRight( 52, 104, t, 0, 0x0B, 0 );
	DrawImage( 54, 104, dfIsCelsius ? 0xC9 : 0xC8 );
}


//=========================================================================
//----- (0000683C) --------------------------------------------------------
__myevic__ void ShowBattVolts()
{
	if ( NumBatteries > 1 )
	{
		DrawStringCentered( String_Batteries, 32 );
		for ( int i = 0 ; i < NumBatteries ; ++i )
		{
			DrawValue(  6, 44+20*i, BattVolts[i], 2, 0x29, 3 );
			DrawImage( 46, 44+20*i, 0xB8 );
		}
	}
	else
	{
		DrawStringCentered( String_Battery, 88 );
		DrawValue( 6, 102, BatteryVoltage, 2, 0x29, 3 );
		DrawImage( 46, 102, 0xB8 );
	}
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
	uint8_t buf[12];

	DrawStringCentered( String_myevic, 32 );

	DrawStringCentered( String_Build, 55 );
	Value2Str( buf, __BUILD1, 0, 0x1F, 0 );
	DrawStringCentered( buf, 69 );

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
		DrawValue( 16, 102, rez, 2, 0x0B, 3 );
	}

	DrawImage( 40, 102, 0xC0 );
	DrawStringCentered( String_Left, 114 );
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
	ShowFireDuration( 49 );
	DrawStringCentered( String_LongFire, 88 );
	DrawStringCentered( String_Protection, 102 );
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
		case SSAVER_CLOCK:
			if ( dfStatus.digclk )
			{
				DrawDigitClock( 40 );
			}
			else
			{
				DrawClock( 25 );
			}
			break;

		case SSAVER_3D:
			anim3d( 1 );
			break;

		case SSAVER_LOGO:
		{
			int h = GetLogoHeight();
			if ( h )
			{
				DrawLOGO( 0, 32 - ( h - 48 )/2 );
			}
			break;
		}

		case SSAVER_QIX:
			qix( 1 );
			break;

		case SSAVER_SNOW:
			Snow( 1 );
			break;

		case SSAVER_SPLASH:
			DrawImage( 0, 0, 0xFF );
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
		case SSAVER_3D:
			anim3d( 0 );
			break;

		case SSAVER_QIX:
			qix( 0 );
			break;

		case SSAVER_SNOW:
			Snow( 0 );
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
	const uint8_t cols[4][3] =
	{
		{ 0x1E, 0x1B, 0x0F },
		{ 0x1E, 0x0F, 0x1B },
		{ 0x1E, 0x1B, 0x0F },
		{ 0x0F, 0x1B, 0x1E }
	};

	int f = dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 );
	int col = cols[f][EditItemIndex];

	S_RTC_TIME_DATA_T rtd;

	GetRTC( &rtd );

	DrawString( String_SetDate, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	DrawTime( 6, 46, &rtd, 0x1F );
	DrawDate( 4, 64, &SetTimeRTD, col );
}


//=========================================================================
__myevic__ int IsMenuScreen()
{
	return (( Screen >= 101 ) && ( Screen <= 107 ));
}


//=========================================================================
__myevic__ void ShowCheckBattery()
{
  DrawStringCentered( String_Check, 88 );
  DrawStringCentered( String_Battery, 102 );
}


//=========================================================================
__myevic__ void ShowCheckUSB()
{
	DrawStringCentered( String_Check, 80 );
	DrawStringCentered( String_USB, 92 );
	DrawStringCentered( String_Adapter, 102 );
}


//=========================================================================
__myevic__ void ShowChargeError()
{
	DrawStringCentered( String_Charge, 88 );
	DrawStringCentered( String_Error, 102 );
}


//=========================================================================
__myevic__ void ShowImbBatts()
{
	DrawStringCentered( String_Imbalanced, 88 );
	DrawStringCentered( String_Batteries, 102 );
}


//=========================================================================
__myevic__ void ShowPowerCurve()
{
	DrawHLine( 10, 127,  60, 1 );
	DrawHLine( 10,  26,  60, 1 );
	DrawVLine( 10,  27, 126, 1 );
	DrawVLine( 60,  27, 126, 1 );

	int t = EditItemIndex * 5;
	int j = -1;

	for ( int i = 0; i < PWR_CURVE_PTS; ++i )
	{
		int t1, t2;

		t1 = dfPwrCurve[i].time;

		if ( ( i > 0 ) && ( t1 == 0 ) )
			break;

		if ( i < PWR_CURVE_PTS - 1 )
		{
			t2 = dfPwrCurve[i+1].time;

			if ( t2 == 0 ) t2 = 250;
		}
		else
		{
			t2 = 250;
		}

		DrawVLine(	10 + dfPwrCurve[i].power / 4,
					27 + 2 * t1 / 5,
					27 + 2 * t2 / 5,
					1 );

		if (( t2 > t ) && ( j < 0 ))
		{
			j = i;

			if ( t == t1 )
			{
				DrawFillRect(	10,
								27 + 2 * t1 / 5,
								10 + dfPwrCurve[i].power / 4,
								28 + 2 * t1 / 5,
								1 );
			}
		}
	}

	if ( !gFlags.edit_value || gFlags.draw_edited_item )
	{
		DrawImage( 6, 23 + EditItemIndex * 2 , 0xD4 );
	}

	DrawImage( 12, 3, 0xAF );
	DrawValueRight( 44, 3, t, 1, 0x0B, 0 );
	DrawImage( 46, 3, 0x94 );

	DrawImage( 12, 13, 0xAB );
	DrawValueRight( 44, 13, dfPwrCurve[j].power, 0, 0x0B, 0 );
	DrawImage( 46, 13, 0xC2 );
}


//=========================================================================
__myevic__ int SplashExists()
{
	int i, h, l;
	const image_t *img = Images[0xFF-1];

	h = img->height;
	l = img->width * h / 8;
	
	if ( img->width != 64 ) return 0;

	for ( i = 0 ; i < l ; ++i )
		if ( img->bitmap[i] ) break;

	return ( ( l && i < l ) ? 1 : 0 );
}


__myevic__ void ShowSplash()
{
	if ( gFlags.splash )
	{
		DrawImage( 0, 0, 0xFF );
		ScreenDuration = 3;
	}
	else
	{
		MainView();
	}
}
