#ifndef __SCREENS__
#define __SCREENS__

//==============================================================================

#define BLINKITEM(i) ((EditModeTimer)&&(!gFlags.draw_edited_item)&&(EditItemIndex==(i)))

//==============================================================================

extern uint16_t	ScreenDuration;
extern uint8_t	Screen;

extern uint8_t	ShowDateFlag;
extern uint16_t	HideLogo;
extern uint8_t	ShowWeakBatFlag;
extern uint8_t	BatAnimLevel;

extern uint8_t	EditItemIndex;
extern uint16_t	EditModeTimer;
extern uint8_t	EditTCRIndex;

extern const uint8_t ScrSaveTimes[8];

extern int IsClockOnScreen();
extern void DrawClock();

//==============================================================================

extern void DrawScreen();
extern void ShowMainView();
extern void ShowBattery();
extern void ShowBatCharging();
extern void ShowBattVolts();
extern void ShowBoardTemp();
extern void ShowVersion();
extern void ShowNewCoil();
extern void ShowLOGOMenu();
extern void ShowGameMenu();
extern void ShowTCRSet();
extern void ShowStealthMode();
extern void ShowDevTooHot();
extern void ShowTiOnOff();
extern void ShowAtoLow();
extern void ShowAtoShort();
extern void ShowBatLow();
extern void ShowBatLowLock();
extern void ShowKeyLock();
extern void ShowKeyUnLock();
extern void ShowNoAtoFound();
extern void Show10sProtec();
extern void ShowWeakBat();
extern void ShowInfos();
extern void ShowContrast();
extern void ShowMenus();
extern void ShowRTCSpeed();
extern void ShowRTCAdjust();
extern void ShowScreenSaver();

extern uint16_t GetScreenProtection();

//==============================================================================

extern void MainView();

//==============================================================================
// Strings

extern const uint16_t String_ON[];
extern const uint16_t String_No[];
extern const uint16_t String_On[];
extern const uint16_t String_Key[];
extern const uint16_t String_OFF[];
extern const uint16_t String_Low[];
extern const uint16_t String_Off[];
extern const uint16_t String_Lock[];
extern const uint16_t String_UnLock[];
extern const uint16_t String_Protec[];
extern const uint16_t String_Version[];
extern const uint16_t String_Device[];
extern const uint16_t String_TooHot[];
extern const uint16_t String_Stealth[];
extern const uint16_t String_TiON[];
extern const uint16_t String_TiOFF[];
extern const uint16_t String_Temp[];
extern const uint16_t String_Battery[];
extern const uint16_t String_Atomizer[];
extern const uint16_t String_Found[];
extern const uint16_t String_Short[];
extern const uint16_t String_Over10s[];
extern const uint16_t String_NewCoil[];
extern const uint16_t String_SameCoil[];
extern const uint16_t String_Right[];
extern const uint16_t String_Left[];
extern const uint16_t String_LOGO[];
extern const uint16_t String_Game[];
extern const uint16_t String_Easy[];
extern const uint16_t String_Normal[];
extern const uint16_t String_Hard[];
extern const uint16_t String_Exit[];
extern const uint16_t String_NI[];
extern const uint16_t String_TI[];
extern const uint16_t String_SS[];
extern const uint16_t String_BF_s[];
extern const uint16_t String_TCR[];
extern const uint16_t String_PWR_s[];
extern const uint16_t String_AMP_s[];
extern const uint16_t String_MAX_s[];
extern const uint16_t String_MIN_s[];
extern const uint16_t String_Weak[];
extern const uint16_t String_TEMP[];
extern const uint16_t String_TCRSet[];
extern const uint16_t String_POWER[];
extern const uint16_t String_BYPASS[];
extern const uint16_t String_VOLT_s[];
extern const uint16_t String_COIL_s[];
extern const uint16_t String_TIME_s[];
extern const uint16_t String_PUFF_s[];
extern const uint16_t String_START[];

//-------------------------------------------------------------------------

// from mainview.c
extern const uint16_t String_BATT_s[];
extern const uint16_t String_VOUT_s[];
extern const uint16_t String_TEMP_s[];

// from screens.c
extern const uint16_t String_Contrast[];
extern const uint16_t String_Fireto[];
extern const uint16_t String_Edit[];
extern const uint16_t String_ClkSpeed[];
extern const uint16_t String_ClkAdjust[];

// from menus.c
extern const uint16_t String_Menus[];
extern const uint16_t String_Modes[];
extern const uint16_t String_TEMP_NI_s[];
extern const uint16_t String_TEMP_TI_s[];
extern const uint16_t String_TEMP_SS_s[];
extern const uint16_t String_TCR_s[];
extern const uint16_t String_POWER_s[];
extern const uint16_t String_BYPASS_s[]; 
extern const uint16_t String_START_s[];
extern const uint16_t String_Coils[];
extern const uint16_t String_Zero_All[];
extern const uint16_t String_Miscs[];
extern const uint16_t String_DateTime[];
extern const uint16_t String_Cancel[];
extern const uint16_t String_Save[];
extern const uint16_t String_Clock[];
extern const uint16_t String_3D[];
extern const uint16_t String_Cube[];
extern const uint16_t String_None[];
extern const uint16_t String_Screen[];
extern const uint16_t String_Min[];
extern const uint16_t String_VCOM[];
extern const uint16_t String_Expert[];
extern const uint16_t String_USB[];
extern const uint16_t String_HID[];
extern const uint16_t String_COM[];
extern const uint16_t String_DSK[];
extern const uint16_t String_DBG[];

//==============================================================================

#endif