
@ ===========================================================================
@ Segment type:	Regular

		.section .sram	@	SRAM
		@ .org 0x20000000

CPUClock:	.ds.b 4			@ DATA XREF: InitHardware+48o
					@ InitHardware+4Ar ...
CPUClockMHz:	.ds.b 4			@ DATA XREF: TickWait_uso
					@ TickWait_us+2r ...
PLLClock:	.ds.b 4			@ DATA XREF: InitHardware+44o
					@ InitHardware+46r ...
@ _DWORD CLKSourcesFreqs[8]
CLKSourcesFreqs:.ds.b 4
		.ds.b 4
		.ds.b 4
		.ds.b 4
		.ds.b 4
		.ds.b 4
		.ds.b 4
		.ds.b 4
ADC_IRQ0_flag:	.ds.b 4			@ DATA XREF: ADC_IRQ0_Handlero
					@ ADC_IRQ0_Handler+4w ...
UpdateDFTimer:	.ds.b 1			@ DATA XREF: sub_854+12Ao sub_854+12Ew ...
UpdatePTTimer:	.ds.b 1			@ DATA XREF: UpdateFlash:loc_2046r
					@ UpdateFlash+22w ...
DisplayModel:	.ds.b 1			@ DATA XREF: InitDataFlash+134w
					@ DisplayWriteByte+8o ...
					@ Display type
AtoShuntValue:	.ds.b 1			@ DATA XREF: ReadAtoTemp:loc_32D6o
					@ ReadAtoTemp+8Cr ...
					@ Atomizer shunt resistance value
fmcCntrsIndex:	.ds.b 2			@ DATA XREF: sub_2080+22w
					@ FMCWriteCounters+Er ...
ConfigIndex:	.ds.b 1			@ DATA XREF: CheckBattery:loc_79Co
					@ CheckBattery+52r ...
					@ Saved	config # in start mode
EditItemIndex:	.ds.b 1			@ DATA XREF: sub_3738+3Ao sub_3738+3Cr ...
					@ Edited Item #	on screen
LOGOMenuIndex:	.ds.b 1			@ DATA XREF: EventHandler+934o
					@ EventHandler+936r ...
		.ds.b 1
		.ds.b 1
		.ds.b 1
byte_2000003C:	.ds.b 1			@ DATA XREF: NewBatteryVoltage+2o
					@ ROM:off_6D8o	...
byte_2000003D:	.ds.b 1			@ DATA XREF: sub_121C+4o sub_121C+6r ...
byte_2000003E:	.ds.b 1			@ DATA XREF: RegulateBuckBoost+182o
					@ RegulateBuckBoost+184r ...
FireClickCount:	.ds.b 1			@ DATA XREF: TimedItems+B6w
FireClickTimer:	.ds.b 1			@ DATA XREF: TimedItems:loc_7E32r
					@ TimedItems+B2w
UserInputs:	.ds.b 1			@ DATA XREF: GetUserInput+Cw
					@ GetUserInput+48w
LastInputs:	.ds.b 1			@ DATA XREF: GetUserInput+22r
					@ InitVariables+16o ...
Event:		.ds.b 1			@ DATA XREF: CheckBattery+62o
					@ CheckBattery+66w ...
					@ Events (20000043)
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
FireClicksEvent:.ds.b 1
byte_20000045:	.ds.b 1			@ DATA XREF: EventHandler+14w
BoardTemp:	.ds.b 1			@ DATA XREF: EventHandler+1F4r
					@ ShowBoardTemp+12o ...
BTempSampleCnt:	.ds.b 1			@ DATA XREF: ReadBoardTemp:loc_7BEAr
					@ ReadBoardTemp+1Cr ...
byte_20000048:	.ds.b 1			@ DATA XREF: GPD_IRQHandler+24w
					@ TimedItems+1Cr ...
BatteryPercent:	.ds.b 1			@ DATA XREF: NewBatteryVoltage+Ew
					@ NewBatteryVoltage+1Cw ...
SavedBatPercent:.ds.b 1			@ DATA XREF: NewBatteryVoltage:loc_67Ar
					@ NewBatteryVoltage+3Ew
BatteryTenth:	.ds.b 1			@ DATA XREF: NewBatteryVoltage+76w
					@ EventHandler+82r ...
					@ = BatteryPercent / 10
