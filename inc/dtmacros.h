#ifndef __DTMACROS_H__
#define __DTMACROS_H__

//-------------------------------------------------------------------------
// Those macros are quite impressive, but they are all compiled
// to a simple numeral constant. They lead to no run-time computation
// nor memory space.
//-------------------------------------------------------------------------

// Example of __DATE__ string: "Jul 27 2012"
//                              01234567890

#define MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a')
#define MONTH_IS_FEB (__DATE__[0] == 'F')
#define MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define MONTH_IS_SEP (__DATE__[0] == 'S')
#define MONTH_IS_OCT (__DATE__[0] == 'O')
#define MONTH_IS_NOV (__DATE__[0] == 'N')
#define MONTH_IS_DEC (__DATE__[0] == 'D')

#define __YEAR__ \
	( \
		(__DATE__[ 7] - '0') * 1000 + \
		(__DATE__[ 8] - '0') * 100 + \
		(__DATE__[ 9] - '0') * 10 + \
		(__DATE__[10] - '0') \
	)

#define __MONTH__ \
    ( \
        (MONTH_IS_JAN) ?  1 : \
        (MONTH_IS_FEB) ?  2 : \
        (MONTH_IS_MAR) ?  3 : \
        (MONTH_IS_APR) ?  4 : \
        (MONTH_IS_MAY) ?  5 : \
        (MONTH_IS_JUN) ?  6 : \
        (MONTH_IS_JUL) ?  7 : \
        (MONTH_IS_AUG) ?  8 : \
        (MONTH_IS_SEP) ?  9 : \
        (MONTH_IS_OCT) ? 10 : \
        (MONTH_IS_NOV) ? 11 : \
        (MONTH_IS_DEC) ? 12 : \
        /* error default */ 0 \
    )

#define __DAY__ \
	( \
		(__DATE__[4] - '0') * 10 + \
		(__DATE__[5] - '0') \
	)


// Example of __TIME__ string: "01:23:45"
//                              01234567

#define __HOURS__ \
	( \
		(__TIME__[0] - '0') * 10 + \
		(__TIME__[1] - '0') \
	)

#define __MINUTES__ \
	( \
		(__TIME__[3] - '0') * 10 + \
		(__TIME__[4] - '0') \
	)

#define __SECONDS__ \
	( \
		(__TIME__[6] - '0') * 10 + \
		(__TIME__[7] - '0') \
	)


// FAT16 Timestamping

#define FAT16_TODAY \
	( \
		(( __YEAR__ - 1980 ) << 9 ) | \
		(( __MONTH__ ) << 5 ) | \
		(( __DAY__ )) \
	)

#define FAT16_NOW \
	( \
		(( __HOURS__ ) << 11 ) | \
		(( __MINUTES__ ) << 5 ) | \
		(( __SECONDS__ )) \
	)
	
#endif
