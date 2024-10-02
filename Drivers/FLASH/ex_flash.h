#ifndef __EX_FLASH_H
#define __EX_FLASH_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


void FLASH_Inside_Wr(uint32_t addr,uint32_t Pdata);
uint32_t FLASH_Inside_Rd(uint32_t addr);
void FLASH_Inside_Test(void);


#ifdef __cplusplus
}
#endif

#endif