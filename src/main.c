#include "myevic.h"
#include "myprintf.h"
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


//=============================================================================
// Globals

volatile gFlags_t gFlags;


//=============================================================================
// Forward declarations

void SleepIfIdle();


//=============================================================================
// Useless function - testing purpose
//------------------------------------------------------------------------------
__myevic__ void CustomStartup()
{
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


//------------------------------------------------------------------------------

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
	// Internal 22.1184MHz oscillator
	CLK_EnableXtalRC( CLK_PWRCTL_HIRCEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HIRCSTB_Msk );
	CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK( 1 ) );

	// 12.000MHz external crystal
	CLK_EnableXtalRC( CLK_PWRCTL_HXTEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk );

	//  32.768kHz external crystal
	SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF0MFP_Msk|SYS_GPF_MFPL_PF1MFP_Msk);
	SYS->GPF_MFPL |=  (SYS_GPF_MFPL_PF0MFP_X32_OUT|SYS_GPF_MFPL_PF1MFP_X32_IN);

	CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
	if ( CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) )
	{
		gFlags.has_x32 = 1;
	}

	// FMC Frequency Optimisation mode <= 72MHz
	FMC_EnableFreqOptimizeMode( FMC_FTCTL_OPTIMIZE_72MHZ );

	// CRC CLK = HCLK/1
	CLK_EnableModuleClock( CRC_MODULE );

	// Setup PLL to 144MHz and HCLK source to PLL/2
	CLK_SetCoreClock( 72000000 );
	CLK_WaitClockReady( CLK_STATUS_PLLSTB_Msk );

	// PWM0 CLK = PCLK0/1
	CLK_EnableModuleClock( PWM0_MODULE );
	CLK_SetModuleClock( PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0 );
	SYS_ResetModule( PWM0_RST );

	// UART0 CLK = HXT/1
	CLK_EnableModuleClock( UART0_MODULE );
	CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UARTSEL_HXT, CLK_CLKDIV0_UART( 1 ) );

	// USB CLK = PLL/3
	CLK_EnableModuleClock( USBD_MODULE );
	CLK_SetModuleClock( USBD_MODULE, 0, CLK_CLKDIV0_USB( 3 ) );
	SYS->USBPHY = SYS_USBPHY_LDO33EN_Msk;

	// WDT CLK = LIRC/1
	CLK_EnableModuleClock( WDT_MODULE );
	CLK_SetModuleClock( WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0 );

	// SPI0 CLK = PCLK0/1
	CLK_EnableModuleClock( SPI0_MODULE );

	// EADC CLK = PCLK1/8
	CLK_EnableModuleClock( EADC_MODULE );
	CLK_SetModuleClock( EADC_MODULE, 0, CLK_CLKDIV0_EADC( 8 ) );

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
	SYS->VREFCTL = SYS_VREFCTL_VREF_2_56V;

	// Brown-out detector; resets under 2.2V
	SYS_EnableBOD( SYS_BODCTL_BOD_RST_EN, SYS_BODCTL_BODVL_2_2V );

	// Update clock data
	SystemCoreClockUpdate();
}


//=============================================================================
//----- (00000C48) --------------------------------------------------------
__myevic__ void InitHardware()
{
	SYS_UnlockReg();

	InitDevices();

	WDT_Open( WDT_TIMEOUT_2POW18, WDT_RESET_DELAY_18CLK, TRUE, FALSE );

	SYS_LockReg();

	InitUART0();
	InitGPIO();
	InitSPI0();
	InitEADC();
	InitPWM();

	InitTimers();

	InitRTC( 0 );
}


//=============================================================================
//----- (0000895C) --------------------------------------------------------
__myevic__ void InitVariables()
{
	InitDataFlash();
	KeyPressTime |= 0x8000;
	LastInputs |= 0x80;
	word_200000B6 = -1;
	byte_200000B3 = 1;
	BatRefreshTmr = -1;
	gFlags.draw_edited_item = 1;
	gFlags.refresh_battery = 1;
	gFlags.read_battery = 1;
	EditItemIndex = 0;
}


