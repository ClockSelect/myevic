#ifndef __EVENTS_H_
#define __EVENTS_H_

//==============================================================================

#define EVENT_DOUBLE_FIRE	100
#define EVENT_EDIT_CONTRAST	101
#define EVENT_ENTER_MENUS	102
#define EVENT_QUAD_FIRE		103


//==============================================================================

extern void KeyRepeat();
extern void GetUserInput();
extern void EventHandler();

extern int CustomEvents();

//==============================================================================

#endif /* __EVENTS_H_ */
