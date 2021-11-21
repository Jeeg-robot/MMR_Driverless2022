/*
 * SampleFlash.h
 *
 *  Created on: 5 ago 2021
 *      Author: Maxmi
 */

#ifndef INC_SAMPLEFLASH_H_
#define INC_SAMPLEFLASH_H_

#include "stdio.h"
#include "main.h"

#define SAMPLING_ARRAY 600
#define FLASH_ADDRESS 0x800F000 // pagina 252

uint32_t Flash_Write_Data (uint32_t StartAddress, uint32_t *Data, uint16_t numberofwords);
#endif /* INC_SAMPLEFLASH_H_ */
