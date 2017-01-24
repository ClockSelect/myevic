#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "events.h"
#include "myrtc.h"
#include "battery.h"
#include "atomizer.h"
#include "flappy.h"
#include "display.h"
#include "miscs.h"

//=========================================================================

volatile uint32_t	TMR0Counter;
volatile uint32_t	TMR1Counter;
volatile uint32_t	TMR2Counter;
volatile uint32_t	TMR3Counter;

volatile uint32_t	WarmUpCounter;
volatile uint32_t	TickCount;

uint16_t	SleepTimer;
uint16_t	AutoPuffTimer;
uint16_t	FadeOutTimer;


//=========================================================================
//----- (00007CD4) --------------------------------------------------------
__myevic__ void InitTimers()
{
	TIMER_Open( TIMER0, TIMER_PERIODIC_MODE, 100000 );
	TIMER_EnableInt( TIMER0 );
	TIMER_Open( TIMER1, TIMER_PERIODIC_MODE, 5000 );
	TIMER_EnableInt( TIMER1 );
	TIMER_Open( TIMER2, TIMER_PERIODIC_MODE, 1000 );
	TIMER_EnableInt( TIMER2 );
	TIMER_Open( TIMER3, TIMER_PERIODIC_MODE, 10 );
	TIMER_EnableInt( TIMER3 );

	NVIC_EnableIRQ( TMR0_IRQn );
	NVIC_EnableIRQ( TMR1_IRQn );
	NVIC_EnableIRQ( TMR2_IRQn );
	NVIC_EnableIRQ( TMR3_IRQn );

	TMR3Counter = 0;
	TMR2Counter = 0;
	TMR1Counter = 0;
	TMR0Counter = 0;

	TIMER_Start( TIMER0 );
	TIMER_Start( TIMER1 );
	TIMER_Start( TIMER2 );
	TIMER_Start( TIMER3 );
}


//=========================================================================
//----- (00007A2C) --------------------------------------------------------
// 100kHz Timer (HXT)

__myevic__ void TMR0_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER0 ) )
	{
		TIMER_ClearIntFlag( TIMER0 );

		++TMR0Counter;

		if ( WarmUpCounter )
			--WarmUpCounter;
	}
}


//=========================================================================
//----- (00007A5C) --------------------------------------------------------
// 5000Hz Timer (PCLK0)

__myevic__ void TMR1_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER1 ) )
	{
		TIMER_ClearIntFlag( TIMER1 );

		gFlags.tick_5khz = 1;
		++TMR1Counter;

		if ( gFlags.led_on )
		{
			LEDControl();
		}
	}
}


//=========================================================================
//----- (00007A94) --------------------------------------------------------
// Millisecond timer (1000Hz) (HIRC)

__myevic__ void TMR2_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER2 ) )
	{
		TIMER_ClearIntFlag( TIMER2 );

		gFlags.tick_1khz = 1;
		gFlags.tick_us = 1;

		if ( !(++TMR2Counter % 10) )
		{
			gFlags.tick_100hz = 1;
		}

		if ( gFlags.playing_fb )
		{
			if ( (!(TMR2Counter % 16) && dfFBSpeed == 0)
			  || (!(TMR2Counter % 13) && dfFBSpeed == 1)
			  || (!(TMR2Counter % 10) && dfFBSpeed == 2) )
			{
				fbTickTimeouts();
			}
		}
	}
}


//=========================================================================
//----- (00007B20) --------------------------------------------------------
// 10 Hz Timer (HXT)

__myevic__ void TMR3_IRQHandler()
{
	if ( TIMER_GetIntFlag( TIMER3 ) )
	{
		TIMER_ClearIntFlag( TIMER3 );

		gFlags.tick_10hz = 1;

		if ( !gFlags.has_x32 )
		{
			ClockCorrection += 1000;
		}

		if ( !(++TMR3Counter & 1) )
			gFlags.tick_5hz = 1;

		if ( !(TMR3Counter % 5) )
			gFlags.tick_2hz = 1;

		if ( !(TMR3Counter % 10) )
			gFlags.tick_1hz = 1;
	}
}


