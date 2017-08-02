#include "myevic.h"
#include "atomizer.h"
#include "display.h"
#include "battery.h"
#include "screens.h"
#include "events.h"
#include "myrtc.h"
#include "timers.h"
#include "dataflash.h"
#include "meadc.h"
#include "megpio.h"
#include "meusbd.h"
#include "miscs.h"
#include "flappy.h"


//=========================================================================
// Globals

volatile gFlags_t gFlags;
uint8_t BoxModel;


//=========================================================================
// Additional initialisations
//-------------------------------------------------------------------------
__myevic__ void CustomStartup()
{

//-------------------------------------------------------------------------
// EADC test

	if ( 0 )
	{
		uint32_t s1, s2, s3;

		SetADCState( 0, 1 );
		SetADCState( 4, 1 );

		do
		{
			ClearScreenBuffer();

			CLK_SysTickDelay( 10 );
			s3 = ADC_Read( 4 );

			CLK_SysTickDelay( 10 );
			s1 = ADC_Read( 18 );

			CLK_SysTickDelay( 10 );
			s2 = ADC_Read( 0 );

			DrawValue( 8,  0, s1, 0, 0x29, 4 );
			DrawValue( 8, 20, s2, 0, 0x29, 4 );
			DrawValue( 8, 40, s3, 0, 0x29, 4 );

			DisplayRefresh();

			WaitOnTMR2( 1000 );
		}
		while ( PD3 );
	}


//-------------------------------------------------------------------------
// Timer test 1

	if ( 0 )
	{
		TIMER_Stop( TIMER3 );
		TIMER_Close( TIMER3 );

		MemClear( gPlayfield.uc, 256 );

		CLK_SetModuleClock( TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_LIRC, 0 );

		gPlayfield.ul[1] =
		TIMER_Open( TIMER3, TIMER_PERIODIC_MODE, 10 );
		TIMER_EnableInt( TIMER3 );
		TIMER_Start( TIMER3 );
	}


//-------------------------------------------------------------------------
// Timer test 2

	if ( 0 )
	{
		TIMER_Close( TIMER2 );
		TIMER_Close( TIMER3 );

		MemClear( gPlayfield.uc, 256 );

		CLK_SetModuleClock( TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0 );
		CLK_SetModuleClock( TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_LIRC, 0 );

		CLK_EnableModuleClock( TMR2_MODULE );
		CLK_EnableModuleClock( TMR3_MODULE );

		__set_PRIMASK(1);

		TIMER3->CTL |= TIMER_CTL_RSTCNT_Msk;
		TIMER2->CTL |= TIMER_CTL_RSTCNT_Msk;

		TIMER3->CMP  = 1000;

		TIMER3->CTL  = TIMER_CTL_CNTEN_Msk | TIMER_ONESHOT_MODE;
		TIMER2->CTL  = TIMER_CTL_CNTEN_Msk | TIMER_CONTINUOUS_MODE;
		while(!(TIMER3->INTSTS & TIMER_INTSTS_TIF_Msk));
		TIMER2->CTL = 0;

		gPlayfield.ul[0] = TIMER2->CNT;

		__set_PRIMASK(0);

		TIMER_Close( TIMER2 );
		TIMER_Close( TIMER3 );

		CLK_SetModuleClock( TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HIRC, 0 );
		CLK_EnableModuleClock( TMR2_MODULE );
		TIMER_Open( TIMER2, TIMER_PERIODIC_MODE, 1000 );
		TIMER_EnableInt( TIMER2 );
		TIMER_Start( TIMER2 );

		CLK_SetModuleClock( TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0 );
		CLK_EnableModuleClock( TMR3_MODULE );
		TIMER_Open( TIMER3, TIMER_PERIODIC_MODE, 10 );
		TIMER_EnableInt( TIMER3 );
		TIMER_Start( TIMER3 );
	}

	return;
}


