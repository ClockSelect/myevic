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


//=========================================================================
//----- (00003738) --------------------------------------------------------
// Called at 5Hz untill KeyTicks >= 5 (1.4s), then at 100Hz.

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
		if ( Screen != 59 )
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
		KRDelay = 3;
	}
	else if ( !KRDelay || !--KRDelay )
	{
		// First event has been emitted by GetUserInput()

		// Second event at +0.6s
		// Next 4 events every .2s
		// Next 100 events every .01s
		// Following events every .03s

	//	if ( KeyTicks >= 105 )
	//	{
	//		KRDelay = 0;
	//	}
	//	else
	//	{
	//		++KeyTicks;
	//	}

		// Second event at +0.6s
		// Next 4 events every .2s (0.8s)
		// Next 50 events every .05s (2.5s)
		// Next 50 events every .03s (1.5s)
		// Following events every .01s

		if ( KeyTicks >= 5 && KeyTicks < 55 )
		{
			KRDelay = 5;
		}
		else if ( KeyTicks >= 55 && KeyTicks < 105 )
		{
			KRDelay = 3;
		}

		++KeyTicks;

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

		if ( !(dfStatus.off) || Screen == 59 )
		{
			if ( !PD2 ) UserInputs = 2;
			if ( !PD3 ) UserInputs = 3;
		}

		if ( !PD2 && !PD3 ) UserInputs = 4;
		if ( !PE0 && !PD2 ) UserInputs = 5;
		if ( !PE0 && !PD3 ) UserInputs = 6;

		if ( USBD_IS_ATTACHED() )
		{
			if ( !(gFlags.usb_attached) )
			{
				UserInputs = 10;
				byte_20000048 = 0;
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
			if ( !PD7 && !(gFlags.battery_charging) )
			{
				if ( !byte_20000048 || byte_20000048 >= 50 )
				{
					UserInputs = 12;
					byte_20000048 = 0;
				}
			}
			else if ( PD7 && gFlags.battery_charging )
			{
				UserInputs = 13;
				byte_20000048 = 1;
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
		return;
	}

	++KeyPressTime;

	// A keypress must be stable during at least
	// 60 milliseconds before an event is emitted.

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
					}
					break;

				case 5:
				case 6:
				case 7:
					FireClicksEvent = 17;	// Switch On/Off
					break;

				case 10:
					FireClicksEvent = 31;	// board temp screen
					break;

				case 20:
					Event = 29;	// firmware version screen
					break;
			}
		}
		else if ( UserInputs == 2 )
		{
			if ( dfStatus.keylock && !EditModeTimer
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
			if ( dfStatus.keylock && !EditModeTimer
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
	}
	else if ( KeyPressTime == 50 )
	{
		if ( UserInputs == 4 )
		{
			if ( IsMenuScreen() )
			{
				Event = EVENT_PARENT_MENU;
			}
		}
		else if ( UserInputs == 5 )
		{
			if ( IsMenuScreen() )
			{
				Event = EVENT_EXIT_MENUS;
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
				if ( dfAPT == 8 )
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
					Event = 18;	// flip display
				else
					Event = 4;	// key (un)lock
			}
		}
		else if ( UserInputs == 5 )
		{
			if ( !(dfStatus.off) )
			{
				if ( !(gFlags.playing_fb) )
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
		else if ( UserInputs == 6 )
		{
			if ( !(dfStatus.off) )
			{
				Event = 6;	// stealth on/off
			}
		}
	}
	else if ( KeyPressTime == 250 )
	{
		if ( UserInputs == 5 )
		{
			if ( dfStatus.off )
			{
				Event = 39;	// tcr set menu
			}
		}
		else if ( UserInputs == 6 )
		{
			if ( dfStatus.off )
			{
				Event = 34;	// battery voltage screen
			}
		}
	}
	else if ( ( KeyPressTime & 0x8000 ) || ( KeyPressTime & 0x7fff ) >= 210 )
	{
		if ( UserInputs == 1 )
		{
			if ( KeyPressTime > 1100 )
			{
				KeyPressTime = 1100;
				gFlags.user_idle = 1;
			}
			else if ( FireDuration >= dfProtec )
			{
				Event = 24;	// 10s protection
			}
		}
		else if ( KeyPressTime & 0x8000 )
		{
			KeyPressTime = 6;
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
			MainView();
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
						KeyUpTimer = 5;
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
			if (( EditItemIndex == 2 && dfStatus.mdy )
				||( EditItemIndex == 1 && !dfStatus.mdy ))
			{
				SetTimeRTD.u32Month = SetTimeRTD.u32Month %12 + 1;
			}
			else if (( EditItemIndex == 1 && dfStatus.mdy )
					|| ( EditItemIndex == 2 && !dfStatus.mdy ))
			{
				SetTimeRTD.u32Day = SetTimeRTD.u32Day %31 + 1;
			}
			else if ( EditItemIndex == 0 )
			{
				if ( SetTimeRTD.u32Year < RTC_YEAR2000 + 1000 ) ++SetTimeRTD.u32Year;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;
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
			if (( EditItemIndex == 2 && dfStatus.mdy )
				||( EditItemIndex == 1 && !dfStatus.mdy ))
			{
				SetTimeRTD.u32Month = ( SetTimeRTD.u32Month + 11 ) %12;
			}
			else if (( EditItemIndex == 1 && dfStatus.mdy )
					|| ( EditItemIndex == 2 && !dfStatus.mdy ))
			{
				SetTimeRTD.u32Day = ( SetTimeRTD.u32Day + 30 ) %31;
			}
			else if ( EditItemIndex == 0 )
			{
				if ( SetTimeRTD.u32Year > RTC_YEAR2000 ) --SetTimeRTD.u32Year;
			}
			gFlags.draw_edited_item = 1;
			gFlags.refresh_display = 1;
			ScreenDuration = 60;
			EditModeTimer = 6000;
			vret = 1;
		}
		break;
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
		case  59:
			UpdateDataFlash();
			if ( dfStatus.off )
			{
				gFlags.refresh_display = 1;
				Screen = 0;
			}
			else
			{
				MainView();
			}
			vret = 1;
			break;

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
	}

	return vret;
}

//-------------------------------------------------------------------------

__myevic__ int EvtContrastMenu()
{
	Screen = 101;
	ScreenDuration = 10;
	gFlags.refresh_display = 1;
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int EvtEnterMenus()
{
	CurrentMenu = 0;
	CurrentMenuItem = 0;
	Screen = 102;
	ScreenDuration = 30;
	gFlags.refresh_display = 1;
	return 1;
}

//-------------------------------------------------------------------------

S_RTC_TIME_DATA_T SetTimeRTD;

__myevic__ int EvtSetTime()
{
	GetRTC( &SetTimeRTD );
	Screen = 105;
	ScreenDuration = 60;
	gFlags.refresh_display = 1;
	EditItemIndex = 2;
	EditModeTimer = 6000;
	return 1;
}

__myevic__ int EvtSetDate()
{
	GetRTC( &SetTimeRTD );
	Screen = 106;
	ScreenDuration = 60;
	gFlags.refresh_display = 1;
	EditItemIndex = 2;
	EditModeTimer = 6000;
	return 1;
}

//-------------------------------------------------------------------------

__myevic__ int CustomEvents()
{
	int vret;

	vret = 0;

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

		case  15:	//Single Fire
			vret = EvtSingleFire();
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
			vret = 1;
			break;

		case EVENT_TOGGLE_TDOM:
			dfStatus.priopwr ^= 1;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

		case EVENT_RESET_VVEL:
		{
			time_t t;
			RTCGetEpoch( &t );
			t = t - ( t% 86400 );
			MilliJoules = 0;
			RTCWriteRegister( RTCSPARE_VV_BASE, t );
			EditModeTimer = 0;
			gFlags.refresh_display = 1;
			vret = 1;
			break;
		}

		case EVENT_FORCE_VCOM:
			dfStatus.storage = 0;
			dfStatus.vcom = 1;
			InitUSB();
			vret = 1;
			break;

		case EVENT_AUTO_PUFF:
			if ( AutoPuffTimer > 0 )
				MainView();
			else
				StopFire();
			vret = 1;
			break;

		case EVENT_CLK_ADJUST:
			Screen = 104;
			ScreenDuration = 120;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

		case EVENT_CLK_SPEED:
			Screen = 103;
			ScreenDuration = 120;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

		default:
			break;
	}
	return vret;
}

//==========================================================================