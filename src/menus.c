#include "myevic.h"
#include "myprintf.h"
#include "events.h"
#include "screens.h"
#include "display.h"
#include "myrtc.h"
#include "dataflash.h"
#include "miscs.h"
#include "flappy.h"
#include "meusbd.h"
#include "atomizer.h"

//=============================================================================
// MENUS
//-----------------------------------------------------------------------------

typedef struct menu_s menu_t;
typedef struct mitem_s mitem_t;

struct mitem_s
{
	const uint16_t *caption;
	const menu_t *submenu;
	const char screen;
	const char sduration;
};

struct menu_s
{
	const uint16_t *caption;
	void (*on_enter)();
	void (*on_drawitem)( int mi, int y, int sel );
	void (*on_selectitem)();
	void (*on_clickitem)();
	int  (*on_event)( int event );
	const short nitems;
	const mitem_t mitems[];
};

struct menu_s const *CurrentMenu;
unsigned char CurrentMenuItem;


//-----------------------------------------------------------------------------
__myevic__ void ExpertMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
		case 0:
			DrawFillRect( 32, line, 63, line+12, 0 );
			if ( dfStatus.vcom )
				DrawString( String_COM, 36, line+2 );
			else if ( dfStatus.storage )
				DrawString( String_DSK, 36, line+2 );
			else
				DrawString( String_HID, 36, line+2 );
			break;

		default:
			break;
	}
}

__myevic__ void ExpertMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:
			if ( dfStatus.vcom )
			{
				dfStatus.vcom = 0;
				dfStatus.storage = 1;
			}
			else if ( dfStatus.storage )
			{
				dfStatus.storage = 0;
			}
			else
			{
				dfStatus.vcom = 1;
			}
			gFlags.refresh_display = 1;
			break;

		case 1:
			UpdateDataFlash();
			InitUSB();
			MainView();
			break;
	}
}

//-----------------------------------------------------------------------------

__myevic__ void ScreenSaveMenuOnEnter()
{
	CurrentMenuItem = dfScreenSave;
}


__myevic__ void ScreenSaveMenuIDraw( int it, int line, int sel )
{
	if ( ScrSaveTimes[it] )
	{
		if ( sel )
		{
			DrawFillRect( 0, line, 63, line+12, 1 );
			DrawValueInv( 4, line+2, ScrSaveTimes[it], 0, 0x0B, 0 );
			DrawStringInv( String_Min, 32, line+2 );
		}
		else
		{
			DrawValue( 4, line+2, ScrSaveTimes[it], 0, 0x0B, 0 );
			DrawString( String_Min, 32, line+2 );
		}
	}
}


__myevic__ void ScreenSaveMenuOnClick()
{
	dfScreenSave = CurrentMenuItem;
	UpdateDataFlash();
	MainView();
}


//-----------------------------------------------------------------------------

__myevic__ void Anim3dOnEnter()
{
	if ( Anim3d ) CurrentMenuItem = ( Anim3d - 1 );
	else CurrentMenuItem = CurrentMenu->nitems - 1;
}

__myevic__ void Anim3dOnClick()
{
	if ( CurrentMenuItem < CurrentMenu->nitems - 1 )
	{
		Anim3d = CurrentMenuItem + 1;
	}
	else
	{
		Anim3d = 0;
	}
	MainView();
}


//-----------------------------------------------------------------------------

__myevic__ void LogoMEnter()
{
	CurrentMenuItem = dfStatus.nologo;
}

__myevic__ void LogoISelect()
{
	dfStatus.nologo = CurrentMenuItem & 1;
	UpdateDFTimer = 50;
}

//-----------------------------------------------------------------------------

__myevic__ void GameMEnter()
{
	CurrentMenuItem = dfFBSpeed;
}

__myevic__ void GameISelect()
{
	dfFBSpeed = CurrentMenuItem;
	UpdateDFTimer = 50;
}

__myevic__ void GameIClick()
{
	fbStartGame();
}

//-----------------------------------------------------------------------------

__myevic__ void ModesMEnter()
{
	dfModesSel &= ~( 1 << dfMode );
	UpdateDFTimer = 50;
}