//=========================================================================
//----- (0000652C) --------------------------------------------------------
void InitDevices()
{
	SYS_UnlockReg();

	// Internal 22.1184MHz oscillator
	CLK_EnableXtalRC( CLK_PWRCTL_HIRCEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HIRCSTB_Msk );
	CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK( 1 ) );

	// 12.000MHz external crystal
	CLK_EnableXtalRC( CLK_PWRCTL_HXTEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk );

	// FMC Frequency Optimisation mode <= 72MHz
	FMC_EnableFreqOptimizeMode( FMC_FTCTL_OPTIMIZE_72MHZ );

	// Setup PLL to 144MHz and HCLK source to PLL/2
	CLK_SetCoreClock( CPU_FREQ );

	// UART0 CLK = HXT/1
	#if (ENABLE_UART)
	CLK_EnableModuleClock( UART0_MODULE );
	CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UARTSEL_HXT, CLK_CLKDIV0_UART( 1 ) );
	#endif

	// USB CLK = PLL/3 (48MHz)
	CLK_EnableModuleClock( USBD_MODULE );
	CLK_SetModuleClock( USBD_MODULE, 0, CLK_CLKDIV0_USB( 3 ) );
	SYS->USBPHY = SYS_USBPHY_LDO33EN_Msk;

	// WDT CLK = LIRC/1
	CLK_EnableModuleClock( WDT_MODULE );
	CLK_SetModuleClock( WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0 );

	// SPI0 CLK = PCLK0/1
	CLK_EnableModuleClock( SPI0_MODULE );

	// EADC CLK = PCLK1/8 (9MHz)
	CLK_EnableModuleClock( EADC_MODULE );
	CLK_SetModuleClock( EADC_MODULE, 0, CLK_CLKDIV0_EADC( 8 ) );

	// CRC CLK = HCLK/1
	CLK_EnableModuleClock( CRC_MODULE );

	// TIMERS CLOCKS
	CLK_EnableModuleClock( TMR0_MODULE );
	CLK_EnableModuleClock( TMR1_MODULE );
	CLK_EnableModuleClock( TMR2_MODULE );
	CLK_EnableModuleClock( TMR3_MODULE );
	CLK_SetModuleClock( TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0 );
	CLK_SetModuleClock( TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0 );
	CLK_SetModuleClock( TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HIRC, 0 );
	CLK_SetModuleClock( TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0 );

	// Enable battery voltage sampling by ADC
	SYS->IVSCTL |= SYS_IVSCTL_VBATUGEN_Msk;

	// ADC reference voltage
	SYS->VREFCTL = SYS_VREFCTL_VREF_2_56V;

	// Brown-out detector; interrupts under 2.2V
	SYS_EnableBOD( SYS_BODCTL_BOD_RST_EN, SYS_BODCTL_BODVL_2_2V );

	// Update clock data
	SystemCoreClockUpdate();

	WDT_Open( WDT_TIMEOUT_2POW18, WDT_RESET_DELAY_18CLK, TRUE, FALSE );

	SYS_LockReg();
}


//=========================================================================
//----- (00000C48) --------------------------------------------------------
__myevic__ void InitHardware()
{
	SYS_UnlockReg();

	//  32.768kHz external crystal
	if ( dfStatus.x32off )
	{
		CLK_DisableXtalRC( CLK_PWRCTL_LXTEN_Msk );
	}
	else
	{
		SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF0MFP_Msk|SYS_GPF_MFPL_PF1MFP_Msk);
		SYS->GPF_MFPL |=  (SYS_GPF_MFPL_PF0MFP_X32_OUT|SYS_GPF_MFPL_PF1MFP_X32_IN);

		CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
		CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk );
	}

	SetPWMClock();

	SYS_LockReg();

	#if (ENABLE_UART)
	InitUART0();
	#endif

	InitGPIO();

	if ( !PD3 )
	{
		gFlags.noclock = 1;
	}

	InitSPI0();
	InitEADC();
	InitPWM();
	InitTimers();
	InitUSB();
}


//=========================================================================
// Real-Time Clock
//-------------------------------------------------------------------------
__myevic__ void InitRTC()
{
	if ( !gFlags.noclock )
	{
		gFlags.has_x32 = !dfStatus.x32off;
		RTCStart( 0 );
	}

	if ( gFlags.has_x32 )
	{
		// Disable Light Sleep mode.
		dfStatus.lsloff = 1;
		UpdateDFTimer = 1;
	}
	else if ( !dfStatus.x32off )
	{
		// Disable X32
		dfStatus.x32off = 1;
		// Enable Light Sleep mode.
		dfStatus.lsloff = 0;
		UpdateDFTimer = 1;
	}

	gFlags.rtcinit = 1;

	time_t vvbase;

	vvbase = RTCReadRegister( RTCSPARE_VV_BASE );

	if ( ( vvbase == 0 ) || ( vvbase % 86400 ) )
	{
		vvbase = RTCGetEpoch( 0 );
		vvbase -= vvbase % 86400;
		RTCWriteRegister( RTCSPARE_VV_BASE, vvbase );
		RTCWriteRegister( RTCSPARE_VV_MJOULES, 0 );
	}
	else
	{
		MilliJoules = RTCReadRegister( RTCSPARE_VV_MJOULES );
	}
}