BatPCCmpCnt:	.ds.b 1			@ DATA XREF: NewBatteryVoltage+26r
					@ NewBatteryVoltage+2Cw ...
BatAnimLevel:	.ds.b 1			@ DATA XREF: EventHandler+EB2w
					@ EventHandler+F8Cw ...
VbatSampleCnt:	.ds.b 1			@ DATA XREF: ReadBatteryVoltage:loc_6EEr
					@ ReadBatteryVoltage+20r ...
byte_2000004F:	.ds.b 1			@ DATA XREF: ReadBatteryVoltage+40r
					@ EventHandler+10w ...
EditTCRIndex:	.ds.b 1			@ DATA XREF: EventHandler+AEr
					@ EventHandler+8FEw ...
byte_20000051:	.ds.b 1			@ DATA XREF: StopFire+98w sub_4F0C+8o	...
EditModeTimer:	.ds.b 2			@ DATA XREF: sub_3738+Ao sub_3738+Er ...
word_20000054:	.ds.b 2			@ DATA XREF: sub_3738+B6o
					@ sub_3738:loc_3804r ...
HideLogo:	.ds.b 2			@ DATA XREF: MainView:loc_1672o
					@ MainView+22w	...
FireDuration:	.ds.b 2			@ DATA XREF: DrawScreen+16o
					@ DrawScreen+1Ar ...
KeyPressTime:	.ds.b 2			@ DATA XREF: GetUserInput+15Cr
					@ sub_4F0C+42o	...
BatteryVoltage:	.ds.b 2			@ DATA XREF: NewBatteryVoltage+4r
					@ NewBatteryVoltage:loc_692r ...
SavedBatVoltage:.ds.b 2			@ DATA XREF: NewBatteryVoltage+3Aw
					@ ReadBatteryVoltage+46r
BuckDuty:	.ds.b 2			@ DATA XREF: RegulateBuckBoost+4Eo
					@ RegulateBuckBoost+86r ...
BoostDuty:	.ds.b 2			@ DATA XREF: RegulateBuckBoost+5Eo
					@ RegulateBuckBoost+84w ...
dword_20000064:	.ds.b 4			@ DATA XREF: NewBatteryVoltage+10r
					@ NewBatteryVoltage+56w ...
					@ Flags1 (20000064)
					@ 0	0x00000001  TMR1 5kHz
					@ 1:2	0x00000006  TMR2 1kHz
					@ 3	0x00000008  TMR2 100Hz
					@ 4	0x00000010  TMR3 10Hz
					@ 5	0x00000020  TMR3 5Hz
					@ 6	0x00000040  TMR3 2Hz
					@ 8	0x00000100  Powering Atomizer
					@ 9	0x00000200  Battery Low
					@ 10	0x00000400  USB	plugged
					@ 12	0x00001000  Battery in box
					@ 15	0x00008000  Sampling VBat
					@ 16	0x00010000  Sampling BoardTemp
					@ 17	0x00020000  Display refresh needed
					@ 18	0x00040000  2Hz	Blink edited item
					@ 19	0x00080000  Batt < 10%
					@ 21	0x00200000  1Hz	Oscillator
					@ 23	0x00800000  ?
					@ 27	0x08000000  ? (relative	to rez TI)
					@ 28	0x10000000  ? (relative	to rez NI)
					@ 31	0x80000000  ? (relative	to rez SS)
dword_20000068:	.ds.b 4			@ DATA XREF: CheckMode:loc_3134o
					@ CheckMode+BEr ...
					@ Flags2 (20000068)
					@ 0	0x00000001  Edit TCR value
					@ 1	0x00000002  ? (relative	to rez TCR)
					@ 8	0x00000100  TMR3 1Hz
					@ 9	0x00000200  In Flappy Bird
AtoMinPower:	.ds.b 4			@ DATA XREF: sub_121C+22o sub_121C+24r ...
AtoMaxPower:	.ds.b 4			@ DATA XREF: sub_121C+2Eo sub_121C+30r ...
MaxTCPower:	.ds.b 4			@ DATA XREF: InitDataFlash+19Ao
					@ InitDataFlash+1A0w ...
