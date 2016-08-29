#ifndef __BATTERY_H__
#define __BATTERY_H__

extern uint16_t RTBatVolts;
extern uint16_t LowBatVolts;
extern uint32_t	PowerScale;
extern uint16_t	BatteryVoltage;
extern uint8_t	BatteryPercent;
extern uint8_t	SavedBatPercent;
extern uint8_t	BatteryTenth;
extern uint8_t	BatRefreshTmr;
extern uint8_t	BatReadTimer;

extern uint8_t	byte_20000048;


extern void ReadBatteryVoltage();
extern int CheckBattery();

#endif /* __BATTERY_H__ */
