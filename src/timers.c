#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "myrtc.h"
#include "flappy.h"

//==============================================================================

volatile uint32_t	TMR0Counter;
volatile uint32_t	TMR1Counter;
volatile uint32_t	TMR2Counter;
volatile uint32_t	TMR3Counter;

//==============================================================================
//----- (00007CD4) --------------------------------------------------------
__myevic__ void InitTimers()
{
  TIMER_Open( TIMER0, TIMER_PERIODIC_MODE, 100000u );
  TIMER_EnableInt( TIMER0 );
  TIMER_Open( TIMER1, TIMER_PERIODIC_MODE, 5000u );
  TIMER_EnableInt( TIMER1 );
  TIMER_Open( TIMER2, TIMER_PERIODIC_MODE, 1000u );
  TIMER_EnableInt( TIMER2 );
  TIMER_Open( TIMER3, TIMER_PERIODIC_MODE, 10u );
  TIMER_EnableInt( TIMER3 );

  NVIC_EnableIRQ(0x20u);
  NVIC_EnableIRQ(0x21u);
  NVIC_EnableIRQ(0x22u);
  NVIC_EnableIRQ(0x23u);

  TMR3Counter = 0;
  TMR2Counter = 0;
  TMR1Counter = 0;
  TMR0Counter = 0;

  TIMER_Start( TIMER0 );
  TIMER_Start( TIMER1 );
  TIMER_Start( TIMER2 );
  TIMER_Start( TIMER3 );
}


//==============================================================================
//----- (00007A2C) --------------------------------------------------------
// 100kHz Timer (HXT)

__myevic__ void TMR0_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER0 ) )
	{
		TIMER_ClearIntFlag( TIMER0 );

		++TMR0Counter;

		if ( TMR0Counter2 < 2000 )
			++TMR0Counter2;
	}
}


//==============================================================================
//----- (00007A5C) --------------------------------------------------------
// 5000Hz Timer (PCLK0)

__myevic__ void TMR1_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER1 ) )
	{
		TIMER_ClearIntFlag( TIMER1 );

		Flags64 |= 1;
		++TMR1Counter;
	}
}


//==============================================================================
//----- (00007A94) --------------------------------------------------------
// Millisecond timer (1000Hz) (HIRC)

__myevic__ void TMR2_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER2 ) )
	{
		TIMER_ClearIntFlag( TIMER2 );

		++ClockCorrection;

		Flags64 |= 6;

		if ( !(++TMR2Counter % 10u) )
		{
			Flags64 |= 8;
		}

		if ( Flags68 & 0x200 )
		{
			if ( (!(TMR2Counter % 16u) && dfFBSpeed == 0)
			  || (!(TMR2Counter % 13u) && dfFBSpeed == 1)
			  || (!(TMR2Counter % 10u) && dfFBSpeed == 2) )
			{
				fbTickTimeouts();
			}
		}
	}
}


//==============================================================================
//----- (00007B20) --------------------------------------------------------
// 10 Hz Timer (HXT)

__myevic__ void TMR3_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER3 ) )
	{
		TIMER_ClearIntFlag( TIMER3 );

		Flags64 |= 0x10u;

		if ( !(++TMR3Counter & 1) )
			Flags64 |=  0x20;

		if ( !(TMR3Counter % 5u) )
			Flags64 |= 0x40u;

		if ( !(TMR3Counter % 10u) )
			Flags68 |= 0x100u;
	}
}


//==============================================================================
//----- (00007D8C) --------------------------------------------------------

__myevic__ void TimedItems()
{
    if ( !Screen && SleepTimer )
        --SleepTimer;

    if ( !PD7 && ( byte_20000048 >= 2 ) && ( byte_20000048 < 50 ) )
        ++byte_20000048;

    if ( BatRefreshTmr )
        --BatRefreshTmr;

    if ( EditModeTimer )
    {
        if ( --EditModeTimer )
        {
            if ( !(EditModeTimer % 50) )
            {
                Flags64 ^= 0x40000u;
                MainView();
            }
        }
        else
        {
            Flags68 &= ~0x10u;
            Flags64 |= 0x60000u;
            ScreenDuration = 30;
            UpdateDFTimer = 50;
        }
    }

    if ( BatReadTimer )
    {
        if ( !--BatReadTimer )
            Flags64 |= 0x800u;
    }

    if ( FireClickTimer )
    {
        if ( !--FireClickTimer )
            FireClickCount = 0;
    }

    if ( ++BatAnimTimer >= 100u )
    {
        BatAnimTimer = 0;

        if ( Flags64 & 0x1000 )
        {
            Flags64 ^= 0x200000;

            if ( Screen == 1 || Screen == 3 || Screen == 5 )
            {
                if ( BatAnimLevel < 10 )
                    ++BatAnimLevel;
                else
                    BatAnimLevel = BatteryTenth;

                if ( Screen != 3 && Screen != 1 )
                    ScreenDuration = 30;

                Flags64 |= 0x20000;
            }
        }
        else if ( Flags64 & 0x80000 )
        {
            Flags64 ^= 0x100000;

            if ( Screen == 1 )
            {
                Flags64 |= 0x20000;
            }
        }
        else if ( Flags64 & 0x200000 || Flags64 & 0x100000 )
        {
            Flags64 &= ~0x300000u;

            if ( Screen == 1 || Screen == 3 )
            {
                Flags64 |= 0x20000;
            }
        }
    }
}


//=============================================================================
//----- (00001380) --------------------------------------------------------
__myevic__ void ResetWatchDog()
{
	SYS_UnlockReg();
	WDT_RESET_COUNTER();
	SYS_LockReg();
}


//==============================================================================
//----- (0000174C) --------------------------------------------------------
// Waits R0 ms
__myevic__ void WaitOnTMR2( int ms )
{
	Flags64 &= ~4;
	while ( ms )
	{
		if ( Flags64 & 4 )
		{
			--ms;
			Flags64 &= ~4;
		}
		ResetWatchDog();
	}
}

