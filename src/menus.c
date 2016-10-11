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
#include "battery.h"

//=============================================================================
// MENUS
//-----------------------------------------------------------------------------

typedef struct menu_s menu_t;
typedef struct mitem_s mitem_t;
typedef struct mdata_s mdata_t;
typedef struct mbitdesc_s mbitdesc_t;

enum {
	MACTION_SUBMENU,
	MACTION_DATA
};

enum {
	MITYPE_BIT
};

struct mbitdesc_s
{
	const uint8_t div;
	const uint8_t pos;
	const uint16_t* const on;
	const uint16_t* const off;
};

struct mdata_s
{
	void* const ptr;
	const void* const desc;
	const uint8_t type;
	const uint8_t bitpos;
};

struct mitem_s
{
	const uint16_t* const caption;
	const void* const action;
	const uint8_t event;
	const uint8_t action_type;
};

struct menu_s
{
	const uint16_t* const caption;
	const struct menu_s const *parent;
	void (*on_enter)();
	void (*on_drawitem)( int mi, int y, int sel );
	void (*on_selectitem)();
	void (*on_clickitem)();
	int  (*on_event)( int event );
	const short nitems;
	const mitem_t mitems[];
};

const menu_t const *CurrentMenu;
uint8_t CurrentMenuItem;


//-----------------------------------------------------------------------------

__myevic__ void VapingMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
		case 2:	// Protec
			DrawFillRect( 34, line, 63, line+12, 0 );
			DrawImage( 58, line+2, 0x94 );
			DrawValueRight( 56, line+2, dfProtec, 1, 0x0B, 0 );
			if ( sel && gFlags.edit_value )
				InvertRect( 0, line, 63, line+12 );
			break;

		case 3:	// Vaped
			DrawFillRect( 39, line, 63, line+12, 0 );
			DrawString( dfStatus.vapedml ? String_ml : String_mld, 43, line+2 );
			break;
		
		case 4: // mL/kJ
			DrawFillRect( 39, line, 63, line+12, 0 );
			DrawValueRight( 61, line+2, dfVVRatio, 0, 0x0B, 0 );
			if ( sel && gFlags.edit_value )
				InvertRect( 0, line, 63, line+12 );
			break;
	}
}


__myevic__ void VapingMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 2:	// Protec
			gFlags.edit_value ^= 1;
			break;

		case 3:	// Vaped
			dfStatus.vapedml ^= 1;
			UpdateDFTimer = 50;
			break;

		case 4: // mL/kJ
			gFlags.edit_value ^= 1;
			break;
	}

	gFlags.refresh_display = 1;
}


__myevic__ int VapingMenuOnEvent( int event )
{
	int vret = 0;

	if ( !gFlags.edit_value )
		return vret;

	switch ( event )
	{
		case 2:
			switch ( CurrentMenuItem )
			{
				case 2: // Protec
					if ( ++dfProtec > 100 )
					{
						if ( KeyTicks < 5 ) dfProtec = 20;
						else dfProtec = 100;
					}
					vret = 1;
					break;
				
				case 4: // mL/kJ
					if ( ++dfVVRatio > VVEL_MAX_RATIO )
					{
						if ( KeyTicks < 5 ) dfVVRatio = VVEL_MIN_RATIO;
						else dfVVRatio = VVEL_MAX_RATIO;
					}
					vret = 1;
					break;
			}
			break;

		case 3:
			switch ( CurrentMenuItem )
			{
				case 2: // Protec
					if ( --dfProtec < 20 )
					{
						if ( KeyTicks < 5 ) dfProtec = 100;
						else dfProtec = 20;
					}
					vret = 1;
					break;

				case 4: // mL/kJ
					if ( --dfVVRatio < VVEL_MIN_RATIO )
					{
						if ( KeyTicks < 5 ) dfVVRatio = VVEL_MAX_RATIO;
						else dfVVRatio = VVEL_MIN_RATIO;
					}
					vret = 1;
					break;
			}
			break;
	}

	if ( vret )
	{
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}

	return vret;
}

//-----------------------------------------------------------------------------