BTempSampleSum:	.ds.b 4			@ DATA XREF: ReadBoardTemp+16r
					@ ReadBoardTemp+1Aw ...
VbatSampleSum:	.ds.b 4			@ DATA XREF: ReadBatteryVoltage+1Ar
					@ ReadBatteryVoltage+1Ew ...
byte_20000080:	.ds.b 1			@ DATA XREF: sub_3738+B4o sub_3738+BCr ...
byte_20000081:	.ds.b 1			@ DATA XREF: DrawScreen+6o
					@ DrawScreen+28w ...
byte_20000082:	.ds.b 1			@ DATA XREF: sub_6038:loc_614Ao
					@ sub_6038+118r ...
		.ds.b 1
AtoMinVolts:	.ds.b 4			@ DATA XREF: GetAtoVWVolts+64o
					@ ROM:off_1328o ...
AtoMaxVolts:	.ds.b 4			@ DATA XREF: GetAtoVWVolts:loc_12F0o
					@ ROM:off_132Co ...
MaxVWVolts:	.ds.b 4			@ DATA XREF: InitDataFlash+180o
					@ InitDataFlash+18Ew ...
MaxPower:	.ds.b 4			@ DATA XREF: sub_121C:loc_1258o
					@ sub_121C+3Er	...
CurrentFD:	.ds.b 2			@ DATA XREF: DrawScreen+14o
					@ DrawScreen+18r ...
					@ Snapshot of fire duration
		.ds.b 1
		.ds.b 1
byte_20000098:	.ds.b 1			@ DATA XREF: GetAtoTemp+2o
					@ GetAtoTemp+38o ...
		.ds.b 1
AtoTemp:	.ds.b 2			@ DATA XREF: GetAtoTemp+80w
					@ GetAtoTemp:loc_2FD6w	...
AtoCurrent:	.ds.b 2			@ DATA XREF: GetAtoCurrent+3Cw
					@ GetAtoCurrent+70r ...
TCR:		.ds.b 2			@ DATA XREF: GetAtoTemp+4Ar
					@ GetAtoTemp+66r ...
ADCShuntSum:	.ds.b 4			@ DATA XREF: ReadAtoTemp+36w
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
ADCAtoSum:	.ds.b 4			@ DATA XREF: ReadAtoTemp+30w
AtoRezMilli:	.ds.b 4			@ DATA XREF: GetAtoTemp+4r
					@ GetAtoTemp+3Er ...
					@ milliOhms
SleepTimer:	.ds.b 2			@ DATA XREF: DrawScreen+164o
					@ DrawScreen+200w ...
AtoError:	.ds.b 1			@ DATA XREF: UpdateDataFlash+Co
					@ UpdateDataFlash+14r ...
					@ 0,1,2,3 = Ok,Short,Low,Open
byte_200000B3:	.ds.b 1			@ DATA XREF: EventHandler+250o
					@ EventHandler+252r ...
AtoRez:		.ds.b 2			@ DATA XREF: sub_121C+Ao sub_121C+Cr ...
word_200000B6:	.ds.b 2			@ DATA XREF: sub_6038+128r
					@ sub_6038+142r ...
word_200000B8:	.ds.b 2			@ DATA XREF: CheckMode+A0o
					@ CheckMode+A2r ...
word_200000BA:	.ds.b 2			@ DATA XREF: CheckMode+AAo
					@ CheckMode+ACr ...
word_200000BC:	.ds.b 2			@ DATA XREF: CheckMode+B4o
					@ CheckMode+B6r ...
word_200000BE:	.ds.b 2			@ DATA XREF: CheckMode+C4o
					@ CheckMode+C6r ...
word_200000C0:	.ds.b 2			@ DATA XREF: EventHandler+260o
					@ EventHandler+262w ...
byte_200000C2:	.ds.b 1			@ DATA XREF: CheckMode+32o
					@ CheckMode:loc_30D0r ...
		.ds.b 1
BBCNextMode:	.ds.b 1			@ DATA XREF: CheckBattery:loc_7BEo
					@ CheckBattery+78t ...
BBCMode:	.ds.b 1			@ DATA XREF: RegulateBuckBoost+5Cr
					@ RegulateBuckBoost+64w ...
