#ifndef __MYPRINTF_H__
#define __MYPRINTF_H__ 1

//=========================================================================
// Minimalist printf implementation
//-------------------------------------------------------------------------
// See comments in myprintf.c to see wich formats are supported.
//-------------------------------------------------------------------------
// Usage:
//	- Define the FILE struct as you want, if needed.
//	- At application startup, Set the stdout global to your custom FILE*
//	  if needed.
//	- Define your own fputc() function. The stdio pointer will be passed
//	  to your function when called.
//	- At application startup, set the myputc function pointer to your
//	  custom fputc() function.
//	- Enjoy.
//=========================================================================

//-------------------------------------------------------------------------
// FILE definition.
// When not using stdio, you may put whatever you want in there.
//-------------------------------------------------------------------------
#ifndef _STDIO_H_
typedef struct __FILE
{
	int handle;
}
FILE;
#endif

typedef char FPUTC_FUNC( char c, FILE *out );

//-------------------------------------------------------------------------
// Global variables you must set before calling myprintf()
//-------------------------------------------------------------------------

extern FILE *stdout;

extern FPUTC_FUNC *myputc;

//-------------------------------------------------------------------------

int myprintf( const char *format, ... );

//=========================================================================

#endif /* __MYPRINTF_H__ */
