#include "myevic.h"
#include "display.h"
#include "screens.h"
#include "menus.h"
#include "miscs.h"
#include "myrtc.h"
#include "dataflash.h"
#include "atomizer.h"
#include "battery.h"
#include "flappy.h"
#include "timers.h"
#include "meusbd.h"

#include "events.h"

//=========================================================================

uint8_t		FireClickCount;
uint8_t		FireClickTimer;
int8_t		UserInputs;
int8_t		LastInputs;
uint8_t		FireClicksEvent;

uint8_t		KeyUpTimer;
uint16_t	KeyTicks;
uint16_t	KeyPressTime;

//-------------------------------------------------------------------------

//=========================================================================
// Power Curve Editing
//-------------------------------------------------------------------------

__myevic__ int PCGetPoint()
{
	int t = EditItemIndex * 5;
	int i;

	for ( i = 0 ; i < PWR_CURVE_PTS ; ++i )
		if ( dfPwrCurve[i].time == t )
			return i;

	return -1;
}

__myevic__ void PCRemovePoint( int i )
{
	if ( i > 0 )
	{
		for ( ; i < PWR_CURVE_PTS ; ++i )
		{
			dfPwrCurve[i].time  = dfPwrCurve[i+1].time;
			dfPwrCurve[i].power = dfPwrCurve[i+1].power;
		}
		--i;
		dfPwrCurve[i].time = 0;
		dfPwrCurve[i].power = 100;
	}
}

__myevic__ int PCAddPoint()
{
	int i;

	int t = EditItemIndex * 5;

	if ( t == 0 )
		return 0;

	for ( i = 1 ; i < PWR_CURVE_PTS ; ++i )
		if ( ( dfPwrCurve[i].time == 0 ) || ( dfPwrCurve[i].time > t ) )
			break;

	if ( dfPwrCurve[i-1].time == t )
		return ( i - 1 );

	if ( dfPwrCurve[PWR_CURVE_PTS - 1].time > 0 )
		return -1;

	for ( int j = PWR_CURVE_PTS - 1 ; j > i ; --j )
	{
		dfPwrCurve[j].time  = dfPwrCurve[j-1].time;
		dfPwrCurve[j].power = dfPwrCurve[j-1].power;
	}

	dfPwrCurve[i].time  = t;
	dfPwrCurve[i].power = dfPwrCurve[i-1].power;

	return i;
}

__myevic__ void PCCompact()
{
	for ( int i = PWR_CURVE_PTS - 1 ; i > 0 ; --i )
		if ( dfPwrCurve[i].time )
			if ( dfPwrCurve[i].power == dfPwrCurve[i-1].power )
				PCRemovePoint( i );
}


//=========================================================================
//----- (00003738) --------------------------------------------------------
// Called at 10Hz untill KeyTicks >= 5 (1.4s), then at 100Hz.

__myevic__ void KeyRepeat()
{
	static uint8_t KRDelay = 0;


	if ( !PE0 )
		return;

	if ( dfStatus.keylock && !EditModeTimer )
	{
		if ( !IsMenuScreen() )
			return;
	}

	if ( dfStatus.off )
	{
		if ( !IsMenuScreen() )
			return;
	}
	else if ( EditModeTimer && ( Screen == 1 ) )
	{
		if ( EditItemIndex != 2 )
			return;
	}

	if ( PD2 == PD3 )
	{
		if ( KeyUpTimer )
		{
			if ( !--KeyUpTimer && Screen == 1 && ( dfTCPower >= 1000 || dfMode == 6 ) )
				MainView();
		}
		KeyTicks = 0;
		KRDelay = 6;
	}
	else if ( !KRDelay || !--KRDelay )
	{
		if ( KeyTicks < 205 )
		{
			++KeyTicks;
		}

		// First event has been emitted by GetUserInput()

		// +0.60s
		// Polled every 100ms...
		if ( KeyTicks < 5 )
		{
			KRDelay = 2;
		}
		// then every 10ms.
		// +0.40s (1.00s)
		else if ( KeyTicks < 105 )
		{
			// Quadratic function having its minimum (1) at 104
			KRDelay = 104 - KeyTicks;
			KRDelay = ( KRDelay * KRDelay ) / 545 + 1;
		}
		// +3.60s (4.60s)
		else if ( KeyTicks < 205 )
		{
			// Step jumped from 1 to 10, ie. speed x10
			// Recover this jump then smooth with same function as above
			KRDelay = 204 - KeyTicks;
			KRDelay = ( KRDelay * KRDelay ) / 545 + 1;
		}

		if ( !PD2 )
		{
			Event = 2;
		}
		else if ( !PD3 )
		{
			Event = 3;
		}
	}
}