BBCNumCmps:	.ds.b 1			@ DATA XREF: RegulateBuckBoost+D0r
					@ RegulateBuckBoost+D6w ...
		.ds.b 1
LowBatVolts:	.ds.b 2			@ DATA XREF: CheckBattery+78r
					@ StopFire+92o	...
		.ds.b 1
		.ds.b 1
AtoVoltsADC:	.ds.b 4			@ DATA XREF: RegulateBuckBoost+30w
AtoVolts:	.ds.b 4			@ DATA XREF: RegulateBuckBoost+36w
					@ RegulateBuckBoost+92r ...
					@ Voltage atomiseur
TMR0Counter2:	.ds.b 4			@ DATA XREF: ReachTargetVoltage+20w
					@ ReachTargetVoltage:loc_2F22r	...
					@ 1000Hz (1kHz)	counter, cap at	#2000
dword_200000D8:	.ds.b 4			@ DATA XREF: CheckBattery+82r
					@ CheckBattery+C0t ...
					@ 100 *	600 / pwr
TargetVolts:	.ds.b 4			@ DATA XREF: RegulateBuckBoost:loc_2D66r
					@ RegulateBuckBoost:loc_2DA0r ...
dword_200000E0:	.ds.b 4			@ DATA XREF: ROM:0000D820o
		.ds.b 4
		.ds.b 4
		.ds.b 4
dword_200000F0:	.ds.b 4			@ DATA XREF: sub_25B8+28o sub_25B8+4Cw ...
DisplayByteData:.ds.b 1			@ DATA XREF: DisplayWriteByteo
					@ DisplayWriteByte+6w ...
ScreenDuration:	.ds.b 1			@ DATA XREF: DrawScreen+4o
					@ DrawScreen+26w ...
Screen:		.ds.b 1			@ DATA XREF: NewBatteryVoltage+46o
					@ NewBatteryVoltage+48r ...
					@ Screen (200000F6)
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
byte_200000F7:	.ds.b 1			@ DATA XREF: TimedItems:loc_7E44o
					@ TimedItems+BAr ...
ShowWeakBatFlag:.ds.b 1			@ DATA XREF: CheckBattery+C6o
					@ CheckBattery+CCw ...
		.ds.b 1
		.ds.b 1
		.ds.b 1
unk_200000FC:	.ds.b 1			@ DATA XREF: PutTextf+8o ROM:off_89F4o
		.ds.b 1
		.ds.b 1
		.ds.b 1
byte_20000100:	.ds.b 1			@ DATA XREF: sub_81EC+4o
					@ sub_81EC:loc_823Co ...
		.ds.b 1
		.ds.b 1
		.ds.b 1
dword_20000104:	.ds.b 4			@ DATA XREF: sub_81EC+18r sub_81EC+2Er ...
dword_20000108:	.ds.b 4			@ DATA XREF: sub_81EC+6r sub_81EC+Er ...
dword_2000010C:	.ds.b 4			@ DATA XREF: sub_8270+1Ar sub_8270+20r ...
dword_20000110:	.ds.b 4			@ DATA XREF: sub_8270+4r sub_8270+26r	...
dword_20000114:	.ds.b 4			@ DATA XREF: sub_8270+6r sub_8270+2Er	...
dword_20000118:	.ds.b 4			@ DATA XREF: sub_81EC+62r sub_81EC+6Cr
dword_2000011C:	.ds.b 4			@ DATA XREF: sub_85B4:loc_85FCr
dword_20000120:	.ds.b 4			@ DATA XREF: sub_81EC+10r sub_81EC+22r ...
dword_20000124:	.ds.b 4			@ DATA XREF: sub_85B4:loc_862Er
dword_20000128:	.ds.b 4			@ DATA XREF: sub_8524:loc_856Ar
byte_2000012C:	.ds.b 1			@ DATA XREF: sub_8524:loc_8566r
		.ds.b 1
		.ds.b 1
		.ds.b 1
unk_20000130:	.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
unk_20000134:	.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
unk_20000138:	.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
dword_2000013C:	.ds.b 4			@ DATA XREF: sub_82B8+18r sub_846C+2w	...
byte_20000140:	.ds.b 1			@ DATA XREF: sub_81EC+54r sub_82B8+2o	...
unk_20000141:	.ds.b 1			@ DATA XREF: sub_81EC+58r sub_85B4+26r
byte_20000142:	.ds.b 1			@ DATA XREF: sub_82B8:loc_8318r
					@ sub_85B4+136r
