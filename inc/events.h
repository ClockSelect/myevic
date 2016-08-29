#ifndef __EVENTS_H_
#define __EVENTS_H_

//=========================================================================

extern uint8_t	FireClickCount;
extern uint8_t	FireClickTimer;
extern int8_t	UserInputs;
extern int8_t	LastInputs;
extern uint8_t	FireClicksEvent;

extern uint8_t	KeyUpTimer;
extern uint16_t	KeyTicks;
extern uint16_t	KeyPressTime;

extern uint8_t	MenuPage;

//-------------------------------------------------------------------------
extern volatile uint8_t Event;
extern uint8_t	LastEvent;

//  0 Idle
//  1 Fire button
//  2 Edit (+ button)
//  3 Edit (- button)
//  4 Key Lock/UnLock
//  5 Menus (fire+ buttons)
//  6 Stealth ON/OFF
// 10 USB cable attach
// 11 USB cable detach
// 12 Battery inserted
// 13 Battery removed
// 15 Single fire click
// 16 Enter edit	mode
// 17 Switch On/Off
// 18 Flip display
// 20 ?
// 21 ?
// 22 Reset Puff	Counter
// 23 Reset Time	Counter
// 24 10s Fire
// 25 Ato Short
// 26 Ato Open
// 27 Ato Low
// 28 Battery < 3.1V idle or < 2.8V firing
// 29 FW	Version	screen select /	(Game menu?)
// 30 Key lock violation
// 31 Board temp	screen select
// 32 New coil detected
// 33 (unused) Ti ON/OFF select
// 34 Battery voltage screen select
// 38 (unused) Change interface	version
// 39 TCR Set menu select
// 40 LOGO menu select
// 41 Game menu select


// Events 100+ are custom events not existing in the OFW

#define EVENT_DOUBLE_FIRE	100
#define EVENT_EDIT_CONTRAST	101
#define EVENT_ENTER_MENUS	102
#define EVENT_QUAD_FIRE		103
#define EVENT_LONG_FIRE		104


//==============================================================================

extern void KeyRepeat();
extern void GetUserInput();
extern void EventHandler();

extern int CustomEvents();

//==============================================================================

#endif /* __EVENTS_H_ */
