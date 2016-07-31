#ifndef __TESTC__
#define __TESTC__

#include "M451Series.h"

//=============================================================================

#define __myevic__ __attribute__ ((section (".myevic")))

#define	ISMODETC(m) ((m)<=3)
#define ISMODEVW(m) (((m)==4)||((m)==6))
#define ISMODEBY(m) ((m)==5)

//=============================================================================

/*
extern uint8_t	SavedDF[0x100];		// .ds.b 0x100

extern uint32_t	dfData;
extern uint32_t	dfCRC;
extern uint32_t	dfHWVersion;
extern uint8_t	dfMagic;
extern uint8_t	dfBootFlag;
extern uint16_t	dfMode;
extern uint16_t	dfPower;
extern uint16_t	dfTemp;
extern uint16_t	dfTCPower;
extern uint16_t	dfVWVolts;
extern uint8_t	dfAPT;
extern uint8_t	dfRezType;
extern uint8_t	dfTempAlgo;
extern uint8_t	dfIsCelsius;
extern uint16_t	dfResistance;
extern uint16_t	dfRezTI;
extern uint16_t	dfRezNI;
extern uint8_t	dfRezLockedTI;
extern uint8_t	dfRezLockedNI;
extern uint8_t	dfTiOn;
extern uint8_t	dfStealthOn;
extern uint16_t	dfTempCoefsNI[21];
extern uint16_t	dfTempCoefsTI[21];
extern uint32_t	dfStatus;
extern uint16_t	dfAtoRez;
extern uint8_t	dfAtoStatus;
extern uint16_t	dfRezSS;
extern uint8_t	dfRezLockedSS;
extern uint8_t	dfUIVersion;
extern uint8_t	dfTCRIndex;
extern uint16_t	dfTCRM1;
extern uint16_t	dfTCRM2;
extern uint16_t	dfTCRM3;
extern uint16_t	dfRezTCR;
extern uint8_t	dfRezLockedTCR;
extern uint16_t	dfLastTCMode;
extern uint16_t	dfSavedCfgRez[10];
extern uint16_t	dfSavedCfgPwr[10];
extern uint16_t	dfFBBest;
extern uint8_t	dfFBSpeed;
extern uint8_t	dfbyte_2000033D;
extern uint8_t	dfContrast;
extern uint8_t	dfModesSel;
extern uint16_t	dfClkRatio;

extern uint32_t	dfFWVersion;
extern uint32_t	dffmcCID;
extern uint32_t	dffmcDID;
extern uint32_t	dffmcPID;
extern uint32_t	dffmcUID[3];
extern uint32_t	dffmcUCID[4];
extern uint32_t	dfPuffCount;
extern uint32_t	dfTimeCount;
extern uint32_t	dfProductID;
extern uint32_t	dfMaxHWVersion;
*/

extern union
{
	int8_t c[0x100];
	uint8_t uc[0x100];
	int16_t s[0x100/sizeof(int16_t)];
	uint16_t us[0x100/sizeof(uint16_t)];
	int32_t l[0x100/sizeof(int32_t)];
	uint32_t ul[0x100/sizeof(uint32_t)];
} gPlayfield;

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

extern uint32_t	StdOut;							// .ds.b 4

/*
extern uint8_t	fbBirdColumn;					// .ds.b 1
extern uint8_t	fbBirdCycle;					// .ds.b 1
extern uint8_t	fbBirdLine;						// .ds.b 1
extern uint8_t	fbBirdDisp;						// .ds.b 1
extern uint8_t	fbDead;							// .ds.b 1
extern uint8_t	fbAnimStep;						// .ds.b 1
extern uint8_t	fbAnimTimer;					// .ds.b 1
extern uint16_t	fbScore;						// .ds.b 2
extern uint8_t	fbColumn1[5];					// .ds.b 5
extern uint8_t	fbColumn2[5];					// .ds.b 5
extern uint8_t	fbColumn3[5];					// .ds.b 5
extern uint8_t	fbTimeoutMask;					// .ds.b 1
extern uint8_t	fbCurrentTimeout;					// .ds.b 1
extern uint8_t	fbUsedTimeouts;					// .ds.b 1
extern uint32_t	RNGSeed;						// .ds.b 4
*/

extern uint32_t	dword_20000168;					// .ds.b 4

extern volatile uint32_t	TMR0Counter;		// .ds.b 4
extern volatile uint32_t	TMR1Counter;		// .ds.b 4
extern volatile uint32_t	TMR2Counter;		// .ds.b 4
extern volatile uint32_t	TMR3Counter;		// .ds.b 4

//=============================================================================

extern void orgInitUSB();

extern void MemCpy( void*, const void*, const uint32_t );
extern void MemCpy2( const void*, void*, const uint32_t );
extern void MemClear( void*, const uint32_t );
extern void MemClear2( void*, void*, const uint32_t );

extern void DrawHexDigit( int x, int y, int v );
extern void DrawHexLong( int x, int y, int v, int font );

extern void fbInitTimeouts();
extern void fbDeleteTimeout( int );
extern void fbCallTimeouts();
extern int	fbCreateTimeout( void (*cb)( void ) );
extern void fbTickTimeouts();
extern void fbBirdAnim( int );
extern void fbStartScreen();

extern uint32_t PutTextf( char *s, ... );

//=============================================================================

extern uint32_t sqrtul( uint32_t v );

//=============================================================================

extern uint8_t myDbgFlag;
extern volatile uint16_t ClockCorrection;

//=============================================================================
#endif /* __TESTC__ */