__myevic__ void ModesIDraw( int it, int line, int sel )
{
	if ( it > 6 ) return;

	(sel?DrawImageInv+1:DrawImage+1)( 53, line+2,
			(dfMode==it) ? 0x9C : (dfModesSel&(1<<it)) ? 0xA9 : 0xB4 );
}

__myevic__ void ModesIClick()
{
	if (( CurrentMenuItem < 7 ) && ( CurrentMenuItem != dfMode ))
	{
		dfModesSel ^= ( 1 << CurrentMenuItem );
		if ( dfModesSel == 0x7f ) dfModesSel ^= ( 1 << 4 );
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}
	else
	{
		UpdateDataFlash();
	}
}

//-----------------------------------------------------------------------------

uint8_t *CoilSelectedLock;
uint16_t *CoilSelectedRez;

__myevic__ void CoilsISelect()
{
	if ( CurrentMenuItem > 3 )
	{
		gFlags.edit_value = 0;
		return;
	}
	switch ( CurrentMenuItem )
	{
		case 0 : CoilSelectedRez = &dfRezNI ; CoilSelectedLock = &dfRezLockedNI ; break;
		case 1 : CoilSelectedRez = &dfRezTI ; CoilSelectedLock = &dfRezLockedTI ; break;
		case 2 : CoilSelectedRez = &dfRezSS ; CoilSelectedLock = &dfRezLockedSS ; break;
		case 3 : CoilSelectedRez = &dfRezTCR; CoilSelectedLock = &dfRezLockedTCR; break;
		default:
			break;
	}
}


__myevic__ void CoilsMEnter()
{
	if ( dfMode < 4 ) CurrentMenuItem = dfMode;
	else CurrentMenuItem = 4;
	CoilsISelect( CurrentMenuItem );
}


__myevic__ void CoilsIDraw( int it, int line, int sel )
{
	if ( it > 3 ) return;
	int rez = 0;
	short img = 0xC0;
	switch ( it )
	{
		case 0 : rez = dfRezNI; if (dfRezLockedNI) img = 0xC3; break;
		case 1 : rez = dfRezTI; if (dfRezLockedTI) img = 0xC3; break;
		case 2 : rez = dfRezSS; if (dfRezLockedSS) img = 0xC3; break;
		case 3 : rez = dfRezTCR; if (dfRezLockedTCR) img = 0xC3; break;
	}
	if ( gFlags.edit_value && sel )
	{
		DrawFillRect( 0, line, 31, line+12, 0 );
		DrawString( CurrentMenu->mitems[CurrentMenuItem].caption, 4, line +2 );
		DrawFillRect( 32, line, 63, line+12, 1 );
		DrawValueInv( 34, line+2, rez, 2, 0x0B, 3 );
		DrawImageInv( 56, line+2, img );
	}
	else
	{
		DrawFillRect( 32, line, 63, line+12, 0 );
		DrawValue( 34, line+2, rez, 2, 0x0B, 3 );
		DrawImage( 56, line+2, img );
	}
	CoilsISelect( it );
}

__myevic__ void CoilsIClick()
{
	switch ( CurrentMenuItem )
	{
		case 0 :
		case 1 :
		case 2 :
		case 3 :
			*CoilSelectedRez = 0;
			*CoilSelectedLock = 0;
			break;
		case 4 :
			dfRezNI  = 0; dfRezLockedNI  = 0;
			dfRezTI  = 0; dfRezLockedTI  = 0;
			dfRezSS  = 0; dfRezLockedSS  = 0;
			dfRezTCR = 0; dfRezLockedTCR = 0;
			break;
	}
	if (( CurrentMenuItem == dfMode ) || ( CurrentMenuItem == 4 ))
	{
		dfResistance = AtoRez;
	}
	if ( CurrentMenuItem == 5 )
	{
		UpdateDataFlash();
	}
	else
	{
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}
}