//=========================================================================
//----- (00004B34) --------------------------------------------------------
// Called at 100Hz
__myevic__ void GetUserInput()
{
	UserInputs = 14;

	if ( ( !PE0 || AutoPuffTimer ) && PD2 && PD3 )
	{
		if ( !PE0 ) AutoPuffTimer = 0;

		if (( LastInputs == 5 ) || ( LastInputs == 6 ))
			return;

		UserInputs = 1;
	}
	else
	{
		if ( gFlags.firing )
		{
			if ( LastInputs == 1 )
			{
				StopFire();
			}
			gFlags.user_idle = 1;
			LastInputs = -1;
			KeyPressTime = 0;
			return;
		}

		if ( !FireClickTimer && FireClicksEvent )
		{
			Event = FireClicksEvent;
			FireClicksEvent = 0;
		}

		if ( !dfStatus.off || IsMenuScreen() )
		{
			if ( !PD2 ) UserInputs = 2;
			if ( !PD3 ) UserInputs = 3;
		}

		if ( !PD2 && !PD3 ) UserInputs = 4;
		if ( !PE0 && !PD2 ) UserInputs = 5;
		if ( !PE0 && !PD3 ) UserInputs = 6;

		if ( USBD_IS_ATTACHED() )
		{
			if ( !gFlags.usb_attached )
			{
				UserInputs = 10;
				BattProbeCount = 0;

				if ( dfStatus.off && FireClickCount == 1 )
				{
					FireClickCount = 0;
				}
			}
		}
		else
		{
			if ( gFlags.usb_attached )
			{
				UserInputs = 11;
			}
		}

		if ( gFlags.usb_attached )
		{
			if ( ISVTCDUAL )
			{
				if ( NumBatteries == 1 )
				{
					if ( !PD1 && !gFlags.battery_charging )
					{
						if ( !BattProbeCount || BattProbeCount >= 50 )
						{
							UserInputs = 12;
							BattProbeCount = 0;
						}
					}
					else if ( PD1 && gFlags.battery_charging && BatteryVoltage >= 414 )
					{
						UserInputs = 13;
						BattProbeCount = 1;
					}
				}
			}
			else if ( !ISCUBOID && !ISCUBO200 && !ISRX200S && !ISRX23 && !ISRX300 )
			{
				if ( !PD7 && !gFlags.battery_charging )
				{
					if ( !BattProbeCount || BattProbeCount >= 50 )
					{
						UserInputs = 12;
						BattProbeCount = 0;
					}
				}
				else if ( PD7 && gFlags.battery_charging )
				{
					UserInputs = 13;
					BattProbeCount = 1;
				}
			}
		}
	}

	if ( UserInputs >= 14 )
	{
		if ( LastInputs == 1 )
			StopFire();
		gFlags.user_idle = 1;
		LastInputs = -1;
		KeyPressTime = 0;
		return;
	}

	if ( UserInputs != LastInputs )
	{
		LastInputs = UserInputs;
		KeyPressTime = 0;
		gFlags.user_idle = 0;

		if ( UserInputs < 10 )
		{
			SplashTimer = 0;
		}

		return;
	}

	++KeyPressTime;

	// A keypress must be stable during at least
	// 60 milliseconds before an event is emitted.
	// (Taking into account the 50ms debounce time)

	if ( KeyPressTime == 6 )
	{
		gFlags.user_idle = 0;

		if (   UserInputs == 4
			|| UserInputs == 5
			|| UserInputs == 6
			|| UserInputs == 7
			|| UserInputs == 8
			|| UserInputs == 9
		)
			return;

		Event = UserInputs;

		if ( UserInputs == 1 )
		{
			FireClickTimer = 40;
			++FireClickCount;
			FireClicksEvent = 0;
			Event = 0;

			// Disable multi-click features in menus
			if ( IsMenuScreen() )
			{
				FireClickCount = 1;
			}

			switch ( FireClickCount )
			{
				case 1:
					FireClicksEvent = 15;	// single click

					if ( Screen != 1 || !EditModeTimer || EditItemIndex != 4 )
					{
						Event = 1;	// fire
					}
					break;

				case 2:
				case 3:
				case 4:
					switch ( dfClick[FireClickCount-2] )
					{
						default:
							break;

						case CLICK_ACTION_NONE:
							if ( FireClickCount == 4 )
								FireClicksEvent = EVENT_DEBUG_MODE;	// debug mode
							break;

						case CLICK_ACTION_EDIT:
							FireClicksEvent = 16;	// edit mode
							break;

						case CLICK_ACTION_TDOM:
							FireClicksEvent = EVENT_TOGGLE_TDOM;	// priority power
							break;

						case CLICK_ACTION_CLOCK:
							FireClicksEvent = EVENT_TOGGLE_CLOCK;	// toggle clock display
							break;

						case CLICK_ACTION_NEXT_MODE:
							FireClicksEvent = EVENT_NEXT_MODE;	// change mode
							break;

						case CLICK_ACTION_ON_OFF:
							FireClicksEvent = 17;	// Switch On/Off
							break;

						case CLICK_ACTION_PROFILE:
							FireClicksEvent = EVENT_NEXT_PROFILE;	// Cycle profile
							break;
					}
					if ( dfStatus.off )
					{
						if ( FireClicksEvent != 17 )
						{
							FireClicksEvent = 0;
						}
					}
					break;

				case 5:
					Event = 17;	// Switch On/Off
					break;

				case 10:
					Event = 17;	// Switch Back
					FireClicksEvent = 31;	// board temp screen
					break;

				case 20:
					Event = 29;	// firmware version screen
					break;
			}
		}
		else if ( UserInputs == 2 )
		{
			if ( Screen == 60 )
			{
				if ( dfScreenSaver == SSAVER_3D )
				{
					Next3DObject();
					Event = 0;
				}
			}
			else if ( dfStatus.keylock && !EditModeTimer
				&& Screen != 51
				&& !IsMenuScreen() )
			{
				Event = 30;	// key lock violation
			}
			else
			{
				Event = 2;	// + button
			}
		}
		else if ( UserInputs == 3 )
		{
			if ( Screen == 60 )
			{
				if ( dfScreenSaver == SSAVER_3D )
				{
					Previous3DObject();
					Event = 0;
				}
			}
			else if ( dfStatus.keylock && !EditModeTimer
				&& Screen != 51
				&& !IsMenuScreen() )
			{
				Event = 30;	// key lock violation
			}
			else
			{
				Event = 3;	// - button
			}
		}
	}
	else if ( KeyPressTime == 20 )
	{
		if ( UserInputs == 1 )
		{
			FireClicksEvent = 0;
		}
		else if ( UserInputs == 4 )
		{
			// Left + Right button
			if ( IsMenuScreen() )
			{
				Event = EVENT_PARENT_MENU;
			}
		}
		else if ( UserInputs == 5 )
		{
			// Fire + Right button
			if ( IsMenuScreen() )
			{
				Event = EVENT_EXIT_MENUS;
			}
			else if ( !dfStatus.off )
			{
				if ( !gFlags.playing_fb )
				{
					Event = EVENT_ENTER_MENUS;
				}
				else
				{
					gFlags.playing_fb = 0;
					Event = 0;
					fbInitTimeouts();
					MainView();
				}
			}
		}
	}
	else if ( KeyPressTime == 200 )
	{
		if ( UserInputs == 1 )
		{
			if (( Screen == 1 )
			&& (( EditModeTimer > 0 ) && ( EditItemIndex == 4 )))
			{
				EditModeTimer = 1000;
				if ( dfAPT == 1 )
					Event = 22;	// puff reset
				if ( dfAPT == 2 )
					Event = 23;	// time reset
				if ( dfAPT == 3 )
					Event = EVENT_RESET_VVEL;	// vvel reset
			}
			else
			{
				Event = EVENT_LONG_FIRE;
			}
		}
		else if ( UserInputs == 4 )
		{
			if ( !EditModeTimer )
			{
				if ( dfStatus.off )
				{
					Event = 18;	// flip display
				}
				else
				{
					Event = 4;	// key (un)lock
				}
			}
		}
		else if ( UserInputs == 5 )
		{
			// Fire + Right button
			if ( dfStatus.off )
			{
				Event = 39;	// tcr set menu
			}
			else
			{
				Event = EVENT_PROFILE_MENU;	// profile selection
			}
		}
		else if ( UserInputs == 6 )
		{
			if ( dfStatus.off )
			{
				Event = 34;	// battery voltage screen
			}
			else
			{
				Event = 6;	// stealth on/off
			}
		}
	}
	else if ( KeyPressTime == 500 )
	{
		if ( UserInputs == 5 )
		{
			Event = EVENT_POWER_CURVE;
		}
	}
	else if ( ( KeyPressTime & 0x8000 ) || ( KeyPressTime & 0x7fff ) > 200 )
	{
		if ( UserInputs == 1 )
		{
			if ( KeyPressTime > FIRE_PROTEC_MAX * 10 + 100 )
			{
				KeyPressTime = FIRE_PROTEC_MAX * 10 + 100;
				gFlags.user_idle = 1;
			}
			else if ( gFlags.firing && FireDuration >= dfProtec )
			{
				Event = 24;	// 10s protection
			}
		}
		else if ( KeyPressTime & 0x8000 )
		{
			KeyPressTime = 1;
		}
	}
}


