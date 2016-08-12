#ifndef __EVENTS_H_
#define __EVENTS_H_

//==============================================================================
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