__myevic__ int CoilsMEvent( int event )
{
	int vret = 0;
	if ( CurrentMenuItem > 3 )
		return vret;

	switch ( event )
	{
		case 1:
			gFlags.edit_value ^= 1;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

		case 2:
			if ( gFlags.edit_value )
			{
				if ( *CoilSelectedRez == 0 )
				{
					*CoilSelectedRez = 5;
				}
				else if ( *CoilSelectedRez < 150 )
				{
					++*CoilSelectedRez;
				}
				*CoilSelectedLock = 1;
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
				vret = 1;
			}
			break;

		case 3:
			if ( gFlags.edit_value )
			{
				if ( *CoilSelectedRez == 5 )
				{
					*CoilSelectedRez = 0;
					*CoilSelectedLock = 0;
				}
				else if ( *CoilSelectedRez > 5 )
				{
					--*CoilSelectedRez;
					*CoilSelectedLock = 1;
				}
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
				vret = 1;
			}
			break;

		case EVENT_LONG_FIRE:
			*CoilSelectedRez = 0;
			*CoilSelectedLock = 0;
			UpdateDFTimer = 50;
			gFlags.edit_value = 0;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

	}
	return vret;
}


//-----------------------------------------------------------------------------

signed char dt_sel = 0;
S_RTC_TIME_DATA_T rtd;

__myevic__ void DTMenuOnEnter()
{
	dt_sel = 2;

	GetRTC( &rtd );

	ScreenDuration = 120;
}

__myevic__ void DTMenuIDraw( int it, int line, int sel )
{
	int colors = sel ? 0x1F & ~(1<<(dt_sel<<1)) : 0x1F;

	switch ( it )
	{
		case 0:
			DrawTimeSmall( 12, line+2, &rtd, colors );
			break;

		case 1:
			DrawDate( 4, line+2, &rtd, colors );
			break;
		
		default:
			break;
	}
}