//=========================================================================
// Event Handling
//-------------------------------------------------------------------------

__myevic__ int EvtFire()
{
	int vret = 0;

	switch ( Screen )
	{
		case 101:
		{
			UpdateDataFlash();
			Event = EVENT_PARENT_MENU;
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			MainView();
			vret = 1;
		}
		break;

		case 104:
		{
			RTCAdjustClock( 0 );
			MainView();
			vret = 1;
		}
		break;

		case 105:
		case 106:
		{
			EditModeTimer = 6000;
			if ( --EditItemIndex > 2 ) EditItemIndex = 2;
			gFlags.draw_edited_item = 1;
			vret = 1;
		}
		break;
		
		case 107:
		{
			EditModeTimer = 3000;
			if ( gFlags.edit_value )
			{
				gFlags.edit_value = 0;
				PCCompact();
			}
			else
			{
				if (( PCGetPoint() >= 0 ) || ( PCAddPoint() >= 0 ))
				{
					gFlags.edit_value = 1;
				}
			}
			vret = 1;
		}
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtSingleFire()
{
	int vret = 0;

	switch ( Screen )
	{
		case 100:
		{
			MainView();
			vret = 1;
		}
		break;

		case 101:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
		{
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtPlusButton()
{
	int vret = 0;

	switch ( Screen )
	{
		case 1:
		{
			if ( EditModeTimer )
			{
				if ( EditItemIndex == 1 )
				{
					if ( dfMode < 3 )
					{
						KeyUpTimer = 10;
						EditModeTimer = 1000;

						do
						{
							if ( ++dfMode > 2 ) dfMode = 0;
						}
						while ( dfModesSel & ( 1 << dfMode ) );
						dfTCMode = dfMode;
						ModeChange();

						UpdateDFTimer = 50;
						gFlags.refresh_display = 1;
						vret = 1;
					}
				}
			}
		}
		break;

		case 101:
		{
			if ( dfContrast <= 240 ) dfContrast += 15;
			else dfContrast = 255;
			UpdateDFTimer = 50;
			DisplaySetContrast( dfContrast );
			gFlags.refresh_display = 1;
			ScreenDuration = 10;
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			if ( !gFlags.has_x32 )
			{
				unsigned int cs = RTCGetClockSpeed();
				if ( KeyTicks < 105 )
				{
					++cs;
				}
				else
				{
					cs -= cs % 10;
					cs += 10;
				}
				if ( cs > RTC_MAX_CLOCK_RATIO )
				{
					if ( KeyTicks < 5 )
					{
						cs = RTC_MIN_CLOCK_RATIO;
					}
					else
					{
						cs = RTC_MAX_CLOCK_RATIO;
					}
				}
				RTCSetClockSpeed( cs );
			}
			gFlags.refresh_display = 1;
			ScreenDuration = 120;
			vret = 1;
		}
		break;
		
		case 104:
		{
			RTCAdjustClock( 1 );
			gFlags.refresh_display = 1;
			ScreenDuration = 120;
			vret = 1;
		}
		break;

		case 105:
		{
			switch ( EditItemIndex )
			{
				case 0:
					++SetTimeRTD.u32Second;
					SetTimeRTD.u32Second %= 60;
					break;
				case 1:
					++SetTimeRTD.u32Minute;
					SetTimeRTD.u32Minute %= 60;
					break;
				case 2:
					++SetTimeRTD.u32Hour;
					SetTimeRTD.u32Hour %= 24;
					break;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;

		case 106:
		{
			int f = dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 );
			switch ( ( f << 2 | EditItemIndex ) )
			{
				case  0:
				case  4:
				case  8:
				case 14:
					if ( SetTimeRTD.u32Year < RTC_YEAR2000 + 1000 ) ++SetTimeRTD.u32Year;
					break;
				case  1:
				case  6:
				case  9:
				case 13:
					SetTimeRTD.u32Month = SetTimeRTD.u32Month %12 + 1;
					break;
				case  2:
				case  5:
				case 10:
				case 12:
					SetTimeRTD.u32Day = SetTimeRTD.u32Day %31 + 1;
					break;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;

		case 107:
		{
			if ( gFlags.edit_value )
			{
				int i = PCGetPoint();

				if ( ++dfPwrCurve[i].power > 200 )
				{
					if ( KeyTicks < 5 ) dfPwrCurve[i].power = 0;
					else dfPwrCurve[i].power = 200;
				}
				UpdateDFTimer = 50;
			}
			else
			{
				++EditItemIndex;
				EditItemIndex %= 50;
			}
			EditModeTimer = 3000;
			gFlags.refresh_display = 1;
			vret = 1;
			break;
		}
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtMinusButton()
{
	int vret = 0;

	switch ( Screen )
	{
		case 101:
		{
			if ( dfContrast >= 15 ) dfContrast -= 15;
			else dfContrast = 0;
			UpdateDFTimer = 50;
			DisplaySetContrast( dfContrast );
			gFlags.refresh_display = 1;
			ScreenDuration = 10;
			vret = 1;
		}
		break;

		case 102:
		{
			vret = MenuEvent( LastEvent );
		}
		break;

		case 103:
		{
			if ( !gFlags.has_x32 )
			{
				unsigned int cs = RTCGetClockSpeed();
				if ( KeyTicks < 105 )
				{
					--cs;
				}
				else
				{
					if ( cs % 10 ) cs -= cs % 10;
					else cs -= 10;
				}
				if ( cs < RTC_MIN_CLOCK_RATIO )
				{
					if ( KeyTicks < 5 )
					{
						cs = RTC_MAX_CLOCK_RATIO;
					}
					else
					{
						cs = RTC_MIN_CLOCK_RATIO;
					}
				}
				RTCSetClockSpeed( cs );
			}
			gFlags.refresh_display = 1;
			ScreenDuration = 120;
			vret = 1;
		}
		break;

		case 104:
		{
			RTCAdjustClock( -1 );
			gFlags.refresh_display = 1;
			ScreenDuration = 120;
			vret = 1;
		}
		break;

		case 105:
		{
			switch ( EditItemIndex )
			{
				case 0:
					SetTimeRTD.u32Second = ( SetTimeRTD.u32Second + 59 ) % 60;
					break;
				case 1:
					SetTimeRTD.u32Minute = ( SetTimeRTD.u32Minute + 59 ) % 60;
					break;
				case 2:
					SetTimeRTD.u32Hour = ( SetTimeRTD.u32Hour + 23 ) % 24;
					break;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;

		case 106:
		{
			int f = dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 );
			switch ( ( f << 2 | EditItemIndex ) )
			{
				case  0:
				case  4:
				case  8:
				case 14:
					if ( SetTimeRTD.u32Year > RTC_YEAR2000 ) --SetTimeRTD.u32Year;
					break;
				case  1:
				case  6:
				case  9:
				case 13:
					SetTimeRTD.u32Month = ( SetTimeRTD.u32Month + 10 ) % 12 + 1;
					break;
				case  2:
				case  5:
				case 10:
				case 12:
					SetTimeRTD.u32Day = ( SetTimeRTD.u32Day + 29 ) % 31 + 1;
					break;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;

		case 107:
		{
			if ( gFlags.edit_value )
			{
				int i = PCGetPoint();

				if ( !dfPwrCurve[i].power-- )
				{
					if ( KeyTicks < 5 ) dfPwrCurve[i].power = 200;
					else dfPwrCurve[i].power = 0;
				}
				UpdateDFTimer = 50;
			}
			else
			{
				if ( !EditItemIndex-- ) EditItemIndex = 49;
			}
			EditModeTimer = 3000;
			gFlags.refresh_display = 1;
			vret = 1;
			break;
		}
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtToggleClock()
{
	dfStatus.clock ^= 1;
	UpdateDFTimer = 50;
	MainView();
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int EvtDebugMode()
{
	if ( dfStatus.dbgena )
	{
		gFlags.debug ^= 1;
	}
	else
	{
		gFlags.debug = 0;
	}
	gFlags.refresh_display = 1;
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int EvtLongFire()
{
	int vret = 0;

	switch ( Screen )
	{
		case 102:
			vret = MenuEvent( LastEvent );
			break;

		case 106:
		{
			S_RTC_TIME_DATA_T rtd;

			GetRTC( &rtd );
			SetTimeRTD.u32Hour = rtd.u32Hour;
			SetTimeRTD.u32Minute = rtd.u32Minute;
			SetTimeRTD.u32Second = rtd.u32Second;
			// NOBREAK
		}

		case 105:
			SetRTC( &SetTimeRTD );
			EditModeTimer = 0;
			MainView();
			vret = 1;
			break;

		case 107:
		{
			int i = PCGetPoint();
			if ( i > 0 )
				PCRemovePoint( i );
			gFlags.edit_value = 0;
			gFlags.refresh_display = 1;
			vret = 1;
		}
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtContrastMenu()
{
	SetScreen( 101, 10 );
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int EvtEnterMenus()
{
	CurrentMenu = 0;
	CurrentMenuItem = 0;
	SetScreen( 102, 30 );
	return 1;
}

//-------------------------------------------------------------------------

S_RTC_TIME_DATA_T SetTimeRTD;

__myevic__ int EvtSetTime()
{
	GetRTC( &SetTimeRTD );
	SetScreen( 105, 60 );
	EditItemIndex = 2;
	EditModeTimer = 6000;
	return 1;
}

__myevic__ int EvtSetDate()
{
	GetRTC( &SetTimeRTD );
	SetScreen( 106, 60 );
	EditItemIndex = 2;
	EditModeTimer = 6000;
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int CustomEvents()
{
	int vret;

	vret = 1;

	switch ( LastEvent )
	{
		case   1:	// Fire button
			vret = EvtFire();
			break;

		case   2:	// + button
			vret = EvtPlusButton();
			break;

		case   3:	// - button
			vret = EvtMinusButton();
			break;

		case  15:	// Single Fire
			vret = EvtSingleFire();
			break;

		case  39:	// TCR Set Menu select
			vret = MenuEvent( LastEvent );
			break;

		case EVENT_TOGGLE_CLOCK:	// Double Fire
			vret = EvtToggleClock();
			break;

		case EVENT_EDIT_CONTRAST:	// Contrast screen
			vret = EvtContrastMenu();
			break;

		case EVENT_ENTER_MENUS:	// Menus screen
			vret = EvtEnterMenus();
			break;

		case EVENT_DEBUG_MODE:
			vret = EvtDebugMode();
			break;

		case EVENT_LONG_FIRE:
			vret = EvtLongFire();
			break;

		case EVENT_EXIT_MENUS:
			vret = MenuEvent( LastEvent );
			break;

		case EVENT_PARENT_MENU:
			vret = MenuEvent( LastEvent );
			break;

		case EVENT_SET_TIME:
			vret = EvtSetTime();
			break;

		case EVENT_SET_DATE:
			vret = EvtSetDate();
			break;

		case EVENT_NEXT_MODE:
			NextMode();
			gFlags.refresh_display = 1;
			break;

		case EVENT_TOGGLE_TDOM:
			if ( ISMODETC(dfMode) )
			{
				dfStatus.priopwr ^= 1;
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
			}
			else if ( ISMODEVW(dfMode) )
			{
				MenuEvent( LastEvent );
			}
			break;

		case EVENT_RESET_VVEL:
		{
			time_t t;
			RTCGetEpoch( &t );
			t = t - ( t% 86400 );
			MilliJoules = 0;
			RTCWriteRegister( RTCSPARE_VV_BASE, t );
			EditModeTimer = 1000;
			gFlags.refresh_display = 1;
			gFlags.draw_edited_item = 1;
			break;
		}

		case EVENT_FORCE_VCOM:
			dfStatus.storage = 0;
			dfStatus.vcom = 1;
			InitUSB();
			break;

		case EVENT_AUTO_PUFF:
			if ( AutoPuffTimer > 0 )
				MainView();
			else
				StopFire();
			break;

		case EVENT_CLK_ADJUST:
			SetScreen( 104, 120 );
			break;

		case EVENT_CLK_SPEED:
			SetScreen( 103, 120 );
			break;

		case EVENT_INVERT_SCREEN:
			DisplaySetInverse( dfStatus.invert );
			break;

		case EVENT_MODE_CHANGE:
			ModeChange();
			break;

		case EVENT_PROFILE_MENU:
			vret = MenuEvent( LastEvent );
			break;

		case EVENT_NEXT_PROFILE:
			LoadProfile( ( dfProfile + 1 ) % DATAFLASH_PROFILES_MAX );
			ShowProfNum = 30;
			break;

		case EVENT_POWER_CURVE:
			SetScreen( 107, 30 );
			EditModeTimer = 3000;
			EditItemIndex = 0;
			break;

		default:
			vret = 0;
			break;
	}

	return vret;
}

//==========================================================================

