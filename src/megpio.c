#include "myevic.h"
#include "dataflash.h"
#include "events.h"
#include "atomizer.h"
#include "battery.h"


//=========================================================================
//----- (000022EC) --------------------------------------------------------
__myevic__ void GPD_IRQHandler()
{
	if ( GPIO_GET_INT_FLAG( PD, GPIO_PIN_PIN7_Msk ) )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN7_Msk );

		if ( gFlags.usb_attached )
		{
			BattProbeCount = 1;

			if ( gFlags.battery_charging )
			{
				Event = 13;
			}
		}
	}
	else if ( GPIO_GET_INT_FLAG( PD, GPIO_PIN_PIN1_Msk ) )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN1_Msk );

		if ( gFlags.usb_attached && ( NumBatteries == 1 ) && ( BatteryVoltage >= 414 ) )
		{
			BattProbeCount = 1;

			if ( gFlags.battery_charging )
			{
				Event = 13;
			}
		}
	}
	else if ( GPIO_GET_INT_FLAG( PD, GPIO_PIN_PIN0_Msk ) )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN0_Msk );

		if ( ISPRESA75W || ISVTCDUAL || ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
		{
			if ( gFlags.firing || gFlags.probing_ato )
			{
				if ( Event != 28 )
				{
					Event = 28;
					StopFire();
				}
			}
		}
	}
	else if ( GPIO_GET_INT_FLAG( PD, GPIO_PIN_PIN2_Msk|GPIO_PIN_PIN3_Msk ) )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN2_Msk|GPIO_PIN_PIN3_Msk );

		if ( dfStatus.wakeonpm )
		{
			gFlags.wake_up = 1;
		}
	}
	else
	{
		PD->INTSRC = PD->INTSRC;
	}
}

//----- (00002334) --------------------------------------------------------
__myevic__ void GPE_IRQHandler()
{
	if ( GPIO_GET_INT_FLAG( PE, GPIO_PIN_PIN0_Msk ) )
	{
		GPIO_CLR_INT_FLAG( PE, GPIO_PIN_PIN0_Msk );

		gFlags.wake_up = 1;
	}
	else
	{
		PE->INTSRC = PE->INTSRC;
	}
}

//----- (00002342) --------------------------------------------------------
__myevic__ void GPF_IRQHandler()
{
	PF->INTSRC = PF->INTSRC;
}


