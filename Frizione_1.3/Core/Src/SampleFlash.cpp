/*
 * SampleFlash.c
 *
 *  Created on: 5 ago 2021
 *      Author: Maxmi
 */

#include "stdio.h"
#include "main.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_flash.h"
#include "stm32f3xx_hal_flash_ex.h"  // non compilare finchè non verranno caricati gli header mancanti
#include "string.h"




static uint32_t GetPage(uint32_t Address)
{
  for (uint16_t indx=0; indx<256; indx++)
  {
	  if((Address < (uint32_t)(0x08000000 + (FLASH_PAGE_SIZE *(indx+1))) ) && (Address >= (uint32_t)(0x08000000 + FLASH_PAGE_SIZE*indx)))
	  {
		  return (0x08000000 + FLASH_PAGE_SIZE*indx);
	  }
  }

  return 0;
}

//uint32_t Flash_Write_Data (uint32_t Address, uint32_t *Data)
//{
//	uint16_t N_word = (sizeof(Data)/4) + ((sizeof(Data)%4)!=0);
//    uint32_t StartAddress = Address;
//	static FLASH_EraseInitTypeDef EraseInitStruct;
//	uint32_t PAGEError;
//
//	int word_counter = 0;
//
//	HAL_FLASH_Unlock();
//
//	uint32_t StartPage = GetPage(StartAddress);
//	uint32_t EndPageAdress = StartAddress + N_word*4;
//	uint32_t EndPage = GetPage(EndPageAdress);
//
//	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
//	EraseInitStruct.PageAddress = StartPage;
//	EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;
//
//	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
//	{
//		return HAL_FLASH_GetError ();
//	}
//	while (word_counter < N_word)
//	{
//		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartAddress, Data[word_counter]) == HAL_OK) //da provare
//		{
//			StartAddress += 4;
//			word_counter++;
//		}
//		else
//		{
//
//			return HAL_FLASH_GetError ();
//		}
//	}
//	HAL_FLASH_Lock();
//	return 1;
//}

uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	int sofar=0;

	  /* Unlock the Flash to enable the flash control register access *************/
	   HAL_FLASH_Unlock();

	   /* Erase the user Flash area*/

	  uint32_t StartPage = GetPage(StartPageAddress);
	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
	  uint32_t EndPage = GetPage(EndPageAdress);

	   /* Fill EraseInit structure*/
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	   EraseInitStruct.PageAddress = StartPage;
	   EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	   {
	     /*Error occurred while page erase.*/
		  return HAL_FLASH_GetError ();
	   }

	   /* Program the user Flash area word by word*/

	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartPageAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	   /* Lock the Flash to disable the flash control register access (recommended
	      to protect the FLASH memory against possible unwanted operation) *********/
	   HAL_FLASH_Lock();

	   return 0;
}







