/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pitches.h"
#include "stdbool.h"
#include "stdio.h"

#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"
typedef float float32_t;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint16_t Adc_Value[2] = {0};

// FIR
float ADC_APPS_array[10] = {0};
float APPS_Filtrati[10] = {0};
bool APPS_Control = false;
float MeanAPPSf = 0;
float MeanAPPSnf = 0;
uint8_t FIR_APS_Index = 0;
#define LOW_PASS_FIR_FILTER_LENGHT 10
#define BLOCK_COMPUTE 10
#define NUM_TAPS    10
const float FIR_Filter_Coefficent[LOW_PASS_FIR_FILTER_LENGHT] = {
		0.016016767419347,
		0.0377623366218183,
		0.0929763121731825,
		0.156003554306458,
		0.197241029479194,
		0.197241029479194,
		0.156003554306458,
		0.0929763121731825,
		0.0377623366218183,
		0.0160167674193471
};
arm_fir_instance_f32 S;
arm_status status;
float32_t  *inputF32, *outputF32;
static float firStateF32[BLOCK_COMPUTE + NUM_TAPS - 1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint8_t APPS_Output_Enabled;
extern uint32_t APPS_Out[2];

void noTone(uint8_t a);
void tone(uint8_t a, int b, int c);

//#define SAMPLEMODE
#define PARTENZAMODE

// BUZZER
uint16_t BuzzerDuty = 1000;
uint16_t BuzzerDuty_last = 0;
uint8_t  BuzzerStato = 0;
uint32_t BuzzerTimer = 0;
// END BUZZER

// UPT APS
uint32_t timerAPS = 0;
// UPT APS

// SICUREZZA
uint32_t Timer[3] = {0}; // due timer
volatile uint32_t Sicurezza_Debounce = 0;
volatile uint32_t Sicurezza1_Debounce = 0;


// PARTENZA
Definizione_Stato_Vettura Stato_Vettura = GuidaManuale;
uint8_t Partenza = 0;

int melody[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5,
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

int durations[] = {
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8,
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8
};
// determine the length of the arrays to use in the loop iteration
int songLength = sizeof(melody)/sizeof(melody[0]);

// END PARTENZA

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Adc_Value, 2);
  tone(8, 2700, 0);
  HAL_Delay(5000);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); //Start PWM
  arm_fir_init_f32(&S, NUM_TAPS, FIR_Filter_Coefficent, &firStateF32, BLOCK_COMPUTE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	// FILTRO FIR ACCELERATORE
	if (timerAPS == 0)
	  timerAPS = uwTick;
	else if (uwTick > timerAPS + 10 && FIR_APS_Index < 10) {
		  ADC_APPS_array[FIR_APS_Index++] = Adc_Value[0];
	}

	if(FIR_APS_Index == 9)
	{
		arm_fir_f32(&S,(float32_t *) &ADC_APPS_array[0], (float32_t *)&APPS_Filtrati[0], BLOCK_COMPUTE);
		 FIR_APS_Index = 0;
		float buffer1 = 0;
		float buffer2 = 0;
		for(int j = 0; j<10; j++)
		{
		  buffer1 += ADC_APPS_array[j];
		  buffer2 += APPS_Filtrati[j];
		}
		MeanAPPSnf = buffer1/10;
		MeanAPPSf = buffer2/10;
		timerAPS = uwTick;
		APPS_Output();
	}

	switch(Stato_Vettura) {
	case GuidaManuale:
		APPS_Output_Enabled = 0;

		if (HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin) == GPIO_PIN_RESET) { // quando si preme il blue botton sulla scheda inizia la partenza
			if (Sicurezza1_Debounce == 0)
				Sicurezza1_Debounce = uwTick;
			else if (uwTick > Sicurezza1_Debounce + 50) {
				#ifdef PARTENZAMODE
				BuzzerDuty = 500;		// procedura di partenza
				Stato_Vettura = GuidaAutonoma;
				Partenza = 1;
				#endif
				#ifdef SAMPLEMODE
				HAL_GPIO_WritePin(Frizione_GPIO_Port, Frizione_Pin, GPIO_PIN_SET); // Procedura di sampling
				#endif
			}
		} else
		  Sicurezza1_Debounce = 0;

		break;
	case GuidaAutonoma:
		  /* Sicurezza viene dall'arduino
		   * Se è basso allora c'è un problema e bisogna fermare la macchina
		   */
		switch (Partenza) {
		case 0:
			break;
		case 1:
			BuzzerDuty = 250;
			HAL_GPIO_WritePin(Frizione_GPIO_Port, Frizione_Pin, GPIO_PIN_SET); // Procedura di partenza
			Partenza = 4;
			Timer[2] = uwTick;
			break;
		case 2:
			break;
		case 3:
			if (uwTick - Timer[2] > TEMPO2) {
				APPS_Out[0] = APPS_PARTITO_Value;
				APPS_Out[1] = APPS_PARTITO_Value/2;
				APPS_Output_Enabled = 1;
				APPS_Output();

				if (HAL_GPIO_ReadPin(Sicurezza_GPIO_Port, Sicurezza_Pin) == GPIO_PIN_RESET) { // entra nel ciclo solo la prima volta
					if (Sicurezza_Debounce == 0)
						Sicurezza_Debounce = uwTick;
					else if (uwTick >= Sicurezza_Debounce + 50) {	// ERRORE
						Stato_Vettura = Errore;
						BuzzerDuty = 100;
						break;
					}
				} else {
					Sicurezza_Debounce = 0;
				}

			}
			break;
		case 4: // Aspetta 5 secondi
			if (uwTick - Timer[2] > TEMPO1) {
				APPS_Out[0] = APPS_Partenza_Value;
				APPS_Out[1] = APPS_Partenza_Value/2;
				APPS_Output_Enabled = 1;
				APPS_Output();
				Partenza = 3;
				Timer[2] = uwTick;
			}
			break;
		}
		break;
	case Errore:
		HAL_GPIO_WritePin(Frizione_GPIO_Port, Frizione_Pin, GPIO_PIN_RESET); // La frizione viene staccata
		APPS_Out[0] = 0;
		APPS_Out[1] = 0;
		APPS_Output_Enabled = 1;
		APPS_Output();
		break;
	}

	if (uwTick > BuzzerTimer + (uint32_t)BuzzerDuty) {
		TIM3->CCR1 = (BuzzerStato) ? 0  : (TIM3->ARR)/2;;
		BuzzerStato = (BuzzerStato) ? 0 : 1;
		BuzzerTimer = uwTick;
	}

//	  for (int thisNote = 0; thisNote < songLength; thisNote++){
//	    // determine the duration of the notes that the computer understands
//	    // divide 1000 by the value, so the first note lasts for 1000/8 milliseconds
//	    int duration = 1000/ durations[thisNote];
//	    tone(8, melody[thisNote], duration);
//	    // pause between notes
//	    int pause = duration * 1.3;
//	    HAL_Delay(pause);
//	    // stop the tone
//	    noTone(8);
//	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void noTone(uint8_t a) {
	TIM3->ARR = 0;
}

void tone(uint8_t a, int b, int c) {
	TIM3->ARR = 100000/b;
	TIM3->CCR1 = (TIM3->ARR)/2;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
