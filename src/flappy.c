#include "myevic.h"
#include "dataflash.h"
#include "display.h"
#include "screens.h"
#include "events.h"
#include "miscs.h"
#include "timers.h"
#include "battery.h"

#include "flappy.h"

//=========================================================================

typedef struct fbColumn_s
{
	int8_t	x;
	int8_t	top_bot;
	int8_t	top_nbb;
	int8_t	bot_top;
	int8_t	bot_nbb;
}
fbColumn_t;

//-------------------------------------------------------------------------

uint8_t		fbBirdColumn = 16;
uint8_t		fbBirdCycle = 0;
uint8_t		fbBirdLine = 24;
int8_t		fbBirdDisp = 0;
uint8_t		fbDead = 0;
uint8_t		fbAnimStep = 0;
uint8_t		fbAnimTimer = 0;
uint16_t	fbScore = 0;
fbColumn_t	fbColumn1 = { 0 };
fbColumn_t	fbColumn2 = { 0 };
fbColumn_t	fbColumn3 = { 0 };
uint8_t		fbTimeoutMask = 0;
uint8_t		fbCurrentTimeout = 0;
uint8_t		fbUsedTimeouts = 0;

//-------------------------------------------------------------------------

extern const uint8_t	fbColumnBody[];
extern const uint8_t	fbColumnBottom[];
extern const uint8_t	fbColumnTop[];
extern const uint8_t	fbBird0[];
extern const uint8_t	fbBird1[];
extern const uint8_t	fbBird2[];
extern const uint8_t	fbBirdDead[];
extern const uint8_t	fbFont[95][16];

//-------------------------------------------------------------------------

#define FB_NUM_TIMERS 3

typedef void (FB_TIMEOUT_FUNC( void ));

typedef struct fbTimeout_s
{
	uint8_t			delay;
	FB_TIMEOUT_FUNC	*function;
}
fbTimeout_t;

fbTimeout_t fbTimeoutsTable[FB_NUM_TIMERS];


//=========================================================================
//----- (000038B0) --------------------------------------------------------
__myevic__ void fbInitTimeouts()
{
	fbCurrentTimeout = 0;
	fbUsedTimeouts = 0;

	for ( int i = 0 ; i < FB_NUM_TIMERS ; ++i )
	{
		fbTimeoutsTable[i].delay = 0;
		fbTimeoutsTable[i].function = 0;
	}
}


//=========================================================================
//----- (000013DC) --------------------------------------------------------
__myevic__ int fbCreateTimeout( FB_TIMEOUT_FUNC f )
{
	uint8_t mask = 1;

	for ( int i = 0 ; i < FB_NUM_TIMERS ; ++i )
	{
		if ( !( mask & fbUsedTimeouts ) )
		{
			fbUsedTimeouts |= mask;
			fbTimeoutsTable[i].function = f;
			return i;
		}
		mask <<= 1;
	}
	return -1;
}


//=========================================================================
//----- (00004EEC) --------------------------------------------------------
__myevic__ void fbDeleteTimeout( int to )
{
	fbUsedTimeouts &= ~( 1 << to );
	fbTimeoutsTable[to].delay = 0;
}


//=========================================================================
//----- (00005ADC) --------------------------------------------------------
__myevic__ void fbSetTimeoutDelay( int v )
{
	fbTimeoutsTable[fbCurrentTimeout].delay = v;
}


//=========================================================================
//----- (00008894) --------------------------------------------------------
// Called at a frequency depending on the choosen difficulty:
//		Easy    62.5Hz 0.016s
//		Normal  76.9Hz 0.013s
//		Hard   100.0Hz 0.010s

__myevic__ void fbTickTimeouts()
{
	for ( int i = 0 ; i < FB_NUM_TIMERS ; ++i )
	{
		if ( fbTimeoutsTable[i].delay )
		{
			--fbTimeoutsTable[i].delay;
		}
	}
}


//=========================================================================
//----- (00007B9C) --------------------------------------------------------
__myevic__ void fbCallTimeouts()
{
	fbTimeoutMask = 1;

	for ( fbCurrentTimeout = 0 ; fbCurrentTimeout < FB_NUM_TIMERS ; ++fbCurrentTimeout )
	{
		if ( fbUsedTimeouts & fbTimeoutMask )
		{
			if ( !fbTimeoutsTable[fbCurrentTimeout].delay )
			{
				fbTimeoutsTable[fbCurrentTimeout].function();
			}
		}
		fbTimeoutMask <<= 1;
	}
}


//=========================================================================
//----- (00001348) --------------------------------------------------------
__myevic__ void fbCLSBuf()
{
	MemClear2( 0, ScreenBuffer, SCREEN_BUFFER_SIZE );
}