__myevic__ void ClicksMenuIDraw( int it, int line, int sel )
{
	if ( it > CurrentMenu->nitems - 2 )
		return;

	DrawFillRect( 15, line, 63, line+12, 0 );

	switch ( dfClick[it] )
	{
		default:
		case CLICK_ACTION_NONE:
			DrawString( String_None, 20, line+2 );
			break;

		case CLICK_ACTION_EDIT:
			DrawString( String_Edit, 20, line+2 );
			break;

		case CLICK_ACTION_CLOCK:
			DrawString( String_Clock, 20, line+2 );
			break;

		case CLICK_ACTION_TDOM:
			DrawString( String_PPwr, 20, line+2 );
			break;

		case CLICK_ACTION_NEXT_MODE:
			DrawString( String_ModePlus, 20, line+2 );
			break;

		case CLICK_ACTION_ON_OFF:
			DrawString( String_OnOff, 20, line+2 );
			break;
	}
}


__myevic__ void ClicksMenuOnClick()
{
	if ( CurrentMenuItem > CurrentMenu->nitems - 2 )
		return;

	if ( ++dfClick[CurrentMenuItem] >= CLICK_ACTION_MAX )
		dfClick[CurrentMenuItem] = CLICK_ACTION_NONE;

	UpdateDFTimer = 50;
	gFlags.refresh_display = 1;
}


//-----------------------------------------------------------------------------

__myevic__ void ClockMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
		case 4:	// Format
			DrawFillRect( 36, line, 63, line+12, 0 );
			DrawString( dfStatus.mdy ? String_MDY : String_DMY, 40, line+2 );
			break;

		case 5:	// Dial
			DrawFillRect( 36, line, 63, line+12, 0 );
			DrawImage( 40, line+2, dfStatus.digclk ? 0x9F : 0x9C );
			break;
	}
}


__myevic__ void ClockMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:
			Event = EVENT_SET_TIME;
			break;

		case 1:
			Event = EVENT_SET_DATE;
			break;

		case 4:	// Format
			dfStatus.mdy ^= 1;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			break;

		case 5:	// Dial
			dfStatus.digclk ^= 1;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			break;
	}
}


//-----------------------------------------------------------------------------

