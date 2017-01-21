#include "myevic.h"
#include "dataflash.h"
#include "timers.h"
#include "display.h"


//=========================================================================
//----- (00005714) --------------------------------------------------------
__myevic__ void SSD1306_Refresh()
{
	uint8_t *sb;

	sb = ScreenBuffer;

	for ( int l = 0 ; l < 0x10 ; ++l )
	{
		DisplaySendCommand( 0xB0 + l );
		DisplaySendCommand( 0 );
		DisplaySendCommand( ( dfStatus.flipped ) ? 0x12 : 0x10 );
		DisplaySendData( sb, 0x40 );
		sb += 0x40;
	}
}


//=========================================================================
//----- (000055D8) --------------------------------------------------------
__myevic__ void SSD1306_ClearBuffer()
{
	uint8_t *v0;
	int v1;
	int v2;

	v0 = ScreenBuffer;
	v1 = 0;
	do
	{
		v2 = 0;
		do
		{
			++v2;
			*v0++ = 0;
		}
		while ( v2 < 0x40 );
		++v1;
	}
	while ( v1 < 0x10 );
}


//=========================================================================
//----- (00005594) --------------------------------------------------------
__myevic__ void SSD1306_CLS()
{
	SSD1306_ClearBuffer();
	SSD1306_Refresh();
}


//=========================================================================
//----- (000055A4) --------------------------------------------------------
__myevic__ void SSD1306_PowerOn()
{
	PA1 = 1;
	PC4 = 1;
	WaitOnTMR2( 1 );
	PA0 = 0;
	WaitOnTMR2( 1 );
	PA0 = 1;
	WaitOnTMR2( 10 );
}


//=========================================================================

const uint8_t SSD1306_InitSeq[] =
	{	0xAE,
		0xA8,
		0x3F,
		0xD5,
		0xF1,
		0xC8,
		0xD3,
		0x20,
		0xDC,
		0x00,
		0x20,
		0x81,
		0x2F,
		0xA1,
		0xA4,
		0xA6,
		0xAD,
		0x8A,
		0xD9,
		0x22,
		0xDB,
		0x35	};

//----- (00005530) --------------------------------------------------------
__myevic__ void SSD1306_Init()
{
	SSD1306_PowerOn();

	for ( int i = 0 ; i < sizeof( SSD1306_InitSeq ) ; ++i )
	{
		DisplaySendCommand( SSD1306_InitSeq[i] );
	}

	if ( dfStatus.flipped )
	{
		DisplaySendCommand( 0xC0 );
		DisplaySendCommand( 0xD3 );
		DisplaySendCommand( 0x60 );
		DisplaySendCommand( 0xDC );
		DisplaySendCommand( 0x20 );
		DisplaySendCommand( 0xA0 );
	}

	SSD1306_CLS();
	DisplaySendCommand( 0xAF );
	WaitOnTMR2( 20 );
}


//=========================================================================
__myevic__ void SSD1306_SetContrast( const uint8_t c )
{
	DisplaySendCommand( 0x81 );
	DisplaySendCommand( c );
}


//=========================================================================
__myevic__ void SSD1306_SetInverse( const uint8_t i )
{
	DisplaySendCommand( i ? 0xA7 : 0xA6 );
}


//=========================================================================
//----- (000056E0) --------------------------------------------------------
__myevic__ void SSD1306_ScreenOff()
{
	DisplaySendCommand( 0xAE );
	PC4 = 0;
	WaitOnTMR2( 100 );
	PA1 = 0;
	WaitOnTMR2( 100 );
	PA0 = 0;
	WaitOnTMR2( 100 );
}


//=========================================================================
//----- (00005500) --------------------------------------------------------
__myevic__ void SSD1306_Plot( int x, int y, int color )
{
	uint8_t mask;
	uint32_t i;

	if (( x < 0 ) || ( x >  63 )) return;
	if (( y < 0 ) || ( y > 127 )) return;

	mask = 1 << ( y & 7 );
	i = x + ( ( y & ~7 ) << 3 );

	if ( color == 1 )
	{
		ScreenBuffer[i] |= mask;
	}
	else if ( color == 0 )
	{
		ScreenBuffer[i] &= ~mask;
	}
	else
	{
		ScreenBuffer[i] ^= mask;
	}
}


//=========================================================================
//----- (000055FC) --------------------------------------------------------
__myevic__ uint32_t SSD1306_Image( int x, int y, uint8_t img, int color )
{
	if ( img == 0x88 || img == 0x8B || img == 0x91 || img == 0x92 || img == 0x9A )
	{
		y += 2;
	}
	return SSD1306_Bitmap( x, y, Images[img - 1], color );
}


//=========================================================================
//----- (00005628) --------------------------------------------------------
__myevic__ uint32_t SSD1306_Bitmap( int x, int y, const image_t *image, int color )
{
	uint32_t shift;
	uint32_t h, w;
	uint32_t bm_ptr;
	uint32_t addr;
	uint32_t lines;
	uint8_t pixels;

	shift = y & 7;

	bm_ptr = 0;

	lines = image->height >> 3;

	for ( h = 0 ; h < lines ; ++h )
	{
		addr = 0x40 * ( ( y >> 3 ) + h ) + x;

		for ( w = 0 ; w < image->width ; ++w )
		{
			pixels = image->bitmap[bm_ptr++];

			if ( color ) pixels = ~pixels;

			if ( shift )
			{
				if ( addr < SCREEN_BUFFER_SIZE )
				{
					ScreenBuffer[ addr ] &= ByteMaskRight[shift];
					ScreenBuffer[ addr ] |= ( pixels << shift ) & ByteMaskLeft[shift];
				}
				if ( addr + 0x40 < SCREEN_BUFFER_SIZE )
				{
					ScreenBuffer[ addr + 0x40 ] &= ByteMaskLeft[shift];
					ScreenBuffer[ addr + 0x40 ] |= ( pixels >> ( 8 - shift )) & ByteMaskRight[shift];
				}
			}
			else
			{
				if ( addr < SCREEN_BUFFER_SIZE )
				{
					ScreenBuffer[ addr ] = pixels;
				}
			}

			++addr;
		}
	}

	return image->width;
}


//=========================================================================
//----- (000064C8) --------------------------------------------------------
__myevic__ void SSD1306_WriteBytes( const int isData, const uint8_t data[], const int len )
{
	register int is_data = ( isData == 0x40 );
	register int byte;

	PE10 = is_data ? 1 : 0;

	for ( int l = 0 ; l < len ; ++l )
	{
		byte = data[l];
		while ( SPI_IS_BUSY( SPI0 ) )
			;
		SPI_WRITE_TX( SPI0, byte );
	}
	while ( SPI_IS_BUSY( SPI0 ) )
		;
}


//=========================================================================
__myevic__ void SSD1306_Screen2Bitmap( uint8_t *pu8Bitmap )
{
	MemClear( pu8Bitmap, SCREEN_BUFFER_SIZE );

	for ( int line = 0 ; line < 16 ; ++line )
	{
		for ( int bit = 0 ; bit < 8 ; ++bit )
		{
			int y = line * 8 + bit;
			int mask = 1 << bit;
			for ( int x = 0 ; x < 64 ; ++x )
			{
				if ( ScreenBuffer[ line * 64 + x ] & mask )
				{
					pu8Bitmap[ y * 8 + ( x >> 3 ) ] |= ( 0x80 >> ( x & 7 ) );
				}
			}
		}
	}
}

