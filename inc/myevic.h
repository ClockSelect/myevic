#ifndef __MYEVIC_H__
#define __MYEVIC_H__

#include "M451Series.h"
#include "myprintf.h"
#include "dtmacros.h"

//=========================================================================

#define __myevic__ __attribute__ ((section (".myevic")))

//-------------------------------------------------------------------------

#define CPU_FREQ		72000000
#define PLL_FREQ		(CPU_FREQ<<1)

#define BBC_PWM_FREQ	150000

#define ENABLE_UART		0

//-------------------------------------------------------------------------

#define	ISMODETC(m) ((m)<=3)
#define ISMODEVW(m) (((m)==4)||((m)==6))
#define ISMODEBY(m) ((m)==5)

//-------------------------------------------------------------------------

#define FWVERSION	330

#define __BUILD1	( __DAY__ + __MONTH__ * 100 + ( __YEAR__ % 100 ) * 10000 )

#define __BUILD2	(	__DAY__ % 10 + ( __DAY__ / 10 << 4 ) \
					+	( __MONTH__ % 10 << 8 ) + ( __MONTH__ / 10 << 12 ) \
					+	( __YEAR__ % 10 << 16 ) + ( __YEAR__ / 10 % 10 << 20 ) )

#define __BUILD3	( ( __YEAR__ % 100 ) + ( __MONTH__ << 8 ) + ( __DAY__ << 16 ) )

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

enum
{
	BOX_VTCMINI,
	BOX_VTWOMINI,
	BOX_PRIMOMINI,
	BOX_VTWO,
	BOX_PRESA75W,
	BOX_WRX75TC,
	BOX_PRESA100W,
	BOX_EVICAIO,
	BOX_EGRIPII,
	BOX_CUBOMINI,
	BOX_EVICBASIC,
	BOX_VTCDUAL,
	BOX_CUBOID,
	BOX_CUBO200,
	BOX_RX200S,
	BOX_RX23,
	BOX_RX300,
	BOX_RXMINI,
	BOX_PRIMO1,
	BOX_PRIMO2,
	BOX_PREDATOR
};

#define ISVTCMINI		(BoxModel==BOX_VTCMINI)
#define ISVTWOMINI		(BoxModel==BOX_VTWOMINI)
#define ISPRIMOMINI		(BoxModel==BOX_PRIMOMINI)
#define ISVTWO			(BoxModel==BOX_VTWO)
#define ISPRESA75W		(BoxModel==BOX_PRESA75W)
#define ISPRESA100W		(BoxModel==BOX_PRESA100W)
#define ISWRX75TC		(BoxModel==BOX_WRX75TC)
#define ISEVICAIO		(BoxModel==BOX_EVICAIO)
#define ISEGRIPII		(BoxModel==BOX_EGRIPII)
#define ISCUBOMINI		(BoxModel==BOX_CUBOMINI)
#define ISEVICBASIC		(BoxModel==BOX_EVICBASIC)
#define ISVTCDUAL		(BoxModel==BOX_VTCDUAL)
#define ISCUBOID		(BoxModel==BOX_CUBOID)
#define ISCUBO200		(BoxModel==BOX_CUBO200)
#define ISRX200S		(BoxModel==BOX_RX200S)
#define ISRX23			(BoxModel==BOX_RX23)
#define ISRX300			(BoxModel==BOX_RX300)
#define ISRXMINI		(BoxModel==BOX_RXMINI)
#define ISPRIMO1		(BoxModel==BOX_PRIMO1)
#define ISPRIMO2		(BoxModel==BOX_PRIMO2)
#define ISPREDATOR		(BoxModel==BOX_PREDATOR)


//=========================================================================
// Global flags bitfield
//-------------------------------------------------------------------------

typedef struct
{
/* 00000001 */	int	tick_5khz:1;
/* 00000002 */	int	tick_1khz:1;
/* 00000004 */	int	tick_us:1;
/* 00000008 */	int	tick_100hz:1;
/* 00000010 */	int	tick_10hz:1;
/* 00000020 */	int	tick_5hz:1;
/* 00000040 */	int	tick_2hz:1;
/* 00000080 */	int read_battery:1;

/* 00000100 */	int firing:1;
/* 00000200 */	int low_battery:1;
/* 00000400 */	int usb_attached:1;
/* 00000800 */	int refresh_battery:1;
/* 00001000 */	int battery_charging:1;
/* 00002000 */	int probing_ato:1;
/* 00004000 */	int user_idle:1;
/* 00008000 */	int sample_vbat:1;

/* 00010000 */	int sample_btemp:1;
/* 00020000 */	int refresh_display:1;
/* 00040000 */	int draw_edited_item:1;
/* 00080000 */	int battery_10pc:1;
/* 00100000 */	int draw_battery:1;
/* 00200000 */	int draw_battery_charging:1;
/* 00400000 */	int decrease_voltage:1;
/* 00800000 */	int check_mode:1;

/* 01000000 */	int unused24:1;
/* 02000000 */	int unused25:1;
/* 04000000 */	int limit_ato_temp:1;
/* 08000000 */	int new_rez_ti:1;
/* 10000000 */	int new_rez_ni:1;
/* 20000000 */	int limit_power:1;
/* 40000000 */	int batteries_ooe:1;
/* 80000000 */	int new_rez_ss:1;

/* 00000001 */	int edit_value:1;
/* 00000002 */	int new_rez_tcr:1;
/* 00000004 */	int unused34:1;
/* 00000008 */	int bad_cell:1;
/* 00000010 */	int edit_capture_evt:1;
/* 00000020 */	int osc_1hz:1;
/* 00000040 */	int unused38:1;
/* 00000080 */	int batt_unk:1;

/* 00000100 */	int	tick_1hz:1;
/* 00000200 */	int	playing_fb:1;
/* 00000400 */	int has_x32:1;
/* 00000800 */	int unused43:1;
/* 00001000 */	int debug:1;
/* 00002000 */	int noclock:1;
/* 00004000 */	int read_bir:1;
/* 00008000 */	int monitoring:1;

/* 00010000 */	int anim3d:1;
/* 00020000 */	int pwm_pll:1;
/* 00040000 */	int rtcinit:1;
/* 00080000 */	int light_sleep:1;
/* 00100000 */	int wake_up:1;
/* 00200000 */	int fading:1;
/* 00400000 */	int led_on:1;
/* 00800000 */	int splash:1;
}

gFlags_t;

extern volatile gFlags_t gFlags;
extern uint8_t BoxModel;


//=========================================================================
// Functions from assembly language part
//-------------------------------------------------------------------------

extern void MemCpy( void*, const void*, const uint32_t );
extern void MemCpy2( const void*, void*, const uint32_t );
extern void MemClear( void*, const uint32_t );
extern void MemClear2( void*, void*, const uint32_t );
extern void MemSet( void*, const char, const uint32_t );

extern void DrawHexDigit( int x, int y, int v );
extern void DrawHexLong( int x, int y, int v, int font );

//-------------------------------------------------------------------------

extern uint32_t sqrtul( uint32_t v );
extern uint32_t GetFirmwareSize();

//=========================================================================
#endif /* __MYEVIC_H__ */
