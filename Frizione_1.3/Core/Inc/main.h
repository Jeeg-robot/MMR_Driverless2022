/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define POT_1 0			// Potenziometro Posizione Frizione 1
#define POT_3 1			// Potenziometro Leva Frizione

// DRIVER DRV8701 CONFIG
#define back_PHASE 		GPIO_PIN_RESET
#define forward_PHASE 	GPIO_PIN_SET

// PID
#define upperThreesold 60
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
typedef enum {
	BoardSetup,
	GuidaManuale,
	GuidaAutonoma,
	Errore
} enum_StatoVeicolo;

typedef struct {
	float value;
	uint32_t time;
} _Punti;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NSLEEP_Pin GPIO_PIN_1
#define NSLEEP_GPIO_Port GPIOA
#define ADC_POT_1_Pin GPIO_PIN_2
#define ADC_POT_1_GPIO_Port GPIOA
#define ADC_POT_LEVA_Pin GPIO_PIN_3
#define ADC_POT_LEVA_GPIO_Port GPIOA
#define PHASE_Pin GPIO_PIN_4
#define PHASE_GPIO_Port GPIOA
#define Frizione_Pin GPIO_PIN_9
#define Frizione_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
typedef struct {
  uint8_t IDSensore;
  uint8_t Nome[24];
  uint32_t lastTimeUPT;
  uint32_t lastTimeChe;
  uint8_t tocheck;
  uint16_t lastValoreChecked;
  uint16_t prelastValoreChecked;
} SensList16;

typedef struct {
  uint8_t IDSensore;
  uint8_t Nome[24];
  uint32_t lastTimeUPT;
  uint32_t lastTimeChe;
  uint8_t tocheck;
  uint8_t lastValoreChecked;
  uint8_t prelastValoreChecked;
} SensList8;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
