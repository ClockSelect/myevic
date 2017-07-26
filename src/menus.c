#include "myevic.h"
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
typedef struct mvaluedesc_s mvaluedesc_t;

enum {
	MACTION_SUBMENU,
	MACTION_DATA
};

enum {
	MITYPE_BIT,
	MITYPE_BYTE,
	MITYPE_SBYTE,
	MITYPE_WORD
};

struct mbitdesc_s
{
	const uint8_t div;
	const uint8_t pos;
	const uint8_t* const on;
	const uint8_t* const off;
};

struct mvaluedesc_s
{
	const uint8_t div;
	const uint8_t posr;
	const uint8_t nd;
	const uint8_t dp;
	const int16_t min;
	const int16_t max;
	void (*draw)( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd );
	const uint8_t* const unit_s;
	const uint8_t unit_c;
	const uint8_t z;
	const uint8_t inc;
	const int16_t def1;
	const int16_t def2;
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
	const uint8_t* const caption;
	const void* const action;
	const uint8_t event;
	const uint8_t action_type;
};

struct menu_s
{
	const uint8_t* const caption;
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
__myevic__ void ProfileMenuIDraw( int it, int line, int sel )
{
	if ( it >= DATAFLASH_PROFILES_MAX )
		return;

	dfParams_t *p = (dfParams_t*)( DATAFLASH_PROFILES_BASE + it * DATAFLASH_PARAMS_SIZE );

	DrawImage( 4, line+2, 0x0B + it + 1 );
	if ( sel ) InvertRect( 0, line, 13, line+12 );

	if ( ( it != dfProfile ) && !IsProfileValid( it ) )
		return;

	uint8_t mode;
	uint16_t rez;

	const uint8_t *modes[] =
		{ String_NI, String_TI, String_SS, String_TC, String_PW, String_BY, String_SM };

	if ( it == dfProfile )
	{
		mode = dfMode;
		rez  = dfResistance;
	}
	else
	{
		mode = p->Mode;
		rez  = p->Resistance;
	}

	if ( mode > 6 )
		return;

	DrawString( modes[mode], 18, line+2 );
	DrawValue( 34, line+2, rez, 2, 0x0B, 3 );
	DrawImage( 56, line+2, 0xC0 );

	if ( it == dfProfile ) InvertRect( 16, line, 63, line+12 );
}


__myevic__ int ProfileMenuOnEvent( int event )
{
	if ( CurrentMenuItem >= DATAFLASH_PROFILES_MAX )
		return 0;

	switch ( event )
	{
		case 1:
			break;

		case 15:
			if ( CurrentMenuItem != dfProfile )
			{
				SaveProfile();
				if ( IsProfileValid( CurrentMenuItem ) )
				{
					LoadProfile( CurrentMenuItem );
				}
				else
				{
					dfProfile = CurrentMenuItem;
					SaveProfile();
				}
			}
			Event = EVENT_EXIT_MENUS;
			break;

		case EVENT_LONG_FIRE:
			if ( CurrentMenuItem != dfProfile )
			{
				if ( IsProfileValid( CurrentMenuItem ) )
				{
					EraseProfile( CurrentMenuItem );
				}
				else
				{
					SaveProfile();
					dfProfile = CurrentMenuItem;
					SaveProfile();
				}
				gFlags.refresh_display = 1;
				UpdateDFTimer = 50;
			}
			break;

		default:
			return 0;
	}

	return 1;
}


//-----------------------------------------------------------------------------

__myevic__ void AlgoMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
		case 0:	// Algo
			DrawFillRect( 30, line, 63, line+12, 0 );
			switch ( dfTCAlgo )
			{
				case TCALGO_JOY:
				default:
					DrawStringRight( String_Off, 64, line + 2 );
					break;

				case TCALGO_SWEET:
					DrawStringRight( String_Sweet, 64, line + 2 );
					break;

				case TCALGO_BOOST:
					DrawStringRight( String_Boost, 64, line + 2 );
					break;

				case TCALGO_PID:
					DrawStringRight( String_PID, 64, line + 2 );
					break;
			}
			break;
	}
}

__myevic__ void AlgoMenuOnClick()
{
	switch ( CurrentMenuItem )
	{
		case 0: // Algo
			if ( ++dfTCAlgo >= TCALGO_MAX ) dfTCAlgo = 0;
			break;
	}

	gFlags.refresh_display = 1;
}

//-----------------------------------------------------------------------------