//=========================================================================
//----- (000022B8) --------------------------------------------------------
__myevic__ void fbFillScreen( char v )
{
	MemSet( ScreenBuffer, v, SCREEN_BUFFER_SIZE );
}


//=========================================================================
//----- (000024DC) --------------------------------------------------------
__myevic__ int fbNumDigits( int v )
{
	int nd = 0;

	while ( v )
	{
		v /= 10;
		++nd;
	}

	if ( !nd ) nd = 1;

	return nd;
}


//=========================================================================
//----- (00008188) --------------------------------------------------------
__myevic__ void fbPlot( int x, int y, int color )
{
	int xx;
	int i;
	uint8_t pix;

	if ( x >= 0 && y >= 0 && x <= 127 && y <= 63 )
	{
		xx = 127 - x;
		if ( DisplayModel )
		{
			i = 8 * xx + ( y >> 3 );
			pix = 1 << ( 7 - ( y & 7 ));
		}
		else
		{
			i = 64 * ( xx >> 3 ) + y;
			pix = 1 << ( xx & 7 );
		}
		if ( color ) ScreenBuffer[i] |= pix;
		else ScreenBuffer[i] &= ~pix;
	}
}


//=========================================================================
//----- (0000805E) --------------------------------------------------------
__myevic__ void fbDrawSprite( int x, int y, int w, int h, const uint8_t bitmap[] )
{
	int bpl;
	uint8_t pixels;

	bpl = w >> 3;

	for ( ; h ; --h )
	{
		for ( int i = 0 ; i < bpl ; ++i )
		{
			pixels = *bitmap++;
			for ( int j = 0 ; j < 8 ; ++j )
			{
				if ( pixels & 1 ) fbPlot( x + 8 * i + j, y, 1 );
				pixels >>= 1;
			}
		}
		++y;
	}
}


//=========================================================================
//----- (00007F9C) --------------------------------------------------------
__myevic__ void fbDrawChar( int x, int y, const char c )
{
	fbDrawSprite( x, y, 8, 16, fbFont[c-32] );
}


//=========================================================================
//----- (00007FB8) --------------------------------------------------------
__myevic__ void fbDrawNumber( int x, int y, int nd, int v )
{
	char d;

	if ( nd > 5 ) nd = 5;
	
	for ( ; nd > 0 ; --nd )
	{
		switch ( nd )
		{
			case 5:
				d = v / 10000;
				break;
			case 4:
				d = v % 10000 / 1000;
				break;
			case 3:
				d = v % 1000 / 100;
				break;
			case 2:
				d = v % 100 / 10;
				break;
			case 1:
				d = v % 10;
				break;
			default:
				break;
		}

		fbDrawChar( x, y, '0' + d );
		x += 8;
	}
}


//=========================================================================
//----- (0000803E) --------------------------------------------------------
__myevic__ void fbDrawText( int x, int y, const char *s )
{
	for ( ; *s ; ++s )
	{
		fbDrawChar( x, y, *s );
		x += 8;
	}
}


//=========================================================================
//----- (000080BA) --------------------------------------------------------
__myevic__ void fbDrawRect( int x1, int y1, int x2, int y2, int color, int fill )
{
	int dx, dy;
	int i, j, t;

	if ( x1 > x2 ) { t = x1; x1 = x2; x2 = t; }
	if ( y1 > y2 ) { t = y1; y1 = y2; y2 = t; }

	dx = x2 - x1;
	dy = y2 - y1;

	if ( fill )
	{
		for ( j = y1 ; j <= y2 ; ++j )
		{
			for ( i = 0 ; i <= dx ; ++i ) fbPlot( x1 + i, j, color );
		}
	}
	else
	{
		for ( i = dx ; i >= 0 ; --i )
		{
			fbPlot( x1 + i, y1, color);
		}

		if ( dy )
		{
			for ( i = dx ; i >= 0 ; --i )
			{
				fbPlot( x1 + i, y2, color );
			}

			for ( j = dy - 1 ; j ; --j )
			{
				fbPlot( x1, y1 + j, color);
			}

			for ( j = dy - 1 ; j ; --j )
			{
				fbPlot( x2, y1 + j, color);
			}
		}
	}
}


//=========================================================================
//----- (00001798) --------------------------------------------------------
__myevic__ void fbDrawDeadBird( int y )
{
	fbDrawSprite( 10, y, 16, 16, fbBirdDead );
}


//=========================================================================
//----- (0000504C) --------------------------------------------------------
__myevic__ void fbBirdAnim( int line )
{
	const uint8_t *sprite = 0;

	if ( fbBirdCycle == 1 )
	{
		sprite = fbBird0;
	}
	else if ( fbBirdCycle == 2 )
	{
		sprite = fbBird1;
	}
	else if ( fbBirdCycle == 3 )
	{
		sprite = fbBird2;
	}
	else if ( fbBirdCycle == 4 )
	{
		sprite = fbBird0;
	}

	if ( sprite )
	{
		fbDrawSprite( fbBirdColumn, line, 16, 16, sprite );
	}

	if ( ++fbBirdCycle == 5 ) fbBirdCycle = 1;
}