__myevic__ void IFMenuIDraw( int it, int line, int sel )
{
	if ( it > CurrentMenu->nitems - 2 )
		return;

	DrawFillRect( 40, line, 63, line+12, 0 );

	switch ( it )
	{
		case 0:	// Batt%
			if ( !dfStatus.battpc )
			{
				DrawString( String_Off, 44, line+2 );
			}
			else
			{
				if ( dfStatus.battv )
				{
					DrawImage( 44, line+2, 0xB1 );
				}
				else
				{
					DrawImage( 44, line+2, 0xC2 );
				}
			}
			break;

		case 1:	// 1Watt
			DrawString( dfStatus.onewatt ? String_On : String_Off, 44, line+2 );
			break;

		case 2:	// 1C5F
			DrawString( dfStatus.onedegree ? String_On : String_Off, 44, line+2 );
			break;

		case 3:	// Wake -+
			DrawString( dfStatus.wakeonpm ? String_On : String_Off, 44, line+2 );
			break;

		case 4:	// Font
			DrawImage( 44, line+2, dfStatus.font ? 0x9D : 0x9C );
			break;

		case 5:	// Temp
			DrawImage( 44, line+2, dfIsCelsius ? 0xC9 : 0xC8 );
			break;

		case 6:	// TDom
			DrawString( dfStatus.priopwr ? String_On : String_Off, 44, line+2 );
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
			if ( !dfStatus.battpc )
			{
				dfStatus.battpc = 1;
				dfStatus.battv = 0;
			}
			else if ( !dfStatus.battv )
			{
				dfStatus.battv = 1;
			}
			else
			{
				dfStatus.battv = 0;
				dfStatus.battpc = 0;
			}
			break;

		case 1:	// 1Watt
			dfStatus.onewatt ^= 1;
			if ( dfStatus.onewatt )
			{
				WattsInc = 10;
				dfPower -= dfPower % 10;
				dfTCPower -= dfTCPower % 10;
				dfPreheatPwr -= dfPreheatPwr % 10;
			}
			else
			{
				WattsInc = 1;
			}
			break;

		case 2:	// 1C5F
			dfStatus.onedegree ^= 1;
			if ( !dfStatus.onedegree )
			{
				dfTemp -= dfTemp % ( dfIsCelsius ? 5 : 10 );
			}
			break;

		case 3:	// Wake -+
			dfStatus.wakeonpm ^= 1;
			break;

		case 4:	// Font
			dfStatus.font ^= 1;
			DisplaySetFont();
			break;

		case 5:	// Temp
			dfIsCelsius ^= 1;
			if ( dfIsCelsius )
			{
				dfTemp = FarenheitToC( dfTemp );
				if ( dfTemp < 100 ) dfTemp = 100;
				if ( dfTemp > 315 ) dfTemp = 315;
			}
			else
			{
				dfTemp = CelsiusToF( dfTemp );
				int rem = dfTemp % 5;
				dfTemp -= rem;
				if ( rem >= 3 ) dfTemp += 5;
				if ( dfTemp < 200 ) dfTemp = 200;
				if ( dfTemp > 600 ) dfTemp = 600;
			}
			break;

		case 6:	// TDom
			dfStatus.priopwr ^= 1;
			break;

		default: // Exit
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

	int v, dp, img;

	switch ( it )
	{
		case 0:	// Unit
			DrawFillRect( 30, line, 63, line+12, 0 );
			DrawImage( 54, line+2, dfStatus.phpct ? 0xC2 : 0x98 );
			return;

		case 1:	// Power
			v = dfPreheatPwr;
			if ( dfStatus.phpct )
			{
				int p = dfPower * dfPreheatPwr / 100;
				if ( p > AtoMaxPower ) p = AtoMaxPower;
				if ( p < 10 ) p = 10;
				if ( p < 1000 )
				{
					dp = 1;
				}
				else
				{
					p = p / 10;
					dp = 0;
				}
				DrawImage(  4, 90, 0xAB );
				DrawHLine(  12, 93, 14, 1 );
				DrawHLine(  12, 96, 14, 1 );
				DrawValueRight( 37, 90, p, dp, 0x0B, 0 );
				DrawImage( 39, 90, 0x98 );

				dp = 0;
				img = 0xC2;
			}
			else
			{
				dp = ( v < 1000 ) ? 1 : 0;
				img = 0x98;
			}
			break;

		case 2:	// Time
			v = dfPreheatTime / 10;
			dp = 1;
			img = 0x94;
			break;

		default:
			return;
	}

	DrawFillRect( 30, line, 63, line+12, 0 );

	if ( v == 0 && it == 2 )
	{
		DrawString( String_Off, 37, line+2 );
	}
	else
	{
		DrawValueRight( 52, line+2, v, dp, 0x0B, 0 );
		DrawImage( 54, line+2, img );
	}

	if ( gFlags.edit_value && sel )
	{
		InvertRect( 0, line, 63, line+12 );
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
					if ( dfStatus.onewatt )
					{
						dfPreheatPwr -= dfPreheatPwr % 10;
					}
					if ( dfStatus.phpct )
					{
						PowerPlus( &dfPreheatPwr, 10, 200 );
					}
					else
					{
						PowerPlus( &dfPreheatPwr, AtoMinPower, AtoMaxPower );
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
					if ( dfStatus.onewatt )
					{
						dfPreheatPwr -= dfPreheatPwr % 10;
					}
					if ( dfStatus.phpct )
					{
						PowerMinus( &dfPreheatPwr, 10, 200 );
					}
					else
					{
						PowerMinus( &dfPreheatPwr, AtoMinPower, AtoMaxPower );
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
	if ( it >= CurrentMenu->nitems - 1 )
		return;

	DrawFillRect( 32, line, 63, line+12, 0 );

	switch ( it )
	{
		case 0:	// USB
			if ( dfStatus.vcom )
				DrawString( String_COM, 36, line+2 );
			else if ( dfStatus.storage )
				DrawString( String_DSK, 36, line+2 );
			else
				DrawString( String_HID, 36, line+2 );
			break;

		case 1:	// DBG
			if ( dfStatus.dbgena )
				DrawString( String_ON, 36, line+2 );
			else
				DrawString( String_OFF, 36, line+2 );
			break;

		case 2:	// X32
			if ( dfStatus.x32off )
				DrawString( String_OFF, 36, line+2 );
			else
				DrawString( String_ON, 36, line+2 );
			break;

		case 3:	// LSL
			if ( dfStatus.lsloff )
				DrawString( String_OFF, 36, line+2 );
			else
				DrawString( String_ON, 36, line+2 );
			break;

		case 4:	// NFE
			if ( dfStatus.nfe )
				DrawString( String_ON, 36, line+2 );
			else
				DrawString( String_OFF, 36, line+2 );
			break;

		case 5:	// BAT
			DrawString( GetBatteryName(), 36, line+2 );
			break;

		case 6:	// SHR
			DrawValue( 36, line+2, AtoShuntRez, 0, 0x0B, 3 );
			if ( gFlags.edit_value && sel )
				InvertRect( 0, line, 63, line+12 );
			break;

		case 7:	// BVO
		{
			int i = 0;
			uint16_t bvo = ( dfBVOffset[i] >= 0 ) ? dfBVOffset[i] : -dfBVOffset[i];
			DrawImage( 36, line+2, ( dfBVOffset[i] >= 0 ) ? 0xFC : 0xFD );
			DrawValue( 44, line+2, bvo * 10, 0, 0x0B, 2 );
			if ( gFlags.edit_value && sel )
				InvertRect( 0, line, 63, line+12 );
			break;
		}

		default:
			break;
	}
}

__myevic__ void ExpertMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:	// USB
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
			InitUSB();
			break;

		case 1:	// DBG
			dfStatus.dbgena ^= 1;
			if ( ! dfStatus.dbgena ) gFlags.debug = 0;
			break;

		case 2:	// X32
			dfStatus.x32off ^= 1;
			break;

		case 3:	// LSL
			dfStatus.lsloff ^= 1;
			break;

		case 4:	// NFE
			dfStatus.nfe ^= 1;
			break;

		case 5:	// BAT
			if ( ++dfBatteryModel >= GetNBatteries() )
				dfBatteryModel = 0;
			SetBatteryModel();
			gFlags.read_battery = 1;
			NewBatteryVoltage();
			SetBatMaxPower();
			break;

		case 6:	// Shunt Rez Value
			if ( !(gFlags.edit_value ^= 1) )
				dfShuntRez = AtoShuntRez;
			break;

		case 7:	// BVO
			gFlags.edit_value ^= 1;
			break;

		case 8:	// Exit
			UpdateDataFlash();
			MainView();
			break;
	}

	gFlags.refresh_display = 1;
}

__myevic__ int ExpertMenuOnEvent( int event )
{
	int vret = 0;

	switch ( event )
	{
		case 2:	// +
			if ( !gFlags.edit_value )
				break;
			switch ( CurrentMenuItem )
			{
				case 6:	// Shunt Rez
					if ( ++AtoShuntRez > SHUNT_MAX_VALUE )
					{
						AtoShuntRez = ( KeyTicks == 0 ) ? SHUNT_MIN_VALUE : SHUNT_MAX_VALUE;
					}
					vret = 1;
					break;
				
				case 7:	// BVO
				{
					if ( ++dfBVOffset[0] > 5 )
					{
						dfBVOffset[0] = ( KeyTicks == 0 ) ? -5 : 5;
					}
					dfBVOffset[2] = dfBVOffset[1] = dfBVOffset[0];
					vret = 1;
					break;
				}
			}
			break;

		case 3:	// -
			if ( !gFlags.edit_value )
				break;
			switch ( CurrentMenuItem )
			{
				case 6:	// Shunt Rez
					if ( --AtoShuntRez < SHUNT_MIN_VALUE )
					{
						AtoShuntRez = ( KeyTicks == 0 ) ? SHUNT_MAX_VALUE : SHUNT_MIN_VALUE;
					}
					vret = 1;
					break;
				
				case 7:	// BVO
				{
					if ( --dfBVOffset[0] < -5 )
					{
						dfBVOffset[0] = ( KeyTicks == 0 ) ? 5 : -5;
					}
					dfBVOffset[2] = dfBVOffset[1] = dfBVOffset[0];
					vret = 1;
					break;
				}
			}
			break;

		case EVENT_LONG_FIRE:
			switch ( CurrentMenuItem )
			{
				case 6:	// Shunt Rez
					AtoShuntRez = GetShuntRezValue();
					dfShuntRez = 0;
					vret = 1;
					break;
			}
			break;
	}

	if ( vret )
	{
		byte_200000B3 = 2;
		AtoProbeCount = 0;
		AtoRezMilli = 0;

		dfShuntRez = AtoShuntRez;
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}

	return vret;
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

	if ( it > 1 )
		return;

	DrawFillRect( 38, line, 63, line+12, 0 );

	switch ( it )
	{
		case 0:	// Saver
			v = ScrSaveTimes[dfScreenProt];
			if ( v )
			{
				DrawValueRight( 53, line+2, v, 0, 0x0B, 0 );
				DrawImage( 55, line+2, 0x8E );
			}
			else
			{
				DrawString( String_Off, 42, line+2 );
			}
			break;

		case 1:	// Main
			v = ScrMainTimes[dfScrMainTime];
			DrawValueRight( 53, line+2, v, 0, 0x0B, 0 );
			DrawImage( 55, line+2, 0x94 );
			break;

		default:
			break;
	}

	if ( gFlags.edit_value && CurrentMenuItem == it )
	{
		InvertRect( 0, line, 63, line+12 );
	}
}


__myevic__ void ScreenProtMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0:	// Saver
		case 1:	// Main
			gFlags.edit_value ^= 1;
			gFlags.refresh_display = 1;
			break;

		case 2:
			UpdateDataFlash();
			break;

		default:
			break;
	}
}


