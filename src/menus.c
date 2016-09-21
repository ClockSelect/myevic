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
	struct menu_s const *parent;
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

__myevic__ void IFMenuIDraw( int it, int line, int sel )
{
	if ( it > CurrentMenu->nitems - 2 )
		return;

	DrawFillRect( 40, line, 63, line+12, 0 );
	
	switch ( it )
	{
		case 0:	// Batt%
			DrawString( dfStatus.battpc ? String_On : String_Off, 44, line+2 );
			break;

		case 1:	// 1Watt
			DrawString( dfStatus.onewatt ? String_On : String_Off, 44, line+2 );
			break;

		case 2:	// Font
			DrawImage( 44, line+2, dfStatus.font ? 0x9D : 0x9C );
			break;

		default:
			break;
	}
}


__myevic__ void IFMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:	// Batt%
			dfStatus.battpc ^= 1;
			break;
		
		case 1:	// 1Watt
			dfStatus.onewatt ^= 1;
			WattsInc = dfStatus.onewatt ? 10 : 1;
			break;
		
		case 2:	// Font
			dfStatus.font ^= 1;
			DisplaySetFont();
			break;
		
		case 3:	// Exit
			UpdateDataFlash();
			return;
	}

	UpdateDFTimer = 50;
	gFlags.refresh_display = 1;
}


//-----------------------------------------------------------------------------

__myevic__ void PreheatIDraw( int it, int line, int sel )
{
	if ( it > 2 ) return;

	int x, v, dp, nd, img;

	switch ( it )
	{
		case 0:
			DrawFillRect( 30, line, 63, line+12, 0 );
			DrawImage( 54, line+2, dfStatus.phpct ? 0xC2 : 0x98 );
			return;

		case 1 : 
			v = dfPreheatPwr;
			nd = ( v < 100 ) ? 2 : 3;
			dp = ( v < 1000 ) ? 1 : 0;
			img = 0x98;
			x = ( v < 100 ) ? 38 : ( v < 1000 ) ? 32 : 34;
			if ( dfStatus.phpct )
			{
				img = 0xC2;
				dp = 0;
				x += 2;
			}
			break;

		case 2 : 
			v = dfPreheatTime / 10;
			dp = 1;
			nd = 2;
			img = 0x94;
			x = 38;
			break;

		default:
			return;
	}

	if ( gFlags.edit_value && sel )
	{
		DrawFillRect( 0, line, 29, line+12, 0 );
		DrawString( CurrentMenu->mitems[it].caption, 4, line +2 );
		DrawFillRect( 30, line, 63, line+12, 1 );
		if ( v == 0 && it == 2 )
		{
			DrawStringInv( String_Off, 37, line+2 );
		}
		else
		{
			DrawValueInv( x, line+2, v, dp, 0x0B, nd );
			DrawImageInv( 54, line+2, img );
		}
	}
	else
	{
		DrawFillRect( 30, line, 63, line+12, 0 );
		if ( v == 0 && it == 2 )
		{
			DrawString( String_Off, 37, line+2 );
		}
		else
		{
			DrawValue( x, line+2, v, dp, 0x0B, nd );
			DrawImage( 54, line+2, img );
		}
	}
}


