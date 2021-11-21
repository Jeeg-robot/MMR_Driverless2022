/*
 * PIDLib.h
 *
 *  Created on: 12 giu 2021
 *      Author: franc
 */

#ifndef INC_PIDLIB_H_
#define INC_PIDLIB_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

class PID_Controller
{
private:
  void init_PID();
  float PIDOutput_PreSaturation; // input of saturation
  float PIDOutput; // output of  PI controller after saturation
  float last_error; // previous term

public:
  float Kp_f;  	// proportional gain forward
  float Ki_f; 	// integral gain forward
  float Kd_f;	// derivative gain forward

  float Kp_b;  	// proportional gain backward
  float Ki_b; 	// integral gain backward
  float Kd_b;	// derivative gain backward

  float Max_out_f; // Max for PI value output for forward
  float Min_out_f; // Min for PI value output for forward
  float Max_out_b; // Max for PI value output for back
  float Min_out_b; // Min for PI value output for back

  float Proportional;
  float Integral;

  float last_PIDOutput_PreSaturation; // store the prev Control Signal before saturation for antiwindup
  float last_PIDOutput;


  uint64_t lastTimer;
  uint64_t Timer;
  uint8_t OutputEnabled;
  uint8_t Sample_time;; // sampling time in ms

  PID_Controller() {init_PID();} // init_PID chiamata quando viene chiamato PID_Controller
  float PID_Compute(float ref_speed, float measured_speed);
  void PID_Call(float target, float reference);
};

#endif /* INC_PIDLIB_H_ */
