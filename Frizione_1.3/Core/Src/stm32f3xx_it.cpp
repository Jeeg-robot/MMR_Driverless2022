/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "SampleFlash.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void WIRING_CHECK();
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
// FLAGS

/*
 * E_calibrazioneFrizione = 0: No errors detected
 * E_calibrazioneFrizione.bit[0] = 1: adjusted lower mechanical stop too high
 * E_calibrazioneFrizione.bit[1] = 1: not used
 * E_calibrazioneFrizione.bit[2] = 1: adjusted upper mechanical stop too low
 * E_calibrazioneFrizione.bit[3] = 1: Motor wiring of H-bridge is reversed
 * E_calibrazioneFrizione.bit[4] = 1: Potentiometers "a" and "b" are swapped
 * E_calibrazioneFrizione.bit[5] = 1: Insufficient throttle movement detected
 */
uint8_t 	E_calibrazioneFrizione = 0;
uint8_t 	E_calibrazioneLevaFrizione = 0;


uint8_t NSLEEP_Enabled = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
/* USER CODE BEGIN EV */

uint16_t 	ufriz_P1 = 2001;
#define 	friz_threesold_P1 2000
uint16_t 	ufriz_P2 = 2999;
#define 	friz_threesold_P2 3000
float 		ufriz_divisore = 0;

uint16_t 	uleva_P1 = 600;
#define 	leva_threesold_P1 601
uint16_t 	uleva_P2 = 2500;
#define 	leva_threesold_P2 2499
float 		uleva_divisore = 0.1;

void WIRING_CHECK() {
	// POTENZIOMETRO FRIZIONE
	if (ufriz_P1 > friz_threesold_P1)
		E_calibrazioneFrizione |= 1 << 0;
	if (ufriz_P2 < friz_threesold_P2)
		E_calibrazioneFrizione |= 1 << 2;
	if (abs(ufriz_P2) <= abs(ufriz_P1))
		E_calibrazioneFrizione |= 1 << 3;
	if (ufriz_P2 - ufriz_P1 <= 0) {
		if(!(abs(ufriz_P2) <= abs(ufriz_P1)))
			E_calibrazioneFrizione |= 1 << 4;
	}
	else
		if((abs(ufriz_P2) <= abs(ufriz_P1)))
			E_calibrazioneFrizione |= 1 << 4;
	if (ufriz_P2 - ufriz_P1 == 0)
		Error_Handler();
	ufriz_divisore = (float)(ufriz_P2 - ufriz_P1);


	// POTENZIOMETRO LEVA
	if (uleva_P1 > leva_threesold_P1)
		E_calibrazioneLevaFrizione |= 1 << 0;
	if (uleva_P2 < leva_threesold_P2)
		E_calibrazioneLevaFrizione |= 1 << 2;
	if (abs(uleva_P2) <= abs(uleva_P1))
		E_calibrazioneLevaFrizione |= 1 << 3;
	uleva_divisore = (float)(uleva_P2 - uleva_P1);
}

//--------------------- Sampling variable
 extern float posizioneAttuale_leva;
 float PosLeva_array[SAMPLING_ARRAY] = {0};
 static uint16_t PosLevaIndex = 0;
 bool Flash_Write = false;
 bool sampling_control = true;
extern bool start_read;
 //-------------------------------------

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	static uint8_t sampling_counter = 0;
if(start_read)
{
	sampling_counter++;

	if(sampling_counter >= 100 && PosLevaIndex < SAMPLING_ARRAY && sampling_control)
	{
		PosLeva_array[PosLevaIndex] = posizioneAttuale_leva;
		sampling_counter = 0;
		PosLevaIndex++;

	}
	 if(PosLevaIndex >= SAMPLING_ARRAY && sampling_control)
	{
		//GPIOA->BSRR = (uint32_t)LD2_Pin;
		sampling_control =false;
		Flash_Write = true;

	}
}


  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
