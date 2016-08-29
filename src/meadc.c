#include "myevic.h"
#include "M451Series.h"


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
	// Configure PB.0 - PB.6 analog input pins
	SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk |
					   SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
					   SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk |
					   SYS_GPB_MFPL_PB6MFP_Msk);

   SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC_CH0 | SYS_GPB_MFPL_PB1MFP_EADC_CH1 |
					 SYS_GPB_MFPL_PB2MFP_EADC_CH2 | SYS_GPB_MFPL_PB3MFP_EADC_CH3 |
					 SYS_GPB_MFPL_PB4MFP_EADC_CH4 | SYS_GPB_MFPL_PB5MFP_EADC_CH13 |
					 SYS_GPB_MFPL_PB6MFP_EADC_CH14);

	// Disable PB.0 - PB.6 digital input paths to avoid leakage currents
	GPIO_DISABLE_DIGITAL_PATH( PB, 0x7F );
}


//=============================================================================
__myevic__ void SetADCState( int module, int onoff )
{
	int pin, mode;

	mode = onoff ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;

	switch ( module )
	{
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

		default:
			return;
	}

	GPIO_SetMode( PB, pin, mode );
}


//=============================================================================
//----- (0000184C) --------------------------------------------------------
__myevic__ uint32_t ADC_Read( uint32_t module )
{
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

	return EADC_GET_CONV_DATA( EADC, module );
}