byte_20000143:	.ds.b 1			@ DATA XREF: sub_82B8+Cr
byte_20000144:	.ds.b 1			@ DATA XREF: sub_85B4+14r sub_87EC+10w
		.ds.b 1
byte_20000146:	.ds.b 1			@ DATA XREF: sub_82B8+Ar
byte_20000147:	.ds.b 1			@ DATA XREF: sub_82B8+8r
byte_20000148:	.ds.b 1			@ DATA XREF: sub_504C+2o sub_504C+Cr ...
byte_20000149:	.ds.b 1			@ DATA XREF: sub_504C+8r
byte_2000014A:	.ds.b 1			@ DATA XREF: sub_854+2o sub_854+58w ...
byte_2000014B:	.ds.b 1			@ DATA XREF: sub_24F8+20r
					@ sub_24F8:loc_251Ew ...
byte_2000014C:	.ds.b 1			@ DATA XREF: sub_9B0+52w sub_A94+8r ...
byte_2000014D:	.ds.b 1			@ DATA XREF: sub_854+8r sub_854+36r ...
byte_2000014E:	.ds.b 1			@ DATA XREF: sub_9B0+50w sub_A94+Cw ...
		.ds.b 1
FlappyScore:	.ds.b 2			@ DATA XREF: sub_854+5Cw sub_9B0+8w ...
dword_20000152:	.ds.b 4			@ DATA XREF: sub_A94:loc_AC6r
					@ sub_A94:loc_B1Cr ...
word_20000156:	.ds.b 2			@ DATA XREF: sub_A94+36r sub_A94+44r ...
word_20000158:	.ds.b 2			@ DATA XREF: sub_A94+13Ar
byte_2000015A:	.ds.b 1			@ DATA XREF: sub_A94+146r
byte_2000015B:	.ds.b 1			@ DATA XREF: sub_A94+42r
dword_2000015C:	.ds.b 4			@ DATA XREF: sub_A94+50r sub_A94+7Cr ...
byte_20000160:	.ds.b 1			@ DATA XREF: sub_A94+56r
TimeoutMask:	.ds.b 1			@ DATA XREF: CreateTimeout+2o
					@ ROM:off_140Co ...
CurrentTimeout:	.ds.b 1			@ DATA XREF: sub_854+64o sub_854+66r ...
UsedTimeouts:	.ds.b 1			@ DATA XREF: CreateTimeout+8r
					@ CreateTimeout+14w ...
RNGSeed:	.ds.b 4			@ DATA XREF: Randomo Random+4r ...
dword_20000168:	.ds.b 4			@ DATA XREF: sub_8A90o	sub_8A90+2w ...
TMR0Counter:	.ds.b 4			@ DATA XREF: sub_9B0+5Ao ROM:off_A78o	...
TMR1Counter:	.ds.b 4			@ DATA XREF: sub_9B0+5Cr
					@ TMR1_IRQHandler+24r ...
TMR2Counter:	.ds.b 4			@ DATA XREF: TMR2_IRQHandler+26r
					@ TMR2_IRQHandler+2Aw ...
TMR3Counter:	.ds.b 4			@ DATA XREF: TMR3_IRQHandler+26r
					@ TMR3_IRQHandler+2Aw ...
byte_2000017C:	.ds.b 0x100		@ DATA XREF: InitDataFlash+22Ao
					@ ROM:off_1C2Co ...
@ int dfData[]
dfData:		.ds.b 4			@ DATA XREF: CheckBattery+4o
					@ CheckBattery+8t ...
					@ 0x2000027C
dfCRC:		.ds.b 4			@ DATA XREF: InitDataFlash+D0r
					@ InitDataFlash+F2r ...
dfHWVersion:	.ds.b 4			@ DATA XREF: InitDataFlash+E4r
					@ InitDataFlash+F8w ...
dfIsInitialized:.ds.b 1
dfBootFlag:	.ds.b 1
dfMode:		.ds.b 2			@ DATA XREF: CheckBattery+8r
					@ CheckBattery+36r ...
					@ 0 TEMP NI
					@ 1 TEMP TI
					@ 2 TEMP SS316
					@ 3 TEMP TCR
					@ 4 POWER
					@ 5 BYPASS
					@ 6 START
