#ifndef __MYPRINTF_H__
#define __MYPRINTF_H__ 1


#ifndef _STDIO_H_
typedef struct __FILE
{
	int handle;
}
FILE;

FILE *stdout;
#endif

typedef char PUTC_FUNC( char c, FILE *out );


extern PUTC_FUNC *myputc;


int myprintf( const char *format, ... );


#endif /* __MYPRINTF_H__ */
