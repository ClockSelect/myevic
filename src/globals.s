.global Reset_Handler

.global MemCpy
.global MemCpy2
.global MemClear
.global MemClear2
.global MemSet

.global PutTextf

.global sqrtul

.global DrawHexDigit
.global DrawHexLong

.global orgfbStartScreen

.global orgfbInitTimeouts
.global orgfbCreateTimeout
.global orgfbDeleteTimeout
.global orgfbSetTimeoutDelay
.global orgfbTickTimeouts
.global orgfbCallTimeouts
.global orgfbCLSBuf
.global orgfbDrawDeadBird
.global orgfbFillScreen
.global orgfbNumDigits
.global orgfbBirdAnim
.global orgfbDrawColumn
.global orgfbDrawChar
.global orgfbDrawNumber
.global orgfbNumDigits
.global orgfbDrawText
.global orgfbDrawSprite
.global orgfbDrawRect
.global orgfbPlot

.ifne 0
.global fbBirdColumn
.global fbBirdCycle
.global fbBirdLine
.global fbBirdDisp
.global fbDead
.global fbAnimStep
.global fbAnimTimer
.global fbScore
.global fbColumn1
.global fbColumn2
.global fbColumn3
.global fbTimeoutMask
.global fbCurrentTimeout
.global fbUsedTimeouts
.endif
.global fbColumnBody
.global fbColumnBottom
.global fbColumnTop
.global fbBird0
.global fbBird1
.global fbBird2
.global fbBirdDead
.global fbFont

.global UpdateDFTimer
.global BatRefreshTmr
.global BatReadTimer
.global BatAnimTimer

.global fbCurrentTimeout

.global Flags64
.global Flags68
.global SleepTimer

.global TMR0Counter
.global TMR1Counter
.global TMR2Counter
.global TMR3Counter

.global ADC00_IRQ_Flag

.global EditItemIndex
.global EditModeTimer
.global HideLogo
.global ShowWeakBatFlag
.global byte_20000080
.global word_20000054
.global byte_200000B3

.global UserInputs
.global LastInputs
.global FireClickTimer
.global FireClicksEvent
.global byte_20000048
.global FireClickCount
.global MenuPage

.global Screen
.global ScreenDuration

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

.global CurrentFD
.global FireDuration
.global ShowFDTimer

.global DisplayModel
.global ScreenBuffer
.global font1
.global font2
.global DisplayCmdByte

.global Event
.global LastEvent
.global KeyPressTime
.global KeyUpTimer

.global BatteryVoltsTable
.global STARTRezValues
.global STARTPowers
.global ConfigIndex
.global fmcCntrsIndex

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
.global dfbyte_2000033D
.global dfContrast
.global dfModesSel
.global dfClkRatio

.global dfFWVersion
.global dffmcCID
.global dffmcDID
.global dffmcPID
.global dffmcUID
.global dffmcUCID
.global dfPuffCount
.global dfTimeCount
.global dfProductID
.global dfMaxHWVersion

.global gPlayfield

