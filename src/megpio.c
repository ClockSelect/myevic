#include "myevic.h"
#include "myprintf.h"

/* DC/DC converters PWM channels */
#define BBC_PWMCH_BUCK  0
#define BBC_PWMCH_BOOST 2


//=========================================================================
//----- (000022EC) --------------------------------------------------------
__myevic__ void GPD_IRQHandler()
{
	if ( PD->INTSRC & GPIO_INTSRC_INTSRC7_Msk )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN7_Msk );

		if ( Flags64 & 0x400 )
		{
			byte_20000048 = 1;

			if ( Flags64 & 0x1000 )
			{
				Event = 13;
			}
		}
	}
	else if ( PD->INTSRC & GPIO_INTSRC_INTSRC0_Msk )
	{
		GPIO_CLR_INT_FLAG( PD, GPIO_PIN_PIN0_Msk );
	}
}

//----- (00002334) --------------------------------------------------------
__myevic__ void GPE_IRQHandler()
{
	PE->INTSRC = PE->INTSRC;
}

//----- (00002342) --------------------------------------------------------
__myevic__ void GPF_IRQHandler()
{
	PF->INTSRC = PF->INTSRC;
}


//=========================================================================
//----- (00005CC0) --------------------------------------------------------
__myevic__ void BBC_Configure( uint32_t chan, uint32_t mode )
{
	if ( chan == 0 )
	{
		SYS->GPC_MFPL &= ~SYS_GPC_MFPL_PC0MFP_Msk;

		if ( mode )
		{
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_PWM0_CH0;
		}
		else
		{
			GPIO_SetMode( PC, GPIO_PIN_PIN0_Msk, GPIO_MODE_OUTPUT );
		}
	}
	else if ( chan == 2 )
	{
		SYS->GPC_MFPL &= ~SYS_GPC_MFPL_PC2MFP_Msk;

		if ( mode )
		{
			SYS->GPC_MFPL |= SYS_GPC_MFPL_PC2MFP_PWM0_CH2;
		}
		else
		{
			GPIO_SetMode( PC, GPIO_PIN_PIN2_Msk, GPIO_MODE_OUTPUT );
		}
	}
}


//=========================================================================
//----- (00002384) --------------------------------------------------------
__myevic__ void InitGPIO()
{
	// PD1 = Data transmitter output pin for UART0
	SYS->GPD_MFPL = SYS_GPD_MFPL_PD1MFP_UART0_TXD;

	// PC0 = PWM0 CH0
	BBC_Configure( 0, 1 );
	// PC2 = PWM0 CH2
	BBC_Configure( 2, 1 );

	// BUTTONS
	GPIO_SetMode( PE, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN2_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN3_Msk, GPIO_MODE_INPUT );

	// BUCK/BOOST CONVERTER
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

	// BATTERY (What is PD.0?)
	GPIO_SetMode( PD, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
	GPIO_EnableInt( PD, 0, GPIO_INT_FALLING );
	GPIO_SetMode( PD, GPIO_PIN_PIN7_Msk, GPIO_MODE_INPUT );
	GPIO_EnableInt( PD, 7, GPIO_INT_RISING );
	GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN7_Msk );

	// SPI0 (Display control)
	PE10 = 0;
	GPIO_SetMode( PE, GPIO_PIN_PIN10_Msk, GPIO_MODE_OUTPUT );
	PE12 = 0;
	GPIO_SetMode( PE, GPIO_PIN_PIN12_Msk, GPIO_MODE_OUTPUT );

	// ? (What is PB.7?)
	PB7 = 1;
	GPIO_SetMode( PB, GPIO_PIN_PIN7_Msk, GPIO_MODE_OUTPUT );

	NVIC_EnableIRQ( GPD_IRQn );
	NVIC_EnableIRQ( GPE_IRQn );
	NVIC_EnableIRQ( GPF_IRQn );

	// Debounce time = 0.1s
	GPIO_SET_DEBOUNCE_TIME( GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024 );
}


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

	myputc = (PUTC_FUNC*)&UART0_Putc;
}


//=========================================================================
//----- (00005C4C) --------------------------------------------------------
__myevic__ void InitPWM()
{
	PWM_ConfigOutputChannel( PWM0, BBC_PWMCH_BUCK, 150000, 0 );
	PWM_ConfigOutputChannel( PWM0, BBC_PWMCH_BOOST, 150000, 0 );

	PWM_EnableOutput( PWM0, 1 << BBC_PWMCH_BUCK );
	PWM_EnablePeriodInt( PWM0, BBC_PWMCH_BUCK, 0 );

	PWM_EnableOutput( PWM0, 1 << BBC_PWMCH_BOOST );
	PWM_EnablePeriodInt( PWM0, BBC_PWMCH_BOOST, 0 );

	PWM_Start( PWM0, 1 << BBC_PWMCH_BUCK );
	PWM_Start( PWM0, 1 << BBC_PWMCH_BOOST );

	BoostDuty = 0;
	PWM_SET_CMR( PWM0, BBC_PWMCH_BOOST, 0 );

	BuckDuty = 0;
	PWM_SET_CMR( PWM0, BBC_PWMCH_BUCK, 0 );
}