//=============================================================================
//----- (00000148) ------------------------------------------------------------
__myevic__ void Main()
{
	InitHardware();
	InitVariables();

	InitUSB();

	myprintf( "\n\nJoyetech APROM\n" );
	myprintf( "CPU @ %dHz(PLL@ %dHz)\n", SystemCoreClock, PllClock );

	gFlags.sample_vbat = 1;
	ReadBatteryVoltage();

	gFlags.sample_btemp = 1;
	ReadBoardTemp();

	InitDisplay();
	MainView();

	CustomStartup();

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
			GetAtoCurrent();
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
				ReadAtoTemp();

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

			if ( Anim3d )
			{
				if (( Screen == 1 ) && ( !EditModeTimer ))
				{
					anim3d( 0 );
				}
			}

			if ( gFlags.firing && BoardTemp >= 70 )
			{
				Overtemp();
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

			if ( gFlags.firing )
				++FireDuration;

			if ( ShowWeakBatFlag )
				--ShowWeakBatFlag;

			if ( ! ((gFlags.firing) && ISMODETC(dfMode)) )
			{
				DrawScreen();
			}
		}

		if ( gFlags.tick_5hz )
		{
			// 5Hz
			gFlags.tick_5hz = 0;

			if ( gFlags.firing )
			{
				if ( !TargetVolts )
				{
					ProbeAtomizer();
				}
			}
			else if
			(	!( dfStatus.off )
				&& Event == -1
				&& ( Screen == 0 || Screen == 1 || Screen == 5 ) )
			{
				ProbeAtomizer();
			}

			if ( KeyTicks < 5 )
			{
				KeyRepeat();
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
		}

		if ( gFlags.tick_1hz )
		{
			// 1Hz
			gFlags.tick_1hz = 0;

			if ( ShowDateFlag ) --ShowDateFlag;

			if ( !(gFlags.firing) && !(dfStatus.off) && !EditModeTimer )
			{
				if ( HideLogo )
				{
					if ( Screen == 1 )
					{
						if ( --HideLogo )
						{
							gFlags.refresh_display = 1;
						}
					}
				}
			}

			if ( IsClockOnScreen() )
			{
				gFlags.refresh_display = 1;
			}
		}

		EventHandler();

	}
}


//=============================================================================
// BSOD

