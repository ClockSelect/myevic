#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>

#include "myprintf.h"

//=========================================================================

FILE *stdout = 0;
FPUTC_FUNC *myputc = 0;

//=========================================================================
// Supported formats:
// %d, %u, %x, %X with optional sizing and padding (e.g. %08X, %5d)
// Yes, this is minimalist - I warned you.
//-------------------------------------------------------------------------

#define FL_NULL		0
#define	FL_FORMAT	0x01
#define	FL_ZEROPAD	0x02
#define	FL_SIZED	0x04
#define	FL_UPCASE	0x08
#define	FL_HEX		0x10
#define	FL_DECIMAL	0x20
#define FL_UNSIGNED	0x40

int my_doprnt( const char *format, va_list ap, FILE *out )
{
	int sent;
	char c;
	int flags, size;

	sent = 0;
	flags = 0;
	size = 0;

	while ( ( c = *format++ ) )
	{
		if ( flags == FL_NULL )
		{
			switch ( c )
			{
				case '%':
					flags |= FL_FORMAT;
					break;

				default:
					myputc( c, out );
					++sent;
					break;
			}
		}
		else if ( flags & FL_FORMAT )
		{
			switch ( c )
			{
				case '0':
					if ( size | ( flags & FL_ZEROPAD ) )
					{
						size = size * 10;
					}
					else
					{
						flags |= FL_ZEROPAD;
					}
					break;

				case '%':
					myputc( c, out );
					++sent;
					flags = FL_NULL;
					break;

				case 'u':
					flags |= FL_UNSIGNED;
				case 'd':
					flags |= FL_DECIMAL;
					flags &= ~FL_FORMAT;
					break;

				case 'X':
					flags |= FL_UPCASE;
				case 'x':
					flags |= FL_HEX;
					flags &= ~FL_FORMAT;
					break;
				
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					flags |= FL_SIZED;
					size = size * 10 + ( c - '0' );
					break;

				default:
					myputc( '?', out );
					++sent;
					flags = FL_NULL;
					break;
			}
		}

		if ( ( flags != FL_NULL ) && !( flags & FL_FORMAT ) )
		{
			if ( flags & (FL_DECIMAL|FL_HEX) )
			{
				uint32_t n = va_arg( ap, uint32_t );

				if ( !size ) size = 1;
				
				if ( flags & FL_DECIMAL )
				{
					int nd = 0;
					char sign = 0;
					char nstr[11];

					if ( size > 10 ) size = 10;

					if (!( flags & FL_UNSIGNED ))
					{
						if ( n > 0x7fffffff )
						{
							sign = '-';
							n = ~n + 1;
						}
					}

					do
					{
						nstr[nd++] = '0' + n % 10;
						n /= 10;
					}
					while ( n );

					if ( flags & FL_ZEROPAD )
					{
						while ( nd < size ) { nstr[nd++] = '0'; }
					}

					if ( sign ) nstr[nd++] = sign;

					while ( size-- > nd )
					{
						myputc( ' ', out );
						++sent;
					}

					while ( nd )
					{
						myputc( nstr[--nd], out );
						++sent;
					}
				}
				else
				{
					int nd = 8;
					int zp = (( flags & FL_ZEROPAD ) != 0 );
					int put = 0;

					do
					{
						--nd;
						char d = n >> ( nd * 4 ) & 0xf;
						if ( put || ( zp && ( size > nd )) || d || !nd )
						{
							if ( ! put )
							{
								put = 1;
								while ( nd < --size )
								{
									myputc( ' ', out );
									++sent;
								}
							}
							if ( flags & FL_UPCASE )
							{
								d = "0123456789ABCDEF"[(int)d];
							}
							else
							{
								d = "0123456789abcdef"[(int)d];
							}
							myputc( d, out );
							++sent;
						}
					}
					while ( nd );
				}
			}
			
			flags = FL_NULL;
			size = 0;
		}
	}

	return sent;
}

//=========================================================================

int myprintf( const char *format, ... )
{
	va_list ap;
	int retval;

	if ( !myputc ) return 0;

	va_start( ap, format );

	retval = my_doprnt( format, ap, stdout );

	va_end( ap );

	return retval;
}

//=========================================================================