__myevic__ void VapingMenuIDraw( int it, int line, int sel )
{
	switch ( it )
	{
		case 4:	// Protec
			DrawFillRect( 34, line, 63, line+12, 0 );
			DrawImage( 58, line+2, 0x94 );
			DrawValueRight( 56, line+2, dfProtec, 1, 0x0B, 0 );
			if ( sel && gFlags.edit_value )
				InvertRect( 0, line, 63, line+12 );
			break;

		case 5:	// Vaped
			DrawFillRect( 39, line, 63, line+12, 0 );
			DrawString( dfStatus.vapedml ? String_ml : String_mld, 43, line+2 );
			break;

		case 6: // mL/kJ
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
		case 4:	// Protec
			gFlags.edit_value ^= 1;
			break;

		case 5:	// Vaped
			dfStatus.vapedml ^= 1;
			UpdateDFTimer = 50;
			break;

		case 6: // mL/kJ
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
		case 2:	// Plus
			switch ( CurrentMenuItem )
			{
				case 4: // Protec
					if ( ++dfProtec > FIRE_PROTEC_MAX )
					{
						if ( KeyTicks < 5 ) dfProtec = FIRE_PROTEC_MIN;
						else dfProtec = FIRE_PROTEC_MAX;
					}
					vret = 1;
					break;

				case 6: // mL/kJ
					if ( ++dfVVRatio > VVEL_MAX_RATIO )
					{
						if ( KeyTicks < 5 ) dfVVRatio = VVEL_MIN_RATIO;
						else dfVVRatio = VVEL_MAX_RATIO;
					}
					vret = 1;
					break;
			}
			break;

		case 3:	// Minus
			switch ( CurrentMenuItem )
			{
				case 4: // Protec
					if ( --dfProtec < FIRE_PROTEC_MIN )
					{
						if ( KeyTicks < 5 ) dfProtec = FIRE_PROTEC_MAX;
						else dfProtec = FIRE_PROTEC_MIN;
					}
					vret = 1;
					break;

				case 6: // mL/kJ
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

		case CLICK_ACTION_PROFILE:
			DrawString( String_ProfPlus, 20, line+2 );
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
		{
			const uint8_t *strings[] =
				{ String_DMY1, String_MDY, String_DMY2, String_YMD };
			int f = dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 );
			const uint8_t *s = strings[f];
			DrawFillRect( 28, line, 63, line+12, 0 );
			DrawString( s, 32, line+2 );
			break;
		}

		case 6:	// Dial
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
		{
			int f = dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 );
			if ( ++f > 3 ) f = 0;
			dfStatus.dfmt1 = f & 1;
			dfStatus.dfmt2 = f >> 1;
			UpdateDFTimer = 50;
			gFlags.refresh_display = 1;
			break;
		}

		case 6:	// Dial
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
		case 0:	// 1Watt
			DrawString( dfStatus.onewatt ? String_On : String_Off, 44, line+2 );
			break;

		case 1:	// 1C5F
			DrawString( dfStatus.onedegree ? String_On : String_Off, 44, line+2 );
			break;

		case 2:	// Wake -+
			DrawString( dfStatus.wakeonpm ? String_On : String_Off, 44, line+2 );
			break;

		case 3:	// Font
			DrawImage( 44, line+2, dfStatus.font ? 0x9D : 0x9C );
			break;

		case 4:	// Temp
			DrawImage( 44, line+2, dfIsCelsius ? 0xC9 : 0xC8 );
			break;

		case 5:	// TDom
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
		case 0:	// 1Watt
			dfStatus.onewatt ^= 1;
			if ( dfStatus.onewatt )
			{
				WattsInc = 10;
				RoundPowers();
			}
			else
			{
				WattsInc = 1;
			}
			break;

		case 1:	// 1C5F
			dfStatus.onedegree ^= 1;
			if ( !dfStatus.onedegree )
			{
				dfTemp -= dfTemp % ( dfIsCelsius ? 5 : 10 );
			}
			break;

		case 2:	// Wake -+
			dfStatus.wakeonpm ^= 1;
			break;

		case 3:	// Font
			dfStatus.font ^= 1;
			DisplaySetFont();
			break;

		case 4:	// Temp
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

		case 5:	// TDom
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
	if ( it > 3 ) return;

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
				DrawImage(  4, 102, 0xAB );
				DrawHLine(  12, 105, 14, 1 );
				DrawHLine(  12, 108, 14, 1 );
				DrawValueRight( 37, 102, p, dp, 0x0B, 0 );
				DrawImage( 39, 102, 0x98 );

				dp = 0;
				img = 0xC2;
			}
			else
			{
				dp = ( v < 1000 ) ? 1 : 0;
				v  = ( v < 1000 ) ? v : v / 10;
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

	if ( sel && gFlags.edit_value )
		InvertRect( 0, line, 63, line+12 );
}


__myevic__ void DrawPreheatDelay( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	if ( v == 0 )
	{
		DrawString( String_Off, x, y+2 );
	}
	else
	{
		DrawValue( x+1, y+2, v / 60, 0, 0x0B, 1 );
		DrawImage( x+6, y+2, 0xD7 );
		DrawValue( x+9, y+2, v % 60, 0, 0x0B, 2 );
	}
}

