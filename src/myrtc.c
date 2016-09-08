#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "myprintf.h"
#include "myrtc.h"

#include <time.h>

//=============================================================================

volatile int32_t ClockCorrection = 0;

//=============================================================================

static time_t ref_date = 0;
static int32_t adjustment = 0;

static volatile uint8_t int_cnt = 0;
static volatile int64_t adj_10k = 0;
static int32_t sleep_ticks = 0;

//=============================================================================

__myevic__ void RTC_IRQHandler()
{
	if ( RTC_GET_TICK_INT_FLAG() )
	{
		RTC_CLEAR_TICK_INT_FLAG();

		if ( int_cnt < 3 )
		{
			++int_cnt;

			adj_10k += (int64_t)RTCGetClockSpeed() - 32768;
		}
		else
		{
			adj_10k += ClockCorrection * 2 - 32768;
		}

		ClockCorrection = 0;
	}
}


//=============================================================================

static const char mon_days[] =
			   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

__myevic__ time_t time_to_epoch ( const struct tm *ltm, int utcdiff )
{
	long tyears, tdays, leaps, utc_hrs;
	int i;

	tyears = ltm->tm_year - 70 ; // tm->tm_year is from 1900.
	leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
	//i = (ltm->tm_year – 100) / 100;
	//leaps -= ( (i/4)*3 + i%4 );
	tdays = 0;
	for (i=0; i < ltm->tm_mon; i++) tdays += mon_days[i];

	tdays += ltm->tm_mday-1; // days of month passed.
	tdays = tdays + (tyears * 365) + leaps;

	utc_hrs = ltm->tm_hour + utcdiff; // for your time zone.
	return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
}

#define	YEAR0				1900			/* the first year */
#define	EPOCH_YR			1970			/* EPOCH = Jan 1 1970 00:00:00 */
#define	SECS_DAY			(24L * 60L * 60L)
#define	LEAPYEAR(year)		(!((year) %	4) && (((year) % 100) || !((year) %	400)))
#define	YEARSIZE(year)		(LEAPYEAR(year)	? 366 :	365)

