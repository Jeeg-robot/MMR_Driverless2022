# MMR_Driverless
## Frizione
### Driver
Per il controllo della frizione, si è scelto di usare il driver DRV8701EVM (https://www.ti.com/tool/DRV8701EVM) della Texas Instruments. 
Questa board di sviluppo è costituita da un H-Bridge (https://it.wikipedia.org/wiki/H-bridge) e da un microcontrollore della Texas Instruments, che però non viene utilizzato.

Per controllare l'H-Bridge, la STM32-F303K8 (https://www.st.com/en/microcontrollers-microprocessors/stm32f303k8.html) utilizza principalmente 3 segnali

- NSLEEP (Attiva/Disattiva il driver) (Attenzione a non abusare di questo segnale. Se viene chiamato troppe volte in un breve tempo, ad esempio 100 volte in un millisecondo, il driver potrebbe non funzionare e andare in errore, e questo può essere visto tramite i due LED sul driver, SNSOUT e NFAULT, accesi)
```c
// PWM OFF se sotto una certa soglia
if ( abs(Calculated_DutyTarget) <= upperThreesold )
	// EQUIVALENTE A HAL_GPIO_WritePin(NSLEEP_GPIO_Port, NSLEEP_Pin, GPIO_PIN_RESET);
	NSLEEP_GPIO_Port->BRR = NSLEEP_Pin; // DRIVER SPENTO
else
	// EQUIVALENTE A HAL_GPIO_WritePin(NSLEEP_GPIO_Port, NSLEEP_Pin, GPIO_PIN_SET);
	NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin; // DRIVER ACCESO
```
- PHASE (Determina la direzione di rotazione del motore. Dipende da come vengono collegati i fili, per questo motivo è presente una define all'interno del codice per cambiare al volo in caso di errato collegamento dei cavi. Il software non è programmato per accorgersene, andrebbe fatto, ma l'unico modo per accorgersene è quello di avviare il software, se il motore a riposo è sotto sforzo, probabilmente i cavi sono collegati male)
```c
#define back_PHASE 	GPIO_PIN_RESET
#define forward_PHASE 	GPIO_PIN_SET
```
- ENABLE (Questo segnale è direttamente collegato al TIMER3 della STM. Infatti la scheda può generare un segnale PWM in modo trasparente al programmatore, utilizzando l'hardware interno. Il TIMER è già configurato e funzionante, settato a una frequenza di 20kHz, che sembra ottimale per il motore. Tramite software si va a variare il DUTY CYCLE del segnale, e non la sua frequenza. Attenzione, non è detto che DUTY CYCLE = 0 corrisponda a zero corrente al motore, per questo si utilizza NSLEEP per interrompere la corrente al motore)
Se si vogliono avere più informazioni su come la STM generi questo segnale PWM, leggere qui -> https://deepbluembedded.com/stm32-pwm-example-timer-pwm-mode-tutorial/
```c
TIM3->CCR2 = 0;		// DUTY CYCLE a 0%
TIM3->CCR2 = 799; 	// DUTY CYCLE al 100%
```

### ADC
Il sampling dei segnali analogici viene fatto tramite il convertitore Analogico Digitale e il DMA della STM. In questo modo, in modo del tutto trasparente, i valori dei segnali analogici vengono salvati e aggiornati in modo trasparente al programmatore, all'interno di un array.
```c
// INIZIALIZZAZIONE ADC E DMA
  MX_DMA_Init();
  MX_ADC1_Init();
  ...
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Adc_Value, 4); //Start DMA
```
```c
volatile uint32_t Adc_Value[4];
/*Array dell'ADC.
*Ad ogni INPUT analogico, viene assegnata una posizione nell'array.
*Ad esempio, durante le prove in AeroClub, con i collegamenti eseguiti in modo corretto, 
*i segnali analogici erano i seguenti:
Adc_Value[POT_1] -> Segnale potenziometro motore frizione
Adc_Value[POT_3] -> Segnale potenziometro leva sul volante frizione
```
### Calcolo posizione motore
```c
// posizioneAttuale viene aggiornata in modo continuo.
posizioneAttuale = ( (float) Adc_Value[POT_1] - (float)ufriz_P1)/ufriz_divisore; // CALCOLO POSIZIONE FRIZIONE
// Adc_Value[POT_1] = valore dall'ADC, va da 0 a 4096
// ufriz_P1 = valore minimo (determina il primo stop meccanico), che può assumere Adc_Value[POT_1].
// ufriz_P2 = valore massimo (determina il secondo stop meccanico), che può assumere Adc_Value[POT_1].
// ufriz_divisore = (float)(ufriz_P2 - ufriz_P1); 
```
![GitHub Logo](/posizioneAttuale.PNG)
