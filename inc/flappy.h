#ifndef __FLAPPY_BIRD_H__
#define __FLAPPY_BIRD_H__

#include <sys/types.h>

typedef struct
{
	int8_t	x;
	uint8_t	unk1;
	uint8_t	unk2;
	uint8_t	unk3;
	uint8_t	unk4;
}
fbColumn_t;

extern void fbFillScreen( uint8_t byte );
extern void fbCLSBuf();
extern void fbSetTimeoutValue( int );
extern void fbDrawColumn( int, int );
extern int	fbCreateTimeout( void(*)() );
extern void fbDeleteTimeout( int );
extern void fbDrawDeadBird( int fbBirdLine );
extern void fbDrawRect( int, int, int, int, int, int );
extern void fbDrawText( int, int, const char * );
extern void fbDrawNumber( int x, int y, int nd, int v );
extern int	fbNumDigits( int v );
extern void fbCreateColumn( int8_t hpos, fbColumn_t *column );

extern uint8_t		fbBirdColumn;
extern uint8_t		fbBirdCycle;
extern uint8_t		fbBirdLine;
extern int8_t		fbBirdDisp;
extern uint8_t		fdDead;
extern uint8_t		fbAnimStep;
extern uint8_t		fbAnimTimer;
extern uint16_t		fbScore;
extern fbColumn_t	fbColumn1;
extern fbColumn_t	fbColumn2;
extern fbColumn_t	fbColumn3;
extern uint8_t		fbTimeoutMask;
extern uint8_t		fbCurrentTimeout;
extern uint8_t		fbUsedTimeouts;


#endif /* __FLAPPY_BIRD_H__ */
