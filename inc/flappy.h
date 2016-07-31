#ifndef __FLAPPY_BIRD_H__
#define __FLAPPY_BIRD_H__

#include <sys/types.h>

typedef struct
{
	union {
		struct {
			int8_t	x;
			int8_t	unk1;
			int8_t	unk2;
			int8_t	unk3;
		};
		uint32_t unk32;
	};
	int8_t	unk4;
}
fbColumn_t;

extern uint8_t		fbBirdColumn;
extern uint8_t		fbBirdCycle;
extern uint8_t		fbBirdLine;
extern int8_t		fbBirdDisp;
extern uint8_t		fbDead;
extern uint8_t		fbAnimStep;
extern uint8_t		fbAnimTimer;
extern uint16_t		fbScore;
extern fbColumn_t	fbColumn1;
extern fbColumn_t	fbColumn2;
extern fbColumn_t	fbColumn3;
extern uint8_t		fbTimeoutMask;
extern uint8_t		fbCurrentTimeout;
extern uint8_t		fbUsedTimeouts;

extern void fbFillScreen( uint8_t byte );
extern void fbCLSBuf();
extern void fbSetTimeoutValue( int );
extern void fbDrawColumn( int32_t, int8_t );
extern int	fbCreateTimeout( void(*)() );
extern void fbDeleteTimeout( int );
extern void fbDrawDeadBird( int fbBirdLine );
extern void fbDrawRect( int, int, int, int, int, int );
extern void fbDrawText( int, int, const char * );
extern void fbDrawNumber( int x, int y, int nd, int v );
extern int	fbNumDigits( int v );
extern void fbCreateColumn( int8_t hpos, fbColumn_t *column );


#endif /* __FLAPPY_BIRD_H__ */
