#include "myevic.h"
#include "timers.h"


//=============================================================================

volatile uint32_t ADC00_IRQ_Flag;

//=============================================================================
//----- (00000558) --------------------------------------------------------
__myevic__ void ADC00_IRQHandler()
{
	ADC00_IRQ_Flag = 1;
	EADC_CLR_INT_FLAG( EADC, 1 << 0 );
}


//=============================================================================
__myevic__ void InitEADC()
{
	if ( ISCUBO200 || ISRX200S || ISRX23 || ISRX300 )
	{
		// Configure PB.0 - PB.7 analog input pins
		SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk |
						   SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
						   SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk |
						   SYS_GPB_MFPL_PB6MFP_Msk | SYS_GPB_MFPL_PB7MFP_Msk);

		SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC_CH0  | SYS_GPB_MFPL_PB1MFP_EADC_CH1  |
						  SYS_GPB_MFPL_PB2MFP_EADC_CH2  | SYS_GPB_MFPL_PB3MFP_EADC_CH3  |
						  SYS_GPB_MFPL_PB4MFP_EADC_CH4  | SYS_GPB_MFPL_PB5MFP_EADC_CH13 |
						  SYS_GPB_MFPL_PB6MFP_EADC_CH14 | SYS_GPB_MFPL_PB7MFP_EADC_CH15);

		// Disable PB.0 - PB.7 digital input paths to avoid leakage currents
		GPIO_DISABLE_DIGITAL_PATH( PB, 0xFF );
	}
	else if ( ISEGRIPII || ISEVICAIO )
	{
		// Configure PB.0,1,2,6 analog input pins
		SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk |
						   SYS_GPB_MFPL_PB2MFP_Msk |
						   SYS_GPB_MFPL_PB6MFP_Msk );

		SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC_CH0  | SYS_GPB_MFPL_PB1MFP_EADC_CH1  |
						  SYS_GPB_MFPL_PB2MFP_EADC_CH2  |
						  SYS_GPB_MFPL_PB6MFP_EADC_CH14 );

		// Disable PB.0,1,2,6 digital input paths to avoid leakage currents
		GPIO_DISABLE_DIGITAL_PATH( PB, 0x47 );
	}
	else
	{
		// Configure PB.0 - PB.6 analog input pins
		SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk |
						   SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
						   SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk |
						   SYS_GPB_MFPL_PB6MFP_Msk );

		SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC_CH0 | SYS_GPB_MFPL_PB1MFP_EADC_CH1  |
						  SYS_GPB_MFPL_PB2MFP_EADC_CH2 | SYS_GPB_MFPL_PB3MFP_EADC_CH3  |
						  SYS_GPB_MFPL_PB4MFP_EADC_CH4 | SYS_GPB_MFPL_PB5MFP_EADC_CH13 |
						  SYS_GPB_MFPL_PB6MFP_EADC_CH14);

		// Disable PB.0 - PB.6 digital input paths to avoid leakage currents
		GPIO_DISABLE_DIGITAL_PATH( PB, 0x7F );
	}
}


//=============================================================================

__myevic__ void SetADCState( uint32_t module, int onoff )
{
	uint32_t pin, mode;

	switch ( module )
	{
		case 0:
			pin = GPIO_PIN_PIN0_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB0MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB0MFP_EADC_CH0;
			}
			else
			{
				PB0 = 0;
			}
			break;

		case 1:
			pin = GPIO_PIN_PIN1_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB1MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB1MFP_EADC_CH1;
			}
			else
			{
				PB1 = 0;
			}
			break;

		case 2:
			pin = GPIO_PIN_PIN2_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB2MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB2MFP_EADC_CH2;
			}
			else
			{
				PB2 = 0;
			}
			break;

		case 3:
			pin = GPIO_PIN_PIN3_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB3MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB3MFP_EADC_CH3;
			}
			else
			{
				PB3 = 0;
			}
			break;

		case 4:
			pin = GPIO_PIN_PIN4_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB4MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB4MFP_EADC_CH4;
			}
			else
			{
				PB4 = 0;
			}
			break;

		case 13:
			pin = GPIO_PIN_PIN5_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB5MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB5MFP_EADC_CH13;
			}
			else
			{
				PB5 = 0;
			}
			break;

		case 14:
			pin = GPIO_PIN_PIN6_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB6MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB6MFP_EADC_CH14;
			}
			else
			{
				PB6 = 0;
			}
			break;

		case 15:
			pin = GPIO_PIN_PIN7_Msk;

			SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB7MFP_Msk;

			if ( onoff )
			{
				SYS->GPB_MFPL |= SYS_GPB_MFPL_PB7MFP_EADC_CH15;
			}
			else
			{
				PB7 = 0;
			}
			break;

		default:
			return;
	}

	mode = onoff ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;

	GPIO_SetMode( PB, pin, mode );
}


//=========================================================================
//----- (0000184C) --------------------------------------------------------
// Average total conversion time: 329 ticks (4.57us)
//-------------------------------------------------------------------------
__myevic__ uint32_t ADC_Read( uint32_t module )
{
	uint32_t result;
	// Total conversion time 15+6=21 ADC_CLK = 2.33us
	EADC_Open( EADC, EADC_CTL_DIFFEN_SINGLE_END );
	EADC_SetInternalSampleTime( EADC, 6 );	// 0.67 us
	EADC_ConfigSampleModule( EADC, module, EADC_SOFTWARE_TRIGGER, module );

	EADC_CLR_INT_FLAG( EADC, 1 << 0 );
	EADC_ENABLE_INT( EADC, 1 << 0 );
	EADC_ENABLE_SAMPLE_MODULE_INT( EADC, 0, 1 << module );
	NVIC_EnableIRQ( ADC00_IRQn );

	ADC00_IRQ_Flag = 0;
	EADC_START_CONV( EADC, 1 << module );
	while ( !ADC00_IRQ_Flag )
		;

	EADC_DISABLE_INT( EADC, 1 << 0 );
	result = EADC_GET_CONV_DATA( EADC, module );

	return result;
}