//=========================================================================
//----- (00007D8C) --------------------------------------------------------
// Called at 100Hz

__myevic__ void TimedItems()
{
	static  uint8_t BatAnimTimer = 0;

	if ( !Screen && SleepTimer )
		--SleepTimer;

	if ( ISVTCDUAL )
	{
		if ( !PD1 && ( BattProbeCount >= 2 ) && ( BattProbeCount < 50 ) && ( NumBatteries == 1 ) )
			++BattProbeCount;
	}
	else if ( !ISCUBOID && !ISCUBO200 && !ISRX200S && !ISRX23 && !ISRX300 )
	{
		if ( !PD7 && ( BattProbeCount >= 2 ) && ( BattProbeCount < 50 ) )
			++BattProbeCount;
	}

	if ( NoEventTimer )
		--NoEventTimer;

	if ( EditModeTimer )
	{
		if ( --EditModeTimer )
		{
			if ( !(EditModeTimer % 25) )
			{
				gFlags.draw_edited_item ^= 1;
				gFlags.refresh_display = 1;
			}
		}
		else
		{
			gFlags.edit_capture_evt = 0;
			gFlags.draw_edited_item = 1;
			UpdateDFTimer = 50;
			MainView();
		}
	}

	if ( BatReadTimer )
	{
		if ( !--BatReadTimer )
			gFlags.refresh_battery = 1;
	}

	if ( FireClickTimer )
	{
		if ( !--FireClickTimer )
			FireClickCount = 0;
	}

	if ( AutoPuffTimer )
		--AutoPuffTimer;

	if ( ++BatAnimTimer >= 100 )
	{
		BatAnimTimer = 0;

		if ( gFlags.battery_charging )
		{
			gFlags.draw_battery_charging ^= 1;

			if ( Screen == 1 || Screen == 5 )
			{
				if ( BatAnimLevel < 10 )
					++BatAnimLevel;
				else
					BatAnimLevel = BatteryTenth;

				if (( Screen == 5 ) && ( dfScreenSaver == SSAVER_NONE ))
					ScreenDuration = GetMainScreenDuration();

				gFlags.refresh_display = 1;
			}
		}
		else if ( gFlags.battery_10pc || gFlags.batteries_ooe )
		{
			gFlags.draw_battery ^= 1;

			if ( Screen == 1 )
			{
				gFlags.refresh_display = 1;
			}
		}
		else if ( gFlags.draw_battery_charging || gFlags.draw_battery )
		{
			gFlags.draw_battery = 1;
			gFlags.draw_battery_charging = 1;

			if ( Screen == 1 )
			{
				gFlags.refresh_display = 1;
			}
		}
	}

	if ( FadeOutTimer )
	{
		--FadeOutTimer;

		if ( FadeOutTimer < dfContrast )
		{
			DisplaySetContrast( FadeOutTimer );
		}
	}

	if ( PreheatDelay )
	{
		--PreheatDelay;
		if ( ( Screen == 1 ) && !( PreheatDelay % 25 ) )
		{
			gFlags.refresh_display = 1;
		}
	}

	++ChBalTimer;
}


//=========================================================================
//----- (00001380) --------------------------------------------------------
__myevic__ void ResetWatchDog()
{
	SYS_UnlockReg();
	WDT_RESET_COUNTER();
	SYS_LockReg();
}


//=========================================================================
//----- (0000174C) --------------------------------------------------------
// Waits R0 ms
__myevic__ void WaitOnTMR2( int ms )
{
	gFlags.tick_us = 0;
	while ( ms )
	{
		if ( gFlags.tick_us )
		{
			--ms;
			gFlags.tick_us = 0;
		}
		ResetWatchDog();
	}
}


//=========================================================================
// Tick Counter
//-------------------------------------------------------------------------
// The TickCount variable will sum up ticks between each pair of calls
// to StartTickCount and StopTickCount. It's the responsability of the
// caller to reset the TickCount variable when appropriate.
//-------------------------------------------------------------------------

__myevic__ void StartTickCount()
{
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL  = (0x00);
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

__myevic__ void StopTickCount()
{
	TickCount += 0xFFFFFF - SysTick->VAL;
	SysTick->CTRL = 0;
}
