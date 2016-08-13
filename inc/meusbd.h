#ifndef __MEUSBD_H__
#define __MEUSBD_H__

#include "myprintf.h"

extern void InitUSB();
extern void VCOM_Poll();
extern char VCOM_Putc( char c, FILE *out );


#endif /* __MEUSBD_H__ */
