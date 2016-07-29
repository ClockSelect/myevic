.global Reset_Handler

.global orgInitDevices
.global orgInitUART0
.global orgInitGPIO
.global orgInitPWM
.global orgInitUSB

.global orgusbdEP2Handler
.global orgusbdEP3Handler

.global hidCmd
.global hidData
.global hidDFData
.global hidDataIndex

.global UpdateDFTimer
.global BatRefreshTmr
.global BatReadTimer
.global BatAnimTimer

.global MemCpy
.global MemCpy2
.global MemClear
.global MemClear2
.global GotoMain
.global PutTextf

.global mysqrtul

.global CurrentTimeout

.global Flags64
.global Flags68
.global SleepTimer

.global TMR0Counter
.global TMR1Counter
.global TMR2Counter
.global TMR3Counter

.global ADC00_IRQ_Flag

.global orgADC_Read
.global orgSetADCState
.global orgBBC_Configure
.global orgDevicesOnOff
.global orgFlushAndSleep
.global orgInitSPI0

.global orgWaitOnTMR2
.global orgCheckBattery
.global orgBatteryVoltsToPercent

.global EditItemIndex
.global EditModeTimer
.global HideLogo
.global ShowWeakBatFlag
.global byte_200000B3
.global byte_20000080
.global word_20000054

.global UserInputs
.global LastInputs
.global FireClickTimer
.global FireClicksEvent
.global byte_20000048
.global FireClickCount
.global MenuPage

.global Screen
.global ScreenDuration

.global orgMainView
.global orgDrawScreen
.global orgShowKeyLock
.global orgShowVersion
.global orgShowTCRSet
.global orgShowLOGOMenu
.global orgShowGameMenu
.global orgShowNewCoil
.global orgShowBattVolts
.global orgShowBoardTemp
.global orgShowStealthMode
.global orgShowTiOnOff
.global orgShowDevTooHot
.global orgShowKeyUnLock
.global orgShowNoAtoFound
.global orgShowAtoShort
.global orgShowAtoLow
.global orgShow10sProtec
.global orgShowBatLow
.global orgShowBatLowLock
.global orgShowMainView
.global orgShowMainFiring
.global orgShowBatCharging
.global orgShowBattery
.global orgShowMainFiring
.global orgShowWeakBat

.global AtoCurrent
.global AtoRez
.global AtoRezMilli
.global AtoVolts
.global AtoVoltsADC
.global AtoMinVolts
.global AtoMaxVolts
.global AtoTemp
.global AtoMinPower
.global AtoMaxPower
.global TargetVolts
.global MaxPower
.global MaxVWVolts
.global MaxTCPower
.global PowerScale
.global BoardTemp
.global LowBatVolts
.global BatteryVoltage
.global BatteryPercent
.global SavedBatPercent
.global BatPCCmpCnt
.global SavedBatVoltage
.global BatteryTenth
.global VbatSampleSum
.global VbatSampleCnt
.global BuckDuty
.global BoostDuty
.global BBCNextMode
.global BBCMode
.global BBCNumCmps
.global TMR0Counter2
.global AtoProbeCount
.global AtoStatus
.global AtoError
.global byte_20000082
.global word_200000B6
.global word_200000B8
.global word_200000BA
.global word_200000BC
.global word_200000BE
.global word_200000C0
.global TCR
.global dfRezType
.global AtoShuntValue
.global ADCAtoSum
.global ADCShuntSum
.global dword_200000A4
.global byte_20000098
.global CheckModeCounter
.global BTempSampleCnt
.global BTempSampleSum

.global UpdatePTTimer
.global BatAnimLevel
.global EditTCRIndex

.global orgSwitchRezLock
.global orgGetTempCoef
.global orgClampPower
.global orgClampPowers
.global orgClampAtoVolts
.global orgAtoPower
.global orgFarenheitToC
.global orgCelsiusToF
.global orgGetVoltsForPower
.global orgRegulateBuckBoost
.global orgGetAtoCurrent
.global orgReadBoardTemp
.global orgReadAtoTemp
.global orgCheckMode
.global orgOvertemp
.global orgProbeAtomizer
.global orgSetAtoLimits
.global orgSetMinMaxVolts
.global orgSetMinMaxPower
.global orgSetAtoSTARTParams
.global orgGetAtoVWVolts
.global orgLowestRezMeasure
.global orgGetAtoTemp
.global orgStopFire
.global orgAtoPowerLimit