//=========================================================================
//----- (0000895C) --------------------------------------------------------
__myevic__ void InitVariables()
{
	InitDataFlash();
	LEDGetColor();
	KeyPressTime |= 0x8000;
	LastInputs |= 0x80;
	byte_200000B3 = 1;
	gFlags.draw_edited_item = 1;
	gFlags.refresh_battery = 1;
	gFlags.read_battery = 1;
	gFlags.read_bir = 1;
	WattsInc = dfStatus.onewatt ? 10 : 1;
	RoundPowers();
	AtoMinVolts = 50;
	AtoMaxVolts = MaxVolts;
	AtoMinPower = 10;
	AtoMaxPower = MaxPower;
	Object3D = 1;
	AtoTemp = 70;
}


//=========================================================================
// BSOD

__myevic__ void Plantouille( int xpsr, int* stack )
{
	int i, k;

	k = 0;

	SYS_UnlockReg();
	WDT_Close();
	SYS_LockReg();

	InitDisplay();

	while ( 1 )
	{
		ClearScreenBuffer();

		DrawImage( 0, 0, 'X'+0x27 );
		DrawHexLong( 16, 0, xpsr, 0 );

		DrawHexDigit( 0, 16, k );

		for ( i = 0; i < 14 ; ++i )
		{
			DrawHexLong( 16, 16+i*8, stack[i+k*14], 0 );
		}

		DisplayRefresh();

		while ( !PE0 || !PD2 || !PD3 )
			CLK_SysTickDelay( 10000 );

		while ( PE0 && PD2 && PD3 )
			CLK_SysTickDelay( 10000 );

		if ( !PE0 )
		{
		  SYS_UnlockReg();
		  SYS_ResetChip();
		  while ( 1 )
			;
		}

		if ( !PD2 ) ++k;
		if ( !PD3 ) --k;

		if ( k < 0 ) k = 0;
		else if ( k > 15 ) k = 15;
	}
}