__myevic__ int ScreenProtMenuOnEvent( int event )
{
	int vret = 0;

	if ( !gFlags.edit_value )
		return vret;

	switch ( event )
	{
		case 2:
			switch ( CurrentMenuItem )
			{
				case 0:	// Saver
					if ( ++dfScreenProt > 7 )
						dfScreenProt = 0;
					vret = 1;
					break;

				case 1:	// Main
					if ( ++dfScrMainTime > 5 )
						dfScrMainTime = 0;
					dfDimTimeout = ScrMainTimes[dfScrMainTime];
					vret = 1;
					break;
			}
			break;

		case 3:
			switch ( CurrentMenuItem )
			{
				case 0:	// Saver
					if ( !dfScreenProt-- )
						dfScreenProt = 7;
					vret = 1;
					break;

				case 1:	// Main
					if ( !dfScrMainTime-- )
						dfScrMainTime = 5;
					dfDimTimeout = ScrMainTimes[dfScrMainTime];
					vret = 1;
					break;
			}
			break;
	}

	if ( vret )
	{
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}

	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ void Object3DOnEnter()
{
	if ( gFlags.anim3d )
		CurrentMenuItem = Object3D;
	else
		CurrentMenuItem = 0;
}

__myevic__ void Object3DOnClick()
{
	Object3D = CurrentMenuItem ? : 1;
	if ( CurrentMenuItem ) gFlags.anim3d = 1;
	else gFlags.anim3d = 0;
	MainView();
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

__myevic__ void CoilsSelectRez( uint8_t mode )
{
	switch ( mode )
	{
		case 0 : CoilSelectedRez = &dfRezNI ; CoilSelectedLock = &dfRezLockedNI ; break;
		case 1 : CoilSelectedRez = &dfRezTI ; CoilSelectedLock = &dfRezLockedTI ; break;
		case 2 : CoilSelectedRez = &dfRezSS ; CoilSelectedLock = &dfRezLockedSS ; break;
		case 3 : CoilSelectedRez = &dfRezTCR; CoilSelectedLock = &dfRezLockedTCR; break;
		default:
			break;
	}
}


__myevic__ void CoilsISelect()
{
	if ( CurrentMenuItem > 3 )
	{
		gFlags.edit_value = 0;
		return;
	}
	CoilsSelectRez( CurrentMenuItem );
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
	CoilsSelectRez( it );
	rez = *CoilSelectedRez;
	if ( *CoilSelectedLock ) img = 0xC3;
	DrawFillRect( 32, line, 63, line+12, 0 );
	DrawValue( 34, line+2, rez, 2, 0x0B, 3 );
	DrawImage( 56, line+2, img );
	if ( gFlags.edit_value && sel )
	{
		InvertRect( 0, line, 63, line+12 );
	}
	CoilsSelectRez( CurrentMenuItem );
}

__myevic__ void CoilsIClick()
{
	switch ( CurrentMenuItem )
	{
		case 4 :	// Zero All
			dfRezNI  = 0; dfRezLockedNI  = 0;
			dfRezTI  = 0; dfRezLockedTI  = 0;
			dfRezSS  = 0; dfRezLockedSS  = 0;
			dfRezTCR = 0; dfRezLockedTCR = 0;
			for ( int i = 0 ; i < 10 ; ++i )
			{
				dfSavedCfgRez[i] = 0;
				dfSavedCfgPwr[i] = 0;
			}
			break;
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

	static char rmodified = 0;

	if ( CurrentMenuItem > 3 )
		return vret;

	switch ( event )
	{
		case 1:
			gFlags.edit_value ^= 1;
			if ( gFlags.edit_value )
			{
				rmodified = 0;
			}
			else if ( !rmodified && *CoilSelectedRez > 0 )
			{
				*CoilSelectedLock ^= 1;
			}
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
				rmodified = 1;
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
				rmodified = 1;
				UpdateDFTimer = 50;
				gFlags.refresh_display = 1;
				vret = 1;
			}
			break;

		case EVENT_LONG_FIRE:
			*CoilSelectedRez = 0;
			*CoilSelectedLock = 0;
			if ( CurrentMenuItem == dfMode )
			{
				ResetResistance();
				if ( AtoStatus == 4 )
				{
					*CoilSelectedRez = AtoRez;
					*CoilSelectedLock = 1;
				}
			}
			UpdateDFTimer = 50;
			gFlags.edit_value = 0;
			gFlags.refresh_display = 1;
			vret = 1;
			break;

	}
	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ void ScreenMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
	//	case 3:	// Logo
	//		dfStatus.nologo ^= 1;
	//		break;

		case 4:	// Invert
			dfStatus.invert ^= 1;
			DisplaySetInverse( dfStatus.invert );
			break;
	}

	UpdateDFTimer = 50;
	gFlags.refresh_display = 1;
}


__myevic__ void ScreenMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
	//	case 3:	// Logo
	//		DrawFillRect( 40, line, 63, line+12, 0 );
	//		DrawString( dfStatus.nologo ? String_Off : String_On, 44, line+2 );
	//		break;

		case 4:	// Invert
			DrawFillRect( 40, line, 63, line+12, 0 );
			DrawString( dfStatus.invert ? String_On : String_Off, 44, line+2 );
			break;
	}
}


