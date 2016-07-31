@ ===========================================================================

					.section .srami
				@	.org 0x20000000

@ ===========================================================================
@ SRAM_Start:
@ ===========================================================================

					.ifne 	0	@keeporglibs_sys

SystemCoreClock:	.ds.b	4
CyclesPerUs:		.ds.b	4
PllClock:			.ds.b	4

gau32ClkSrcTbl:
					.ds.b	4
					.ds.b	4
					.ds.b	4
					.ds.b	4
					.ds.b	4
					.ds.b	4
					.ds.b	4
					.ds.b	4

					.endif

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
								@  5 (fire+ buttons)
								@  6 Stealth ON/OFF
								@ 10 USB cable attach
								@ 11 USB cable detach
								@ 12 Battery inserted
								@ 13 Battery removed
								@ 15 (unused?) Single fire click
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
								@ 33 (unused?) Ti ON/OFF select
								@ 34 Battery voltage screen select
								@ 38 (unused?) Change interface	version
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
word_20000054:		.ds.b	2
HideLogo:			.ds.b	2
FireDuration:		.ds.b	2
KeyPressTime:		.ds.b	2
BatteryVoltage:		.ds.b	2
SavedBatVoltage:	.ds.b	2
BuckDuty:			.ds.b	2
BoostDuty:			.ds.b	2

Flags64:			.ds.b	4	@ Flags1 (20000064)
								@ 0		0x00000001  TMR1 5kHz
								@ 1:2	0x00000006  TMR2 1kHz
								@ 3		0x00000008  TMR2 100Hz
								@ 4		0x00000010  TMR3 10Hz
								@ 5		0x00000020  TMR3 5Hz
								@ 6		0x00000040  TMR3 2Hz
								@ 8		0x00000100  Powering Atomizer
								@ 9		0x00000200  Battery Low
								@ 10	0x00000400  USB	plugged
								@ 11	0000000800  Refresh battery value
								@ 12	0x00001000  Battery in box
								@ 15	0x00008000  Sampling VBat
								@ 16	0x00010000  Sampling BoardTemp
								@ 17	0x00020000  Display refresh needed
								@ 18	0x00040000  2Hz	Blink edited item
								@ 19	0x00080000  Batt < 10%
								@ 21	0x00200000  1Hz	Oscillator
								@ 23	0x00800000  ?
								@ 27	0x08000000  Unchecked rez TI
								@ 28	0x10000000  Unchecked rez NI
								@ 31	0x80000000  Unchecked rez SS

Flags68:			.ds.b	4	@ Flags2 (20000068)
								@ 0		0x00000001  Edit TCR value
								@ 1		0x00000002  nchecked rez TCR
								@ 4     0x00000010  Editing TC pwr value
								@ 8		0x00000100  TMR3 1Hz
								@ 9		0x00000200  In Flappy Bird
AtoMinPower:		.ds.b	4
AtoMaxPower:		.ds.b	4
MaxTCPower:			.ds.b	4
BTempSampleSum:		.ds.b	4
VbatSampleSum:		.ds.b	4
byte_20000080:		.ds.b	1
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
TMR0Counter2:		.ds.b	4	@ 1000Hz (1kHz)	counter, cap at	#2000
PowerScale:			.ds.b	4	@ 100 *	600 / pwr
TargetVolts:		.ds.b	4

					.ifne	keeporgcode_usbd
usbdStrings:		.long	usbdStr0
					.long	usbdStr1
					.long	usbdStr2
					.long	usbdStr3
hidDataIndex:		.ds.b	4
					.endif

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
					.ds.b	1
					.ds.b	1
					.ds.b	1
StdOut:				.ds.b	4


			.ifne	keeporglibs_usbd

g_usbd_RemoteWakeupEn:			.ds.b	1
								.ds.b	1
								.ds.b	1
								.ds.b	1
g_usbd_CtrlInPointer:			.ds.b	4
g_usbd_CtrlInSize:				.ds.b	4
g_usbd_CtrlOutPointer:			.ds.b	4
g_usbd_CtrlOutSize:				.ds.b	4
g_usbd_CtrlOutSizeLimit:		.ds.b	4
g_usbd_UsbAddr:					.ds.b	4
g_usbd_UsbConfig:				.ds.b	4
g_usbd_CtrlMaxPktSize:			.long	8
g_usbd_UsbAltInterface:			.ds.b	4
g_usbd_pfnVendorRequest:		.ds.b	4
g_usbd_pfnClassRequest:			.ds.b	4
g_usbd_pfnSetInterface:			.ds.b	4
g_usbd_pfnSetConfigCallback:	.ds.b	4
g_u32EpStallLock:				.ds.b	4
g_usbd_sInfo:					.ds.b	4
g_usbd_SetupPacket:				.ds.b	1
usbdSetupRq:					.ds.b	1
usbdSetupVal:					.ds.b	1
usbdSetupValH:					.ds.b	1
usbdSetupIdx:					.ds.b	2
usbdSetupLen:					.ds.b	1
usbdSetupLenH:					.ds.b	1

			.endif


fbBirdColumn:		.byte	16
fbBirdCycle:		.ds.b	1
fbBirdLine:			.byte	24
fbBirdDisp:			.ds.b	1
fbDead:				.ds.b	1
fbAnimStep:			.ds.b	1
fbAnimTimer:		.ds.b	1
					.ds.b	1
fbScore:			.ds.b	2
fbColumn1:			.ds.b	5
fbColumn2:			.ds.b	5
fbColumn3:			.ds.b	5
fbTimeoutMask:		.ds.b	1
fbCurrentTimeout:	.ds.b	1
fbUsedTimeouts:		.ds.b	1

					.ifne	keeporgcode
