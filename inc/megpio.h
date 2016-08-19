#ifndef __MEGPIO_H__
#define __MEGPIO_H__


/* DC/DC converters PWM channels */
#define BBC_PWMCH_BUCK  0
#define BBC_PWMCH_BOOST 2


extern void InitGPIO();

extern void InitUART0();
extern void UART0_Cout( uint8_t c );



#endif /* __MEGPIO_H__ */
