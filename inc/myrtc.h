#ifndef __MERTC_H__
#define __MERTC_H__

#include <time.h>
#include "M451Series.h"

//==============================================================================

#define DEF_RTC_CLK_RATIO 23400

//==============================================================================

extern void InitRTC( S_RTC_TIME_DATA_T *d );
extern void GetRTC( S_RTC_TIME_DATA_T *rtd );
extern void SetRTC( S_RTC_TIME_DATA_T *rtd );

extern void RTCTimeToEpoch( time_t *t, const S_RTC_TIME_DATA_T *d );
extern void RTCEpochToTime( S_RTC_TIME_DATA_T *d, const time_t *t );

extern void RTCSetClockSpeed( const unsigned int cs );
extern unsigned int RTCGetClockSpeed();

//==============================================================================

#endif	/* __MERTC_H__ */

