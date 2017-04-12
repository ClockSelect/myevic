#ifndef __FLAPPY_BIRD_H__
#define __FLAPPY_BIRD_H__

//=========================================================================

extern void fbInitTimeouts();
extern int	fbCreateTimeout( void (*cb)( void ) );
extern void fbDeleteTimeout( int );
extern void fbSetTimeoutDelay( int );
extern void fbTickTimeouts();
extern void fbCallTimeouts();

extern void fbStartScreen();
extern void fbBirdAnim( int );
extern void fbStartGame();

//=========================================================================

#endif /* __FLAPPY_BIRD_H__ */
