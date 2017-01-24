#include "myevic.h"
#include "display.h"
#include "dataflash.h"
#include "screens.h"


//=========================================================================

uint8_t	DisplayModel;
uint8_t	DisplayCmdByte;

const image_t **Images;

uint8_t ScreenBuffer[SCREEN_BUFFER_SIZE] __attribute__((aligned(8)));

const uint8_t ByteMaskRight[] = { 0x00, 0x01, 0x03,	0x07, 0x0F, 0x1F, 0x3F,	0x7F };
const uint8_t ByteMaskLeft[]  = { 0xFF, 0xFE, 0xFC,	0xF8, 0xF0, 0xE0, 0xC0,	0x80 };


//=========================================================================

typedef void (PLOT_FUNC(int,int,int));

PLOT_FUNC *DrawPoint;


//=========================================================================
//----- (00006228) --------------------------------------------------------
__myevic__ void InitSPI0()
{
	SYS->GPE_MFPH &= (~(SYS_GPE_MFPH_PE11MFP_Msk|SYS_GPE_MFPH_PE12MFP_Msk|SYS_GPE_MFPH_PE13MFP_Msk));
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE11MFP_SPI0_MOSI0|SYS_GPE_MFPH_PE12MFP_SPI0_SS|SYS_GPE_MFPH_PE13MFP_SPI0_CLK);

	SPI_Open( SPI0, SPI_MASTER, SPI_MODE_0, 8, 4000000 );
	SPI_EnableAutoSS( SPI0, SPI_SS, SPI_SS_ACTIVE_LOW );
}


//=========================================================================
//----- (00005160) --------------------------------------------------------
__myevic__ void DisplaySendCommand( const uint8_t cmd )
{
	DisplayCmdByte = cmd;

	switch ( DisplayModel )
	{
		case 0:
			SSD1306_WriteBytes( 0, &DisplayCmdByte, 1 );
			break;
			
		case 1:
			SSD1327_WriteBytes( 0, &DisplayCmdByte, 1 );
			break;
	}
}


//=========================================================================
//----- (00005184) --------------------------------------------------------
__myevic__ void DisplaySendData( const uint8_t *data, const uint32_t len )
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_WriteBytes( 0x40, data, len );
			break;

		case 1:
			SSD1327_WriteBytes( 0x40, data, len );
			break;
	}
}


//=========================================================================
//----- (00005240) --------------------------------------------------------
__myevic__ void InitDisplay()
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_Init();
			DrawPoint = SSD1306_Plot+1;
			break;

		case 1:
			SSD1327_Init();
			DrawPoint = SSD1327_Plot+1;
			break;
	}
	DisplaySetContrast( dfContrast );
	DisplaySetInverse( dfStatus.invert );
	DisplaySetFont();

	gFlags.splash = SplashExists();
}

//=========================================================================
__myevic__ void DisplaySetContrast( const uint8_t c )
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_SetContrast( c );
			break;

		case 1:
			SSD1327_SetContrast( c );
			break;
	}
}

//=========================================================================
__myevic__ void DisplaySetInverse( const uint8_t i )
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_SetInverse( i );
			break;

		case 1:
			SSD1327_SetInverse( i );
			break;
	}
}

//=========================================================================

__myevic__ void DrawTimeSmall( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	if ( gFlags.draw_edited_item ) colors = 0x1F;

	if (colors&0x10) DrawValue( x   , y, rtd->u32Hour, 0, 0x0B, 2 );
	if (colors&0x08) DrawImage( x+12, y, 0xD7 );
	if (colors&0x04) DrawValue( x+14, y, rtd->u32Minute, 0, 0x0B, 2 );
	if (colors&0x02) DrawImage( x+26, y, 0xD7 );
	if (colors&0x01) DrawValue( x+28, y, rtd->u32Second, 0, 0x0B, 2 );
}

