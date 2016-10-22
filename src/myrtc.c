#include "myevic.h"
#include "dataflash.h"
#include "screens.h"
#include "timers.h"
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

static uint16_t cache_flags;
static uint32_t spare_cache[16];

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
			adj_10k += ClockCorrection - 32768;
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

__myevic__ int RTCEnableAccess()
{
	// Wait RWENF bit is cleared
	// and enable RWENF bit (Access Enable bit) again

	int c;

	// Potential waiting time is 1024 RTC cycles
	// If the RTC is driven by the LIRC, it's 0.1024s
	c = 102400;
	while( ( RTC->RWEN & RTC_RWEN_RWENF_Msk ) )
	{
		if ( !--c ) return 0;
		CLK_SysTickDelay( 1 );
	}

	RTC->RWEN = RTC_WRITE_KEY;
	// Wait RWENF bit is set and user can access
	// the protected-register of RTC from now on
	c = 10000;
	while( !( RTC->RWEN & RTC_RWEN_RWENF_Msk ) )
	{
		if ( !--c ) return 0;
		CLK_SysTickDelay( 1 );
	}

	return 1;
}


__myevic__ int RTCSpareAccess()
{
	int c;

	if ( !RTCEnableAccess() )
		return 0;

	RTC->SPRCTL |= RTC_SPRCTL_SPRRWEN_Msk;

	c = 10000;
	while( !( RTC->SPRCTL & RTC_SPRCTL_SPRRWRDY_Msk ) )
	{
		if ( !--c ) return 0;
		CLK_SysTickDelay( 1 );
	}

	return 1;
}

__myevic__ void RTCWriteRegister( uint32_t r, uint32_t v )
{
	if ( !gFlags.rtcinit )
		return;

	if ( gFlags.noclock )
		return;

	if ( RTCSpareAccess() )
	{
		RTC_WRITE_SPARE_REGISTER( r, v );

		cache_flags |= ( 1 << r );
		spare_cache[r] = v;
	}
}

__myevic__ uint32_t RTCReadRegister( uint32_t r )
{
	uint32_t v = 0;

	if ( cache_flags & ( 1 << r ) )
		return spare_cache[r];

	if ( !gFlags.rtcinit )
		return 0;

	if ( gFlags.noclock )
		return 0;

	if ( RTCSpareAccess() )
	{
		v = RTC_READ_SPARE_REGISTER( r );

		cache_flags |= ( 1 << r );
		spare_cache[r] = v;
	}

	return v;
}

__myevic__ void RTCSetReferenceDate( time_t *t )
{
	ref_date = *t;

	RTCWriteRegister( RTCSPARE_REF_DATE, ref_date );
}

__myevic__ time_t RTCGetReferenceDate()
{
	if ( !ref_date )
	{
		ref_date = RTCReadRegister( RTCSPARE_REF_DATE );
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

__myevic__ void RTCStart( S_RTC_TIME_DATA_T *d )
{
	register int rtccnt;
	uint32_t rtcclk;

	SYS_UnlockReg();

	if ( !dfStatus.x32off && gFlags.has_x32 )
	{
		if ( !( CLK->STATUS & CLK_STATUS_LXTSTB_Msk ) )
		{
			//  32.768kHz external crystal
			SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF0MFP_Msk|SYS_GPF_MFPL_PF1MFP_Msk);
			SYS->GPF_MFPL |=  (SYS_GPF_MFPL_PF0MFP_X32_OUT|SYS_GPF_MFPL_PF1MFP_X32_IN);

			CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
			if ( CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) )
			{
				rtcclk = CLK_CLKSEL3_RTCSEL_LXT;
			}
			else
			{
				gFlags.has_x32 = 0;
			}
		}
	}
	else
	{
		gFlags.has_x32 = 0;
	}

	if ( !gFlags.has_x32 )
	{
		CLK_DisableXtalRC( CLK_PWRCTL_LXTEN_Msk );

		// Enable LIRC 10kHz clock
		if ( !( CLK->STATUS & CLK_STATUS_LIRCSTB_Msk ) )
		{
			CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
			CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk );
		}

		rtcclk = CLK_CLKSEL3_RTCSEL_LIRC;
	}

	CLK_EnableModuleClock( RTC_MODULE );
	CLK_SetModuleClock( RTC_MODULE, rtcclk, 0 );

	// Try to Open RTC
	rtccnt = 10000;	// Give it plenty of time
	RTC->INIT = RTC_INIT_KEY;
	if ( RTC->INIT != RTC_INIT_ACTIVE_Msk )
	{
		RTC->INIT = RTC_INIT_KEY;
		while ( RTC->INIT != RTC_INIT_ACTIVE_Msk )
		{
			if (!--rtccnt)
				break;
			CLK_SysTickDelay( 1 );
		}
	}

	if ( !rtccnt )
	{
		// Failed to open the RTC.
		// Should not occur if the X32 works.
		if ( gFlags.has_x32 )
		{
			// Disable X32 and retry.
			gFlags.has_x32 = 0;
			RTCStart( d );
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
		if ( !RTCEnableAccess() )
		{
			// In case we did not acces to the protected registers,
			// disable X32 usage and  re-init the RTC with the LIRC.
			gFlags.has_x32 = 0;
			RTCStart( d );
			return;
		}
	}
	else
	{
		RTC_32KCalibration( 3276800 );
	}

	SYS_LockReg();

	if ( d )
	{
		SetRTC( d );
	}

	if ( !gFlags.has_x32 )
	{
		RTC_EnableInt( RTC_INTEN_TICKIEN_Msk );
		NVIC_EnableIRQ( RTC_IRQn );
	}
}


//=============================================================================

__myevic__ void SetRTC( S_RTC_TIME_DATA_T *rtd )
{
	time_t t;

	if ( !gFlags.rtcinit )
		return;

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

	if ( gFlags.noclock || !gFlags.rtcinit )
	{
		MemClear( rtd, sizeof( *rtd ) );
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

			d  = 32768ull * ( t - ref - sleep_ticks );
			d += (int64_t)ClockCorrection + adj_10k;
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
	if ( !gFlags.rtcinit )
		return;

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
		adj_frac = ( adj_10k + ClockCorrection ) % 10000;
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

__myevic__ time_t RTCGetEpoch( time_t *t )
{
	time_t e;
	S_RTC_TIME_DATA_T rtd;

	GetRTC( &rtd );
	RTCTimeToEpoch( &e, &rtd );
	if ( t ) *t = e;
	return e;
}


//=============================================================================

__myevic__ void RTCSleep()
{
	if ( !gFlags.rtcinit )
		return;

	if ( gFlags.noclock )
		return;

	if ( gFlags.light_sleep )
		return;

	if ( gFlags.has_x32 )
		return;

	RTC_DisableInt( RTC_INTEN_TICKIEN_Msk );
	RTCAdjustClock( 0 );
}


__myevic__ void RTCWakeUp()
{
	if ( !gFlags.rtcinit )
		return;

	if ( gFlags.noclock )
		return;

	if ( gFlags.light_sleep )
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

