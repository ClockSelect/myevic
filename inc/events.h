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

extern uint8_t	WattsInc;

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

#define EVENT_TOGGLE_CLOCK		100
#define EVENT_DEBUG_MODE		101
#define EVENT_EDIT_CONTRAST		102
#define EVENT_ENTER_MENUS		103
#define EVENT_LONG_FIRE			104
#define EVENT_EXIT_MENUS		105
#define EVENT_PARENT_MENU		106
#define EVENT_SET_TIME			107
#define EVENT_SET_DATE			108
#define EVENT_NEXT_MODE			109
#define EVENT_TOGGLE_TDOM		110
#define EVENT_RESET_VVEL		111
#define EVENT_FORCE_VCOM		112
#define EVENT_AUTO_PUFF			113
#define EVENT_CLK_SPEED			114
#define EVENT_CLK_ADJUST		115
#define EVENT_INVERT_SCREEN		116
#define EVENT_MODE_CHANGE		117
#define EVENT_PROFILE_MENU		118
#define EVENT_NEXT_PROFILE		119
#define EVENT_POWER_CURVE		120


//==============================================================================

enum
{
	CLICK_ACTION_NONE = 0,
	CLICK_ACTION_EDIT,
	CLICK_ACTION_CLOCK,
	CLICK_ACTION_TDOM,
	CLICK_ACTION_NEXT_MODE,
	CLICK_ACTION_ON_OFF,
	CLICK_ACTION_PROFILE,
	CLICK_ACTION_GAME,
	CLICK_ACTION_MAX
};


//==============================================================================

extern S_RTC_TIME_DATA_T SetTimeRTD;

//------------------------------------------------------------------------------

extern void KeyRepeat();
extern void GetUserInput();
extern void EventHandler();

extern int CustomEvents();

extern void PowerPlus( uint16_t *pwr, uint16_t min, uint16_t max );
extern void PowerMinus( uint16_t *pwr, uint16_t min, uint16_t max );


//==============================================================================

#endif /* __EVENTS_H_ */