//=========================================================================
//----- (000050A0) --------------------------------------------------------
__myevic__ void fbDrawColumn( const fbColumn_t *c )
{
	int i;

	for ( i = 0 ; i < c->top_nbb ; ++i )
	{
		fbDrawSprite( c->x, 8 * i, 24, 8, fbColumnBody );
	}
	for ( i = 0 ; i < c->bot_nbb ; ++i )
	{
		fbDrawSprite( c->x, 56 - 8 * i, 24, 8, fbColumnBody );
	}
	fbDrawSprite( c->x, c->top_bot - 8	, 24, 8, fbColumnBody );
	fbDrawSprite( c->x, c->top_bot		, 24, 8, fbColumnBottom );
	fbDrawSprite( c->x, c->bot_top		, 24, 8, fbColumnTop );
	fbDrawSprite( c->x, c->bot_top + 8	, 24, 8, fbColumnBody );
}


//=========================================================================
//----- (000013A4) --------------------------------------------------------
__myevic__ void fbCreateColumn( int8_t hpos, fbColumn_t *column )
{
	uint8_t vpos;

	if ( hpos > 0 )
	{
		column->x = hpos;
		vpos = (int8_t)( Random() % 24 + 1 );
		column->top_bot = vpos;
		column->top_nbb = vpos >> 3;
		vpos += 40;
		column->bot_top = vpos - 8;
		column->bot_nbb = (64 - vpos) >> 3;
	}
}


//=========================================================================
//----- (00000854) --------------------------------------------------------
__myevic__ void fbDeathScreen()
{
	switch ( fbAnimStep )
	{
		case 0:
			fbFillScreen( -1 );
			DisplayRefresh();
			fbSetTimeoutDelay( 5 );
			++fbAnimStep;
			break;

		case 1:
			fbCLSBuf();
			DisplayRefresh();
			fbSetTimeoutDelay( 10 );
			++fbAnimStep;
			break;

		case 3:
			if ( !PE0 )
			{
				fbAnimStep = 4;
				fbSetTimeoutDelay( 100 );
			}
			break;

		case 4:
			if ( PE0 )
			{
				fbBirdLine = 1;
				fbAnimStep = 0;
				fbScore = 0;
				fbCreateTimeout( fbStartScreen+1 );
				fbDeleteTimeout( fbCurrentTimeout );
			}
			break;

		default:
			if ( fbBirdLine >= 48 )
			{
				if ( fbDead == 3 )
				{
					fbDead = 0;
					fbAnimStep = 3;
				}
				fbSetTimeoutDelay( 200 );
			}
			else
			{
				fbBirdLine += 4;
			}
			fbDrawDeadBird( fbBirdLine );
			fbDrawColumn( &fbColumn1 );
			fbDrawColumn( &fbColumn2 );
			fbDrawColumn( &fbColumn3 );
			fbDrawRect( 26, 16, 98, 52, 0, 1 );
			fbDrawRect( 26, 16, 98, 52, 1, 0 );
			fbDrawText( 27, 18, "SCORE" );
			fbDrawText( 27, 36, "BEST" );
			fbDrawNumber( 71, 18, fbNumDigits( fbScore ), fbScore );
			fbDrawNumber( 71, 36, fbNumDigits( dfFBBest ), dfFBBest );
			DisplayRefresh();
			fbCLSBuf();
			fbSetTimeoutDelay( 4 );
			UpdateDFTimer = 50;
			break;
	}
}