//=========================================================================
//----- (00005D24) --------------------------------------------------------
__myevic__ void DevicesOnOff( int off )
{
	if ( off )
	{
		TIMER_DisableInt( TIMER0 );
		TIMER_DisableInt( TIMER1 );
		TIMER_DisableInt( TIMER2 );

		if ( !gFlags.light_sleep )
		{
			TIMER_DisableInt( TIMER3 );
		}

		EADC_Close( EADC );
		SetADCState( 1, 0 );
		SetADCState( 2, 0 );
		SetADCState( 14, 0 );

		if ( ISVTCDUAL || ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 || ISPRIMO1 || ISPRIMO2 || ISPREDATOR )
		{
			SetADCState( 3, 0 );
			SetADCState( 13, 0 );

			if ( ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
			{
				SetADCState( 15, 0 );
			}

			PD7 = 0;
			BBC_Configure( BBC_PWMCH_CHARGER, 0 );
			PD7 = 0;

			if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 )
			{
				PF2 = 0;
			}
		}

		PC1 = 0;
		PC0 = 0;
		BBC_Configure( BBC_PWMCH_BUCK, 0 );
		if ( !ISVTCDUAL ) PC3 = 0;
		PC2 = 0;
		BBC_Configure( BBC_PWMCH_BOOST, 0 );

		if ( ISCUBO200 || ISRX200S || ISRX23 )
		{
			PF1 = 0;
		}
		else if ( ISRX300 )
		{
			PD1 = 0;
		}
		else
		{
			PB7 = 0;
		}

		GPIO_DisableInt( PD, 0 );
		PD0 = 0;
		GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_OUTPUT );

		if ( ISPRIMO1 || ISPRIMO2 || ISPREDATOR || ISRX300 )
		{
			PA3 = 0;
			PA2 = 0;

			if ( ISRX300 )
			{
				PF5 = 0;
				PF6 = 0;
			}
		}

		if ( ISVTCDUAL )
		{
			GPIO_DisableInt( PD, 1 );
			PD1 = 0;
			GPIO_SetMode( PD, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
		}
		else if ( !ISCUBOID && !ISCUBO200 && !ISRX200S && !ISRX23 && !ISRX300 && !ISPRIMO1 && !ISPRIMO2 && !ISPREDATOR )
		{
			GPIO_DisableInt( PD, 7 );
			PD7 = 0;
			GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );
		}

		SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE11MFP_Msk|SYS_GPE_MFPH_PE12MFP_Msk|SYS_GPE_MFPH_PE13MFP_Msk);
		SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE11MFP_GPIO|SYS_GPE_MFPH_PE12MFP_GPIO|SYS_GPE_MFPH_PE13MFP_GPIO);
		PE11 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN11_Msk, GPIO_MODE_OUTPUT );
		PE12 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN12_Msk, GPIO_MODE_OUTPUT );
		PE13 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN13_Msk, GPIO_MODE_OUTPUT );
		PE10 = 0;

		GPIO_EnableInt( PE, 0, GPIO_INT_BOTH_EDGE );
		GPIO_EnableInt( PD, 2, GPIO_INT_BOTH_EDGE );
		GPIO_EnableInt( PD, 3, GPIO_INT_BOTH_EDGE );

		if ( ISVTCDUAL )
		{
			PA3 = 0;
			PC3 = 0;
			PF2 = 0;
			PA2 = 0;
		}
		else if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
		{
			PF0 = 0;
		}
		else if ( ISPRIMO1 || ISPRIMO2 || ISPREDATOR )
		{
			PD1 = 0;
		}

		SYS_UnlockReg();
		SYS->USBPHY &= ~SYS_USBPHY_LDO33EN_Msk;
		SYS->IVSCTL &= ~(SYS_IVSCTL_VBATUGEN_Msk|SYS_IVSCTL_VTEMPEN_Msk);
		SYS_DisableBOD();
		SYS->VREFCTL = 0;
		SYS_LockReg();

		USBD_CLR_INT_FLAG( USBD_INTSTS_WAKEUP|USBD_INTSTS_FLDET|USBD_INTSTS_BUS|USBD_INTSTS_USB );
		USBD_ENABLE_INT( USBD_INT_WAKEUP );
	}
	else
	{
		USBD_CLR_INT_FLAG( USBD_INTSTS_WAKEUP );

		SYS_UnlockReg();
		SYS->USBPHY |= SYS_USBPHY_LDO33EN_Msk;
		SYS->IVSCTL |= SYS_IVSCTL_VBATUGEN_Msk;
		if ( ISRX300 )
		{
			SYS->IVSCTL |= SYS_IVSCTL_VTEMPEN_Msk;
		}
		SYS->VREFCTL = SYS_VREFCTL_VREF_2_56V;
		SYS_EnableBOD( SYS_BODCTL_BOD_RST_EN, SYS_BODCTL_BODVL_2_2V );
		SYS_LockReg();

		GPIO_DisableInt( PE, 0 );
		GPIO_DisableInt( PD, 2 );
		GPIO_DisableInt( PD, 3 );

		if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 )
		{
			PF2 = 1;
		}

		SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE11MFP_Msk|SYS_GPE_MFPH_PE12MFP_Msk|SYS_GPE_MFPH_PE13MFP_Msk);
		SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE11MFP_SPI0_MOSI0|SYS_GPE_MFPH_PE12MFP_SPI0_SS|SYS_GPE_MFPH_PE13MFP_SPI0_CLK);

		GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
		GPIO_EnableInt( PD, 0, GPIO_INT_FALLING );

		if ( ISVTCDUAL )
		{
			GPIO_SetMode( PD, GPIO_PIN_PIN1_Msk, GPIO_MODE_INPUT );
			GPIO_EnableInt( PD, 1, GPIO_INT_RISING );
			GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN1_Msk );
		}
		else if ( !ISCUBOID && !ISCUBO200 && !ISRX200S && !ISRX23 && !ISRX300 && !ISPRIMO1 && !ISPRIMO2 && !ISPREDATOR )
		{
			GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_INPUT );
			GPIO_EnableInt( PD, 7, GPIO_INT_RISING );
			GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN7_Msk );
		}

		if ( ISCUBO200 || ISRX200S || ISRX23 )
		{
			PF1 = 1;
		}
		else if ( ISRX300 )
		{
			PD1 = 1;
		}
		else
		{
			PB7 = 1;
		}

		SetADCState( 1, 1 );
		SetADCState( 2, 1 );
		SetADCState( 14, 1 );

		if ( ISVTCDUAL || ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 || ISPRIMO1 || ISPRIMO2 || ISPREDATOR )
		{
			SetADCState( 3, 1 );
			SetADCState( 13, 1 );

			if ( ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
			{
				SetADCState( 15, 1 );
			}
		}

		TIMER_EnableInt( TIMER0 );
		TIMER_EnableInt( TIMER1 );
		TIMER_EnableInt( TIMER2 );
		TIMER_EnableInt( TIMER3 );
	}
}