__myevic__ int PreheatMEvent( int event )
{
	int vret = 0;
	if ( CurrentMenuItem > 2 )
		return vret;

	switch ( event )
	{
		case 1:
			if ( CurrentMenuItem == 0 )
			{
				dfStatus.phpct ^= 1;
				if ( dfStatus.phpct )
				{
					dfPreheatPwr = 100 * dfPreheatPwr / dfPower;
					if ( dfPreheatPwr > 200 ) dfPreheatPwr = 200;
					if ( dfPreheatPwr < 10 ) dfPreheatPwr = 10;
				}
				else
				{
					dfPreheatPwr = dfPower * dfPreheatPwr / 100;
					if ( dfPreheatPwr > MaxPower ) dfPreheatPwr = MaxPower;
					if ( dfPreheatPwr < 10 ) dfPreheatPwr = 10;
				}
			}
			else
			{
				gFlags.edit_value ^= 1;
			}
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

		case 2:
			if ( gFlags.edit_value )
			{
				if ( CurrentMenuItem == 1 )
				{
					if ( KeyTicks >= 105 )
					{
						if ( dfPreheatPwr < ( dfStatus.phpct ? 200 : MaxPower ) )
						{
							dfPreheatPwr -= dfPreheatPwr % 10;
							dfPreheatPwr += 10;
						}
					}
					else
					{
						if ( dfPreheatPwr < ( dfStatus.phpct ? 200 : MaxPower ) )
						{
							if ( dfPreheatPwr < 1000 )
							{
								if ( dfStatus.phpct ) ++dfPreheatPwr;
								else
								{
									dfPreheatPwr += WattsInc;
									if ( dfPreheatPwr > MaxPower ) dfPreheatPwr = MaxPower;
								}
							}
							else dfPreheatPwr += 10;
						}
						else dfPreheatPwr = 10;
					}
				}
				else if ( CurrentMenuItem == 2 )
				{
					if ( dfPreheatTime < 200 ) dfPreheatTime += 10;
					else if ( KeyTicks < 5 ) dfPreheatTime = 0;
				}
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
				vret = 1;
			}
			break;

		case 3:
			if ( gFlags.edit_value )
			{
				if ( CurrentMenuItem == 1 )
				{
					if ( KeyTicks >= 105 )
					{
						if ( dfPreheatPwr > 10 )
						{
							if ( dfPreheatPwr % 10 )
								dfPreheatPwr -= dfPreheatPwr % 10;
							else
								dfPreheatPwr -= 10;
						}
					}
					else
					{
						if ( dfPreheatPwr > 1000 ) dfPreheatPwr -= 10;
						else if ( dfPreheatPwr > 10 )
						{
							if ( dfStatus.phpct ) --dfPreheatPwr;
							else
							{
								dfPreheatPwr -= WattsInc;
								if ( dfPreheatPwr < 10 ) dfPreheatPwr = 10;
							}
						}
						else
						{
							dfPreheatPwr = ( dfStatus.phpct ? 200 : MaxPower );
						}
					}
				}
				else if ( CurrentMenuItem == 2 )
				{
					if ( dfPreheatTime > 0 ) dfPreheatTime -= 10;
					else if ( KeyTicks < 5 ) dfPreheatTime = 200;
				}
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
				vret = 1;
			}
			break;

		case EVENT_LONG_FIRE:
			UpdateDataFlash();
			MainView();
			vret = 1;
			break;

	}
	return vret;
}


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

		case 1:
			DrawFillRect( 32, line, 63, line+12, 0 );
			if ( dfStatus.dbgena )
				DrawString( String_ON, 36, line+2 );
			else
				DrawString( String_OFF, 36, line+2 );
			break;

		case 2:
			DrawFillRect( 32, line, 63, line+12, 0 );
			if ( dfStatus.x32off )
				DrawString( String_OFF, 36, line+2 );
			else
				DrawString( String_ON, 36, line+2 );
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
			dfStatus.dbgena ^= 1;
			if ( ! dfStatus.dbgena ) gFlags.debug = 0;
			gFlags.refresh_display = 1;
			break;
			
		case 2:
			dfStatus.x32off ^= 1;
			gFlags.refresh_display = 1;
			break;
			
		case 3:
			UpdateDataFlash();
			InitUSB();
			MainView();
			break;
	}
}

//-----------------------------------------------------------------------------

__myevic__ void ScreenSaveOnEnter()
{
	CurrentMenuItem = dfScreenSaver;
}


__myevic__ void ScreenSaveOnSelect()
{
	dfScreenSaver = CurrentMenuItem;
	UpdateDFTimer = 50;
}


//-----------------------------------------------------------------------------

__myevic__ void ScreenProtMenuIDraw( int it, int line, int sel )
{
	int v;

	switch ( it )
	{
		case 0:	// Saver
			DrawFillRect( 38, line, 63, line+12, 0 );
			v = ScrSaveTimes[dfScreenProt];
			if ( v )
			{
				DrawValue( ( v < 10 ) ? 47 : 41, line+2, v, 0, 0x0B, 0 );
				DrawImage( 55, line+2, 0x8E );
			}
			else
			{
				DrawString( String_Off, 42, line+2 );
			}
			break;

		case 1:	// Main
			DrawFillRect( 38, line, 63, line+12, 0 );
			v = ScrMainTimes[dfScrMainTime];
			DrawValue( ( v < 10 ) ? 49 : 43, line+2, v, 0, 0x0B, 0 );
			DrawImage( 57, line+2, 0x94 );
			break;

		default:
			break;
	}
}


__myevic__ void ScreenProtMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:	// Saver
			if ( ++dfScreenProt > 7 )
				dfScreenProt = 0;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			break;

		case 1:	// Main
			if ( ++dfScrMainTime > 4 )
				dfScrMainTime = 0;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			break;

		case 2:
			UpdateDataFlash();
			break;

		default:
			break;
	}
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
	CurrentMenuItem = dfStatus.nologo ? 1 : 0;
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
	if ( CurrentMenuItem == CurrentMenu->nitems - 1 )
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
	
	ScreenRefreshTimer = 5;
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
							rtd.u32Month = rtd.u32Month %12 + 1;
							break;
						case 2:
							rtd.u32Day = rtd.u32Day %31 + 1;
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
// Forward declarations for parent menu pointers

const menu_t MainMenu;
const menu_t CoilsMenu;
const menu_t ClockMenu;
const menu_t ScreenMenu;
const menu_t MiscsMenu;

//-----------------------------------------------------------------------------

