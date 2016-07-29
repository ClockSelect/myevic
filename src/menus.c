#include "myevic.h"
#include "screens.h"
#include "display.h"
#include "myrtc.h"
#include "dataflash.h"
#include "miscs.h"
#include "M451Series.h"

//=============================================================================
// Menus

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
	CurrentMenuItem = ( dfStatus >> 3 ) & 1;
}

__myevic__ void LogoISelect()
{
	dfStatus &= ~0x8u;
	dfStatus |= CurrentMenuItem << 3;
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
	UpdateDataFlash();
	if ( dfFBSpeed <= 2 )
	{
		Flags68 |= 0x200u;
		Screen = 0;
		Flags64 |= 0x24000u;
		BatRefreshTmr = 0;
		SleepTimer = 3000;
		InitTimeouts();
		ClearScreenBuffer();
		DisplayRefresh();
	//	fbBirdAnim( 24 );
		CreateTimeout( fbStartScreen + 1 );
	}
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
		Flags64 |= 0x20000;
	}
	else
	{
		UpdateDataFlash();
	}
}

//-----------------------------------------------------------------------------

__myevic__ void CoilsMEnter()
{
	if ( dfMode < 4 ) CurrentMenuItem = dfMode;
	else CurrentMenuItem = 4;
}

__myevic__ void CoilsIDraw( int it, int line, int sel )
{
	if ( it > 3 ) return;
	DrawFillRect( 32, line, 63, line+12, 0 );
	int rez = 0;
	short img = 0xC0;
	switch ( it )
	{
		case 0 : rez = dfRezNI; if (dfRezLockedNI) img = 0xC3; break;
		case 1 : rez = dfRezTI; if (dfRezLockedTI) img = 0xC3; break;
		case 2 : rez = dfRezSS; if (dfRezLockedSS) img = 0xC3; break;
		case 3 : rez = dfRezTCR; if (dfRezLockedTCR) img = 0xC3; break;
	}
	DrawValue( 34, line+2, rez, 2, 0x0B, 3 );
	DrawImage( 56, line+2, img );
}

__myevic__ void CoilsIClick()
{
	switch ( CurrentMenuItem )
	{
		case 0 : dfRezNI  = 0; dfRezLockedNI  = 0; break;
		case 1 : dfRezTI  = 0; dfRezLockedTI  = 0; break;
		case 2 : dfRezSS  = 0; dfRezLockedSS  = 0; break;
		case 3 : dfRezTCR = 0; dfRezLockedTCR = 0; break;
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
		Flags64 |= 0x20000;
	}
}

//-----------------------------------------------------------------------------

static signed char dt_sel = 0;
static S_RTC_TIME_DATA_T rtd;

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
					Flags64 |= 0x20000;
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
					Flags64 |= 0x20000;
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
					Flags64 |= 0x20000;
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
					Flags64 |= 0x20000;
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
					Flags64 |= 0x20000;
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

__myevic__ void MainIClick()
{
	if ( CurrentMenu->mitems[CurrentMenuItem].screen == 101 ) Flags68 |= 0x10;
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
		{ String_On, 0, 1, 30 },
		{ String_Off, 0, 1, 30 }
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
		{ String_Exit, 0, 1, 30 }
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
		{ String_Exit, 0, 1, 30 }
	}
};

const menu_t CoilsMenu =
{
	String_Coils,
	CoilsMEnter+1,
	CoilsIDraw+1,
	0,
	CoilsIClick+1,
	0,
	6,
	{
		{ String_NI, 0, -1, 0 },
		{ String_TI, 0, -1, 0 },
		{ String_SS, 0, -1, 0 },
		{ String_TCR, 0, -1, 0 },
		{ String_Zero_All, 0, -1, 0 },
		{ String_Exit, 0, 1, 30 }
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
		{ String_Exit, 0, 1, 30 }
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
		{ String_Exit, 0, 1, 30 }
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
		{ String_Cancel, 0, 1, 30 },
		{ String_Save, 0, 1, 30 }
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
	3,
	{
		{ String_DateTime, &TimeMenu, -1, 0 },
		{ String_ClkSpeed, 0, 103, 30 },
		{ String_Exit, 0, 1, 30 }
	}
};

const menu_t MainMenu =
{
	String_Menus,
	0,
	0,
	0,
	MainIClick+1,
	0,
	6,
	{
		{ String_Contrast, 0, 101, 10 },
		{ String_Coils, &CoilsMenu, -1, 0 },
		{ String_Clock, &ClockMenu, -1, 0 },
		{ String_Modes, &ModesMenu, -1, 0 },
		{ String_Miscs, &MiscsMenu, -1, 0 },
		{ String_Exit, 0, 1, 30 }
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

				Flags64 |= 0x20000;
			}
			else if ( mi->sduration > 0 )
			{
				CurrentMenu = 0;
				CurrentMenuItem = 0;

				Screen = mi->screen;
				ScreenDuration = mi->sduration;

				Flags64 |= 0x20000;
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
			Flags64 |= 0x20000;

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
			Flags64 |= 0x20000;

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
