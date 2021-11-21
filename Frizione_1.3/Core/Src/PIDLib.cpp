/*
 * PIDLib.cpp
 *
 *  Created on: 12 giu 2021
 *      Author: franc
 */
#include "main.h"
#include "PIDLib.h"
#include "stm32f3xx_hal.h"
#include "math.h"

int16_t Calculated_DutyTarget = 0;


float PID_Controller::PID_Compute(float reference, float measured)
{
	if (this->OutputEnabled == 0) {
		NSLEEP_GPIO_Port->BRR = NSLEEP_Pin;
		TIM3->CCR2 = 0;
		return 0;
	}

	bool compare_signal = 0;
	float error = reference - measured;

	//Proportional term
	Proportional = (this->Kp_f) * error;

	//Integral term + Anti-windup clamping
	compare_signal = (this->last_PIDOutput_PreSaturation != this->last_PIDOutput) ? 1 : 0;

	if (compare_signal) // Significa che è andato in saturazione
		this->Integral = 0;
	else
		this->Integral = this->Integral + (this->Ki_f) * 0.5 * (this->Sample_time) * (error + this->last_error);

	// Control Signal
	this->PIDOutput_PreSaturation = Proportional + this->Integral;
	this->PIDOutput = this->PIDOutput_PreSaturation;

	//Saturation
	if (this->PIDOutput >= this->Max_out_f)
		this->PIDOutput = this->Max_out_f;
	else if (this->PIDOutput <= this->Min_out_f)
		this->PIDOutput = this->Min_out_f;
	//update terms

	this->last_error = error;
	this->last_PIDOutput_PreSaturation = this->PIDOutput_PreSaturation;
	this->last_PIDOutput = this->PIDOutput;

	return this->PIDOutput;
};

void PID_Controller::init_PID()
{
	this->Max_out_f = 0.6;
	this->Min_out_f = -0.3;

	this->Kp_f = 3.25;
	this->Ki_f = 0.0085;

	this->Sample_time = 10;

	this->PIDOutput = 0;

	this->PIDOutput_PreSaturation = 0;

	this->last_error = 0;

	this->last_PIDOutput = 0;

	this->last_PIDOutput_PreSaturation = 0;

	this->OutputEnabled = 0;
};

void PID_Controller::PID_Call(float target, float reference) {
	bool PHASE_Flag_indicator = 0;

	switch (this->OutputEnabled) {
	case 0:
		NSLEEP_GPIO_Port->BRR = NSLEEP_Pin;				// DRIVER DISATTIVATO
		break;
	case 1:
		TIM3->CCR2 = 0;
		NSLEEP_GPIO_Port->BSRR = (uint32_t)NSLEEP_Pin; 	// Accensione del driver con Duty = 0
														// la fase viene settata successivamente
		this->OutputEnabled = 2;
		this->PID_Compute(target, reference);
		break;
	case 2:
		static int16_t buff_PID = 0;
		buff_PID = (int16_t)(this->last_PIDOutput*799);

		this->PID_Compute(target, reference);

		// PWM OFF se sotto una certa soglia
		if ( abs(buff_PID) <= upperThreesold ) {
			NSLEEP_GPIO_Port->BRR = NSLEEP_Pin;
			Calculated_DutyTarget = 0;
		}
		else {
			NSLEEP_GPIO_Port->BSRR = NSLEEP_Pin;
			Calculated_DutyTarget = buff_PID;
		}
		TIM3->CCR2 = abs(Calculated_DutyTarget);

		// CALCOLO FASE MOTORE in base al segno del PID
		PHASE_Flag_indicator = (Calculated_DutyTarget < 0) ? 0 : 1;
		if (PHASE_Flag_indicator == 0)  // CAMBIO DI FASE
			PHASE_GPIO_Port->BRR = PHASE_Pin;
		else if (PHASE_Flag_indicator == 1)
			PHASE_GPIO_Port->BSRR = PHASE_Pin;
		break;
	}

};