//-----------------------------------------------------------------------------
// Forward declarations for parent menu pointers

const menu_t MainMenu;
const menu_t CoilsMenu;
const menu_t ClockMenu;
const menu_t ScreenMenu;
const menu_t MiscsMenu;
const menu_t IFMenu;
const menu_t VapingMenu;

//-----------------------------------------------------------------------------

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
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
		{ String_TEMP_NI_s, 0, 0, 0 },
		{ String_TEMP_TI_s, 0, 0, 0 },
		{ String_TEMP_SS_s, 0, 0, 0 },
		{ String_TCR_s, 0, 0, 0 },
		{ String_POWER_s, 0, 0, 0 },
		{ String_BYPASS_s, 0, 0, 0 },
		{ String_SMART_s, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t PreheatMenu =
{
	String_Preheat,
	&VapingMenu,
	0,
	PreheatIDraw+1,
	0,
	0,
	PreheatMEvent+1,
	4,
	{
		{ String_Unit, 0, 0, 0 },
		{ String_Pwr, 0, 0, 0 },
		{ String_Time, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 },
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
		{ String_NI, 0, 0, 0 },
		{ String_TI, 0, 0, 0 },
		{ String_SS, 0, 0, 0 },
		{ String_TCR, 0, 0, 0 },
		{ String_Zero_All, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
	3,
	{
		{ String_Manage, &CoilsMgmtMenu, 0, MACTION_SUBMENU },
		{ String_TCRSet, 0, 39, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t Object3DMenu =
{
	String_3D,
	&MiscsMenu,
	Object3DOnEnter+1,
	0,
	0,
	Object3DOnClick+1,
	0,
	3,
	{
		{ String_None, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Tetra, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Cube, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t MiscsMenu =
{
	String_Miscs,
	&ScreenMenu,
	0,
	0,
	0,
	0,
	0,
	3,
	{
		{ String_Game, &GameMenu, 0, MACTION_SUBMENU },
		{ String_3D, &Object3DMenu, 0, MACTION_SUBMENU },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t ClockMenu =
{
	String_Clock,
	&MainMenu,
	0,
	ClockMenuIDraw+1,
	0,
	ClockMenuOnClick+1,
	0,
	7,
	{
		{ String_SetTime, 0, 0, 0 },
		{ String_SetDate, 0, 0, 0 },
		{ String_ClkAdjust, 0, EVENT_CLK_ADJUST, 0 },
		{ String_ClkSpeed, 0, EVENT_CLK_SPEED, 0 },
		{ String_Fmt, 0, 0, 0 },
		{ String_Dial, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
	ScreenProtMenuOnEvent+1,
	3,
	{
		{ String_Saver, 0, 0, 0 },
		{ String_Main, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
	ExpertMenuOnEvent+1,
	9,
	{
		{ String_USB, 0, 0, 0 },
		{ String_DBG, 0, 0, 0 },
		{ String_X32, 0, 0, 0 },
		{ String_LSL, 0, 0, 0 },
		{ String_NFE, 0, 0, 0 },
		{ String_BAT, 0, 0, 0 },
		{ String_SHR, 0, 0, 0 },
		{ String_BVO, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
		{ String_None, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Clock, 0, EVENT_EXIT_MENUS, 0 },
		{ String_3D, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Logo, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Qix, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Snow, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const mbitdesc_t InvBitDesc =
{
	0, 0,
	String_Off,
	String_On
};

const mbitdesc_t TopBotDesc =
{
	0, 0,
	String_Mid,
	String_Top
};

const mdata_t LogoShowData =
{
	&dfStatus,
	&InvBitDesc,
	MITYPE_BIT,
	3
};

const mdata_t LogoWhereData =
{
	&dfStatus,
	&TopBotDesc,
	MITYPE_BIT,
	23
};

const menu_t LogoMenu =
{
	String_Logo,
	&ScreenMenu,
	0,
	0,
	0,
	0,
	0,
	3,
	{
		{ String_Show, &LogoShowData, 0, MACTION_DATA },
		{ String_Where, &LogoWhereData, 0, MACTION_DATA },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t ScreenMenu =
{
	String_Screen,
	&MainMenu,
	0,
	ScreenMenuIDraw+1,
	0,
	ScreenMenuOnClick+1,
	0,
	7,
	{
		{ String_Contrast, 0, EVENT_EDIT_CONTRAST,  },
		{ String_Protection, &ScreenProtMenu, 0, MACTION_SUBMENU },
		{ String_Saver, &ScreenSaveMenu, 0, MACTION_SUBMENU },
		{ String_Logo, &LogoMenu, 0, MACTION_SUBMENU },
		{ String_Invert, 0, 0, 0 },
		{ String_Miscs, &MiscsMenu, 0, MACTION_SUBMENU },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t ClicksMenu =
{
	String_Clicks,
	&IFMenu,
	0,
	ClicksMenuIDraw+1,
	0,
	ClicksMenuOnClick+1,
	0,
	4,
	{
		{ String_2, 0, 0, 0 },
		{ String_3, 0, 0, 0 },
		{ String_4, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 },
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
	9,
	{
		{ String_BattPC, 0, 0, 0 },
		{ String_1Watt, 0, 0, 0 },
		{ String_1C5F, 0, 0, 0 },
		{ String_WakeMP, 0, 0, 0 },
		{ String_Font, 0, 0, 0 },
		{ String_Temp, 0, 0, 0 },
		{ String_PPwr, 0, 0, 0 },
		{ String_Clicks, &ClicksMenu, 0, MACTION_SUBMENU },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const menu_t VapingMenu =
{
	String_Vaping,
	&MainMenu,
	0,
	VapingMenuIDraw+1,
	0,
	VapingMenuOnClick+1,
	VapingMenuOnEvent+1,
	6,
	{
		{ String_Preheat, &PreheatMenu, 0, MACTION_SUBMENU },
		{ String_Modes, &ModesMenu, 0, MACTION_SUBMENU },
		{ String_Prot, 0, 0, 0 },
		{ String_Vaped, 0, 0, 0 },
		{ String_mlkJ, 0, 0, 0 },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
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
	7,
	{
		{ String_Screen, &ScreenMenu, 0, MACTION_SUBMENU },
		{ String_Coils, &CoilsMenu, 0, MACTION_SUBMENU },
		{ String_Vaping, &VapingMenu, 0, MACTION_SUBMENU },
		{ String_Clock, &ClockMenu, 0, MACTION_SUBMENU },
		{ String_Interface, &IFMenu, 0, MACTION_SUBMENU },
		{ String_Expert, &ExpertMenu, 0, MACTION_SUBMENU },
		{ String_Exit, 0, EVENT_EXIT_MENUS, 0 }
	}
};

//-----------------------------------------------------------------------------

__myevic__ void DrawMenuData( int line, int sel, const mdata_t *data )
{
	switch ( data->type )
	{
		case MITYPE_BIT:
		{
			uint32_t const *p;
			uint32_t b;

			const mbitdesc_t *desc = data->desc;

			p = data->ptr;
			p += data->bitpos / 32;
			b = 1 << ( data->bitpos % 32 );
			b &= *p;

			if ( !desc )
			{
				DrawFillRect( 40, line, 63, line + 12, 0 );
				DrawString( b ? String_On : String_Off, 44, line+2 );
			}
			else
			{
				DrawFillRect( desc->div ? : 40, line, 63, line + 12, 0 );
				DrawString( b ? desc->on : desc->off, desc->pos ? : 44, line+2 );
			}
			break;
		}
	}
}


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

		mitem_t const *mi = &CurrentMenu->mitems[firstitem+i];

		int line = 18 + 14 * i;

		if ( mi->caption )
		{
			if ( CurrentMenuItem == firstitem + i )
			{
				DrawFillRect( 0, line, 63, line + 12, 1 );
				DrawStringInv( mi->caption, 4, line + 2 );
			}
			else
			{
				DrawString( mi->caption, 4, line + 2 );
			}
		}

		if ( mi->action_type == MACTION_DATA )
		{
			DrawMenuData( line, CurrentMenuItem == firstitem + i, mi->action );
		}

		if ( CurrentMenu->on_drawitem )
		{
			CurrentMenu->on_drawitem( firstitem + i, 18+14*i, CurrentMenuItem == firstitem + i );
		}
	}
}

//-----------------------------------------------------------------------------

__myevic__ int MenuDataAction( int event, const mdata_t *data )
{
	int vret = 0;

	switch ( event )
	{
		case 1:
		{
			if ( data->type == MITYPE_BIT )
			{
				uint32_t *p = data->ptr;
				p += data->bitpos / 32;
				uint32_t mask = 1 << ( data->bitpos % 32 );
				*p ^= mask;
				vret = 1;
			}
			else
			{
				// Other types to come
			}
			break;
		}
		
		default:
			// Other events to come
			break;
	}

	if ( vret )
	{
		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;
	}
			
	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ int MenuEvent( int event )
{
	int vret = 0;

	if ( CurrentMenu && CurrentMenu->on_event )
	{
		ScreenDuration = 30;
		vret = CurrentMenu->on_event( event );
		if ( vret ) return vret;
	}

	mitem_t const *mi = &CurrentMenu->mitems[CurrentMenuItem];

	if ( mi->action )
	{
		switch ( mi->action_type )
		{
			case MACTION_DATA:
			{
				vret = MenuDataAction( event, (const mdata_t*)mi->action );
				if ( vret ) return vret;
				break;
			}
		}
	}

	switch ( event )
	{
		case 1:
		{
			ScreenDuration = 30;
			if ( CurrentMenu->on_clickitem ) CurrentMenu->on_clickitem();

			if ( mi->action )
			{
				switch ( mi->action_type )
				{
					case MACTION_SUBMENU:
					{
						CurrentMenu = (const menu_t*)mi->action;
						CurrentMenuItem = 0;

						if ( CurrentMenu->on_enter ) CurrentMenu->on_enter();

						gFlags.refresh_display = 1;
						break;
					}
				}
			}
			else if ( mi->event > 0 )
			{
				Event = mi->event;
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

			ScreenDuration = 30;
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

			ScreenDuration = 30;
			gFlags.refresh_display = 1;

			if ( CurrentMenu->on_selectitem ) CurrentMenu->on_selectitem();

			vret = 1;

		}	break;

		case 15:
			vret = 1;
			break;

		case EVENT_EXIT_MENUS:
			EditModeTimer = 0;
			UpdateDataFlash();
			MainView();
			vret = 1;
			break;

		case EVENT_PARENT_MENU:
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
				case 105:
				case 106:
					CurrentMenu = &ClockMenu;
					break;

				default:
					CurrentMenu = 0;
					break;
			}

			if ( !dfStatus.off && CurrentMenu )
			{
				CurrentMenuItem = 0;
				if ( CurrentMenu->on_enter ) CurrentMenu->on_enter();
				Screen = 102;
				ScreenDuration = 30;
				gFlags.refresh_display = 1;
			}
			else
			{
				MainView();
			}

			EditModeTimer = 0;
			vret = 1;
			break;
		}

		default:
			break;
	}

	return vret;
}
