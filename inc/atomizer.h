#ifndef __ATOMIZER_H__
#define __ATOMIZER_H__


extern void StopFire();

extern uint16_t LowestRezMeasure();
extern uint16_t ClampPower( uint16_t pwr, int clampmax );
extern uint16_t AtoPowerLimit( uint16_t pwr );
extern uint16_t AtoPower( uint16_t volts );
extern uint16_t GetVoltsForPower( uint16_t pwr );
extern uint16_t GetAtoVWVolts( uint16_t pwr );
extern void ClampPowers();
extern void ClampAtoVolts();
extern void SetMinMaxPower();
extern void SetMinMaxVolts();

extern void RegulateBuckBoost();
extern void ReachTargetVoltage();
extern void TweakTargetVoltsVW();
extern void TweakTargetVoltsTC();
extern void ProbeAtomizer();
extern void GetAtoCurrent();
extern void GetAtoTemp();
extern void ReadAtoTemp();
extern void GetTempCoef( const uint16_t tc[] );
extern void CheckMode();
extern void ReadBoardTemp();
extern void Overtemp();

extern void SwitchRezLock();

extern int SearchSTARTRez( uint16_t rez );
extern void SetAtoSTARTParams();

extern uint16_t CelsiusToF( uint16_t tc );
extern uint16_t FarenheitToC( uint16_t tf );

extern const uint16_t STARTRezValues[];
extern const uint16_t STARTPowers[];


#endif /* __ATOMIZER_H__ */