//=========================================================================
__myevic__ void LightSleep()
{
	// Switch Core Clock to HXT/3 (4MHz)
	CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK( 3 ) );

	// Update clock data
	SystemCoreClockUpdate();

	// Switch off the PLL Clock & the HIRC
	CLK_DisablePLL();
	CLK_DisableXtalRC( CLK_PWRCTL_HIRCEN_Msk );

	// Disable Clocks of Modules using HCLK/HXT or LIRC
	CLK_DisableModuleClock( PWM0_MODULE );
	CLK_DisableModuleClock( SPI0_MODULE );
	CLK_DisableModuleClock( CRC_MODULE );
	#if (ENABLE_UART)
	CLK_DisableModuleClock( UART0_MODULE );
	#endif

	gFlags.wake_up = 0;

	do
	{
		CLK_Idle();
	}
	while ( !gFlags.wake_up );

	// Wake up the HIRC
	CLK_EnableXtalRC( CLK_PWRCTL_HIRCEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HIRCSTB_Msk );

	// Wake up the PLL
	CLK_SetCoreClock( CPU_FREQ );

	// Update clock data
	SystemCoreClockUpdate();

	// Wake up Modules
	#if (ENABLE_UART)
	CLK_EnableModuleClock( UART0_MODULE );
	#endif
	CLK_EnableModuleClock( PWM0_MODULE );
	CLK_EnableModuleClock( SPI0_MODULE );
	CLK_EnableModuleClock( CRC_MODULE );
}


//=========================================================================
//----- (00005D14) --------------------------------------------------------
__myevic__ void FlushAndSleep()
{
	#if (ENABLE_UART)
		UART_WAIT_TX_EMPTY( UART0 );
	#endif

	if ( !gFlags.light_sleep )
	{
		CLK_PowerDown();
	}
	else
	{
		LightSleep();
	}
}


//=========================================================================
//----- (00004F0C) --------------------------------------------------------

void GoToSleep()
{
	gFlags.light_sleep = !( gFlags.has_x32 || dfStatus.lsloff || gFlags.noclock );

	ScreenOff();
	LEDOff();
	gFlags.firing = 0;
	BatReadTimer = 50;
	RTCSleep();
	DevicesOnOff( 1 );
	CLK_SysTickDelay( 250 );
	CLK_SysTickDelay( 250 );
	CLK_SysTickDelay( 250 );
	if ( dfStatus.off || PE0 || KeyPressTime == 1100 )
	{
		SYS_UnlockReg();
		WDT_Close();
		FlushAndSleep();
		PreheatDelay = 0;
	}
	WDT_Open( WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, TRUE, FALSE );
	SYS_LockReg();
	gFlags.refresh_battery = 1;
	DevicesOnOff( 0 );
	RTCWakeUp();
	InitDisplay();
}


//=========================================================================
//----- (0000782C) --------------------------------------------------------
__myevic__ void SleepIfIdle()
{
	if ( !gFlags.firing && !NoEventTimer )
	{
		if ( ( Screen == 0 ) && ( SleepTimer == 0 ) && ( gFlags.user_idle ) )
		{
			GoToSleep();

			byte_200000B3 = 2;
			AtoProbeCount = 0;
			AtoRezMilli = 0;
			gFlags.sample_vbat = 1;
			ReadBatteryVoltage();
			if (( BatteryVoltage <= BatteryCutOff + 20 ) && !gFlags.usb_attached )
			{
				dfStatus.off = 1;
				Screen = 0;
			}
			gFlags.sample_btemp = 1;
		}
		NoEventTimer = 200;
	}
}


