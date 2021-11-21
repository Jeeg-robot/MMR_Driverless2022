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
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "PIDLib.h"
#include "SampleFlash.h"
#include "stdbool.h"
#include "stdio.h"

#define ARM_MATH_CM4
#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"

//#define SAMPLEMODE
#define PARTENZAMODE

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
enum_StatoVeicolo StatoVeicolo = BoardSetup;

//ADC
volatile uint32_t Adc_Value[2];

// PID
PID_Controller PID_FrizPOS;
float CAN_BUS_TARGET = 0.1; // usato sempre come target del PID. Non deve scendere sotto 0.1
float posizioneAttuale = 0; // posizione frizione calcolata tramite potenziometro

#ifdef DEBUGFRA
// DEBUG
float PID_TUNING_Kp = 5;
float PID_TUNING_Ki = 0.05;
float PID_TUNING_Max = 0.6f;
float PID_TUNING_Min = -0.5f;
uint8_t PID_TUNING_Sample = 1;
uint16_t forward_Duty_Register = 0;
uint16_t back_Duty_Register = 0;
uint16_t ARR_Value = 0;
uint16_t ARR_Value_Attuale = 0;
uint8_t back = 0;			// 0 oppure 1
float 	back_Duty = 0.4;		// 0.0 to 1.0
uint8_t forward = 0;		// 0 oppure 1
float 	forward_Duty = 0.4; 	// 0.0 to 1.0
float 	Duty_Attuale = 0;
float 	frequenzaPWM = 0;
#endif

/* Security
* CALIBRARE FRIZIONE E LEVA IN stm32f3xx_it.cpp
* TODO: Autocalibrazione frizione?
*/
extern uint8_t 	E_calibrazioneFrizione;
extern uint8_t 	E_calibrazioneLevaFrizione;
extern float  	ufriz_divisore;
extern uint16_t ufriz_P1;
extern uint16_t ufriz_P2;

extern float 	uleva_divisore;
extern uint16_t	uleva_P1;

// Program Variables
float posizioneAttuale_leva = 0;
float posizioneAttuale_leva_last = 0;

// Procedura di partenza
uint8_t 		partenza		= 0; 		// flag partenza
uint32_t  		istanteIniziale	= 0; 		// istante iniziale
uint32_t		timer[3] = {0};

_Punti Punti[5] = {
		{0.5f, 	0},
		{0.28f, 2000},
		{0.20f, 8000},
		{0.1f,  10000},
		{0.1f, 	10001}
};

float Punti_Coeff[4] = {
		(Punti[1].value - Punti[0].value)*1000/(Punti[1].time - Punti[0].time),
		(Punti[2].value - Punti[1].value)*1000/(Punti[2].time - Punti[1].time),
		(Punti[3].value - Punti[2].value)*1000/(Punti[3].time - Punti[2].time),
		(Punti[4].value - Punti[3].value)*1000/(Punti[4].time - Punti[3].time)
};

// FIR filter
float32_t FIR_Array_Sample[10] = {0};
float32_t FIR_Array_Sample2[10] = {0};
float32_t FIR_Array_Filter[10] = {0};
uint8_t FIR_IndexSample = 0;
uint8_t FIR_IndexSample2 = 0;

extern __IO uint32_t uwTick;

// FIR
#define LOW_PASS_FIR_FILTER_LENGHT 5
#define BLOCK_COMPUTE 10
#define NUM_TAPS    10
const float32_t FIR_Filter_Coefficent[LOW_PASS_FIR_FILTER_LENGHT] = {
		0.0355435363596614,
		0.240988385412421,
		0.446936156455835,
		0.240988385412421,
		0.0355435363596614
};
arm_fir_instance_f32 S;
arm_status status;
float32_t  *inputF32, *outputF32;
static float firStateF32[ BLOCK_COMPUTE + NUM_TAPS - 1];
float Filter_Array[10] = {0};
float meanValue_Filtered = 0;
float meanValue_NotFiltered = 0;
float meanValue_Filtered2 = 0;
float meanValue_NotFiltered2 = 0;

uint32_t POT3_Array[10] = {0};
uint32_t POT3_Mean = 0;
uint8_t Index_Mean_POT[3] = {0};
//----------------------------- Sampling variable
extern bool Flash_Write;
extern float PosLeva_array[SAMPLING_ARRAY];
bool start_read = false;
//----------------------------------------------

uint32_t Sicurezza_Debounce = 0;
uint32_t Sicurezza_Debounce1 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
extern void WIRING_CHECK();