dfPower:	.ds.b 2			@ DATA XREF: CheckBattery+Er
					@ DFCheckValuesValidity+26r ...
dfTemp:		.ds.b 2			@ DATA XREF: TweakTargetVoltsTC+58r
					@ sub_3738+7Cr	...
dfTCPower:	.ds.b 2			@ DATA XREF: DFCheckValuesValidity+36r
					@ DFCheckValuesValidity:loc_1DEEw ...
dfVWVolts:	.ds.b 2			@ DATA XREF: DFCheckValuesValidity+12r
					@ DFCheckValuesValidity+22w ...
dfAPT:		.ds.b 1			@ DATA XREF: DFCheckValuesValidity:loc_1E2Er
					@ EventHandler+998r ...
					@ Amps,	Puffs or Time display
byte_20000295:	.ds.b 1			@ DATA XREF: CheckMode+64r
					@ CheckMode+6Cw
dfTempAlgo:	.ds.b 1			@ DATA XREF: GetAtoTemp+3Ar
					@ ModeChange+20w ...
dfIsCelsius:	.ds.b 1			@ DATA XREF: TweakTargetVoltsTC:loc_36ACr
					@ sub_3738+7Ar	...
dfResistance:	.ds.b 2			@ DATA XREF: GetAtoTemp+14r
					@ GetAtoTemp+3Cr ...
dfRezTI:	.ds.b 2			@ DATA XREF: EventHandler:loc_44A2w
					@ EventHandler:loc_4748r ...
dfRezNI:	.ds.b 2			@ DATA XREF: EventHandler:loc_44A6w
					@ EventHandler:loc_4750r ...
dfRezLockedTI:	.ds.b 1			@ DATA XREF: ModeChange+24r
					@ SwitchRezLock:loc_88E0r ...
dfRezLockedNI:	.ds.b 1			@ DATA XREF: ModeChange+3Cr
					@ SwitchRezLock:loc_88FEr ...
dfTiOn:		.ds.b 1			@ DATA XREF: EventHandler+880r
					@ EventHandler+886w ...
dfStealthOn:	.ds.b 1			@ DATA XREF: DrawScreen:loc_14C0r
					@ DrawScreen:loc_15F8r	...
byte_200002A2:	.ds.b 0x2A		@ DATA XREF: sub_388C+12o
					@ ROM:off_38ACo ...
byte_200002CC:	.ds.b 0x2A		@ DATA XREF: sub_38D8+12o
					@ ROM:off_38F8o
		.ds.b 1
		.ds.b 1
dfStatus:	.ds.b 4			@ DATA XREF: DrawScreen:loc_162Ar
					@ sub_3738+Cr ...
					@ Status bits
					@ 0  0x01  Switched off
					@ 1  0x02  Key locked
					@ 2  0x04  Flipped display
					@ 3  0x08  LOGO	Off
word_200002FC:	.ds.b 2			@ DATA XREF: UpdateDataFlash+8w
byte_200002FE:	.ds.b 1			@ DATA XREF: UpdateDataFlash+16w
		.ds.b 1
dfRezSS:	.ds.b 2			@ DATA XREF: EventHandler:loc_44B0w
					@ EventHandler:loc_4758r ...
dfRezLockedSS:	.ds.b 1			@ DATA XREF: ModeChange+5Cr
					@ SwitchRezLock:loc_891Cr ...
dfUIVersion:	.ds.b 1			@ DATA XREF: DFCheckValuesValidity:loc_1E12r
					@ DFCheckValuesValidity+70w ...
dfTCRIndex:	.ds.b 1			@ DATA XREF: EventHandler:loc_438Cr
					@ EventHandler+9B4w ...
		.ds.b 1
dfTCRM1:	.ds.b 2			@ DATA XREF: ShowTCRSet+7At
					@ ShowTCRSet+94t ...
dfTCRM2:	.ds.b 2
dfTCRM3:	.ds.b 2
dfRezTCR:	.ds.b 2			@ DATA XREF: EventHandler:loc_44B6w
					@ EventHandler:loc_4762r ...