__myevic__ void Plantouille( int xpsr, int* stack )
{
	int i, k;

	k = 0;

	SYS_UnlockReg();
	WDT_Open( WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, FALSE, FALSE );
	SYS_LockReg();

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


//=============================================================================
//----- (00005D24) --------------------------------------------------------
__myevic__ void DevicesOnOff( int off )
{
	if ( off )
	{
		TIMER_DisableInt( TIMER0 );
		TIMER_DisableInt( TIMER1 );
		TIMER_DisableInt( TIMER2 );
		TIMER_DisableInt( TIMER3 );

		RTC_DisableInt( RTC_INTEN_TICKIEN_Msk );

		EADC_Close( EADC );
		SetADCState( 1, 0 );
		SetADCState( 2, 0 );
		SetADCState( 14, 0 );

		PC1 = 0;
		PC0 = 0;
		BBC_Configure( BBC_PWMCH_BUCK, 0 );
		PC3 = 0;
		PC2 = 0;
		BBC_Configure( BBC_PWMCH_BOOST, 0 );

		PB7 = 0;

		GPIO_DisableInt( PD, 0 );
		PD0 = 0;
		GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_OUTPUT );
		GPIO_DisableInt( PD, 7 );
		PD7 = 0;
		GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );

		SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE11MFP_Msk|SYS_GPE_MFPH_PE12MFP_Msk|SYS_GPE_MFPH_PE13MFP_Msk);
		PE11 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN11_Msk, GPIO_MODE_OUTPUT );
		PE12 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN12_Msk, GPIO_MODE_OUTPUT );
		PE13 = 0;
		GPIO_SetMode( PE, GPIO_PIN_PIN13_Msk, GPIO_MODE_OUTPUT );	// org: pin 3. bug.
		PE10 = 0;

		GPIO_EnableInt( PE, 0, GPIO_INT_BOTH_EDGE );
		GPIO_EnableInt( PD, 2, GPIO_INT_BOTH_EDGE );
		GPIO_EnableInt( PD, 3, GPIO_INT_BOTH_EDGE );

		SYS_UnlockReg();
		SYS->USBPHY &= ~SYS_USBPHY_LDO33EN_Msk;
		SYS->IVSCTL &= ~SYS_IVSCTL_VBATUGEN_Msk;
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
		SYS->VREFCTL = SYS_VREFCTL_VREF_2_56V;
		SYS_EnableBOD( SYS_BODCTL_BOD_RST_EN, SYS_BODCTL_BODVL_2_2V );
		SYS_LockReg();

		GPIO_DisableInt( PE, 0 );
		GPIO_DisableInt( PD, 2 );
		GPIO_DisableInt( PD, 3 );

		SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE11MFP_Msk|SYS_GPE_MFPH_PE12MFP_Msk|SYS_GPE_MFPH_PE13MFP_Msk);
		SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE11MFP_SPI0_MOSI0|SYS_GPE_MFPH_PE12MFP_SPI0_SS|SYS_GPE_MFPH_PE13MFP_SPI0_CLK);

		GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
		GPIO_EnableInt( PD, 0, GPIO_INT_FALLING );
		GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_INPUT );
		GPIO_EnableInt( PD, 7, GPIO_INT_RISING );
		GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN7_Msk );

		PB7 = 1;

		SetADCState( 1, 1 );
		SetADCState( 2, 1 );
		SetADCState( 14, 1 );

		RTC_EnableInt( RTC_INTEN_TICKIEN_Msk );

		TIMER_EnableInt( TIMER0 );
		TIMER_EnableInt( TIMER1 );
		TIMER_EnableInt( TIMER2 );
		TIMER_EnableInt( TIMER3 );
	}
}


//=============================================================================
//----- (00005D14) --------------------------------------------------------
__myevic__ void FlushAndSleep()
{
	UART_WAIT_TX_EMPTY( UART0 );
	CLK_PowerDown();
}


//=============================================================================
//----- (00004F0C) --------------------------------------------------------

void GoToSleep()
{
	ScreenOff();
	gFlags.firing = 0;
	BatReadTimer = 50;
	DevicesOnOff( 1 );
	CLK_SysTickDelay( 250 );
	CLK_SysTickDelay( 250 );
	CLK_SysTickDelay( 250 );
	if ( dfStatus.off || PE0 || KeyPressTime == 1100 )
	{
		SYS_UnlockReg();
		WDT_Open( WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, FALSE, FALSE );
		FlushAndSleep();
	}
	WDT_Open( WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_18CLK, TRUE, FALSE );
	SYS_LockReg();
	gFlags.refresh_battery = 1;
	DevicesOnOff( 0 );
	InitDisplay();
}


//=============================================================================
//----- (0000782C) --------------------------------------------------------
__myevic__ void SleepIfIdle()
{
	if ( !( gFlags.firing ) && !BatRefreshTmr )
	{
		if ( ( Screen == 0 && SleepTimer == 0 ) && ( gFlags.user_idle ) )
		{
			GoToSleep();

			byte_200000B3 = 2;
			AtoProbeCount = 0;
			AtoRezMilli = 0;
			gFlags.sample_vbat = 1;
			ReadBatteryVoltage();
			if ( BatteryVoltage <= 300 && !(gFlags.usb_attached) )
			{
				dfStatus.off = 1;
				Screen = 0;
			}
			gFlags.sample_btemp = 1;
		}
		BatRefreshTmr = 200;
	}
}

