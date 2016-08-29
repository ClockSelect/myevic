#include "myevic.h"
#include "display.h"
#include "dataflash.h"


//=========================================================================

uint8_t	DisplayModel;
uint8_t	DisplayCmdByte;

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

__myevic__ void DrawTimeSmall( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	(colors&0x10?DrawValue+1:DrawValueInv+1)( x   , y, rtd->u32Hour, 0, 0x0B, 2 );
	(colors&0x08?DrawImage+1:DrawImageInv+1)( x+12, y, 0x103 );
	(colors&0x04?DrawValue+1:DrawValueInv+1)( x+14, y, rtd->u32Minute, 0, 0x0B, 2 );
	(colors&0x02?DrawImage+1:DrawImageInv+1)( x+26, y, 0x103 );
	(colors&0x01?DrawValue+1:DrawValueInv+1)( x+28, y, rtd->u32Second, 0, 0x0B, 2 );
}

__myevic__ void DrawTime( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	(colors&0x10?DrawValue+1:DrawValueInv+1)( x   , y, rtd->u32Hour, 0, 0x1F, 2 );
	(colors&0x08?DrawImage+1:DrawImageInv+1)( x+16, y, 0xDD );
	(colors&0x04?DrawValue+1:DrawValueInv+1)( x+19, y, rtd->u32Minute, 0, 0x1F, 2 );
	(colors&0x02?DrawImage+1:DrawImageInv+1)( x+35, y, 0xDD );
	(colors&0x01?DrawValue+1:DrawValueInv+1)( x+38, y, rtd->u32Second, 0, 0x1F, 2 );
}

__myevic__ void DrawDate( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors )
{
	(colors&0x10?DrawValue+1:DrawValueInv+1)( x   , y, rtd->u32Day, 0, 0x0B, 2 );
	(colors&0x08?DrawImage+1:DrawImageInv+1)( x+12, y, 0x102 );
	(colors&0x04?DrawValue+1:DrawValueInv+1)( x+16, y, rtd->u32Month, 0, 0x0B, 2 );
	(colors&0x02?DrawImage+1:DrawImageInv+1)( x+28, y, 0x102 );
	(colors&0x01?DrawValue+1:DrawValueInv+1)( x+32, y, rtd->u32Year, 0, 0x0B, 4 );
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
__myevic__ int GetImageWidth( const uint16_t imgnum )
{
	const image_t *img;

	switch ( DisplayModel )
	{
		case 0:
			img = font1[imgnum - 1];
			break;

		case 1:
			img = font2[imgnum - 1];
			break;
		
		default:
			return 0;
	}

	return img->width;
}


//=========================================================================
//----- (000051A8) --------------------------------------------------------
__myevic__ int GetStrCenteredX( const uint16_t str[] )
{
	int width = 0;

	for ( int l = 0 ; str[l] ; ++l )
	{
		width += GetImageWidth( str[l] );
	}

	return ( ( 64 - width ) >> 1 );
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
__myevic__ uint32_t DrawImage( const int x, const int y, const uint16_t img )
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
__myevic__ uint32_t DrawImageInv( const int x, const int y, const uint16_t img )
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
//----- (000057D4) --------------------------------------------------------
__myevic__ void DrawLOGO( const int x, const int y )
{
	uint8_t buffer[DATAFLASH_LOGO_SIZE];
	image_t *img;
	uint32_t base_addr;

	if ( dfStatus.nologo ) return;

	switch ( DisplayModel )
	{
		case 0:
			base_addr = DATAFLASH_LOGO_1306_BASE;
			break;

		case 1:
			base_addr = DATAFLASH_LOGO_1327_BASE;
			break;

		default:
			return;
	}

	MemClear( buffer, DATAFLASH_LOGO_SIZE );
	img = (image_t*)buffer;

	SYS_UnlockReg();
	FMC_ENABLE_ISP();

	for ( uint32_t addr = 0; addr < DATAFLASH_LOGO_SIZE ; addr += 0x100 )
	{
		FMCRead256( base_addr + addr, (uint32_t*)( buffer + addr ) );
	}

	FMC_DISABLE_ISP();
	SYS_LockReg();

	if ( img->width == 64 && img->height == 40 )
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
__myevic__ uint16_t* Value2Str( uint16_t *str, int v, int dp, uint16_t z, int nd )
{
	uint16_t dot;
	int i, d;

	if ( !nd )
	{
		if ( v >= 10000 )
		{
			nd = 5;
		}
		else if ( v >= 1000 )
		{
			nd = 4;
		}
		else if ( v >= 100 )
		{
			nd = 3;
		}
		else if ( v >= 10 )
		{
			nd = 2;
		}
		else
		{
			nd = 1;
		}
	}

	for ( i = 0 ; i < nd ; ++i )
	{
		d = v % 10;
		v /= 10;
		str[nd - i - 1] = d + z;
	}
	str[i] = 0;

	if ( dp && dp < nd )
	{
		int dot_pos = nd - dp;

		while ( nd >= dot_pos )
		{
			str[nd + 1] = str[nd];
			--nd;
		}

		switch ( z )
		{
			case 0x0B:
			default:
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
__myevic__ void DrawValue( int x, int y, int v, int dp, uint16_t z, int nd )
{
	uint16_t str[12];

	Value2Str( str, v, dp, z, nd );
	DrawString( str, x, y );
}


//=========================================================================
//----- (000058A4) --------------------------------------------------------
__myevic__ void DrawValueInv( int x, int y, int v, int dp, uint16_t z, int nd )
{
	uint16_t str[7];

	Value2Str( str, v, dp, z, nd );
	DrawStringInv( str, x, y );
}


//=========================================================================
//----- (00005A52) --------------------------------------------------------
__myevic__ void DrawString( const uint16_t s[], int x, int y )
{
	while ( *s )
	{
		x += DrawImage( x, y, *s++ );
	}
}


//=========================================================================
//----- (00005A72) --------------------------------------------------------
__myevic__ void DrawStringInv( const uint16_t s[], int x, int y )
{
	while ( *s )
	{
		x += DrawImageInv( x, y, *s++ );
	}
}


//=========================================================================
//----- (00005A92) --------------------------------------------------------
__myevic__ void DrawStringCentered( const uint16_t s[], int y )
{
	DrawString( s, GetStrCenteredX( s ), y);
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