//=========================================================================
//----- (00002384) --------------------------------------------------------
__myevic__ void InitGPIO()
{
	if ( ISVTCDUAL )
	{
		PA3 = 0;
		GPIO_SetMode( PA, GPIO_PIN_PIN3_Msk, GPIO_MODE_OUTPUT );
	}

	if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
	{
		SYS->GPF_MFPL &= ~SYS_GPF_MFPL_PF0MFP_Msk;
		SYS->GPF_MFPL |= SYS_GPF_MFPL_PF0MFP_GPIO;
		PF0 = 0;
		GPIO_SetMode( PF, GPIO_PIN_PIN0_Msk, GPIO_MODE_OUTPUT );
	}

	// PD1 = Data transmitter output pin for UART0
	#if (ENABLE_UART)
	SYS->GPD_MFPL |= SYS_GPD_MFPL_PD1MFP_UART0_TXD;
	#endif

	if ( ISRX300 )
	{
		SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk|SYS_GPD_MFPL_PD1MFP_Msk);
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD0MFP_GPIO|SYS_GPD_MFPL_PD1MFP_GPIO;
	}

	// PC0 = PWM0 CH0
	BBC_Configure( BBC_PWMCH_BUCK, 1 );
	// PC2 = PWM0 CH2
	BBC_Configure( BBC_PWMCH_BOOST, 1 );

	if ( ISVTCDUAL || ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
	{
		PD7 = 0;
		BBC_Configure( BBC_PWMCH_CHARGER, 0 );
		PD7 = 0;
	}

	// BUTTONS
	GPIO_SetMode( PE, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN2_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN3_Msk, GPIO_MODE_INPUT );

	if ( ISCUBOID || ISCUBO200 || ISRX200S || ISRX23 )
	{
		PF2 = 1;
		GPIO_SetMode( PF, GPIO_PIN_PIN2_Msk, GPIO_MODE_OUTPUT );
	}
	else if ( ISRX300 )
	{
		SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF5MFP_Msk|SYS_GPF_MFPL_PF6MFP_Msk);
		SYS->GPF_MFPL |= SYS_GPF_MFPL_PF5MFP_GPIO|SYS_GPF_MFPL_PF6MFP_GPIO;
		PF5 = 0;
		GPIO_SetMode( PF, GPIO_PIN_PIN5_Msk, GPIO_MODE_OUTPUT );
		PF6 = 0;
		GPIO_SetMode( PF, GPIO_PIN_PIN6_Msk, GPIO_MODE_OUTPUT );
		PA3 = 0;
		GPIO_SetMode( PA, GPIO_PIN_PIN3_Msk, GPIO_MODE_OUTPUT );
		PA2 = 0;
		GPIO_SetMode( PA, GPIO_PIN_PIN2_Msk, GPIO_MODE_OUTPUT );
	}

	// BUCK/BOOST CONVERTER CONTROL LINES
	PC1 = 0;
	GPIO_SetMode( PC, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
	PC3 = 0;
	GPIO_SetMode( PC, GPIO_PIN_PIN3_Msk, GPIO_MODE_OUTPUT );

	// SSD RESET/VDD/VCC
	PA0 = 0;
	GPIO_SetMode( PA, GPIO_PIN_PIN0_Msk, GPIO_MODE_OUTPUT );
	PA1 = 0;
	GPIO_SetMode( PA, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
	PC4 = 0;
	GPIO_SetMode( PC, GPIO_PIN_PIN4_Msk, GPIO_MODE_OUTPUT );

	// BATTERY
	GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
	GPIO_EnableInt( PD, 0, GPIO_INT_FALLING );
	if ( ISVTCDUAL )
	{
		PA2 = 0;
		GPIO_SetMode( PA, GPIO_PIN_PIN2_Msk, GPIO_MODE_OUTPUT );
		PF2 = 0;
		GPIO_SetMode( PF, GPIO_PIN_PIN2_Msk, GPIO_MODE_OUTPUT );

		GPIO_SetMode( PD, GPIO_PIN_PIN1_Msk, GPIO_MODE_INPUT );
		GPIO_EnableInt( PD, 1, GPIO_INT_RISING );
		GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN1_Msk );
	}
	else if ( !ISCUBOID && !ISCUBO200 && !ISRX200S && !ISRX23 && !ISRX300 )
	{
		GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_INPUT );
		GPIO_EnableInt( PD, 7, GPIO_INT_RISING );
		GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN7_Msk );
	}

	// SPI0 (Display control)
	PE10 = 0;
	GPIO_SetMode( PE, GPIO_PIN_PIN10_Msk, GPIO_MODE_OUTPUT );
	PE12 = 0;
	GPIO_SetMode( PE, GPIO_PIN_PIN12_Msk, GPIO_MODE_OUTPUT );

	// LED Control
	if ( ISEGRIPII || ISEVICAIO )
	{
		PB3 = 0;	// Blue
		PB4 = 0;	// Red
		PB5 = 0;	// Green
		GPIO_SetMode( PB, GPIO_PIN_PIN3_Msk|GPIO_PIN_PIN4_Msk|GPIO_PIN_PIN5_Msk, GPIO_MODE_OUTPUT );
	}

	if ( ISCUBO200 || ISRX200S || ISRX23 )
	{
		SYS->GPF_MFPL &= ~SYS_GPF_MFPL_PF1MFP_Msk;
		SYS->GPF_MFPL |= SYS_GPF_MFPL_PF1MFP_GPIO;
		PF1 = 1;
		GPIO_SetMode( PF, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
	}
	else if ( ISRX300 )
	{
		SYS->GPD_MFPL &= ~SYS_GPD_MFPL_PD1MFP_Msk;
		SYS->GPD_MFPL |= SYS_GPD_MFPL_PD1MFP_GPIO;
		PD1 = 1;
		GPIO_SetMode( PD, GPIO_PIN_PIN1_Msk, GPIO_MODE_OUTPUT );
	}
	else
	{
		// ? (What is PB.7?)
		PB7 = 1;
		GPIO_SetMode( PB, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );
	}

	NVIC_EnableIRQ( GPD_IRQn );
	NVIC_EnableIRQ( GPE_IRQn );
	NVIC_EnableIRQ( GPF_IRQn );

	// Debounce time = 100ms
	GPIO_SET_DEBOUNCE_TIME( GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024 );
}


#if (ENABLE_UART)

//=========================================================================
//----- (00006738) --------------------------------------------------------
__myevic__ void UART0_Cout( uint8_t c )
{
	UART_WAIT_TX_EMPTY( UART0 );
	UART_WRITE( UART0, c );

	if ( c == '\n' )
	{
		UART_WAIT_TX_EMPTY( UART0 );
		UART_WRITE( UART0, '\r' );
	}
}


//=========================================================================
__myevic__ char UART0_Putc( char c, FILE *out )
{
	UART0_Cout( (uint8_t)c );
	return c;
}


//=========================================================================
//----- (00007EF4) --------------------------------------------------------
__myevic__ void InitUART0()
{
	SYS_ResetModule( UART0_RST );
	UART_Open( UART0, 115200 );

	myputc = (FPUTC_FUNC*)&UART0_Putc;
}

#endif
