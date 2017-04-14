#ifndef __TIMERS_H__
#define __TIMERS_H__

//=========================================================================

extern volatile uint32_t TMR0Counter;
extern volatile uint32_t TMR1Counter;
extern volatile uint32_t TMR2Counter;
extern volatile uint32_t TMR3Counter;

extern volatile uint32_t WarmUpCounter;
extern volatile uint32_t TickCount;

extern uint16_t	SleepTimer;
extern uint16_t	AutoPuffTimer;
extern uint16_t	FadeOutTimer;

//-------------------------------------------------------------------------

extern void InitTimers();
extern void TimedItems();
extern void WaitOnTMR2( int ms );
extern void ResetWatchDog();

extern void StartTickCount();
extern void StopTickCount();

//=========================================================================

#endif /* __TIMERS_H__ */
