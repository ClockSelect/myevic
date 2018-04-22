#ifndef __MISCS_H__
#define __MISCS_H__


#define FILTER_SIZE 5

typedef struct {
	uint16_t	data[FILTER_SIZE];
	uint8_t		order[FILTER_SIZE];
	uint8_t		oldest;
	uint8_t		count;
} filter_t;

extern void ModeChange();
extern void NextMode();

extern void InitFilter( filter_t *filter );
extern uint16_t FilterMedian( filter_t *filter, uint16_t data );
extern uint16_t FilterTriangle( filter_t *filter, uint16_t data );
extern uint16_t FilterWMean( filter_t *filter, uint16_t data );

extern uint8_t Object3D;
extern void anim3d( int );
extern void Next3DObject();
extern void Previous3DObject();
					
extern int32_t sine( int32_t x );
extern int32_t cosine( int32_t x );

extern uint32_t Random();
extern void SetRandSeed( uint32_t s );

extern void qix( int );
extern void qix_mvpoint(int16_t *tx, int16_t *ty, int16_t *v_x, int16_t *v_y );
extern void qix_diddle(int16_t *ptr );

extern void Snow( int );

extern void AnimPwrBar( int );

extern uint8_t LEDRed;
extern uint8_t LEDGreen;
extern uint8_t LEDBlue;
extern volatile uint8_t LEDTimer;

extern void LEDGetColor();
extern void LEDSetColor();
extern void LEDWhite();
extern void LEDControl();
extern void LEDOff();
extern void LEDTimerTick();

#endif /* __MISCS_H__ */