__myevic__ void DrawTime( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	if ( gFlags.draw_edited_item ) colors = 0x1F;

	if (colors&0x10) DrawValue( x   , y, rtd->u32Hour, 0, 0x1F, 2 );
	if (colors&0x08) DrawImage( x+16, y, 0xDD );
	if (colors&0x04) DrawValue( x+19, y, rtd->u32Minute, 0, 0x1F, 2 );
	if (colors&0x02) DrawImage( x+35, y, 0xDD );
	if (colors&0x01) DrawValue( x+38, y, rtd->u32Second, 0, 0x1F, 2 );
}

typedef struct
{
	uint8_t separator;
	uint8_t	dayoffset;
	uint8_t	monthoffset;
	uint8_t	yearoffset;
	uint8_t sep1offset;
	uint8_t sep2offset;
}
datefmt_t;

__myevic__ void DrawDate( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	const datefmt_t format[] =
	{
		{ 0xC1,  0, 16, 32, 12, 28 },
		{ 0xD6, 16,  0, 32, 12, 28 },
		{ 0xD6,  0, 16, 32, 12, 28 },
		{ 0xD9, 44, 28,  0, 24, 40 }
	};

	const datefmt_t *f = &format[ dfStatus.dfmt1 | ( dfStatus.dfmt2 << 1 ) ];

	if ( gFlags.draw_edited_item ) colors = 0x1F;

	if (colors&0x10) DrawValue( x + f->dayoffset,   y, rtd->u32Day,   0, 0x0B, 2 );
	if (colors&0x04) DrawValue( x + f->monthoffset, y, rtd->u32Month, 0, 0x0B, 2 );
	if (colors&0x01) DrawValue( x + f->yearoffset,  y, rtd->u32Year,  0, 0x0B, 4 );
	if (colors&0x08) DrawImage( x + f->sep1offset,  y, f->separator );
	if (colors&0x02) DrawImage( x + f->sep2offset,  y, f->separator );
}


//=========================================================================
//----- (00005784) --------------------------------------------------------
__myevic__ void ClearScreenBuffer()
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_ClearBuffer();
			break;

		case 1:
			SSD1327_ClearBuffer();
			break;
	}
}


//=========================================================================
//----- (00005AAC) --------------------------------------------------------
__myevic__ void ScreenOff()
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_ScreenOff();
			break;

		case 1:
			SSD1327_ScreenOff();
			break;
	}
}


//=========================================================================
//----- (00005AC4) --------------------------------------------------------
__myevic__ void DisplayRefresh()
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_Refresh();
			break;

		case 1:
			SSD1327_Refresh();
			break;
	}
}


//=========================================================================
//----- (000051CC) --------------------------------------------------------
__myevic__ int GetImageWidth( const uint8_t imgnum )
{
	return Images[imgnum - 1]->width;
}


//=========================================================================
__myevic__ int GetStringWidth( const uint8_t str[] )
{
	int width = 0;

	for ( int l = 0 ; str[l] ; ++l )
	{
		width += GetImageWidth( str[l] );
	}

	return width;
}


//=========================================================================
//----- (000051A8) --------------------------------------------------------
__myevic__ int GetStrCenteredX( const uint8_t str[] )
{
	return ( ( 64 - GetStringWidth( str ) ) >> 1 );
}


//=========================================================================
//----- (000051F8) --------------------------------------------------------
__myevic__ void DrawHLine( const int x1, const int y, const int x2, const int color )
{
	int inc = ( x1 < x2 ) * 2 - 1;

	for ( int x = x1 ; x != x2 + inc ; x += inc )
	{
		DrawPoint( x, y, color );
	}
}


//=========================================================================
__myevic__ void DrawVLine( const int x, const int y1, const int y2, const int color )
{
	int inc = ( y1 < y2 ) * 2 - 1;

	for ( int y = y1 ; y != y2 + inc ; y += inc )
	{
		DrawPoint( x, y, color );
	}
}


//=========================================================================
//----- (0000575C) --------------------------------------------------------
__myevic__ void DrawFillRect( const int x1, const int y1,const  int x2, const int y2, const int color)
{
	for ( int y = y1 ; y <= y2 ; ++y )
	{
		DrawHLine( x1, y, x2, color );
	}
}


