#ifndef __VCOM_H__
#define __VCOM_H__

#include "myevic.h"
#include "meusbd.h"

//=========================================================================

#define VCOM_INTERFACE	1

/*!<Define CDC class specific requests */
#define CDC_SET_LINE_CODE          0x20
#define CDC_GET_LINE_CODE          0x21
#define CDC_SET_CONTROL_LINE_STATE 0x22

/*!<Mask for DTR bit in line state */
#define VCOM_LINESTATE_MASK_DTR (1 << 0)
#define VCOM_DEFAULT_INT_INTERVAL 0

/*!<Endpoints logical numbers */
#define VCOM_INT_IN_EP_NUM		0x03
#define VCOM_BULK_IN_EP_NUM		0x04
#define VCOM_BULK_OUT_EP_NUM	0x05

//=========================================================================

extern const uint8_t usbdVCOMConfigDesc[];
extern volatile uint16_t gCtrlSignal;

//-------------------------------------------------------------------------

extern void VCOM_Poll();
extern void VCOM_EP5Handler();
extern void VCOM_EP6Handler();
extern void VCOM_ClassRequest( uint8_t *token );
extern char VCOM_Putc( char c, FILE *out );

//=========================================================================

#endif /* __VCOM_H__ */