//=========================================================================
// Monitoring
//-------------------------------------------------------------------------
__myevic__ void Monitor()
{
	if ( gFlags.firing )
	{
		myprintf( "FIRING "
					"RESM=%d BATT=%d VOUT=%d CUR=%d",
					AtoRezMilli,
					RTBattVolts,
					AtoVolts,
					AtoCurrent
				);

		if ( ISMODETC(dfMode) )
		{
			myprintf( " SPWR=%d RPWR=%d CELS=%d STEMP=%d RTEMP=%d\n",
						dfTCPower,
						AtoPower( AtoVolts ),
						dfIsCelsius ? 1 : 0,
						dfTemp,
						dfIsCelsius ? FarenheitToC( AtoTemp ) : AtoTemp
					);
		}
		else if ( ISMODEBY(dfMode) )
		{
			myprintf( " RPWR=%d\n",
						AtoPower( AtoVolts )
					);
		}
		else
		{
			myprintf( " SPWR=%d RPWR=%d\n",
						dfPower,
						AtoPower( AtoVolts )
					);
		}
	}
	else
	{
		myprintf( "STANDBY "
					"BATT=%d CHG=%d BRD=%d ATO=%d "
					"RES=%d RESM=%d MODE=%d",
					BatteryVoltage,
					gFlags.battery_charging ? 1 : 0,
					dfIsCelsius ? BoardTemp : CelsiusToF( BoardTemp ),
					AtoStatus,
					AtoRez,
					AtoRezMilli,
					dfMode
				);

		if ( ISMODETC(dfMode) )
		{
			ReadAtoTemp();
			myprintf( " SPWR=%d CELS=%d STEMP=%d RTEMP=%d\n",
						dfTCPower,
						dfIsCelsius ? 1 : 0,
						dfTemp,
						dfIsCelsius ? FarenheitToC( AtoTemp ) : AtoTemp
					);
		}
		else if ( ISMODEBY(dfMode) )
		{
			myprintf( "\n" );
		}
		else
		{
			myprintf( " SPWR=%d\n",
						dfPower
					);
		}
	}
}


