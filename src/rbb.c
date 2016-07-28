
//----- (00002CD8) --------------------------------------------------------
__myevic__ void RegulateBuckBoost()
{

	if ( ( (Flags64 & 0x100) && CheckBattery() )
		|| ( !(Flags64 & 0x2000) && !(Flags64 & 0x100) ) )
	{
		return;
	}

	AtoVoltsADC = ADC_Read( 1 );
	AtoVolts = ( 1109 * AtoVoltsADC ) >> 12;


	switch ( BBCNextMode )
	{
		case 1:	// Transition mode Buck <-> Boost
		{
			if ( BBCMode != 1 )
			{
				PC2 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BOOST, 0 );
				PC3 = 1;

				PC0 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BUCK, 0 );
				PC1 = 1;
			}
		
			if ( AtoVolts == TargetVolts )
			{
				BBCNumCmps = 0;
			}
			else if ( ++BBCNumCmps >= 5 )
			{
				if ( dfMode != 5 )
				{
					if ( BBCMode == 2 ) BBCNextMode = 3;
					else BBCNextMode = 2;
				}
				BBCMode = 1;
			}
		}

		case 2:	// Buck mode
		{
			if ( BBCMode != 2 )
			{
				PC2 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BOOST, 0 );

				PC3 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BUCK, 1 );
				BuckDuty = ( BBCMode == 0 ) ? 10 : 479;
				PWM_SET_CMR( PWM0, ATOMIZER_PWMCH_BUCK, BuckDuty );
				PC1 = 1;

				BBCMode = BBCNextMode;
			}

			if ( AtoVolts < TargetVolts )
			{
				if ( BuckDuty < 479 )
				{
					++BuckDuty;
				}
				else
				{
					if ( 12 * TargetVolts > 10 * BatteryVoltage )
					{
						BBCNextMode = 1;
					}
				}
			}
			else if ( AtoVolts > TargetVolts )
			{
				if ( BuckDuty > 10 ) --BuckDuty;
				else BuckDuty = 0;
			}

			if (	( AtoStatus == 0 || AtoStatus == 1 )
				||	( !(Flags64 & 0x100) && AtoProbeCount >= 12 ) )
			{
				if ( BuckDuty >= 45 ) BuckDuty = 45;
			}

			PWM_SET_CMR( PWM0, ATOMIZER_PWMCH_BUCK, BuckDuty );
		}

		case 3:	// Boost mode
		{
			if ( BBCMode != 3 )
			{
				BBCMode = 3;

				PC0 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BUCK, 0 );

				PC1 = 1;
				InitGPIO_PWM( ATOMIZER_PWMCH_BOOST, 1 );
				BoostDuty = 479;
				PWM_SET_CMR( PWM0, ATOMIZER_PWMCH_BOOST, BoostDuty );
				PC3 = 1;
			}

			if ( AtoVolts < TargetVolts && BoostDuty > 80 )
			{
				--BoostDuty;
			}
			else if ( AtoVolts > TargetVolts )
			{
				if ( BoostDuty < 479 )
				{
					++BoostDuty;
				}
				else
				{
					BBCNextMode = 1;
				}
			}

			PWM_SET_CMR( PWM0, ATOMIZER_PWMCH_BOOST, BoostDuty );
		}

		default:
			break;
	}
}