//=========================================================================
//----- (00000A94) --------------------------------------------------------
__myevic__ void fbGame()
{
	int nd;

	if ( fbDead )
	{
		fbAnimTimer = 0;
		fbAnimStep = 0;
		if ( dfFBBest < fbScore ) dfFBBest = fbScore;
		fbDeleteTimeout( fbCurrentTimeout );
		fbCreateTimeout( fbDeathScreen+1 );
		return;
	}

	fbDrawColumn( &fbColumn1 );
	fbDrawColumn( &fbColumn2 );
	fbDrawColumn( &fbColumn3 );

	fbColumn1.x -= 2;
	if ( fbAnimStep > 0 ) fbColumn2.x -= 2;
	if ( fbAnimStep > 1 ) fbColumn3.x -= 2;

	if ( ++fbAnimTimer == 32 )
	{
		fbAnimTimer = 0;
		if ( fbAnimStep < 2 ) ++fbAnimStep;
	}

	if ( fbColumn1.x < -55 ) fbCreateColumn( 127, &fbColumn1 );
	if ( fbColumn2.x < -55 ) fbCreateColumn( 127, &fbColumn2 );
	if ( fbColumn3.x < -55 ) fbCreateColumn( 127, &fbColumn3 );

	if ( fbColumn1.x == 21 || fbColumn2.x == 21 || fbColumn3.x == 21 ) ++fbScore;

	if ( fbBirdLine >= 48 ) fbDead = 1;
	else fbBirdAnim( fbBirdLine );

	if ( fbColumn1.x >= -8 && fbColumn1.x <= 31
			&& (fbColumn1.top_bot + 6 > fbBirdLine || fbColumn1.bot_top - 12 < fbBirdLine) )
	{
		fbDead = 1;
	}

	if ( fbColumn2.x >= -8 && fbColumn2.x <= 31
			&& (fbColumn2.top_bot + 6 > fbBirdLine || fbColumn2.bot_top - 12 < fbBirdLine) )
	{
		fbDead = 1;
	}

	if ( fbColumn3.x >= -8 && fbColumn3.x <= 31
			&& (fbColumn3.top_bot + 6 > fbBirdLine || fbColumn3.bot_top - 12 < fbBirdLine) )
	{
		fbDead = 1;
	}

	nd = fbNumDigits( fbScore );
	fbDrawNumber( 64 - 4 * nd, 4, nd, fbScore);
	DisplayRefresh();
	fbCLSBuf();
	fbSetTimeoutDelay( 4 );
}


//=========================================================================
//----- (00008834) --------------------------------------------------------
__myevic__ void fbCheckFire()
{
	if ( fbDead )
	{
		++fbDead;
		fbDeleteTimeout( fbCurrentTimeout );
	}

	if ( fbBirdLine >= 0 && !PE0 )
	{
		if ( dfFBSpeed == 2 )
		{
			fbBirdDisp = -20;
		}
		else if ( dfFBSpeed == 1 )
		{
			fbBirdDisp = -17;
		}
		else
		{
			fbBirdDisp = -14;
		}

		fbSetTimeoutDelay( 10 );
	}
}


//=========================================================================
//----- (000024F8) --------------------------------------------------------
__myevic__ void fbMoveBird()
{
	if ( fbDead )
	{
		++fbDead;
		fbDeleteTimeout( fbCurrentTimeout );
	}

	if ( dfFBSpeed == 2 )
	{
		fbBirdDisp += 6;
	}
	else if ( dfFBSpeed == 1 )
	{
		fbBirdDisp += 5;
	}
	else
	{
		fbBirdDisp += 4;
	}

	if ( fbBirdDisp <= 0 )
	{
		fbBirdLine = fbBirdLine + fbBirdDisp / 12 - 1;
	}
	else
	{
		fbBirdLine = fbBirdLine + fbBirdDisp / 12 + 1;
	}

	fbSetTimeoutDelay( 8 );
}


//=========================================================================
//----- (00006758) --------------------------------------------------------
__myevic__ void fbSetBirdColumn( uint8_t c )
{
  fbBirdColumn = c;
}


//=========================================================================
//----- (000009B0) --------------------------------------------------------
__myevic__ void fbStartScreen()
{
	fbScore = 0;

	if ( PE0 )
	{
		if ( fbAnimStep )
		{
			if ( ++fbBirdLine == 4 )
			fbAnimStep = 0;
		}
		else
		{
			--fbBirdLine;
			if ( fbBirdLine == 0 )
			fbAnimStep = 1;
		}
		fbSetBirdColumn( 100 );
		fbDrawText( 10, fbBirdLine + 16, "Flappy Bird" );
		fbBirdAnim( fbBirdLine + 16 );
		DisplayRefresh();
		fbCLSBuf();
		fbSetTimeoutDelay( 10 );
	}
	else
	{
		fbCreateColumn( 127, &fbColumn1 );
		fbCreateColumn( 127, &fbColumn2 );
		fbCreateColumn( 127, &fbColumn3 );
		fbSetBirdColumn( 16 );
		fbBirdLine = 24;
		fbAnimStep = 0;
		fbAnimTimer = 0;
		fbDead = 0;
		fbDeleteTimeout( fbCurrentTimeout );
		SetRandSeed( TMR1Counter );
		fbCreateTimeout( fbGame );
		fbCreateTimeout( fbCheckFire );
		fbCreateTimeout( fbMoveBird );
	}
}


//=========================================================================
__myevic__ void fbStartGame()
{
	UpdateDataFlash();
	if ( dfFBSpeed <= 2 )
	{
		gFlags.playing_fb = 1;
		Screen = 0;
		gFlags.user_idle = 1;
		gFlags.refresh_display = 1;
		NoEventTimer = 0;
		SleepTimer = 3000;
		fbInitTimeouts();
		ClearScreenBuffer();
		DisplayRefresh();
		fbBirdAnim( 24 );
		fbCreateTimeout( fbStartScreen + 1 );
	}
}
