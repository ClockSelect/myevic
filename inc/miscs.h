#ifndef __MISCS_H__
#define __MISCS_H__


extern void ModeChange();
extern void NextMode();

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

#endif /* __MISCS_H__ */
