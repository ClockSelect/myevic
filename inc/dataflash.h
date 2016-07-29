#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

#include "M451Series.h"

//=============================================================================

void InitDataFlash();
void ResetDataFlash();
void DFCheckValuesValidity();
void FMCReadCounters();
void FMCWriteCounters();

void CpyTmpCoefsNI();
void CpyTmpCoefsTI();

void WriteDataFlash( uint32_t u32Addr, const uint32_t *pu32Data );
void UpdateDataFlash();
void UpdateFlash();

//=============================================================================

void FMCWrite800( uint32_t u32Addr, uint32_t *pu32Data );
uint32_t FMCVerif800( uint32_t u32Addr, uint32_t *pu32Data );
int FMCEraseWrite800( uint32_t u32Addr, uint32_t *src );

void FMCRead100( uint32_t u32Addr, uint32_t *pu32Buf );

//=============================================================================

#endif /* __DATAFLASH_H__ */
