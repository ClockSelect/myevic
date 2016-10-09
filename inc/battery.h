#ifndef __BATTERY_H__
#define __BATTERY_H__

//=========================================================================

#define BBC_PWMCH_CHARGER	5

//-------------------------------------------------------------------------

extern uint16_t RTBatVolts;
extern uint16_t LowBatVolts;
extern uint32_t	PowerScale;
extern uint16_t	BatteryVoltage;
extern uint16_t	BatteryCutOff;
extern uint16_t	BatteryIntRez;
extern uint16_t	BatteryMaxPwr;
extern uint8_t	BatteryPercent;
extern uint8_t	SavedBatPercent;
extern uint8_t	BatteryTenth;
extern uint8_t	NoEventTimer;
extern uint8_t	BatReadTimer;
extern uint8_t	NumBatteries;

extern uint8_t	byte_20000048;

//-------------------------------------------------------------------------

extern void ReadBatteryVoltage();
extern void NewBatteryVoltage();
extern int CheckBattery();
extern int GetNBatteries();
extern void SetBatteryModel();
extern const uint16_t *GetBatteryName();
extern void ReadInternalResistance();
extern void SetBatMaxPower();
extern void BatteryCharge();

//=========================================================================
#endif /* __BATTERY_H__ */