//=========================================================================
//----- (0000579C) --------------------------------------------------------
__myevic__ uint32_t DrawImage( const int x, const int y, const uint8_t img )
{
	switch ( DisplayModel )
	{
		case 0:
			return SSD1306_Image( x, y, img, 0 );

		case 1:
			return SSD1327_Image( x, y, img, 0 );

		default:
			return 0;
	}
}

//=========================================================================
//----- (000057B8) --------------------------------------------------------
__myevic__ uint32_t DrawImageInv( const int x, const int y, const uint8_t img )
{
	switch ( DisplayModel )
	{
		case 0:
			return SSD1306_Image( x, y, img, 1 );

		case 1:
			return SSD1327_Image( x, y, img, 1 );

		default:
			return 0;
	}
}



//=========================================================================
// Retrieve the Logo height (0=No logo)
//-------------------------------------------------------------------------
__myevic__ int GetLogoHeight()
{
	image_t *img;
	int h;

	img = (image_t*)DATAFLASH_LOGO_1306_BASE;

	h = 0;

	if ( img->width == 64 )
	{
		h = img->height;
		if ( h < 40 || h > 63 ) h = 0;
	}

	return h;
}


//=========================================================================
__myevic__ void DrawLOGO( const int x, const int y )
{
	image_t *img;

	if (( dfStatus.nologo ) && ( Screen != 60 )) return;

	img = (image_t*)DATAFLASH_LOGO_1306_BASE;

	if ( img->width == 64 && img->height >= 40 && img->height <= 63 )
	{
		switch ( DisplayModel )
		{
			case 0:
				SSD1306_Bitmap( x, y, img, 0 );
				break;

			case 1:
				SSD1327_Bitmap( x, y, img, 0 );
				break;
		}
	}
}


//=========================================================================
//-------------------------------------------------------------------------
__myevic__ uint8_t* Value2Str( uint8_t *str, int v, int dp, uint8_t z, int nd )
{
	uint8_t dot;
	int i, d;

	if ( !nd )
	{
		for ( int i = v ; i ; i /= 10 ) ++nd;
		if ( nd <= dp ) nd = dp + 1;
	}

	for ( i = 0 ; i < nd ; ++i )
	{
		d = v % 10;
		v /= 10;
		str[nd - i - 1] = d + z;
	}
	str[i] = 0;

	if ( dp && dp <= nd )
	{
		int dot_pos = nd - dp;

		while ( nd >= dot_pos )
		{
			str[nd + 1] = str[nd];
			--nd;
		}

		switch ( z )
		{
			default:
			case 0x0B:
				dot = 0xC1;
				break;
			case 0x15:
				dot = 0xD8;
				break;
			case 0x1F:
				dot = 0xDC;
				break;
			case 0x29:
				dot = 0xDF;
				break;
			case 0x33:
				dot = 0xF1;
				break;
			case 0x3D:
				dot = 0x47;
				break;
			case 0x48:
				dot = 0xF7;
				break;
			case 0x52:
				dot = 0xF9;
				break;
			case 0x5C:
				dot = 0x66;
				break;
		}

		str[nd + 1] = dot;
	}
	return str;
}


//=========================================================================
//----- (000058A4) --------------------------------------------------------
__myevic__ void DrawValue( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	uint8_t str[12];

	Value2Str( str, v, dp, z, nd );
	DrawString( str, x, y );
}


//=========================================================================
// Draw right-justified numerical value
//-------------------------------------------------------------------------
__myevic__ void DrawValueRight( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	uint8_t str[12];

	Value2Str( str, v, dp, z, nd );
	DrawString( str, x - GetStringWidth( str ), y );
}


//=========================================================================
//----- (000058A4) --------------------------------------------------------
__myevic__ void DrawValueInv( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd )
{
	uint8_t str[12];

	Value2Str( str, v, dp, z, nd );
	DrawStringInv( str, x, y );
}


//=========================================================================
//----- (00005A52) --------------------------------------------------------
__myevic__ void DrawString( const uint8_t s[], int x, int y )
{
	while ( *s )
	{
		x += DrawImage( x, y, *s++ );
	}
}