//=========================================================================
//----- (00000148) --------------------------------------------------------
__myevic__ void Main()
{
	InitDevices();

	InitVariables();

	// Enable chip temp sensor sampling by ADC
	if ( ISRX300 )
	{
		SYS->IVSCTL |= SYS_IVSCTL_VTEMPEN_Msk;
	}

	InitHardware();

	myprintf( "\n\nJoyetech APROM\n" );
	myprintf( "CPU @ %dHz(PLL@ %dHz)\n", SystemCoreClock, PllClock );

	SetBatteryModel();

	gFlags.sample_vbat = 1;
	ReadBatteryVoltage();

	gFlags.sample_btemp = 1;
	ReadBoardTemp();

	InitDisplay();
	MainView();
	SplashTimer = 3;

	CustomStartup();

	if ( !PD3 )
	{
		DrawScreen();
		while ( !PD3 )
			;
	}

	while ( 1 )
	{
		while ( gFlags.playing_fb )
		{
			// Flappy Bird game loop
			fbCallTimeouts();
			if ( gFlags.tick_100hz )
			{
				// 100Hz
				gFlags.tick_100hz = 0;
				ResetWatchDog();
				TimedItems();
				SleepIfIdle();
				GetUserInput();
				if ( !PE0 )
					SleepTimer = 3000;
			}
			if ( gFlags.tick_10hz )
			{
				// 10Hz
				gFlags.tick_10hz = 0;
				DataFlashUpdateTick();
			}
		}

		if ( gFlags.firing )
		{
			ReadAtoCurrent();
		}

		if ( gFlags.tick_5khz )
		{
			// 5000Hz
			gFlags.tick_5khz = 0;

			if ( gFlags.firing )
			{
				RegulateBuckBoost();
			}
		}

		if ( gFlags.tick_1khz )
		{
			// 1000Hz
			gFlags.tick_1khz = 0;

			if ( gFlags.firing )
			{
				ReadAtomizer();

				if ( ISMODETC(dfMode) )
				{
					if ( gFlags.check_mode )
					{
						CheckMode();
					}
					TweakTargetVoltsTC();
				}
				else if ( ISMODEVW(dfMode) )
				{
					TweakTargetVoltsVW();
				}
			}

			if ( dfStatus.vcom )
			{
				VCOM_Poll();
			}
		}

		if ( gFlags.tick_100hz )
		{
			// 100Hz
			gFlags.tick_100hz = 0;

			ResetWatchDog();

			if ( gFlags.read_battery )
			{
				gFlags.read_battery = 0;
			}

			TimedItems();
			SleepIfIdle();
			ReadBatteryVoltage();
			ReadBoardTemp();

			if ( gFlags.firing && BoardTemp >= 70 )
			{
				Overtemp();
			}

			if ( ISVTCDUAL )
			{
				BatteryChargeDual();
			}
			else if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 || ISPRIMO1 || ISPRIMO2 || ISPREDATOR )
			{
				BatteryCharge();
			}

			if (( gFlags.anim3d ) && ( Screen == 1 ) && ( !EditModeTimer ))
			{
				anim3d( 0 );
			}

			if ( Screen == 60 )
			{
				AnimateScreenSaver();
			}

			if ( gFlags.firing )
			{
				if ( gFlags.read_bir && ( FireDuration > 10 ) )
				{
					ReadInternalResistance();
				}

				if ( PreheatTimer && !--PreheatTimer )
				{
					uint16_t pwr;

					if ( dfMode == 6 )
					{
						pwr = dfSavedCfgPwr[ConfigIndex];
					}
					else
					{
						pwr = dfPower;
					}

					if ( pwr > BatteryMaxPwr )
					{
						gFlags.limit_power = 1;
						PowerScale = 100 * BatteryMaxPwr / pwr;
					}
					else
					{
						gFlags.limit_power = 0;
						PowerScale = 100;
					}
				}
			}

			if ( KeyTicks >= 5 )
			{
				KeyRepeat();
			}

			GetUserInput();
		}

		if ( gFlags.tick_10hz )
		{
			// 10Hz
			gFlags.tick_10hz = 0;

			DataFlashUpdateTick();
			LEDTimerTick();

			if ( gFlags.firing )
			{
				++FireDuration;

				if ( gFlags.monitoring )
				{
					Monitor();
				}
			}

			if ( ShowWeakBatFlag )
				--ShowWeakBatFlag;

			if ( ShowProfNum )
				--ShowProfNum;

			if ( !( gFlags.firing && ISMODETC(dfMode) ) )
			{
				DrawScreen();
			}

			if ( KeyTicks < 5 )
			{
				KeyRepeat();
			}
		}

		if ( gFlags.tick_5hz )
		{
			// 5Hz
			gFlags.tick_5hz = 0;

			if ( !gFlags.rtcinit && NumBatteries )
			{
				InitRTC();
			}

			if ( gFlags.firing )
			{
				if ( TargetVolts == 0 )
				{
					ProbeAtomizer();
				}
			}
			else
			{
				if
				(	!dfStatus.off
					&& Event == 0
					&& ( AtoProbeCount < 12 )
					&& ( Screen == 0 || Screen == 1 || Screen == 5 ) )
				{
					ProbeAtomizer();
				}
			}

			if ( IsClockOnScreen() )
			{
				static uint8_t u8Seconds = 61;
				S_RTC_TIME_DATA_T rtd;

				GetRTC( &rtd );

				if ( (uint8_t)rtd.u32Second != u8Seconds )
				{
					u8Seconds = (uint8_t)rtd.u32Second;
					gFlags.refresh_display = 1;
				}
			}
		}

		if ( gFlags.tick_2hz )
		{
			// 2Hz
			gFlags.tick_2hz = 0;

			gFlags.osc_1hz ^= 1;

			if ( gFlags.firing )
			{
				if ( ISMODETC(dfMode) )
				{
					DrawScreen();
				}
			}
			else
			{
				if
				(	!dfStatus.off
					&& Event == 0
					&& ( AtoProbeCount >= 12 )
					&& ( Screen == 0 || Screen == 1 || Screen == 5 ) )
				{
					ProbeAtomizer();
				}

				if ( gFlags.monitoring )
				{
					Monitor();
				}
			}
		}

		if ( gFlags.tick_1hz )
		{
			// 1Hz
			gFlags.tick_1hz = 0;

			if ( SplashTimer )
			{
				--SplashTimer;
				
				if ( !SplashTimer )
				{
					MainView();
				}
			}

			if ( !gFlags.firing && !dfStatus.off && !EditModeTimer )
			{
				if ( HideLogo )
				{
					if ( Screen == 1 )
					{
						--HideLogo;

						if ( !HideLogo )
						{
							gFlags.refresh_display = 1;
						}
					}
				}
			}
		}

		EventHandler();

	}
}