const menu_t LOGOMenu =
{
	String_LOGO,
	&MiscsMenu,
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
	&MiscsMenu,
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
	&MainMenu,
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

const menu_t PreheatMenu =
{
	String_Preheat,
	&CoilsMenu,
	0,
	PreheatIDraw+1,
	0,
	0,
	PreheatMEvent+1,
	4,
	{
		{ String_Unit, 0, -1, 0 },
		{ String_Pwr, 0, -1, 0 },
		{ String_Time, 0, -1, 0 },
		{ String_Exit, 0, 1, 0 },
	}
};

const menu_t CoilsMgmtMenu =
{
	String_Coils,
	&CoilsMenu,
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

const menu_t CoilsMenu =
{
	String_Coils,
	&MainMenu,
	0,
	0,
	0,
	0,
	0,
	4,
	{
		{ String_Manage, &CoilsMgmtMenu, -1, 0 },
		{ String_Preheat, &PreheatMenu, -1, 0 },
		{ String_TCRSet, 0, 59, 10 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t Anim3dMenu =
{
	String_3D,
	&MiscsMenu,
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
	&MainMenu,
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
	&ClockMenu,
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
	&MainMenu,
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

const menu_t ScreenProtMenu =
{
	String_Screen,
	&ScreenMenu,
	0,
	ScreenProtMenuIDraw+1,
	0,
	ScreenProtMenuOnClick+1,
	0,
	3,
	{
		{ String_Saver, 0, -1, 0 },
		{ String_Main, 0, -1, 0 },
		{ String_Exit, 0, 1, 30 }
	}
};

const menu_t ExpertMenu =
{
	String_Expert,
	&MainMenu,
	0,
	ExpertMenuIDraw+1,
	0,
	ExpertMenuOnClick+1,
	0,
	4,
	{
		{ String_USB, 0, -1, 0 },
		{ String_DBG, 0, -1, 0 },
		{ String_X32, 0, -1, 0 },
		{ String_Exit, 0, 1, 30 }
	}
};

const menu_t ScreenSaveMenu =
{
	String_Saver,
	&ScreenMenu,
	ScreenSaveOnEnter+1,
	0,
	ScreenSaveOnSelect+1,
	0,
	0,
	6,
	{
		{ String_None, 0, 1, 0 },
		{ String_LOGO, 0, 1, 0 },
		{ String_Analog_Clk, 0, 1, 0 },
		{ String_Digit_Clk, 0, 1, 0 },
		{ String_Cube, 0, 1, 0 },
		{ String_Qix, 0, 1, 0 }
	}
};

const menu_t ScreenMenu =
{
	String_Screen,
	&MainMenu,
	0,
	0,
	0,
	ScreenIClick+1,
	0,
	4,
	{
		{ String_Contrast, 0, 101, 10 },
		{ String_Protec, &ScreenProtMenu, -1, 0 },
		{ String_Saver, &ScreenSaveMenu, -1, 0 },
		{ String_Exit, 0, 1, 0 }
	}
};

const menu_t IFMenu =
{
	String_Interface,
	&MainMenu,
	0,
	IFMenuIDraw+1,
	0,
	IFMenuOnClick+1,
	0,
	4,
	{
		{ String_BattPC, 0, -1, 0 },
		{ String_1Watt, 0, -1, 0 },
		{ String_Font, 0, -1, 0 },
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
	0,
	8,
	{
		{ String_Screen, &ScreenMenu, -1, 0 },
		{ String_Coils, &CoilsMenu, -1, 0 },
		{ String_Clock, &ClockMenu, -1, 0 },
		{ String_Modes, &ModesMenu, -1, 0 },
		{ String_Miscs, &MiscsMenu, -1, 0 },
		{ String_Interface, &IFMenu, -1, 0 },
		{ String_Expert, &ExpertMenu, -1, 0 },
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

	if ( CurrentMenu && CurrentMenu->on_event )
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

		case EVENT_EXITMENUS:
			MainView();
			vret = 1;
			break;

		case EVENT_PARENTMENU:
		{
			switch ( Screen )
			{
				case 59:
					CurrentMenu = &CoilsMenu;
					break;
					
				case 101:
					CurrentMenu = &ScreenMenu;
					break;

				case 102:
					if ( CurrentMenu )
					{
						CurrentMenu = CurrentMenu->parent;
					}
					break;

				case 103:
				case 104:
					CurrentMenu = &ClockMenu;
					break;

				default:
					CurrentMenu = 0;
					break;
			}

			if ( CurrentMenu )
			{
				CurrentMenuItem = 0;
				if ( CurrentMenu->on_enter ) CurrentMenu->on_enter();
				Screen = 102;
				ScreenDuration = 10;
				gFlags.refresh_display = 1;
			}
			else
			{
				MainView();
			}

			vret = 1;
			break;
		}

		default:
			break;
	}
	return vret;
}