void FIR_Filter();
void FRIZ_Homing();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Adc_Value, 2); //Start DMA
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); //Start PWM
	arm_fir_init_f32(&S, NUM_TAPS, (float*)FIR_Filter_Coefficent, (float*)firStateF32, BLOCK_COMPUTE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		/*
		 * Viene filtrato il POT posizione frizione
		 * Viene filtrato il POT posizione leva frizione
		 */
		static uint32_t FIR_Timer = 0;
		if ( (uwTick >= FIR_Timer + 1) && StatoVeicolo != BoardSetup) { // Funzione chiamata ogni millisecondo
			FIR_Timer = uwTick;
			FIR_Filter();
		}

		switch(StatoVeicolo) {
		case BoardSetup:
			PID_FrizPOS.OutputEnabled = 3;
			FRIZ_Homing();
			break;
		case GuidaManuale:
			CAN_BUS_TARGET = posizioneAttuale_leva;

			if (PID_FrizPOS.OutputEnabled == 0)
				PID_FrizPOS.OutputEnabled = 1;

			if (HAL_GPIO_ReadPin(Frizione_GPIO_Port, Frizione_Pin) == GPIO_PIN_SET) { // entra nel ciclo solo la prima volta
				if (Sicurezza_Debounce1 == 0)
					Sicurezza_Debounce1 = uwTick;
				else if (uwTick >= Sicurezza_Debounce1 + 50) {	// ERRORE
					#ifdef PARTENZAMODE
					StatoVeicolo = GuidaAutonoma;	// procedura di partenza
					partenza = 1;					// procedura di partenza
					#endif
					#ifdef SAMPLEMODE
					start_read = true;				// procedura di sampling
					#endif
				}
			} else {
				Sicurezza_Debounce1 = 0;
			}

			#ifdef SAMPLEMODE
			if(Flash_Write)
			{
				Flash_Write = false;
				start_read = false;
				Flash_Write_Data(FLASH_ADDRESS, (uint32_t *)PosLeva_array, 600);
			}
			#endif

			break;
		case GuidaAutonoma:

			if (PID_FrizPOS.OutputEnabled == 0)
				PID_FrizPOS.OutputEnabled = 1;

			if (HAL_GPIO_ReadPin(Frizione_GPIO_Port, Frizione_Pin) == GPIO_PIN_RESET) { // entra nel ciclo solo la prima volta
				if (Sicurezza_Debounce == 0)
					Sicurezza_Debounce = uwTick;
				else if (uwTick >= Sicurezza_Debounce + 50) {	// ERRORE
					StatoVeicolo = Errore;
					break;
				}
			} else {
				Sicurezza_Debounce = 0;
			}

			switch (partenza) { // partenza rilascio frizione
			case 1: // prima chiamata
				if(posizioneAttuale <= 0.3f)
					CAN_BUS_TARGET = Punti[0].value;
				else {
					partenza = 5;
					istanteIniziale = uwTick;
				}
				break;
			case 2:
				break;
			case 3:
				for (uint8_t i = 0; i < 5; i++) {
					if(i == 4)
					{
						partenza = 4;
						break;
					}
					if(uwTick-istanteIniziale >= Punti[i].time) {
						if(uwTick-istanteIniziale <= Punti[i+1].time)
						{
							float CAN_BUS_TARGET_buffer = Punti[i].value + Punti_Coeff[i]*(uwTick-istanteIniziale-Punti[i].time)/1000;
							if (posizioneAttuale_leva > CAN_BUS_TARGET_buffer)
								CAN_BUS_TARGET = posizioneAttuale_leva;
							else {
								CAN_BUS_TARGET = CAN_BUS_TARGET_buffer;
							}
							break;
						}
					}
				}
				break;
			case 4: // Procedura Finita
				CAN_BUS_TARGET = posizioneAttuale_leva;
				break;

			case 5: // Aspetta 5 secondi
				if (uwTick - istanteIniziale > 5000) {
					istanteIniziale=uwTick;
					partenza = 3;
				}
				break;
			}

			break;
		case Errore:
			if (timer[0] == 0) {
				timer[0] = uwTick;
				CAN_BUS_TARGET = 0.5f;
			}
			else if (uwTick - timer[0] > 1000) {
				CAN_BUS_TARGET = 0.3f;
			}
			if (PID_FrizPOS.OutputEnabled == 0)
				PID_FrizPOS.OutputEnabled = 1;
			break;
		default:
			break;
		}


#ifdef DEBUGFRA
		// DEBUG, DA NON METTERE NEL CODICE FINALE
		//		PID_FrizPOS.Kp_f = PID_TUNING_Kp;
		//		PID_FrizPOS.Ki_f = PID_TUNING_Ki;
		//		PID_FrizPOS.Max_out_f = PID_TUNING_Max;
		//		PID_FrizPOS.Min_out_f = PID_TUNING_Min;
		//		PID_FrizPOS.Sample_time = PID_TUNING_Sample;

		ARR_Value_Attuale = TIM3->ARR;
		frequenzaPWM = 16000/(ARR_Value + 1);

		forward_Duty_Register = forward_Duty*799;
		back_Duty_Register = back_Duty*799;

		Duty_Attuale = (PHASE_Flag_indicator) ? ((float)TIM3->CCR2 / 799.0f) : -((float)TIM3->CCR2 / 799.0f);

		if (forward_Duty_Register > 799 || back_Duty_Register > 799)
			Error_Handler();

		if (back && !forward) {  // back --> Control_Phase = back = 0
			TIM3->CCR2 = back_Duty_Register;
			NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin;
			HAL_GPIO_WritePin(PHASE_GPIO_Port, PHASE_Pin, back_PHASE);
			PHASE_Flag_indicator = 0;
		}
		else if (!back && forward) { // forward --> Control_Phase = forward = 1
			TIM3->CCR2 = forward_Duty_Register;
			NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin;
			HAL_GPIO_WritePin(PHASE_GPIO_Port, PHASE_Pin, forward_PHASE);
			PHASE_Flag_indicator = 1;
		}
		else if (!back && !forward && !AS_Ready) {
			TIM3->CCR2 = 0;
			NSLEEP_GPIO_Port->BRR = NSLEEP_Pin;
		}

		else
			Error_Handler();

		// FINE DEBUG
#endif
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void FIR_Filter() {
	if (Index_Mean_POT[2] == 10) { // Campionati 10 valori
		Index_Mean_POT[2] = 0;
		POT3_Mean = 0;
		for (uint8_t i  = 0; i < 10; i++)
			POT3_Mean += POT3_Array[i];
		POT3_Mean = POT3_Mean/10;

		// CALCOLO POSIZIONE LEVA FRIZIONE
		float posizioneAttuale_leva_buffer = float((POT3_Mean - uleva_P1))/uleva_divisore;
		if (posizioneAttuale_leva_buffer >= 0.1 && posizioneAttuale_leva_buffer <= 0.9) { // saturazione sulla posizione calcolata
			posizioneAttuale_leva =  posizioneAttuale_leva_buffer;
			posizioneAttuale_leva_last = posizioneAttuale_leva;
		}
		else {
			posizioneAttuale_leva = (posizioneAttuale_leva_last >= 0.50) ? 0.9 : 0.1;
		}
	} else {
		POT3_Array[Index_Mean_POT[2]] = Adc_Value[POT_3];
		Index_Mean_POT[2]++;
	}

	if (FIR_IndexSample < 10) {
		int32_t bufferPreFIR = (Adc_Value[POT_1] - ufriz_P1);
		FIR_Array_Sample[FIR_IndexSample++] = bufferPreFIR > 50 ? bufferPreFIR/ufriz_divisore : 0.05f;
	} else {
		FIR_IndexSample = 0;

		arm_fir_f32(&S, &FIR_Array_Sample[0], &Filter_Array[0], BLOCK_COMPUTE);

		for(int i = 0; i < 10; i++)
		{
			meanValue_Filtered += Filter_Array[i];
			meanValue_NotFiltered += FIR_Array_Sample[i];
		}
		meanValue_Filtered = meanValue_Filtered/10;
		meanValue_NotFiltered = meanValue_NotFiltered/10;

		posizioneAttuale = meanValue_Filtered;

		PID_FrizPOS.lastTimer = uwTick;
		PID_FrizPOS.PID_Call(CAN_BUS_TARGET, posizioneAttuale);
	}
}

void FRIZ_Homing() {
	static uint32_t timerHoming_F = 0;
	static uint32_t timerHoming_B = 0;
	if (!timerHoming_F && !timerHoming_B) {
		timerHoming_F = uwTick;
		TIM3->CCR2 = 330;
		PHASE_GPIO_Port->BSRR = PHASE_Pin;
		NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin;
	} else if (timerHoming_F && !timerHoming_B) {
		if (uwTick - timerHoming_F > 3000) {
			ufriz_P2 = Adc_Value[POT_1];
			timerHoming_B = uwTick;
			timerHoming_F = 0;
			TIM3->CCR2 = 100;
			PHASE_GPIO_Port->BRR = PHASE_Pin;
			NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin;
		}
	} else if (!timerHoming_F && timerHoming_B) {
		if (uwTick - timerHoming_B > 1500) {
			ufriz_P1 = Adc_Value[POT_1];
			timerHoming_B = 0;
			timerHoming_F = 0;
			TIM3->CCR2 = 0;
			PHASE_GPIO_Port->BSRR = PHASE_Pin;
			NSLEEP_GPIO_Port->BRR = NSLEEP_Pin;
			PID_FrizPOS.OutputEnabled = 0;

			WIRING_CHECK();
			if (E_calibrazioneFrizione)
				while(1) {} // errore
			if (E_calibrazioneLevaFrizione)
				while(1) {} // errore

			StatoVeicolo = GuidaManuale;
		}
	}
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
	if (StatoVeicolo == GuidaAutonoma)
		StatoVeicolo = Errore;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
