#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "myevic.h"
#include "meusbd.h"

//=========================================================================

#define MSC_INTERFACE	2

/*!<Define Mass Storage class specific requests */
#define MSC_ACCEPT_DEVSPEC_RQ		0x00
#define MSC_GET_MAX_LUN				0xFE
#define MSC_BOMSR					0xFF
#define UFI_TEST_UNIT_READY			0x00
#define UFI_REQUEST_SENSE			0x03
#define UFI_INQUIRY					0x12
#define UFI_MODE_SENSE				0x1A
#define UFI_PREVENT_MEDIA_REMOVAL	0x1E
#define UFI_READ_FORMAT_CAPACITIES	0x23
#define UFI_READ_CAPACITY			0x25
#define UFI_READ_10					0x28

/*!<Endpoints logical numbers */
#define MSC_BULK_IN_EP_NUM		0x04
#define MSC_BULK_OUT_EP_NUM		0x05

//=========================================================================

extern const uint8_t usbdMSCConfigDesc[];

//-------------------------------------------------------------------------

extern void MSC_Init();
extern void MSC_SetConfig();
extern void MSC_EP5Handler();
extern void MSC_EP6Handler();
extern void MSC_ClassRequest( uint8_t *token );

//=========================================================================

#endif /* __STORAGE_H__ */
