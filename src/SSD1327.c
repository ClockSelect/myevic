#include "myevic.h"
#include "dataflash.h"
#include "timers.h"
#include "display.h"


//=========================================================================
//----- (000017B0) --------------------------------------------------------
__myevic__ void SSD1327_SetColsRowsAddrs(char col_start, char col_end, char row_start, char row_end)
{
  DisplaySendCommand( 0x15 );
  DisplaySendCommand( col_start );
  DisplaySendCommand( col_end );
  DisplaySendCommand( 0x75 );
  DisplaySendCommand( row_start );
  DisplaySendCommand( row_end );
}


//=========================================================================
//----- (000054E0) --------------------------------------------------------
__myevic__ void SSD1327_Refresh()
{
	SSD1327_SetColsRowsAddrs( 16, 47, 0, 127 );
	DisplaySendData( ScreenBuffer, 0x400 );
}


//=========================================================================
//----- (00005320) --------------------------------------------------------
__myevic__ void SSD1327_ClearBuffer()
{
	for ( int i = 0 ; i < 0x400 ; ++i )
	{
		ScreenBuffer[i] = 0;
	}
}


//=========================================================================
//----- (000052E4) --------------------------------------------------------
__myevic__ void SSD1327_CLS()
{
	SSD1327_ClearBuffer();
	SSD1327_Refresh();
}


//=========================================================================
//----- (000052F4) --------------------------------------------------------
__myevic__ void SSD1327_PowerOn()
{
	PA1 = 1;
	WaitOnTMR2( 1 );
	PA0 = 0;
	WaitOnTMR2( 10 );
	PA0 = 1;
	WaitOnTMR2( 1 );
}


//=========================================================================

const uint8_t SSD1327_InitSeq[] =
	{	0xAE,
		0xA0,
		0x40,
		0xA1,
		0x00,
		0xA2,
		0x00,
		0xA4,
		0xA8,
		0x7F,
		0xAB,
		0x01,
		0x81,
		0xA6,
		0xB1,
		0x31,
		0xB3,
		0xB1,
		0xB4,
		0xB5,
		0xB6,
		0x0D,
		0xBC,
		0x07,
		0xBE,
		0x07,
		0xD5,
		0x02	};

//----- (00005280) --------------------------------------------------------
__myevic__ void SSD1327_Init()
{
	SSD1327_PowerOn();

	for ( int i = 0 ; i < sizeof( SSD1327_InitSeq ) ; ++i )
	{
		DisplaySendCommand( SSD1327_InitSeq[i] );
	}

	if ( dfStatus.flipped )
	{
		DisplaySendCommand( 0xA2 );
		DisplaySendCommand( 0x80 );
		DisplaySendCommand( 0xA0 );
		DisplaySendCommand( 0x53 );
	}

	SSD1327_CLS();

	PC4 = 1;
	WaitOnTMR2( 1 );
	DisplaySendCommand( 0xAF );
	WaitOnTMR2( 20 );
}


//=========================================================================
__myevic__ void SSD1327_SetContrast( const uint8_t c )
{
	DisplaySendCommand( 0x81 );
	DisplaySendCommand( c );
}


//=========================================================================
__myevic__ void SSD1327_SetInverse( const uint8_t i )
{
	DisplaySendCommand( i ? 0xA7 : 0xA4 );
}


//=========================================================================
//----- (000054AC) --------------------------------------------------------
__myevic__ void SSD1327_ScreenOff()
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
//----- (00005258) --------------------------------------------------------
__myevic__ void SSD1327_Plot( int x, int y, int color )
{
	uint8_t mask;
	uint32_t i;

	if (( x < 0 ) || ( x >  63 )) return;
	if (( y < 0 ) || ( y > 127 )) return;

	mask = 1 << ( 7 - ( x & 7 ));
	i = 8 * y + ( x >> 3 );

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
//----- (0000533C) --------------------------------------------------------
__myevic__ uint32_t SSD1327_Image( int x, int y, uint8_t img, int color )
{
	if ( img == 0x88 || img == 0x8B || img == 0x91 || img == 0x92 || img == 0x9A )
	{
		y += 2;
	}
	return SSD1327_Bitmap( x, y, Images[img - 1], color );
}


//=========================================================================
// Modified SSD1327 Bitmap drawing function to draw SSD1306 Bitmaps.
//-------------------------------------------------------------------------
__myevic__ uint32_t SSD1327_Bitmap( int x, int y, const image_t *image, int color )
{
	int sblinebase, sbi;
	uint8_t sbbyte;

	const uint8_t *pixline;
	uint8_t bitline;
	uint8_t pixel;

	int h;
	h = ( y < 0 ) ? -y : 0;

	sblinebase = ( ( y + h ) << 3 ) + ( x >> 3 );

	pixline = &image->bitmap[ ( h << 3 ) * image->width ];
	bitline = h & 7;

	for ( ; h < image->height ; ++h )
	{
		if ( y + h >= 128 )
			break;

		sbi = sblinebase;

		uint8_t pixmask = ( 1 << ( 7 - ( x & 7 ) ) );
		uint8_t bitmask = ( 1 << bitline );

		int w;
		w = ( x < 0 ) ? -x : 0;

		for ( ; w < image->width; ++w )
		{
			if ( x + w >= 64 )
				break;

			sbbyte = ScreenBuffer[sbi];
			sbbyte &= ~pixmask;

			pixel = pixline[w];
			if ( color ) pixel = ~pixel;
			pixel &= bitmask;

			ScreenBuffer[sbi] = sbbyte | ( pixel ? pixmask : 0 );

			pixmask >>= 1;
			if ( !pixmask )
			{
				pixmask = 0x80;
				++sbi;
			}
		}

		sblinebase += 8;

		if ( ++bitline == 8 )
		{
			bitline = 0;
			pixline += image->width;
		}
	}

	return image->width;
}


//=========================================================================
//----- (00006444) --------------------------------------------------------
__myevic__ void SSD1327_WriteBytes( const int isData, const uint8_t data[], const int len )
{
	register int is_data;
	unsigned int pairs;
	unsigned int mask;
	uint8_t d;
	uint8_t byte;

	is_data = ( isData == 0x40 );

	PE10 = is_data ? 1 : 0;

	for ( int l = 0 ; l < len ; ++l )
	{
		d = data[l];

		if ( is_data )
		{
			for ( pairs = 0 ; pairs < 4 ; ++pairs )
			{
				mask = ( 1 << ( 7 - 2 * pairs ));
				byte = 0x00;
				if ( d &   mask       ) byte  = 0x0F;
				if ( d & ( mask >> 1 )) byte |= 0xF0;

				SPI_WRITE_TX( SPI0, byte );
				while ( SPI_IS_BUSY( SPI0 ) )
					;
			}
		}
		else
		{
			SPI_WRITE_TX( SPI0, d );
			while ( SPI_IS_BUSY( SPI0 ) )
				;
		}
	}
}


//=========================================================================
// Advantage: SSD1327!
//-------------------------------------------------------------------------
__myevic__ void SSD1327_Screen2Bitmap( uint8_t *pu8Bitmap )
{
	MemCpy( pu8Bitmap, ScreenBuffer, SCREEN_BUFFER_SIZE );
}