//=========================================================================
//----- (00005A72) --------------------------------------------------------
__myevic__ void DrawStringInv( const uint8_t s[], int x, int y )
{
	while ( *s )
	{
		x += DrawImageInv( x, y, *s++ );
	}
}


//=========================================================================
//----- (00005A92) --------------------------------------------------------
__myevic__ void DrawStringCentered( const uint8_t s[], int y )
{
	DrawString( s, GetStrCenteredX( s ), y );
}


//=========================================================================
__myevic__ void DrawStringRight( const uint8_t s[], int x, int y )
{
	DrawString( s, x - GetStringWidth( s ), y );
}


//=========================================================================
__myevic__ void DrawLine( int x1, int y1, int x2, int y2, int color, int thick )
{
	int dx, dy, e;
	int xinc, yinc;
	int h1, h2;

	if ( !thick ) return;

	h1 = thick >> 1;
	h2 = thick - h1 - 1;

	dx = ( x2 - x1 ) << 1;
	dy = ( y2 - y1 ) << 1;

	if ( dy < 0 )
	{
		yinc = -1;
		dy = -dy;
	}
	else
	{
		yinc = 1;
	}
	
	if ( dx < 0 )
	{
		xinc = -1;
		dx = -dx;
	}
	else
	{
		xinc = 1;
	}
	
	if ( dx > dy )
	{
		e = dx >> 1;
		while ( x1 != x2 )
		{
			DrawVLine( x1, y1 - h1, y1 + h2, color );

			x1 += xinc;
			if (( e -= dy ) <= 0 )
			{
				y1 += yinc;
				e += dx;
			}
		}

		DrawVLine( x2, y2 - h1, y2 + h2, color );
	}
	else
	{
		e = dy >> 1;
		while ( y1 != y2 )
		{
			DrawHLine( x1 - h1, y1, x1 + h2, color );

			y1 += yinc;
			if (( e -= dx ) <= 0 )
			{
				x1 += xinc;
				e += dy;
			}
		}

		DrawHLine( x2 - h1, y2, x2 + h2, color );
	}
}


//=========================================================================
//
// Algorithme de tracé de cercle d'Andres
//
__myevic__ void DrawCircle( int x_centre, int y_centre, int r, int color, int fill )
{
	int x = 0;
	int y = r;
	int d = r - 1;

	while( y >= x )
	{
		if ( fill )
		{
			DrawHLine( x_centre - x, y_centre - y, x_centre + x, color );
			DrawHLine( x_centre - x, y_centre + y, x_centre + x, color );
			DrawHLine( x_centre - y, y_centre - x, x_centre + y, color );
			DrawHLine( x_centre - y, y_centre + x, x_centre + y, color );
		}
		else
		{
			DrawPoint( x_centre - x, y_centre - y, color );
			DrawPoint( x_centre + x, y_centre - y, color );
			DrawPoint( x_centre - x, y_centre + y, color );
			DrawPoint( x_centre + x, y_centre + y, color );
			DrawPoint( x_centre - y, y_centre + x, color );
			DrawPoint( x_centre + y, y_centre + x, color );
			DrawPoint( x_centre - y, y_centre - x, color );
			DrawPoint( x_centre + y, y_centre - x, color );
		}

		if (d >= 2*x)
		{
			d -= 2*x + 1;
			x ++;
		}
		else if (d < 2 * (r-y))
		{
			d += 2*y - 1;
			y --;
		}
		else
		{
			d += 2*(y - x - 1);
			y --;
			x ++;
		}
	}
}


//=========================================================================
// Converts screen buffer in something readable
//-------------------------------------------------------------------------
__myevic__ void Screen2Bitmap( uint8_t *pu8Bitmap )
{
	switch ( DisplayModel )
	{
		case 0:
			SSD1306_Screen2Bitmap( pu8Bitmap );
			break;

		case 1:
			SSD1327_Screen2Bitmap( pu8Bitmap );
			break;
	}
}


//=========================================================================
// Set the font
//-------------------------------------------------------------------------
__myevic__ void DisplaySetFont()
{
	if ( dfStatus.font )
	{
		Images = font0_1306;
	}
	else
	{
		Images = font1_1306;
	}
}