__myevic__ int PreheatMEvent( int event )
{
	int vret = 0;
	if ( CurrentMenuItem > 2 )
		return vret;

	switch ( event )
	{
		case 1:	// Fire
			if ( CurrentMenuItem == 0 )
			{
				dfStatus.phpct ^= 1;
				if ( dfStatus.phpct )
				{
					dfPreheatPwr = 100 * dfPreheatPwr / dfPower;
					if ( dfPreheatPwr > 300 ) dfPreheatPwr = 300;
					if ( dfPreheatPwr < 50 ) dfPreheatPwr = 50;
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

		case 2:	// Plus
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
						PowerPlus( &dfPreheatPwr, 50, 300 );
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

		case 3:	// Minus
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
						PowerMinus( &dfPreheatPwr, 50, 300 );
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

	}
	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ void BVOMenuIDraw( int it, int line, int sel )
{
	if ( !it )
	{
		for ( int i = 0 ; i < NumBatteries ; ++i )
		{
			DrawValue(  2 + 32 * ( i & 1 ), 100 + ( i >> 1 ) * 12, BattVolts[i], 2, 0x0B, 3 );
			DrawImage( 23 + 32 * ( i & 1 ), 100 + ( i >> 1 ) * 12, 0x7D );
		}
		ScreenRefreshTimer = 10;
	}

	if ( it >= CurrentMenu->nitems - 1 )
		return;

	DrawFillRect( 22, line, 63, line+12, 0 );

	uint16_t bvo = ( dfBVOffset[it] >= 0 ) ? dfBVOffset[it] : -dfBVOffset[it];
	DrawImage( 26, line+2, ( dfBVOffset[it] >= 0 ) ? 0xFC : 0xFD );
	DrawValue( 34, line+2, bvo, 2, 0x0B, 3 );
	DrawImage( 55, line+2, 0x97 );
	if ( gFlags.edit_value && sel )
		InvertRect( 0, line, 63, line+12 );
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

		case 5:	// SHR
			DrawValue( 36, line+2, AtoShuntRez, 0, 0x0B, 3 );
			if ( gFlags.edit_value && sel )
				InvertRect( 0, line, 63, line+12 );
			break;

		case 6:	// UCH
			if ( dfStatus.usbchgoff )
				DrawString( String_OFF, 36, line+2 );
			else
				DrawString( String_ON, 36, line+2 );
			break;

		case 7:	// BAT
			DrawString( GetBatteryName(), 36, line+2 );
			break;

		case 8:	// BVO
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

		case 5:	// SHR
			if ( !(gFlags.edit_value ^= 1) )
				dfShuntRez = AtoShuntRez;
			break;

		case 6:	// UCH
			dfStatus.usbchgoff ^= 1;
			break;

		case 7:	// BAT
			if ( ++dfBatteryModel >= GetNBatteries() )
				dfBatteryModel = 0;
			SetBatteryModel();
			break;

		case 8:	// BVO
			break;

		case 9:	// Back
			UpdateDataFlash();
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
				case 5:	// Shunt Rez
					if ( ++AtoShuntRez > SHUNT_MAX_VALUE )
					{
						AtoShuntRez = ( KeyTicks == 0 ) ? SHUNT_MIN_VALUE : SHUNT_MAX_VALUE;
					}
					vret = 1;
					break;
			}
			break;

		case 3:	// -
			if ( !gFlags.edit_value )
				break;
			switch ( CurrentMenuItem )
			{
				case 5:	// Shunt Rez
					if ( --AtoShuntRez < SHUNT_MIN_VALUE )
					{
						AtoShuntRez = ( KeyTicks == 0 ) ? SHUNT_MAX_VALUE : SHUNT_MIN_VALUE;
					}
					vret = 1;
					break;
			}
			break;

		case EVENT_LONG_FIRE:
			switch ( CurrentMenuItem )
			{
				case 5:	// Shunt Rez
					AtoShuntRez = GetShuntRezValue();
					dfShuntRez = 0;
					gFlags.edit_value = 0;
					vret = 1;
					break;

				case 7:	// Battery model
					dfBatteryModel = BATTERY_CUSTOM;
					SetBatteryModel();
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

uint8_t CoilShift;
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
	CoilShift = mode << 2;
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
	switch ( it )
	{
		case 0:
		case 1:
		case 2:
		case 3:
		{
			int rez = 0;
			short img = 0xC0;
			CoilsSelectRez( it );
			rez = *CoilSelectedRez * 10;
			rez += ( dfMillis >> CoilShift ) & 0xf;
			if ( *CoilSelectedLock ) img = 0xC3;
			DrawFillRect( 26, line, 63, line+12, 0 );
			DrawValue( 28, line+2, rez, 3, 0x0B, 4 );
			DrawImage( 56, line+2, img );
			CoilsSelectRez( CurrentMenuItem );
			break;
		}

		case 5:	// Check
		{
			const uint8_t *s;
			DrawFillRect( 36, line, 63, line+12, 0 );
			s = ( ISMODETC(dfMode) && dfStatus.chkmodeoff ) ?
				String_No : String_Yes;
			DrawString( s, 44, line+2 );
			break;
		}
	}

	if ( gFlags.edit_value && sel )
	{
		InvertRect( 0, line, 63, line+12 );
	}
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
			dfMillis = 0;
			for ( int i = 0 ; i < 10 ; ++i )
			{
				dfSavedCfgRez[i] = 0;
				dfSavedCfgPwr[i] = 0;
			}
			ResetResistance();
			if ( AtoStatus == 4 )
			{
				SwitchRezLock();
			}
			break;

		case 5:	// Check
			if ( ISMODETC(dfMode) )
			{
				dfStatus.chkmodeoff ^= 1;
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

	int millis = ( dfMillis >> CoilShift ) & 0xf;
	int rez = *CoilSelectedRez * 10 + millis;

	if ( event != 1 ) rmodified = 0;

	switch ( event )
	{
		case 1:
			gFlags.edit_value ^= 1;
			if ( !gFlags.edit_value && !rmodified && *CoilSelectedRez > 0 )
			{
				*CoilSelectedLock ^= 1;
			}
			gFlags.refresh_display = 1;
			rmodified = 0;
			vret = 1;
			break;

		case 2:
			if ( gFlags.edit_value )
			{
				if ( rez == 0 )
				{
					rez = 50;
				}
				else if ( rez < 1500 )
				{
					++rez;
				}
				*CoilSelectedLock = 1;
				rmodified = 1;
				vret = 1;
			}
			break;

		case 3:
			if ( gFlags.edit_value )
			{
				if ( rez == 50 )
				{
					rez = 0;
					*CoilSelectedLock = 0;
				}
				else if ( rez > 50 )
				{
					--rez;
					*CoilSelectedLock = 1;
				}
				rmodified = 1;
				vret = 1;
			}
			break;

		case EVENT_LONG_FIRE:
			rez = 0;
			*CoilSelectedLock = 0;
			if ( CurrentMenuItem == dfMode )
			{
				ResetResistance();
				if ( AtoStatus == 4 )
				{
					rez = 10 * AtoRez + AtoMillis;
					*CoilSelectedLock = 1;
				}
			}
			rmodified = 1;
			gFlags.edit_value = 0;
			vret = 1;
			break;

	}

	if ( rmodified )
	{
		*CoilSelectedRez = rez / 10;
		millis = rez % 10;
		dfMillis &= ~( 0xf << CoilShift );
		dfMillis |= millis << CoilShift;

		UpdateDFTimer = 50;
		gFlags.refresh_display = 1;

		if ( CurrentMenuItem == dfMode )
		{
			AtoRez = *CoilSelectedRez;
			AtoMillis = millis;

			dfResistance = *CoilSelectedRez;
			RezMillis = millis;

			SetAtoLimits();
		}
	}

	return vret;
}


//-----------------------------------------------------------------------------

__myevic__ void DrawTCRP( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	if ( v == 0 )
	{
		DrawString( String_DEF, x - 18, y + 2 );
	}
	else
	{
		DrawValueRight( x, y + 2, v, dp, z, nd );
	}
}

__myevic__ int TCRSetOnEvent( int event )
{
	Event = EVENT_MODE_CHANGE;
	return 0;
}


//-----------------------------------------------------------------------------

__myevic__ void DrawLedColor( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	DrawValueRight( x, y + 2, v * 4, dp, z, nd );
	DrawImage( x + 1, y + 2, 0xC2 );
}

__myevic__ void LedMenuEnter()
{
	LEDGetColor();
	if ( ISEGRIPII || ISEVICAIO ) gFlags.led_on = 1;
}

__myevic__ int LedMenuEvent( int event )
{
	LEDSetColor();
	return 0;
}


//-----------------------------------------------------------------------------

__myevic__ void CurveMenuOnClick()
{
	if ( CurrentMenuItem == 1 )
	{
		ResetPowerCurve();
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
const menu_t CurveMenu;
const menu_t VapingMenu;
const menu_t ExpertMenu;

//-----------------------------------------------------------------------------

const mbitdesc_t BitDesc =
{
	0, 0,
	String_On,
	String_Off
};

const mbitdesc_t InvBitDesc =
{
	0, 0,
	String_Off,
	String_On
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
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const menu_t ModesMenu =
{
	String_Modes,
	&VapingMenu,
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
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const mvaluedesc_t PreheatDelayDesc =
{
	34, 37,
	3, 2,
	0, 180,
	&DrawPreheatDelay+1,
	0,
	0,
	0x0B,
	1,
	-1, 0
};

const mdata_t PreheatDelayData =
{
	&dfPHDelay,
	&PreheatDelayDesc,
	MITYPE_BYTE,
	0
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
	5,
	{
		{ String_Unit, 0, 0, 0 },
		{ String_Pwr, 0, 0, 0 },
		{ String_Time, 0, 0, 0 },
		{ String_Delay, &PreheatDelayData, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const mvaluedesc_t TCRNIDesc =
{
	32, 59,
	0, 0,
	0, 999,
	&DrawTCRP+1,
	0,
	0,
	0x0B,
	1,
	0, 600
};

const mvaluedesc_t TCRTIDesc =
{
	32, 59,
	0, 0,
	0, 999,
	&DrawTCRP+1,
	0,
	0,
	0x0B,
	1,
	0, 366
};

const mvaluedesc_t TCRSSDesc =
{
	32, 59,
	0, 0,
	0, 999,
	&DrawTCRP+1,
	0,
	0,
	0x0B,
	1,
	0, 120
};

const mvaluedesc_t TCRMDesc =
{
	32, 59,
	0, 0,
	1, 999,
	0,
	0,
	0,
	0x0B,
	1,
	0, 120
};

const mdata_t TCRNIData =
{
	&dfTCRP[0],
	&TCRNIDesc,
	MITYPE_WORD,
	0
};

const mdata_t TCRTIData =
{
	&dfTCRP[1],
	&TCRTIDesc,
	MITYPE_WORD,
	0
};

const mdata_t TCRSSData =
{
	&dfTCRP[2],
	&TCRSSDesc,
	MITYPE_WORD,
	0
};

const mdata_t TCRM1Data =
{
	&dfTCRM[0],
	&TCRMDesc,
	MITYPE_WORD,
	0
};

const mdata_t TCRM2Data =
{
	&dfTCRM[1],
	&TCRMDesc,
	MITYPE_WORD,
	0
};

const mdata_t TCRM3Data =
{
	&dfTCRM[2],
	&TCRMDesc,
	MITYPE_WORD,
	0
};

const menu_t TCRSetMenu =
{
	String_TCRSet,
	&CoilsMenu,
	0,
	0,
	0,
	0,
	&TCRSetOnEvent+1,
	7,
	{
		{ String_M1, &TCRM1Data, 0, MACTION_DATA },
		{ String_M2, &TCRM2Data, 0, MACTION_DATA },
		{ String_M3, &TCRM3Data, 0, MACTION_DATA },
		{ String_NI, &TCRNIData, 0, MACTION_DATA },
		{ String_TI, &TCRTIData, 0, MACTION_DATA },
		{ String_SS, &TCRSSData, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	7,
	{
		{ String_NI, 0, 0, 0 },
		{ String_TI, 0, 0, 0 },
		{ String_SS, 0, 0, 0 },
		{ String_TCR, 0, 0, 0 },
		{ String_Zero_All, 0, 0, 0 },
		{ String_Check, 0, 0, 0 },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
		{ String_TCRSet, &TCRSetMenu, 0, MACTION_SUBMENU },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	6,
	{
		{ String_None, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Tetra, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Cube, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Octa, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Dodeca, 0, EVENT_EXIT_MENUS, 0 },
		{ String_Isoca, 0, EVENT_EXIT_MENUS, 0 }
	}
};

const mvaluedesc_t LedDesc =
{
	34, 53,
	0, 0,
	0, 25,
	DrawLedColor+1,
	0,
	0xC2,
	0x0B,
	1,
	26, 26
};

const mdata_t LedRedData =
{
	&LEDRed,
	&LedDesc,
	MITYPE_BYTE,
	0
};

const mdata_t LedGreenData =
{
	&LEDGreen,
	&LedDesc,
	MITYPE_BYTE,
	0
};

const mdata_t LedBlueData =
{
	&LEDBlue,
	&LedDesc,
	MITYPE_BYTE,
	0
};

const menu_t LedMenu =
{
	String_Led,
	&MiscsMenu,
	LedMenuEnter+1,
	0,
	0,
	0,
	LedMenuEvent+1,
	4,
	{
		{ String_Red, &LedRedData, 0, MACTION_DATA },
		{ String_Green, &LedGreenData, 0, MACTION_DATA },
		{ String_Blue, &LedBlueData, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	4,
	{
		{ String_Game, &GameMenu, 0, MACTION_SUBMENU },
		{ String_Led, &LedMenu, 0, MACTION_SUBMENU },
		{ String_3D, &Object3DMenu, 0, MACTION_SUBMENU },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const mbitdesc_t HmsDesc =
{
	36, 40,
	String_HM,
	String_hms
};

const mdata_t ClkSizeData =
{
	&dfStatus,
	&HmsDesc,
	MITYPE_BIT,
	24
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
	8,
	{
		{ String_SetTime, 0, 0, 0 },
		{ String_SetDate, 0, 0, 0 },
		{ String_ClkAdjust, 0, EVENT_CLK_ADJUST, 0 },
		{ String_ClkSpeed, 0, EVENT_CLK_SPEED, 0 },
		{ String_Fmt, 0, 0, 0 },
		{ String_Size, &ClkSizeData, 0, MACTION_DATA },
		{ String_Dial, 0, 0, 0 },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const mvaluedesc_t BVODesc =
{
	32, 56,
	2, 0,
	BVO_MIN, BVO_MAX,
	0,
	0,
	0,
	0x0B,
	1,
	127, 0
};

const mdata_t BVO1Data =
{
	&dfBVOffset[0],
	&BVODesc,
	MITYPE_SBYTE,
	0
};

const mdata_t BVO2Data =
{
	&dfBVOffset[1],
	&BVODesc,
	MITYPE_SBYTE,
	0
};

const mdata_t BVO3Data =
{
	&dfBVOffset[2],
	&BVODesc,
	MITYPE_SBYTE,
	0
};

const mdata_t BVO4Data =
{
	&dfBVOffset[3],
	&BVODesc,
	MITYPE_SBYTE,
	0
};

const menu_t BVOMenu =
{
	String_BVO,
	&ExpertMenu,
	0,
	BVOMenuIDraw+1,
	0,
	0,
	0,
	5,
	{
		{ String_B1, &BVO1Data, 0, MACTION_DATA },
		{ String_B2, &BVO2Data, 0, MACTION_DATA },
		{ String_B3, &BVO3Data, 0, MACTION_DATA },
		{ String_B4, &BVO4Data, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	10,
	{
		{ String_USB, 0, 0, 0 },
		{ String_DBG, 0, 0, 0 },
		{ String_X32, 0, 0, 0 },
		{ String_LSL, 0, 0, 0 },
		{ String_NFE, 0, 0, 0 },
		{ String_SHR, 0, 0, 0 },
		{ String_UCH, 0, 0, 0 },
		{ String_BAT, 0, 0, 0 },
		{ String_BVO, &BVOMenu, 0, MACTION_SUBMENU },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	7,
	{
		{ String_None, 0, EVENT_PARENT_MENU, 0 },
		{ String_Clock, 0, EVENT_PARENT_MENU, 0 },
		{ String_3D, 0, EVENT_PARENT_MENU, 0 },
		{ String_Logo, 0, EVENT_PARENT_MENU, 0 },
		{ String_Qix, 0, EVENT_PARENT_MENU, 0 },
		{ String_Snow, 0, EVENT_PARENT_MENU, 0 },
		{ String_Splash, 0, EVENT_PARENT_MENU, 0 }
	}
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
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};

const mdata_t ScreenInvData =
{
	&dfStatus,
	&BitDesc,
	MITYPE_BIT,
	18
};

const menu_t ScreenMenu =
{
	String_Screen,
	&MainMenu,
	0,
	0,
	0,
	0,
	0,
	7,
	{
		{ String_Contrast, 0, EVENT_EDIT_CONTRAST,  },
		{ String_Protection, &ScreenProtMenu, 0, MACTION_SUBMENU },
		{ String_Saver, &ScreenSaveMenu, 0, MACTION_SUBMENU },
		{ String_Logo, &LogoMenu, 0, MACTION_SUBMENU },
		{ String_Invert, &ScreenInvData, EVENT_INVERT_SCREEN, MACTION_DATA },
		{ String_Miscs, &MiscsMenu, 0, MACTION_SUBMENU },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	8,
	{
		{ String_1Watt, 0, 0, 0 },
		{ String_1C5F, 0, 0, 0 },
		{ String_WakeMP, 0, 0, 0 },
		{ String_Font, 0, 0, 0 },
		{ String_Temp, 0, 0, 0 },
		{ String_PPwr, 0, 0, 0 },
		{ String_Clicks, &ClicksMenu, 0, MACTION_SUBMENU },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};


const mvaluedesc_t BoostDesc =
{
	34, 53,
	0, 0,
	0, 100,
	0,
	0,
	0xC2,
	0x0B,
	1,
	101, 50
};

const mdata_t BoostData =
{
	&dfTCBoost,
	&BoostDesc,
	MITYPE_WORD,
	0
};

const mvaluedesc_t PIDPDesc =
{
	24, 60,
	0, 0,
	PID_P_MIN, PID_P_MAX,
	0,
	0,
	0,
	0x0B,
	1,
	PID_P_MAX+1, PID_P_DEF
};

const mdata_t PIDPData =
{
	&dfPID.P,
	&PIDPDesc,
	MITYPE_WORD,
	0
};

const mvaluedesc_t PIDIDesc =
{
	24, 60,
	0, 0,
	PID_I_MIN, PID_I_MAX,
	0,
	0,
	0,
	0x0B,
	1,
	PID_I_MAX+1, PID_I_DEF
};

const mdata_t PIDIData =
{
	&dfPID.I,
	&PIDIDesc,
	MITYPE_WORD,
	0
};

const mvaluedesc_t PIDDDesc =
{
	24, 60,
	0, 0,
	PID_D_MIN, PID_D_MAX,
	0,
	0,
	0,
	0x0B,
	1,
	PID_D_MAX+1, PID_D_DEF
};

const mdata_t PIDDData =
{
	&dfPID.D,
	&PIDDDesc,
	MITYPE_WORD,
	0
};

const menu_t AlgoMenu =
{
	String_Algo,
	&VapingMenu,
	0,
	AlgoMenuIDraw+1,
	0,
	AlgoMenuOnClick+1,
	0,
	6,
	{
		{ String_Algo, 0, 0, 0 },
		{ String_Boost, &BoostData, 0, MACTION_DATA },
		{ String_P, &PIDPData, 0, MACTION_DATA },
		{ String_I, &PIDIData, 0, MACTION_DATA },
		{ String_D, &PIDDData, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};


const mbitdesc_t CurveEnaDesc =
{
	40, 44,
	String_Yes,
	String_No
};

const mdata_t CurveEnaData =
{
	&dfStatus,
	&CurveEnaDesc,
	MITYPE_BIT,
	28
};

const mvaluedesc_t CurveDelayDesc =
{
	36, 40,
	3, 2,
	0, 180,
	&DrawPreheatDelay+1,
	0,
	0,
	0x0B,
	1,
	-1, 0
};

const mdata_t CurveDelayData =
{
	&dfPHDelay,
	&CurveDelayDesc,
	MITYPE_BYTE,
	0
};

const menu_t CurveMenu =
{
	String_Curve,
	&VapingMenu,
	0,
	0,
	0,
	CurveMenuOnClick+1,
	0,
	5,
	{
		{ String_Enable, &CurveEnaData, 0, MACTION_DATA },
		{ String_Reset, 0, EVENT_POWER_CURVE, 0 },
		{ String_Edit, 0, EVENT_POWER_CURVE, 0 },
		{ String_Delay, &CurveDelayData, 0, MACTION_DATA },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
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
	8,
	{
		{ String_Preheat, &PreheatMenu, 0, MACTION_SUBMENU },
		{ String_Modes, &ModesMenu, 0, MACTION_SUBMENU },
		{ String_Algo, &AlgoMenu, 0, MACTION_SUBMENU },
		{ String_Curve, &CurveMenu, 0, MACTION_SUBMENU },
		{ String_Prot, 0, 0, 0 },
		{ String_Vaped, 0, 0, 0 },
		{ String_mlkJ, 0, 0, 0 },
		{ String_Back, 0, EVENT_PARENT_MENU, 0 }
	}
};


const menu_t ProfileMenu =
{
	String_Profile,
	0,
	0,
	ProfileMenuIDraw+1,
	0,
	0,
	ProfileMenuOnEvent+1,
	7,
	{
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
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
	uint32_t const *p = data->ptr;

	switch ( data->type )
	{
		case MITYPE_BIT:
		{
			uint32_t b;
			const mbitdesc_t *desc = data->desc;

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

		case MITYPE_BYTE:
		case MITYPE_WORD:
		{
			uint16_t v;

			if ( data->type == MITYPE_BYTE )
			{
				v = *(const uint8_t*)p;
			}
			else
			{
				v = *(const uint16_t*)p;
			}

			const mvaluedesc_t *desc = data->desc;

			DrawFillRect( desc->div, line, 63, line+12, 0 );

			if ( desc->draw )
			{
				desc->draw( desc->posr, line, v, desc->dp, desc->z, desc-> nd );
			}
			else
			{
				DrawValueRight( desc->posr, line + 2, v, desc->dp, desc->z, desc-> nd );
				if ( desc->unit_s )
				{
					DrawString( desc->unit_s, desc->posr + 1, line + 2 );
				}
				else if ( desc->unit_c )
				{
					DrawImage( desc->posr + 1, line + 2, desc->unit_c );
				}
			}

			if ( sel && gFlags.edit_value )
			{
				InvertRect( 0, line, 63, line+12 );
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
		case 1:	// Fire
		{
			switch ( data->type )
			{
				case MITYPE_BIT:
				{
					uint32_t *p = data->ptr;
					p += data->bitpos / 32;
					uint32_t mask = 1 << ( data->bitpos % 32 );
					*p ^= mask;
					vret = 1;
					break;
				}

				case MITYPE_BYTE:
				case MITYPE_SBYTE:
				case MITYPE_WORD:
					gFlags.edit_value ^= 1;
					vret = 1;
					break;
			}

			Event = CurrentMenu->mitems[CurrentMenuItem].event;

			break;
		}

		case 2:	// Plus
		{
			if ( !gFlags.edit_value )
				break;

			const mvaluedesc_t *desc = data->desc;

			switch ( data->type )
			{
				case MITYPE_BYTE:
				{
					uint8_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					*p += desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( *p > desc->max )
					{
						*p = ( KeyTicks < 5 ) ? desc->min : desc->max;
					}
					break;
				}

				case MITYPE_SBYTE:
				{
					int8_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					*p += desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( *p > desc->max )
					{
						*p = ( KeyTicks < 5 ) ? desc->min : desc->max;
					}
					break;
				}

				case MITYPE_WORD:
				{
					int16_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					*p += desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( *p > desc->max )
					{
						*p = ( KeyTicks < 5 ) ? desc->min : desc->max;
					}
					break;
				}
			}

			break;
		}

		case 3:	// Minus
		{
			if ( !gFlags.edit_value )
				break;

			const mvaluedesc_t *desc = data->desc;

			switch ( data->type )
			{
				case MITYPE_BYTE:
				{
					uint8_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					uint16_t inc = desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( ( *p <= desc->min + inc ) || ( ( *p -= inc ) < desc->min ) )
					{
						*p = ( KeyTicks < 5 ) ? desc->max : desc->min;
					}
					break;
				}

				case MITYPE_SBYTE:
				{
					int8_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					uint16_t inc = desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( ( *p -= inc ) < desc->min )
					{
						*p = ( KeyTicks < 5 ) ? desc->max : desc->min;
					}
					break;
				}

				case MITYPE_WORD:
				{
					int16_t *p = data->ptr;

					vret = 1;

					if ( *p == desc->def1 )
						break;

					uint16_t inc = desc->inc * ( KeyTicks >= 105 ) ? 10 : 1;

					if ( ( *p -= inc ) < desc->min )
					{
						*p = ( KeyTicks < 5 ) ? desc->max : desc->min;
					}
					break;
				}
			}

			break;
		}

		case EVENT_LONG_FIRE:
		{
			const mvaluedesc_t *desc = data->desc;

			switch ( data->type )
			{
				case MITYPE_WORD:
				{
					int16_t *p = data->ptr;
					if ( *p == desc->def1 )
					{
						*p = desc->def2;
						gFlags.edit_value = 1;
						vret = 1;
					}
					else if ( desc->def1 >= desc->min && desc->def1 <= desc->max )
					{
						*p = desc->def1;
						gFlags.edit_value = 0;
						vret = 1;
					}
					else if ( desc->def2 >= desc->min && desc->def2 <= desc->max )
					{
						*p = desc->def2;
						gFlags.edit_value = 0;
						vret = 1;
					}
					break;
				}
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
		ScreenDuration = 30;
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

	if ( ISEGRIPII )
	{
		if ( event == 2 ) event = 3;
		else if ( event == 3 ) event = 2;
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

		case 39:
			CurrentMenu = &TCRSetMenu;
			CurrentMenuItem = 0;
			SetScreen( 102, 30 );
			vret = 1;
			break;

		case EVENT_EXIT_MENUS:
			EditModeTimer = 0;
			gFlags.edit_capture_evt = 0;
			gFlags.edit_value = 0;
			LEDOff();
			UpdateDataFlash();
			MainView();
			vret = 1;
			break;

		case EVENT_PARENT_MENU:
		{
			switch ( Screen )
			{
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

				case 107:
					CurrentMenu = &CurveMenu;
					break;

				default:
					CurrentMenu = 0;
					break;
			}

			if ( !dfStatus.off && CurrentMenu )
			{
				CurrentMenuItem = 0;
				if ( CurrentMenu->on_enter ) CurrentMenu->on_enter();
				SetScreen( 102, 30 );
			}
			else
			{
				MainView();
			}

			EditModeTimer = 0;
			gFlags.edit_capture_evt = 0;
			gFlags.edit_value = 0;
			LEDOff();
			vret = 1;
			break;
		}

		case EVENT_TOGGLE_TDOM:
			CurrentMenu = &PreheatMenu;
			CurrentMenuItem = 1;
			SetScreen( 102, 30 );
			vret = 1;
			break;

		case EVENT_PROFILE_MENU:
			CurrentMenu = &ProfileMenu;
			CurrentMenuItem = dfProfile;
			SetScreen( 102, 30 );
			vret = 1;
			break;

		default:
			break;
	}

	return vret;
}


