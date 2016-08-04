@ ===========================================================================

					.section .srami
				@	.org 0x20000000

@ ===========================================================================
@ SRAM_Start:
@ ===========================================================================

ADC00_IRQ_Flag:		.ds.b	4
UpdateDFTimer:		.ds.b	1
UpdatePTTimer:		.ds.b	1
DisplayModel:		.ds.b	1	@ Display type
AtoShuntValue:		.ds.b	1	@ Atomizer shunt resistance value
fmcCntrsIndex:		.ds.b	2
ConfigIndex:		.ds.b	1
EditItemIndex:		.ds.b	1	@ Edited Item #	on screen
MenuPage:			.ds.b	1
					.ds.b	1
					.ds.b	1
					.ds.b	1
KeyUpTimer:			.ds.b	1
AtoError:			.ds.b	1	@ 0,1,2,3 = Ok,Open/Large,Short,Low
AtoProbeCount:		.ds.b	1
FireClickCount:		.ds.b	1
FireClickTimer:		.ds.b	1
UserInputs:			.ds.b	1
LastInputs:			.ds.b	1

Event:				.ds.b	1	@ Events (20000043)
								@  0 Idle
								@  1 Fire button
								@  2 Edit (+ button)
								@  3 Edit (- button)
								@  4 Key Lock/UnLock
								@  5 Menus (fire+ buttons)
								@  6 Stealth ON/OFF
								@ 10 USB cable attach
								@ 11 USB cable detach
								@ 12 Battery inserted
								@ 13 Battery removed
								@ 15 Single fire click
								@ 16 Enter edit	mode
								@ 17 Switch On/Off
								@ 18 Flip display
								@ 20 ?
								@ 21 ?
								@ 22 Reset Puff	Counter
								@ 23 Reset Time	Counter
								@ 24 10s Fire
								@ 25 Ato Short
								@ 26 Ato Open
								@ 27 Ato Low
								@ 28 Battery < 3.1V idle or < 2.8V firing
								@ 29 FW	Version	screen select /	(Game menu?)
								@ 30 Key lock violation
								@ 31 Board temp	screen select
								@ 32 New coil detected
								@ 33 (unused) Ti ON/OFF select
								@ 34 Battery voltage screen select
								@ 38 (unused) Change interface	version
								@ 39 TCR Set menu select
								@ 40 LOGO menu select
								@ 41 Game menu select

FireClicksEvent:	.ds.b	1
LastEvent:			.ds.b	1
BoardTemp:			.ds.b	1
BTempSampleCnt:		.ds.b	1
byte_20000048:		.ds.b	1
BatteryPercent:		.ds.b	1
SavedBatPercent:	.ds.b	1
BatteryTenth:		.ds.b	1	@ = BatteryPercent / 10
BatPCCmpCnt:		.ds.b	1
BatAnimLevel:		.ds.b	1
VbatSampleCnt:		.ds.b	1
BatRefreshTmr:		.ds.b	1
EditTCRIndex:		.ds.b	1
BatReadTimer:		.ds.b	1
EditModeTimer:		.ds.b	2
KeyTicks:		.ds.b	2
HideLogo:			.ds.b	2
FireDuration:		.ds.b	2
KeyPressTime:		.ds.b	2
BatteryVoltage:		.ds.b	2
SavedBatVoltage:	.ds.b	2
BuckDuty:			.ds.b	2
BoostDuty:			.ds.b	2
AtoMinPower:		.ds.b	4
AtoMaxPower:		.ds.b	4
MaxTCPower:			.ds.b	4
BTempSampleSum:		.ds.b	4
VbatSampleSum:		.ds.b	4
KRDelay:		.ds.b	1
ShowFDTimer:		.ds.b	1
byte_20000082:		.ds.b	1
					.ds.b	1
AtoMinVolts:		.ds.b	4
AtoMaxVolts:		.ds.b	4
MaxVWVolts:			.ds.b	4
MaxPower:			.ds.b	4
CurrentFD:			.ds.b	2	@ Snapshot of fire duration
					.ds.b	1
					.ds.b	1
byte_20000098:		.ds.b	1
					.ds.b	1
AtoTemp:			.ds.b	2
AtoCurrent:			.ds.b	2
TCR:				.ds.b	2
ADCShuntSum:		.ds.b	4
dword_200000A4:		.ds.b	4
ADCAtoSum:			.ds.b	4
AtoRezMilli:		.ds.b	4	@ milliOhms
SleepTimer:			.ds.b	2
AtoStatus:			.ds.b	1	@ 0,1,2,3,4 = Open,Short,Low,Large,Ok
byte_200000B3:		.ds.b	1
AtoRez:				.ds.b	2
word_200000B6:		.ds.b	2
word_200000B8:		.ds.b	2
word_200000BA:		.ds.b	2
word_200000BC:		.ds.b	2
word_200000BE:		.ds.b	2
word_200000C0:		.ds.b	2
CheckModeCounter:	.ds.b	1
					.ds.b	1
BBCNextMode:		.ds.b	1
BBCMode:			.ds.b	1
BBCNumCmps:			.ds.b	1
					.ds.b	1
LowBatVolts:		.ds.b	2
					.ds.b	1
					.ds.b	1
AtoVoltsADC:		.ds.b	4
AtoVolts:			.ds.b	4	@ Voltage atomiseur
TMR0Counter2:		.ds.b	4	@ 100kHz counter, cap at #2000
PowerScale:			.ds.b	4	@ 100 *	600 / pwr
TargetVolts:		.ds.b	4

DisplayCmdByte:		.ds.b	1
ScreenDuration:		.ds.b	1
Screen:				.ds.b	1	@ Screen (200000F6)
								@  0 Black
								@  1 Main view
								@  2 Firing
								@  3 Main view (?)
								@  4 (unused?)
								@  5 Black w/ Battery
								@ 20 No	Atomizer Found
								@ 21 Atomizer Short
								@ 22 Atomizer Low
								@ 23 10s Protection
								@ 24 Battery Low
								@ 25 Battery Low Lock
								@ 28 Key Lock
								@ 29 Device too	hot
								@ 31 Key UnLock
								@ 37 Board Temp
								@ 40 Stealth ON/OFF
								@ 41 Ti	ON/OFF
								@ 50 FW	Version
								@ 51 New Coil
								@ 54 Battery Voltage
								@ 59 TCR Set Menu
								@ 82 LOGO Menu
								@ 83 Game Menu
BatAnimTimer:		.ds.b	1
ShowWeakBatFlag:	.ds.b	1


@ ---------------------------------------------------------------------------
@ End of pre-initialized SRAM data
@ ---------------------------------------------------------------------------

					.section .sramz


@ ===========================================================================
@ End of SRAM
@ ===========================================================================
