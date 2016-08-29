#include "myevic.h"
#include "myprintf.h"
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
			byte_20000048 = 1;

			if ( gFlags.battery_charging )
			{
				Event = 13;
			}
		}
	}
	else if ( GPIO_GET_INT_FLAG( PD, GPIO_PIN_PIN0_Msk ) )
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
//----- (00002384) --------------------------------------------------------
__myevic__ void InitGPIO()
{
	// PD1 = Data transmitter output pin for UART0
	SYS->GPD_MFPL = SYS_GPD_MFPL_PD1MFP_UART0_TXD;

	// PC0 = PWM0 CH0
	BBC_Configure( BBC_PWMCH_BUCK, 1 );
	// PC2 = PWM0 CH2
	BBC_Configure( BBC_PWMCH_BOOST, 1 );

	// BUTTONS
	GPIO_SetMode( PE, GPIO_PIN_PIN0_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN2_Msk, GPIO_MODE_INPUT );
	GPIO_SetMode( PD, GPIO_PIN_PIN3_Msk, GPIO_MODE_INPUT );

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

	myputc = (FPUTC_FUNC*)&UART0_Putc;
}


