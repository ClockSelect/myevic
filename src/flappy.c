#include "myevic.h"
#include "dataflash.h"
#include "display.h"
#include "miscs.h"

#include "flappy.h"


//=========================================================================
//----- (000013A4) --------------------------------------------------------
__myevic__ void fbCreateColumn( int8_t hpos, fbColumn_t *column )
{
	uint8_t vpos;

	if ( hpos > 0 )
	{
		column->x = hpos;
		vpos = Random() % 24 + 1;
		column->unk2 = vpos >> 3;
		column->unk1 = vpos;
		vpos += 40;
		column->unk3 = vpos - 8;
		column->unk4 = (64 - vpos) >> 3;
	}
}


//=========================================================================
//----- (00000854) --------------------------------------------------------
__myevic__ void fbDeathScreen()
{
	switch ( fbAnimStep )
	{
		case 0:
			fbFillScreen( 0xFF );
			DisplayRefresh();
			fbCLSBuf();
			fbSetTimeoutValue( 5 );
			++fbAnimStep;
			break;

		case 1:
			DisplayRefresh();
			fbSetTimeoutValue( 10 );
			++fbAnimStep;
			break;

		case 3:
			if ( !PE0 )
			{
				fbAnimStep = 4;
				fbSetTimeoutValue(100);
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
				if ( fdDead == 3 )
				{
					fdDead = 0;
					fbAnimStep = 3;
				}
				fbSetTimeoutValue( 200 );
			}
			else
			{
				fbBirdLine += 4;
			}
			fbDrawDeadBird( fbBirdLine );
			fbDrawColumn( fbColumn1.x, fbColumn1.unk4 );
			fbDrawColumn( fbColumn2.x, fbColumn2.unk4 );
			fbDrawColumn( fbColumn3.x, fbColumn3.unk4 );
			fbDrawRect( 26, 16, 98, 52, 0, 1 );
			fbDrawRect( 26, 16, 98, 52, 1, 0 );
			fbDrawText( 27, 18, "SCORE" );
			fbDrawText( 27, 36, "BEST" );
			fbDrawNumber( 71, 18, fbNumDigits( fbScore ), fbScore );
			fbDrawNumber( 71, 36, fbNumDigits( dfFBBest ), dfFBBest );
			DisplayRefresh();
			fbCLSBuf();
			fbSetTimeoutValue( 4 );
			UpdateDFTimer = 50;
			break;
	}
}


//=========================================================================
//----- (00000A94) --------------------------------------------------------
__myevic__ void fbGame()
{
	int nd;

	if ( fdDead )
	{
		fbAnimTimer = 0;
		fbAnimStep = 0;
		if ( dfFBBest < fbScore ) dfFBBest = fbScore;
		fbDeleteTimeout( fbCurrentTimeout );
		fbCreateTimeout( fbDeathScreen );
		return;
	}

	fbDrawColumn( fbColumn1.x, fbColumn1.unk4 );
	fbDrawColumn( fbColumn2.x, fbColumn2.unk4 );
	fbDrawColumn( fbColumn3.x, fbColumn3.unk4 );

	if		( fbAnimStep == 0 ) fbColumn1.x -= 2;
	else if	( fbAnimStep == 1 ) fbColumn2.x -= 2;
	else if	( fbAnimStep == 2 ) fbColumn3.x -= 2;

	if ( ++fbAnimTimer == 32 )
	{
		fbAnimTimer = 0;
		if ( fbAnimStep < 2 ) ++fbAnimStep;
	}

	if ( fbColumn1.x < -55 ) fbCreateColumn( 127, &fbColumn1 );
	if ( fbColumn2.x < -55 ) fbCreateColumn( 127, &fbColumn2 );
	if ( fbColumn3.x < -55 ) fbCreateColumn( 127, &fbColumn3 );

	if ( fbColumn1.x == 21 || fbColumn2.x == 21 || fbColumn3.x == 21 ) ++fbScore;

	if ( fbBirdLine >= 48 ) fdDead = 1;
	else fbBirdAnim( fbBirdLine );

	if ( fbColumn1.x >= -8
			&& (fbColumn1.unk1 + 6 > fbBirdLine || fbColumn1.unk3 - 12 < fbBirdLine) )
	{
		fdDead = 1;
	}

	if ( fbColumn2.x >= -8
			&& (fbColumn2.unk1 + 6 > fbBirdLine || fbColumn2.unk3 - 12 < fbBirdLine) )
	{
		fdDead = 1;
	}

	if ( fbColumn3.x >= -8
			&& (fbColumn3.unk1 + 6 > fbBirdLine || fbColumn3.unk3 - 12 < fbBirdLine) )
	{
		fdDead = 1;
	}

	nd = fbNumDigits( fbScore );
	fbDrawNumber( 64 - 4 * nd, 4, nd, fbScore);
	DisplayRefresh();
	fbCLSBuf();
	fbSetTimeoutValue( 4 );
}


//=========================================================================
//----- (00008834) --------------------------------------------------------
__myevic__ void fbCheckFire()
{
	if ( fdDead )
	{
		++fdDead;
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

		fbSetTimeoutValue( 10 );
	}
}


//=========================================================================
//----- (000024F8) --------------------------------------------------------
__myevic__ void fbMoveBird()
{
	if ( fdDead )
	{
		++fdDead;
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

	fbSetTimeoutValue( 8 );
}

