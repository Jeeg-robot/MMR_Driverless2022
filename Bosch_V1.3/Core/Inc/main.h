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
#include "stm32f4xx_hal.h"

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
typedef enum {
	GuidaManuale,
	GuidaAutonoma,
	Errore,
} Definizione_Stato_Vettura;

// Valore (tra 0 e 4000) che definisce la posizione dell'acceleratore (APS) nella manovra di partenza
#define APPS_Partenza_Value 1850
//-------------------------------

// Valore che definisce la posizione dell'acceleratore (APS) dopo che la macchina è partita
// (dopo TEMPO1 + TEMPO2 millisecondi dalla pressione del bottone sulla scheda)
#define APPS_PARTITO_Value 	1300
//------------------------------

// Tempo (in millisecondi) tra il momento in cui si preme il pulsante sulla scheda
// dell'acceleratore e il momento in cui si alzano i giri motore
// quando si preme il pulsante la frizione si attacca (non trasferisce coppia)
#define TEMPO1 4000
//------------------------------

// Tempo (in millisecondi) tra il momento in cui la frizione si attacca
// e il momento in cui si abbassano i giri motore
#define TEMPO2 9000
//------------------------------

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define Sicurezza_Pin GPIO_PIN_1
#define Sicurezza_GPIO_Port GPIOC
#define Frizione_Pin GPIO_PIN_10
#define Frizione_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
