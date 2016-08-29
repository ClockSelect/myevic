#ifndef __MERTC_H__
#define __MERTC_H__

#include <time.h>
#include "M451Series.h"

//=========================================================================
// REAL-TIME CLOCK
//=========================================================================

//=========================================================================
// LIRC correction ratio
//-------------------------------------------------------------------------
// On boards without a 32k Xtal, we use the LIRC to drive the RTC. Since
// its frequency is way off, a corrective ratio is necessary to compute the
// real time. Since it may vary from box to box, it is adjustable by the
// "Clk Speed" configuration menu.
// This value is unused if a X32 is soldered on the board.

#define RTC_DEF_CLK_RATIO 33700

//=========================================================================
// When the RTC is driven by the LIRC, a "second" of the RTC is approx
// 2.36 real seconds, but we'd like the clock to display with a correct
// second tick. The correction value is driven by the 1kHZ TMR2 and reset
// by the RTC tick.
// This value is unused if a X32 is soldered on the board.

extern volatile uint16_t ClockCorrection;

//=========================================================================
// Functions

extern void InitRTC( S_RTC_TIME_DATA_T *d );
extern void GetRTC( S_RTC_TIME_DATA_T *rtd );
extern void SetRTC( S_RTC_TIME_DATA_T *rtd );

extern void RTCTimeToEpoch( time_t *t, const S_RTC_TIME_DATA_T *d );
extern void RTCEpochToTime( S_RTC_TIME_DATA_T *d, const time_t *t );

extern void RTCSetClockSpeed( const unsigned int cs );
extern unsigned int RTCGetClockSpeed();
extern void RTCAdjustClock( int seconds );

//=========================================================================

#endif	/* __MERTC_H__ */

