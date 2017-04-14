#ifndef __BATTERY_H__
#define __BATTERY_H__

//=========================================================================

#define BBC_PWMCH_CHARGER	5

#define BVO_MIN -100
#define BVO_MAX  100

#define BATTERY_CUSTOM	255

//-------------------------------------------------------------------------

typedef struct
{
	uint16_t percent;
	uint16_t voltage;
}
BatV2P_t;

typedef struct
{
	const uint8_t	*name;
	BatV2P_t	V2P[11];
	uint16_t	cutoff;
	uint16_t	intrez;
	uint16_t	maxamp;
}
Battery_t;

//-------------------------------------------------------------------------

extern uint16_t RTBattVolts;
extern uint16_t	RTBVolts[4];
extern uint16_t	RTBVTotal;
extern uint16_t LowBatVolts;
extern uint32_t	PowerScale;
extern uint16_t	BatteryVoltage;
extern uint16_t	BattVoltsTotal;
extern uint16_t	BattVolts[4];
extern uint16_t	BatteryCutOff;
extern uint16_t	BatteryIntRez;
extern uint16_t	BatteryMaxPwr;
extern uint8_t	BatteryPercent;
extern uint8_t	SavedBatPercent;
extern uint8_t	BatteryTenth;
extern uint8_t	NoEventTimer;
extern uint8_t	BatReadTimer;
extern uint8_t	NumBatteries;
extern uint8_t	MaxBatteries;
extern uint16_t	ChargerDuty;
extern uint16_t	MaxChargerDuty;
extern uint16_t	ChBalTimer;

extern uint8_t	BattProbeCount;

extern uint8_t	USBMaxLoad;
extern uint8_t	ChargeStatus;
extern uint8_t	BatteryStatus;
extern uint8_t	BBBits;
extern uint8_t	ChargeMode;
extern uint8_t	ChargeStep;

extern Battery_t CustomBattery;

//-------------------------------------------------------------------------

extern void ReadBatteryVoltage();
extern void NewBatteryData();
extern void NewBatteryVoltage();
extern int CheckBattery();
extern int GetNBatteries();
extern void SetBatteryModel();
extern const uint8_t *GetBatteryName();
extern void ReadInternalResistance();
extern void SetBatMaxPower();
extern void BatteryChargeDual();
extern void BatteryCharge();
extern void SaveCustomBattery( const Battery_t *b );
extern void LoadCustomBattery();
extern void ResetCustomBattery();
extern int  CheckCustomBattery();

//=========================================================================
#endif /* __BATTERY_H__ */