RNGSeed:			.ds.b	4
					.endif

dword_20000168:		.ds.b	4

@ ---------------------------------------------------------------------------
@ End of initialized RAM data
@ ---------------------------------------------------------------------------

					.section .sramz

@ ---------------------------------------------------------------------------

TMR0Counter:		.ds.b	4
TMR1Counter:		.ds.b	4
TMR2Counter:		.ds.b	4
TMR3Counter:		.ds.b	4

@ ===========================================================================
@ DataFlash Area
@ ---------------------------------------------------------------------------

					.ifne keeporgcode

SavedDF:			.ds.b	0x100

dfData:				.ds.b	4	@ 0x2000027C
dfCRC:				.ds.b	4
dfHWVersion:		.ds.b	4
dfMagic:			.ds.b	1	@ = 0x36
dfBootFlag:			.ds.b	1
dfMode:				.ds.b	2	@ 0 TEMP NI
								@ 1 TEMP TI
								@ 2 TEMP SS316
								@ 3 TEMP TCR
								@ 4 POWER
								@ 5 BYPASS
								@ 6 START
dfPower:			.ds.b	2
dfTemp:				.ds.b	2
dfTCPower:			.ds.b	2
dfVWVolts:			.ds.b	2
dfAPT:				.ds.b	1
dfRezType:			.ds.b	1
dfTempAlgo:			.ds.b	1
dfIsCelsius:		.ds.b	1
dfResistance:		.ds.b	2
dfRezTI:			.ds.b	2
dfRezNI:			.ds.b	2
dfRezLockedTI:		.ds.b	1
dfRezLockedNI:		.ds.b	1
dfTiOn:				.ds.b	1
dfStealthOn:		.ds.b	1
dfTempCoefsNI:		.ds.b	0x2A
dfTempCoefsTI:		.ds.b	0x2A
					.ds.b	1
					.ds.b	1
dfStatus:			.ds.b	4	@ Status bits
								@ 0  0x01  Switched off
								@ 1  0x02  Key locked
								@ 2  0x04  Flipped display
								@ 3  0x08  LOGO	Off
dfAtoRez:			.ds.b	2
dfAtoStatus:		.ds.b	1	@ 0,1,2,3,4 = Open,Short,Low,Large,Ok
					.ds.b	1
dfRezSS:			.ds.b	2
dfRezLockedSS:		.ds.b	1
dfUIVersion:		.ds.b	1
dfTCRIndex:			.ds.b	1
					.ds.b	1
dfTCRM1:			.ds.b	2
dfTCRM2:			.ds.b	2
dfTCRM3:			.ds.b	2
dfRezTCR:			.ds.b	2
dfRezLockedTCR:		.ds.b	1
					.ds.b	1
dfLastTCMode:		.ds.b	2
dfSavedCfgRez:		.ds.b	0x14
dfSavedCfgPwr:		.ds.b	0x14
dfFBBest:			.ds.b	2
dfFBSpeed:			.ds.b	1	@ Flappy Bird Speed
								@ 0 Easy    62.5Hz 0.016s
								@ 1 Normal  76.9Hz 0.013s
								@ 2 Hard   100.0Hz 0.010s
dfbyte_2000033D:		.ds.b	1

dfContrast:			.ds.b	1	@ Screen contrast
dfModesSel:			.ds.b	1	@ Mode selector mask
dfClkRatio:			.ds.b	2	@ RTC LIRC/X32 ratio constant


					.ds.b ( 0x100 - ( . - dfData ))

					.if (( . - dfData ) != 0x100 )
					.error "Invalid Dataflash page size!"
					.endif

@ ---------------------------------------------------------------------------
@ End of real DataFlash Area
@ ---------------------------------------------------------------------------

dfPage2:			.ds.b	4
dfFWVersion:		.ds.b	4
					.ds.b	4
dffmcCID:			.ds.b	4
dffmcDID:			.ds.b	4
dffmcPID:			.ds.b	4
dffmcUID:			.ds.b	4*3
dffmcUCID:			.ds.b	4*4
dfPuffCount:		.ds.b	4
dfTimeCount:		.ds.b	4
dfProductID:		.ds.b	4
dfMaxHWVersion:		.ds.b	4

					.ds.b	( 0x100 - ( . - dfPage2 ))

					.ds.b	4
gPlayfield:
					.ds.b ( 0x800 - ( . - dfData ))

					.if (( . - dfData ) != 0x800 )
					.error "Invalid Dataflash full size!"
					.endif
					
					.endif

@ ---------------------------------------------------------------------------
@ End of DataFlash Area
@ ===========================================================================

					.ifne	keeporgcode_usbd

hidData:			.ds.b	0x800
hidDFData:			.ds.b	0x800

hidCmd:
hidRequest:			.ds.b	1
hidRQLen:			.ds.b	1
hidRQvalue1:		.ds.b	4
hidRQvalue2:		.ds.b	4
hidRQhidc:			.ds.b	4
hidRQchk:			.ds.b	4

					.endif

@ ===========================================================================

ScreenBuffer:		.ds.b	0x400

@ ===========================================================================

					.balign	4,0
TimeoutsTable:		.ds.b	0x18

@ ===========================================================================

					.balign 4,0
					.section .stack

Stack_Bottom:		.ds.b	0x400	@ Stack	space
Stack_Top:

@ ===========================================================================
@ SRAM_End:
@ ===========================================================================