.global orgInitHardware
.global orgInitVariables
.global orgUpdateFlash

.global orgEventHandler

.global CurrentFD
.global FireDuration
.global ShowFDTimer

.global DisplayModel
.global ScreenBuffer
.global font1
.global font2
.global DisplayCmdByte

.global orgScreenOff
.global orgClearScreenBuffer
.global orgDisplayRefresh
.global orgDisplaySendCommand
.global orgDisplaySendData

.global orgDrawString
.global orgDrawStringInv
.global orgDrawStringCentered
.global orgDrawImage
.global orgDrawImageInv
.global orgDrawFillRect
.global orgDrawHLine
.global orgDrawLOGO
.global orgDrawValue
.global orgDrawValueInv
.global orgDrawBFLine

.global	orgResetWatchDog
.global	orgTimedItems
.global	orgSleepIfIdle
.global	orgGetUserInput

.global InitTimeouts
.global CreateTimeout
.global DeleteTimeout
.global CallTimeouts
.global TickFBTimeouts
.global fbStartScreen
.global fbBirdAnim

.global Event
.global LastEvent
.global KeyPressTime
.global KeyUpTimer

.global DrawHexDigit
.global DrawHexLong

.global orgUpdateDataFlash
.global orgDFCheckValuesValidity
.global orgFMCWriteCounters
.global orgCpyTmpCoefsNI
.global orgCpyTmpCoefsTI

.global orgSearchSTARTRez

.global BatteryVoltsTable
.global STARTRezValues
.global STARTPowers
.global ConfigIndex
.global fmcCntrsIndex

.global String_ON
.global String_No
.global String_On
.global String_Key
.global String_OFF
.global String_Low
.global String_Off
.global String_Lock
.global String_UnLock
.global String_Protec
.global String_Version
.global String_Device
.global String_TooHot
.global String_Stealth
.global String_TiON
.global String_TiOFF
.global String_Temp
.global String_Battery
.global String_Atomizer
.global String_Found
.global String_Short
.global String_Over10s
.global String_NewCoil
.global String_SameCoil
.global String_Right
.global String_Left
.global String_LOGO
.global String_Game
.global String_Easy
.global String_Normal
.global String_Hard
.global String_Exit
.global String_NI
.global String_TI
.global String_SS
.global String_BF_s
.global String_TCR
.global String_PWR_s
.global String_AMP_s
.global String_MAX_s
.global String_MIN_s
.global String_Protec
.global String_Weak
.global String_TEMP
.global String_Battery
.global String_TCRSet
.global String_POWER
.global String_BYPASS
.global String_VOLT_s
.global String_COIL_s
.global String_TIME_s
.global String_PUFF_s
.global String_START

.global SavedDF

.global dfData
.global dfCRC
.global dfHWVersion
.global dfMagic
.global dfBootFlag
.global dfMode
.global dfPower
.global dfTemp
.global dfTCPower
.global dfVWVolts
.global dfAPT
.global dfTempAlgo
.global dfIsCelsius
.global dfResistance
.global dfRezTI
.global dfRezNI
.global dfRezLockedTI
.global dfRezLockedNI
.global dfTiOn
.global dfStealthOn
.global dfTempCoefsNI
.global dfTempCoefsTI
.global dfStatus
.global dfAtoRez
.global dfAtoStatus
.global dfRezSS
.global dfRezLockedSS
.global dfUIVersion
.global dfTCRIndex
.global dfTCRM1
.global dfTCRM2
.global dfTCRM3
.global dfRezTCR
.global dfRezLockedTCR
.global dfLastTCMode
.global dfSavedCfgRez
.global dfSavedCfgPwr
.global dfFBBest
.global dfFBSpeed
.global byte_2000033D
.global dfContrast
.global dfModesSel
.global dfClkRatio

.global dfFWVersion
.global fmcCID
.global fmcDID
.global fmcPID
.global fmcUID
.global fmcUCID
.global dfPuffCount
.global dfTimeCount
.global dfProductID
.global dfMaxHWVersion

.global gPlayfield

