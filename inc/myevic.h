#ifndef __TESTC__
#define __TESTC__

#include "M451Series.h"

//=============================================================================

#define __myevic__ __attribute__ ((section (".myevic")))

#define	ISMODETC(m) ((m)<=3)
#define ISMODEVW(m) (((m)==4)||((m)==6))
#define ISMODEBY(m) ((m)==5)

//=============================================================================

typedef union
{
	int8_t c[0x100];
	uint8_t uc[0x100];
	int16_t s[0x100/sizeof(int16_t)];
	uint16_t us[0x100/sizeof(uint16_t)];
	int32_t l[0x100/sizeof(int32_t)];
	uint32_t ul[0x100/sizeof(uint32_t)];
} Playfield_t;

//=============================================================================

extern volatile uint32_t	ADC00_IRQ_Flag;		// .ds.b 4
extern uint8_t	UpdateDFTimer;					// .ds.b 1
extern uint8_t	UpdatePTTimer;					// .ds.b 1
extern uint8_t	DisplayModel;					// .ds.b 1
extern uint8_t	AtoShuntValue;					// .ds.b 1
extern uint16_t	fmcCntrsIndex;					// .ds.b 2
extern uint8_t	ConfigIndex;					// .ds.b 1
extern uint8_t	EditItemIndex;					// .ds.b 1
extern uint8_t	MenuPage;						// .ds.b 1
extern uint8_t	KeyUpTimer;						// .ds.b 1
extern uint8_t	AtoError;						// .ds.b 1
extern uint8_t	AtoProbeCount;					// .ds.b 1
extern uint8_t	FireClickCount;					// .ds.b 1
extern uint8_t	FireClickTimer;					// .ds.b 1
extern int8_t	UserInputs;						// .ds.b 1
extern int8_t	LastInputs;						// .ds.b 1
extern volatile int8_t	Event;					// .ds.b 1
extern uint8_t	FireClicksEvent;				// .ds.b 1
extern uint8_t	LastEvent;						// .ds.b 1
extern uint8_t	BoardTemp;						// .ds.b 1
extern uint8_t	BTempSampleCnt;					// .ds.b 1
extern uint8_t	byte_20000048;					// .ds.b 1
extern uint8_t	BatteryPercent;					// .ds.b 1
extern uint8_t	SavedBatPercent;				// .ds.b 1
extern uint8_t	BatteryTenth;					// .ds.b 1
extern uint8_t	BatPCCmpCnt;					// .ds.b 1
extern uint8_t	BatAnimLevel;					// .ds.b 1
extern uint8_t	VbatSampleCnt;					// .ds.b 1
extern uint8_t	BatRefreshTmr;					// .ds.b 1
extern uint8_t	EditTCRIndex;					// .ds.b 1
extern uint8_t	BatReadTimer;					// .ds.b 1
extern uint16_t	EditModeTimer;					// .ds.b 2
extern uint16_t	word_20000054;					// .ds.b 2
extern uint16_t	HideLogo;						// .ds.b 2
extern uint16_t	FireDuration;					// .ds.b 2
extern uint16_t	KeyPressTime;					// .ds.b 2
extern uint16_t	BatteryVoltage;					// .ds.b 2
extern uint16_t	SavedBatVoltage;				// .ds.b 2
extern uint16_t	BuckDuty;						// .ds.b 2
extern uint16_t	BoostDuty;						// .ds.b 2
extern volatile uint32_t	Flags64;			// .ds.b 4
extern volatile uint32_t	Flags68;			// .ds.b 4
extern uint32_t	AtoMinPower;					// .ds.b 4
extern uint32_t	AtoMaxPower;					// .ds.b 4
extern uint32_t	MaxTCPower;						// .ds.b 4
extern uint32_t	BTempSampleSum;					// .ds.b 4
extern uint32_t	VbatSampleSum;					// .ds.b 4
extern uint8_t	byte_20000080;					// .ds.b 1
extern uint8_t	ShowFDTimer;					// .ds.b 1
extern uint8_t	byte_20000082;					// .ds.b 1
extern uint32_t	AtoMinVolts;					// .ds.b 4
extern uint32_t	AtoMaxVolts;					// .ds.b 4
extern uint32_t	MaxVWVolts;						// .ds.b 4
extern uint32_t	MaxPower;						// .ds.b 4
extern uint16_t	CurrentFD;						// .ds.b 2
extern uint8_t	byte_20000098;					// .ds.b 1
extern uint16_t	AtoTemp;						// .ds.b 2
extern uint16_t	AtoCurrent;						// .ds.b 2
extern uint16_t	TCR;							// .ds.b 2
extern uint32_t	ADCShuntSum;					// .ds.b 4
extern uint32_t	dword_200000A4;					// .ds.b 4
extern uint32_t	ADCAtoSum;						// .ds.b 4
extern uint32_t	AtoRezMilli;					// .ds.b 4
extern uint16_t	SleepTimer;						// .ds.b 2
extern uint8_t	AtoStatus;						// .ds.b 1
extern uint8_t	byte_200000B3;					// .ds.b 1
extern uint16_t	AtoRez;							// .ds.b 2
extern uint16_t	word_200000B6;					// .ds.b 2
extern uint16_t	word_200000B8;					// .ds.b 2
extern uint16_t	word_200000BA;					// .ds.b 2
extern uint16_t	word_200000BC;					// .ds.b 2
extern uint16_t	word_200000BE;					// .ds.b 2
extern uint16_t	word_200000C0;					// .ds.b 2
extern uint8_t	CheckModeCounter;				// .ds.b 1
extern uint8_t	BBCNextMode;					// .ds.b 1
extern uint8_t	BBCMode;						// .ds.b 1
extern uint8_t	BBCNumCmps;						// .ds.b 1
extern uint16_t	LowBatVolts;					// .ds.b 2
extern uint32_t	AtoVoltsADC;					// .ds.b 4
extern uint32_t	AtoVolts;						// .ds.b 4
extern uint32_t	TMR0Counter2;					// .ds.b 4
extern uint32_t	PowerScale;						// .ds.b 4
extern uint32_t	TargetVolts;					// .ds.b 4
extern uint32_t	hidDataIndex;					// .ds.b 4
extern uint8_t	DisplayCmdByte;					// .ds.b 1
extern uint8_t	ScreenDuration;					// .ds.b 1
extern uint8_t	Screen;							// .ds.b 1
extern uint8_t	BatAnimTimer;					// .ds.b 1
extern uint8_t	ShowWeakBatFlag;				// .ds.b 1

extern uint32_t	__stdout;							// .ds.b 4
extern uint32_t	dword_20000168;					// .ds.b 4

//=============================================================================

extern void MemCpy( void*, const void*, const uint32_t );
extern void MemCpy2( const void*, void*, const uint32_t );
extern void MemClear( void*, const uint32_t );
extern void MemClear2( void*, void*, const uint32_t );
extern void MemSet( void*, const uint32_t, const char );

extern void DrawHexDigit( int x, int y, int v );
extern void DrawHexLong( int x, int y, int v, int font );

extern uint32_t PutTextf( char *s, ... );

//=============================================================================

extern uint32_t sqrtul( uint32_t v );

//=============================================================================

extern uint8_t myDbgFlag;

//=============================================================================
#endif /* __TESTC__ */