dfRezLockedTCR:	.ds.b 1			@ DATA XREF: ModeChange+72r
					@ SwitchRezLock+20r ...
		.ds.b 1
@ unsigned __int8 dfLastTCMode[42]
dfLastTCMode:	.ds.b 2			@ DATA XREF: NextMode:loc_5B1Et
dfSavedCfgRez:	.ds.b 0x14
dfSavedCfgPwr:	.ds.b 0x14		@ DATA XREF: CheckBattery+58t
dfFlappyBest:	.ds.b 2			@ DATA XREF: sub_854+108r sub_A94+14r	...
dfFBDifficulty:	.ds.b 1			@ DATA XREF: sub_24F8+16r
					@ EventHandler+17Er ...
					@ Flappy Bird Speed
					@ 0 Easy    62.5Hz 0.016s
					@ 1 Normal  76.9Hz 0.013s
					@ 2 Hard   100.0Hz 0.010s
@ unsigned __int8 byte_2000033D[63]
byte_2000033D:	.ds.b 1
		.ds.b 0x3E
dword_2000037C:	.ds.b 4			@ DATA XREF: GetProductID+6o
					@ ROM:off_207Co
dfFWVersion:	.ds.b 4
		.ds.b 4
fmcCID:		.ds.b 4			@ DATA XREF: InitDataFlash+30w
fmcUNKaddr0:	.ds.b 4			@ DATA XREF: InitDataFlash+42w
fmcUNKaddr4:	.ds.b 4			@ DATA XREF: InitDataFlash+5Aw
fmcPIDaddr0:	.ds.b 4
fmcPIDaddr4:	.ds.b 4
fmcPIDaddr8:	.ds.b 4
fmcPIDaddr10:	.ds.b 4
fmcPIDaddr14:	.ds.b 4
fmcPIDaddr18:	.ds.b 4
fmcPIDaddr1C:	.ds.b 4
dfPuffCount:	.ds.b 4			@ DATA XREF: DFCheckValuesValidity+46r
					@ DFCheckValuesValidity:loc_1E06w ...
dfTimeCount:	.ds.b 4			@ DATA XREF: DFCheckValuesValidity+52r
					@ DFCheckValuesValidity+5Ew ...
dfProductID:	.ds.b 4
dfMaxHWVersion:	.ds.b 4
		.ds.b 0x6BC		@ 0x800+dfData-*
byte_20000A7C:	.ds.b 0x800		@ DATA XREF: sub_25B8:loc_25FCo
					@ sub_25B8+5Eo	...
dword_2000127C:	.ds.b 4			@ DATA XREF: sub_2788+4o sub_2788+9Co	...
unk_20001280:	.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
byte_20001289:	.ds.b 1			@ DATA XREF: sub_2788+C2r
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
dword_20001388:	.ds.b 4			@ DATA XREF: sub_2788+A6r
dword_2000138C:	.ds.b 4			@ DATA XREF: sub_2788+B0r
dword_20001390:	.ds.b 4			@ DATA XREF: sub_2788+BAr
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
		.ds.b 1
byte_20001A7C:	.ds.b 1			@ DATA XREF: sub_2788+12r
								@ sub_2788:loc_287Cw ...
byte_20001A7D:	.ds.b 1			@ DATA XREF: sub_5F74+14r
dword_20001A7E:	.ds.b 4			@ DATA XREF: sub_2788+1Ar
dword_20001A82:	.ds.b 4			@ DATA XREF: sub_2788+1Er
dword_20001A86:	.ds.b 4			@ DATA XREF: sub_2788+22r
								@ sub_2788+184w ...
dword_20001A8A:	.ds.b 4			@ DATA XREF: sub_5F74+2Er
ScreenBuffer:	.ds.b 0x400		@ DATA XREF: sub_1348o
								@ sub_1348:loc_134Ew ...
		.ds.b 2
@ _BYTE	TimeoutsTable[1048]
TimeoutsTable:	.ds.b 0x18		@ DATA XREF: CreateTimeout+12o
								@ ROM:off_1410o ...
Stack_Bottom:	.ds.b 0x400		@ Stack	space
Stack_Top:						@ DATA XREF: ROM:00000000o Startupo ...

@		.end