static const char _ytab[2][12] = {
						{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
						{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }	};

__myevic__ struct tm * epoch_to_time(struct tm *timep, register const time_t *timer)
{
	time_t time = *timer;
	register unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long)time % SECS_DAY;
	dayno = (unsigned long)time / SECS_DAY;

	timep->tm_sec = dayclock % 60;
	timep->tm_min = (dayclock % 3600) / 60;
	timep->tm_hour = dayclock / 3600;
	timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
	while (dayno >= YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	timep->tm_year = year - YEAR0;
	timep->tm_yday = dayno;
	timep->tm_mon = 0;
	while (dayno >= _ytab[LEAPYEAR(year)][timep->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
		timep->tm_mon++;
	}
	timep->tm_mday = dayno + 1;
	timep->tm_isdst = 0;

	return timep;
}

//=============================================================================

__myevic__ void RTCTimeToEpoch( time_t *t, const S_RTC_TIME_DATA_T *d )
{
	struct tm s;

	s.tm_year  = d->u32Year - RTC_YEAR2000 + 100;
	s.tm_mon   = d->u32Month - 1;
	s.tm_mday  = d->u32Day;
	s.tm_hour  = d->u32Hour;
	s.tm_min   = d->u32Minute;
	s.tm_sec   = d->u32Second;
	s.tm_yday  = 0;
	s.tm_wday  = 0;
	s.tm_isdst = 0;

	*t = time_to_epoch( &s, 0 );
}


__myevic__ void RTCEpochToTime( S_RTC_TIME_DATA_T *d, const time_t *t )
{
	struct tm s;

	epoch_to_time( &s, t );

	d->u32Year		= s.tm_year + RTC_YEAR2000 - 100;
	d->u32Month		= s.tm_mon + 1;
	d->u32Day		= s.tm_mday;
	d->u32Hour		= s.tm_hour;
	d->u32Minute	= s.tm_min;
	d->u32Second	= s.tm_sec;
	d->u32DayOfWeek	= s.tm_wday;
	d->u32TimeScale	= RTC_CLOCK_24;
}

//=============================================================================

__myevic__ void RTCFullAccess()
{
	if ( !(RTC->RWEN&RTC_RWEN_RWENF_Msk) || !(RTC->SPRCTL&RTC_SPRCTL_SPRRWEN_Msk) )
	{
		RTC_EnableSpareAccess();
	}
}

__myevic__ void RTCSetReferenceDate( time_t *t )
{
	ref_date = *t;

	RTCFullAccess();
	RTC_WRITE_SPARE_REGISTER( 0, ref_date );
}

__myevic__ time_t RTCGetReferenceDate()
{
	if ( !ref_date )
	{
		RTCFullAccess();
		ref_date = (time_t)RTC_READ_SPARE_REGISTER( 0 );
	}
	return ref_date;
}

__myevic__ void RTCSetClockSpeed( const unsigned int cs )
{
	dfClkRatio = cs;
	UpdateDFTimer = 50;
}

__myevic__ unsigned int RTCGetClockSpeed()
{
	if ( ! dfClkRatio )
	{
		dfClkRatio = RTC_DEF_CLK_RATIO;
		UpdateDFTimer = 50;
	}
	return dfClkRatio;
}

//=============================================================================

__myevic__ void InitRTC( S_RTC_TIME_DATA_T *d )
{
	register int rtccnt;

	SYS_UnlockReg();

	CLK_EnableModuleClock( RTC_MODULE );

	if ( gFlags.has_x32 )
	{
		RTC->LXTCTL |= RTC_LXTCTL_LXTEN_Msk;
		CLK_SetModuleClock( RTC_MODULE, CLK_CLKSEL3_RTCSEL_LXT, 0 );
	}
	else
	{
		// Enable LIRC 10kHz clock
		if ( !( CLK->STATUS & CLK_STATUS_LIRCSTB_Msk ) )
		{
			CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
			CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk );
		}

		CLK_SetModuleClock( RTC_MODULE, CLK_CLKSEL3_RTCSEL_LIRC, 0 );
	}

	SYS_LockReg();

	// Try to Open RTC
	rtccnt = 100000;	// Give it plenty of time
	RTC->INIT = RTC_INIT_KEY;
	if ( RTC->INIT != RTC_INIT_ACTIVE_Msk )
	{
		RTC->INIT = RTC_INIT_KEY;
		while ( RTC->INIT != RTC_INIT_ACTIVE_Msk )
			if (--rtccnt)
				break;
	}

	if ( !rtccnt )
	{
		// Failed to open the RTC.
		// Should not occur if the X32 works.
		if ( gFlags.has_x32 )
		{
			// Disable X32 and retry.
			gFlags.has_x32 = 0;
			InitRTC( d );
			return;
		}
		else
		{
			// RTC should always accept to be opened
			// with the LIRC.
			// Real hardware error.
			// No clock available.
			gFlags.noclock = 1;
			return;
		}
	}

	// Check that everything works fine with the X32 crystal.
	// If not, revert to the LIRC.
	if ( gFlags.has_x32 )
	{
		// Checking that we correctly get access to the RTC registers
		// should be a good test.
		rtccnt = 100000;	// Plenty again
		while( ( RTC->RWEN & RTC_RWEN_RWENF_Msk ) == RTC_RWEN_RWENF_Msk );
		RTC->RWEN = RTC_WRITE_KEY;
		while( ( RTC->RWEN & RTC_RWEN_RWENF_Msk ) == 0x0 )
			if ( !--rtccnt )
				break;

		// In case we did not acces to the protected registers,
		// disable X32 usage and  re-init the RTC with the LIRC.
		if ( !rtccnt )
		{
			gFlags.has_x32 = 0;
			InitRTC( d );
			return;
		}
	}
	else
	{
		RTC_32KCalibration( 3276800 );
	}

	if ( d )
	{
		SetRTC( d );
	}
	else
	{
		RTCWakeUp();
	}

	if ( !gFlags.has_x32 )
	{
		NVIC_EnableIRQ( RTC_IRQn );
	}
}


//=============================================================================

__myevic__ void SetRTC( S_RTC_TIME_DATA_T *rtd )
{
	time_t t;

	if ( gFlags.noclock )
		return;

	__set_PRIMASK( 1 );

	RTC_SetDateAndTime( rtd );

	RTCTimeToEpoch( &t, rtd );
	RTCSetReferenceDate( &t );

	ClockCorrection = 0;
	adjustment = 0;

	adj_10k = 0;
	sleep_ticks = 0;

	__set_PRIMASK( 0 );
}


//=============================================================================

__myevic__ void GetRTC( S_RTC_TIME_DATA_T *rtd )
{
	time_t t, ref;
	uint32_t cs;
	uint64_t d;

	if ( gFlags.noclock )
	{
		MemClear( rtd, sizeof( rtd ) );
		return;
	}

	RTC_GetDateAndTime( rtd );

	if ( ( !gFlags.has_x32 ) || ( adjustment != 0 ) )
	{
		RTCTimeToEpoch( &t, rtd );

		if ( !gFlags.has_x32 )
		{
			ref = RTCGetReferenceDate();
			cs  = RTCGetClockSpeed();

		//	d = (( (uint64_t)t - ref ) * cs + 2 * ClockCorrection ) / 10000;
		//	t = ref + d;

			d  = 32768ull * ( t - ref - sleep_ticks );
			d += (int64_t)ClockCorrection * 2 + adj_10k;
			d += (int64_t)sleep_ticks * cs;
			d /= 10000;

			t  = ref + d;
		}

		t += adjustment;

		RTCEpochToTime( rtd, &t );
	}
}


//=============================================================================

__myevic__ void RTCAdjustClock( int seconds )
{
	if ( gFlags.noclock )
		return;

	if ( seconds )
	{
		adjustment += seconds;
	}
	else
	{
		int adj;
		time_t ref, t;
		int64_t adj_frac;
		S_RTC_TIME_DATA_T rtd;

		adj = adjustment;
		ref = RTCGetReferenceDate();

		GetRTC( &rtd );
		adj_frac = ( adj_10k + 2 * ClockCorrection ) % 10000;
		SetRTC( &rtd );
		adj_10k = adj_frac;

		if ( gFlags.has_x32 )
		{
			RTCTimeToEpoch( &t, &rtd );

			int dt1 = t - ref;
			int dt2 = t + adj - ref;

			if ( dt1 <= 0 || dt2 <= 0 ) return;

			uint64_t f1 =
				  (((( RTC->FREQADJ & RTC_FREQADJ_INTEGER_Msk )
						>> RTC_FREQADJ_INTEGER_Pos )
							+ RTC_FCR_REFERENCE ) * 100 )
				+ ((( RTC->FREQADJ & RTC_FREQADJ_FRACTION_Msk )
						>> RTC_FREQADJ_FRACTION_Pos ) * 100) / 60;

			int32_t f2 = f1 * dt2 / dt1;

			RTC_32KCalibration( f2 );
		}
	}
}


//=============================================================================

__myevic__ void RTCSleep()
{
	if ( gFlags.noclock )
		return;

	if ( gFlags.has_x32 )
		return;

	RTC_DisableInt( RTC_INTEN_TICKIEN_Msk );

	RTCAdjustClock( 0 );
}


__myevic__ void RTCWakeUp()
{
	if ( gFlags.noclock )
		return;

	if ( gFlags.has_x32 )
		return;

	S_RTC_TIME_DATA_T rtd;
	time_t ref, t;

	RTC_GetDateAndTime( &rtd );
	RTCTimeToEpoch( &t, &rtd );
	ref = RTCGetReferenceDate();

	sleep_ticks = ( t - ref );
	int_cnt = 0;

	RTC_EnableInt( RTC_INTEN_TICKIEN_Msk );
}

