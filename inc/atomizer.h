#ifndef __ATOMIZER_H__
#define __ATOMIZER_H__


//=============================================================================

/* DC/DC converters PWM channels */
#define BBC_PWMCH_BUCK		0
#define BBC_PWMCH_BOOST		2

#define SHUNT_MIN_VALUE	75
#define SHUNT_MAX_VALUE	150

#define VVEL_DEF_RATIO	360
#define VVEL_MIN_RATIO	200
#define VVEL_MAX_RATIO	500

//-------------------------------------------------------------------------

extern uint32_t	AtoVolts;
extern uint32_t	TargetVolts;
extern uint32_t	AtoRezMilli;
extern uint32_t	ADCShuntSum;
extern uint32_t	ADCAtoSum;
extern uint32_t	AtoMinVolts;
extern uint32_t	AtoMaxVolts;
extern uint32_t	AtoMinPower;
extern uint32_t	AtoMaxPower;
extern uint32_t	MaxTCPower;
extern uint32_t	MaxVolts;
extern uint32_t	MaxPower;
extern uint16_t	TCR;
extern uint16_t	FireDuration;
extern uint16_t	AtoTemp;
extern uint16_t	AtoCurrent;
extern uint16_t	AtoRez;
extern uint8_t	AtoProbeCount;
extern uint8_t	AtoShuntRez;
extern uint8_t	AtoError;
extern uint8_t	AtoStatus;
extern uint8_t	BoardTemp;
extern uint8_t	ConfigIndex;
extern uint8_t	LastAtoError;
extern uint8_t	PreheatTimer;
extern uint16_t	PreheatPower;
extern uint32_t	MilliJoules;

extern uint8_t	byte_200000B3;
extern uint16_t	LastAtoRez;
extern uint16_t	word_200000B8;
extern uint16_t	word_200000BA;
extern uint16_t	word_200000BC;
extern uint16_t	word_200000BE;
extern uint16_t	word_200000C0;

extern const uint8_t TempCoefsNI[];
extern const uint8_t TempCoefsTI[];

//-------------------------------------------------------------------------

extern void InitPWM();
extern void BBC_Configure( uint32_t chan, uint32_t mode );

extern void StopFire();

extern uint16_t LowestRezMeasure();
extern uint16_t ClampPower( uint16_t pwr, int clampmax );
extern uint16_t AtoPowerLimit( uint16_t pwr );
extern uint16_t AtoPower( uint16_t volts );
extern uint16_t GetVoltsForPower( uint16_t pwr );
extern uint16_t GetAtoVWVolts( uint16_t pwr );
extern void ClampAtoPowers();
extern void ClampAtoVolts();
extern void SetMinMaxPower();
extern void SetMinMaxVolts();
extern void SetAtoLimits();

extern void RegulateBuckBoost();
extern void AtoWarmUp();
extern void TweakTargetVoltsVW();
extern void TweakTargetVoltsTC();
extern void ProbeAtomizer();
extern void ReadAtoCurrent();
extern void ReadAtoTemp();
extern void ReadAtomizer();
extern void GetTempCoef( const uint16_t tc[] );
extern void CheckMode();
extern void ReadBoardTemp();
extern void Overtemp();
extern void ResetResistance();

extern void SwitchRezLock();

extern int SearchSMARTRez( uint16_t rez );
extern void SetAtoSMARTParams();

extern uint16_t CelsiusToF( uint16_t tc );
extern uint16_t FarenheitToC( uint16_t tf );

extern const uint16_t SMARTRezValues[];
extern const uint16_t SMARTPowers[];

//=============================================================================

#endif /* __ATOMIZER_H__ */
