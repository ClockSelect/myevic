#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "M451Series.h"

//=========================================================================
// DISPLAY
//-------------------------------------------------------------------------

#define SCREEN_BUFFER_SIZE 0x400

//-------------------------------------------------------------------------

#define InvertRect(a,b,c,d) DrawFillRect((a),(b),(c),(d),2)

//-------------------------------------------------------------------------

typedef struct {
	uint8_t	width;
	uint8_t	height;
	uint8_t	bitmap[];
} image_t;

//-------------------------------------------------------------------------

extern const uint8_t ByteMaskRight[];
extern const uint8_t ByteMaskLeft[];

extern uint8_t DisplayModel;
extern uint8_t ScreenBuffer[];

extern const image_t **Images;

extern const image_t const *font0_1306[];
extern const image_t const *font0_1327[];
extern const image_t const *font1_1306[];
extern const image_t const *font1_1327[];

//-------------------------------------------------------------------------

extern void InitSPI0();
extern void InitDisplay();
extern void DisplaySendCommand( const uint8_t );
extern void DisplaySendData( const uint8_t*, const uint32_t );
extern void ScreenOff();
extern void DisplaySetContrast( const uint8_t c );
extern void DisplaySetInverse( const uint8_t i );
extern void ClearScreenBuffer();
extern void DisplayRefresh();
extern void Screen2Bitmap( uint8_t *pu8Bitmap );
extern void DisplaySetFont();

extern int GetImageWidth( const uint8_t imgnum );
extern int GetStringWidth( const uint8_t str[] );
extern int GetStrCenteredX( const uint8_t str[] );
extern uint8_t* Value2Str( uint8_t *str, int v, int dp, uint8_t z, int nd );

extern void DrawTimeSmall( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors );
extern void DrawTime( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors );
extern void DrawDate( int x, int y, S_RTC_TIME_DATA_T *rtd, int colors );

extern void DrawHLine( const int x1, const int y, const int x2, const int color );
extern void DrawVLine( const int x, const int y1, const int y2, const int color );
extern void DrawFillRect( const int x1, const int y1,const  int x2, const int y2, const int color);
extern uint32_t DrawImage( const int x, const int y, const uint8_t img );
extern uint32_t DrawImageInv( const int x, const int y, const uint8_t img );
extern int GetLogoHeight();
extern void DrawLOGO( const int x, const int y );
extern void DrawValue( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd );
extern void DrawValueRight( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd );
extern void DrawValueInv( int x, int y, int v, uint8_t dp, uint8_t z, uint8_t nd );
extern void DrawString( const uint8_t s[], int x, int y );
extern void DrawStringInv( const uint8_t s[], int x, int y );
extern void DrawStringCentered( const uint8_t s[], int y );
extern void DrawStringRight( const uint8_t s[], int x, int y );
extern void DrawLine( int x1, int y1, int x2, int y2, int color, int thick );
extern void DrawCircle( int xc, int yc, int r, int color, int fill );


extern void SSD1306_Init();
extern void SSD1306_WriteBytes( const int isData, const uint8_t data[], const int len );
extern void SSD1306_ClearBuffer();
extern void SSD1306_Refresh();
extern void SSD1306_ScreenOff();
extern void SSD1306_SetContrast( const uint8_t c );
extern void SSD1306_Plot( int x, int y, int color );
extern uint32_t SSD1306_Image( int x, int y, uint8_t img, int color );
extern uint32_t SSD1306_Bitmap( int x, int y, const image_t *image, int color );
extern void SSD1306_Screen2Bitmap( uint8_t *pu8Bitmap );
extern void SSD1306_SetInverse( const uint8_t i );

extern void SSD1327_Init();
extern void SSD1327_WriteBytes( const int isData, const uint8_t data[], const int len );
extern void SSD1327_ClearBuffer();
extern void SSD1327_Refresh();
extern void SSD1327_ScreenOff();
extern void SSD1327_SetContrast( const uint8_t c );
extern void SSD1327_Plot( int x, int y, int color );
extern uint32_t SSD1327_Image( int x, int y, uint8_t img, int color );
extern uint32_t SSD1327_Bitmap( int x, int y, const image_t *image, int color );
extern void SSD1327_Screen2Bitmap( uint8_t *pu8Bitmap );
extern void SSD1327_SetInverse( const uint8_t i );


//=========================================================================
#endif /* __DISPLAY_H__ */