__myevic__ int DTMenuOnEvent( int event )
{
	int vret = 0;

	ScreenDuration = 120;

	switch ( event )
	{
		case 1:
			switch ( CurrentMenuItem )
			{
				case 0:
				case 1:
					if ( --dt_sel < 0 )
					{
						dt_sel = 2;
						++CurrentMenuItem;
					}
					gFlags.refresh_display = 1;
					vret = 1;
					break;

				case 3:
					{
						SetRTC( &rtd );
					}
					break;

				default:
					break;
			}
			break;

		case 2:
			switch ( CurrentMenuItem )
			{
				case 0:
					switch ( dt_sel )
					{
						case 0:
							++rtd.u32Second;
							rtd.u32Second %= 60;
							break;
						case 1:
							++rtd.u32Minute;
							rtd.u32Minute %= 60;
							break;
						case 2:
							++rtd.u32Hour;
							rtd.u32Hour %= 24;
							break;
					}
					gFlags.refresh_display = 1;
					vret = 1;
					break;

				case 1:
					switch ( dt_sel )
					{
						case 0:
							if ( rtd.u32Year < RTC_YEAR2000 + 1000 ) ++rtd.u32Year;
							break;
						case 1:
							rtd.u32Month = (rtd.u32Month+1) %12;
							break;
						case 2:
							rtd.u32Day = (rtd.u32Day+1) %31;
							break;
					}
					gFlags.refresh_display = 1;
					vret = 1;
					break;

				default:
					break;
			}
			break;

		case 3:
			switch ( CurrentMenuItem )
			{
				case 0:
					switch ( dt_sel )
					{
						case 0:
							rtd.u32Second = ( rtd.u32Second + 59 ) % 60;
							break;
						case 1:
							rtd.u32Minute = ( rtd.u32Minute + 59 ) % 60;
							break;
						case 2:
							rtd.u32Hour = ( rtd.u32Hour + 23 ) % 24;
							break;
					}
					gFlags.refresh_display = 1;
					vret = 1;
					break;

				case 1:
					switch ( dt_sel )
					{
						case 0:
							if ( rtd.u32Year > RTC_YEAR2000 ) --rtd.u32Year;
							break;
						case 1:
							rtd.u32Month = ( rtd.u32Month+11 ) %12;
							break;
						case 2:
							rtd.u32Day = ( rtd.u32Day + 30 ) %31;
							break;
					}
					gFlags.refresh_display = 1;
					vret = 1;
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ void ScreenIClick()
{
	if ( CurrentMenu->mitems[CurrentMenuItem].screen == 101 )
	{
		gFlags.edit_capture_evt = 1;
	}
}

//-----------------------------------------------------------------------------

const menu_t LOGOMenu =
{
	String_LOGO,
	LogoMEnter+1,
	0,
	LogoISelect+1,
	UpdateDataFlash+1,
	0,
	2,
	{
		{ String_On, 0, 1, 0 },
		{ String_Off, 0, 1, 0 }
	}
};

const menu_t GameMenu =
{
	String_Game,
	GameMEnter+1,
	0,
	GameISelect+1,
	GameIClick+1,
	0,
	4,
	{
		{ String_Easy, 0, 0, 0 },
		{ String_Normal, 0, 0, 0 },
		{ String_Hard, 0, 0, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t ModesMenu =
{
	String_Modes,
	ModesMEnter+1,
	ModesIDraw+1,
	0,
	ModesIClick+1,
	0,
	8,
	{
		{ String_TEMP_NI_s, 0, -1, 0 },
		{ String_TEMP_TI_s, 0, -1, 0 },
		{ String_TEMP_SS_s, 0, -1, 0 },
		{ String_TCR_s, 0, -1, 0 },
		{ String_POWER_s, 0, -1, 0 },
		{ String_BYPASS_s, 0, -1, 0 },
		{ String_START_s, 0, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t CoilsMenu =
{
	String_Coils,
	CoilsMEnter+1,
	CoilsIDraw+1,
	CoilsISelect+1,
	CoilsIClick+1,
	CoilsMEvent+1,
	6,
	{
		{ String_NI, 0, -1, 0 },
		{ String_TI, 0, -1, 0 },
		{ String_SS, 0, -1, 0 },
		{ String_TCR, 0, -1, 0 },
		{ String_Zero_All, 0, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t Anim3dMenu =
{
	String_3D,
	Anim3dOnEnter+1,
	0,
	0,
	Anim3dOnClick+1,
	0,
	2,
	{
		{ String_Cube, &Anim3dMenu, -1, 0 },
		{ String_None, 0, 1, 0 }
	}
};

const menu_t MiscsMenu =
{
	String_Miscs,
	0,
	0,
	0,
	0,
	0,
	4,
	{
		{ String_LOGO, &LOGOMenu, -1, 0 },
		{ String_Game, &GameMenu, -1, 0 },
		{ String_3D, &Anim3dMenu, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t TimeMenu =
{
	String_DateTime,
	DTMenuOnEnter+1,
	DTMenuIDraw+1,
	0,
	0,
	DTMenuOnEvent+1,
	4,
	{
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ String_Cancel, 0, 1, 0 },
		{ String_Save, 0, 1, 0 }
	}
};

const menu_t ClockMenu =
{
	String_Clock,
	0,
	0,
	0,
	0,
	0,
	4,
	{
		{ String_DateTime, &TimeMenu, -1, 0 },
		{ String_ClkAdjust, 0, 104, 120 },
		{ String_ClkSpeed, 0, 103, 120 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t ScreenSaveMenu =
{
	String_Screen,
	ScreenSaveMenuOnEnter+1,
	ScreenSaveMenuIDraw+1,
	0,
	ScreenSaveMenuOnClick+1,
	0,
	8,
	{
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ String_Off, 0, 1, 0 }
	}
};

const menu_t ExpertMenu =
{
	String_Expert,
	0,
	ExpertMenuIDraw+1,
	0,
	ExpertMenuOnClick+1,
	0,
	2,
	{
		{ String_USB, 0, -1, 0 },
		{ String_Exit, 0, 1, 30 }
	}
};

const menu_t ScreenMenu =
{
	String_Screen,
	0,
	0,
	0,
	ScreenIClick+1,
	0,
	3,
	{
		{ String_Contrast, 0, 101, 10 },
		{ String_Protec, &ScreenSaveMenu, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t MainMenu =
{
	String_Menus,
	0,
	0,
	0,
	0,
	0,
	7,
	{
		{ String_Screen, &ScreenMenu, -1, 0 },
		{ String_Coils, &CoilsMenu, -1, 0 },
		{ String_Clock, &ClockMenu, -1, 0 },
		{ String_Modes, &ModesMenu, -1, 0 },
		{ String_Expert, &ExpertMenu, -1, 0 },
		{ String_Miscs, &MiscsMenu, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

//-----------------------------------------------------------------------------

__myevic__ void DrawMenu()
{
	unsigned int firstitem;

	if ( CurrentMenu == 0 )
	{
		CurrentMenu = &MainMenu;
		CurrentMenuItem = 0;
	}

	if ( !CurrentMenu->nitems )
	{
		MainView();
		return;
	}

	if ( CurrentMenuItem >= CurrentMenu->nitems ) CurrentMenuItem = 0;

	DrawString( CurrentMenu->caption, 4, 6 );
	DrawHLine( 0, 16, 63, 1 );

	if (( CurrentMenu->nitems > 7 ) && ( CurrentMenuItem > 3 ))
	{
		if ( CurrentMenuItem > CurrentMenu->nitems-4 )
		{
			firstitem = CurrentMenu->nitems - 7;
		}
		else
		{
			firstitem = CurrentMenuItem - 3;
		}
	}
	else
	{
		firstitem = 0;
	}

	for ( int i = 0; i < 7; ++i )
	{
		if ( firstitem + i >= CurrentMenu->nitems ) break;

		if ( CurrentMenu->mitems[firstitem+i].caption )
		{
			if ( CurrentMenuItem == firstitem + i )
			{
				DrawFillRect( 0, 18+14*i, 63, 30+14*i, 1 );
				DrawStringInv( CurrentMenu->mitems[firstitem+i].caption, 4, 20+14*i );
			}
			else
			{
				DrawString( CurrentMenu->mitems[firstitem+i].caption, 4, 20+14*i );
			}
		}

		if ( CurrentMenu->on_drawitem )
		{
			CurrentMenu->on_drawitem( firstitem + i, 18+14*i, CurrentMenuItem == firstitem + i );
		}
	}
}

//-----------------------------------------------------------------------------

__myevic__ int MenuEvent( int event )
{
	int vret = 1;

	if ( !CurrentMenu )
	{
		MainView();
		return 1;
	}

	if ( CurrentMenu->on_event )
	{
		ScreenDuration = 10;
		vret = CurrentMenu->on_event( event );
		if ( vret ) return 1;
	}

	switch ( event )
	{
		case 1:
		{
			KeyPressTime |= 0x8000;

			ScreenDuration = 10;
			if ( CurrentMenu->on_clickitem ) CurrentMenu->on_clickitem();

			mitem_t const *mi = &CurrentMenu->mitems[CurrentMenuItem];

			if ( mi->submenu )
			{
				CurrentMenu = mi->submenu;
				CurrentMenuItem = 0;

				if ( CurrentMenu->on_enter ) CurrentMenu->on_enter();

				gFlags.refresh_display = 1;
			}
			else if ( (signed char)mi->screen > 0 )
			{
				CurrentMenu = 0;
				CurrentMenuItem = 0;

				if ( mi->screen == 1 )
				{
					MainView();
				}
				else
				{
					Screen = mi->screen;
					ScreenDuration = mi->sduration;
				}

				gFlags.refresh_display = 1;
			}

			vret = 1;

		}	break;

		case 2:
		{
			if ( CurrentMenuItem < CurrentMenu->nitems - 1 )
			{
				++CurrentMenuItem;
			}
			else
			{
				CurrentMenuItem = 0;
			}

			ScreenDuration = 10;
			gFlags.refresh_display = 1;

			if ( CurrentMenu->on_selectitem ) CurrentMenu->on_selectitem();

			vret = 1;

		}	break;

		case 3:
		{
			if ( CurrentMenuItem )
			{
				--CurrentMenuItem;
			}
			else
			{
				CurrentMenuItem = CurrentMenu->nitems - 1;
			}

			ScreenDuration = 10;
			gFlags.refresh_display = 1;

			if ( CurrentMenu->on_selectitem ) CurrentMenu->on_selectitem();

			vret = 1;

		}	break;

		case 15:
			vret = 1;
			break;

		default:
			break;
	}
	return vret;
}
