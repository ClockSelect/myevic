#ifndef __MISCS_H__
#define __MISCS_H__


extern void ModeChange();
extern void NextMode();

extern uint8_t Anim3d;
extern void anim3d( int );

extern int32_t sine( int32_t x );
extern int32_t cosine( int32_t x );

extern uint32_t Random();
extern void SetRandSeed( uint32_t s );


#endif /* __MISCS_H__ */
