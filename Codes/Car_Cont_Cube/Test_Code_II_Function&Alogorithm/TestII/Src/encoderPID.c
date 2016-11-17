/*
	@Copyright Han ZHANG(haldak) | All rights reserved.
	@Last Modified Time Nov.13, 2016
*/

/* Includes ------------------------------------------------------------------*/
#include "encoderPID.h"
#include "tim.h"
#include "motor_cont.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int32_t valEncoderL, valEncoderR, saveEncoderL, saveEncoderR;
static int32_t targetSpeedL, targetSpeedR;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void readEncoRAWData(void)
{
	saveEncoderL = valEncoderL;
	saveEncoderR = valEncoderR;
	valEncoderL = (int32_t)(__HAL_TIM_GET_COUNTER(&htim3));
	valEncoderR = (int32_t)(__HAL_TIM_GET_COUNTER(&htim4));
}

int32_t getEncoderDiffL(void)
{
	return valEncoderL - saveEncoderL;
}	

int32_t getEncoderDiffR(void)
{
	return valEncoderR - saveEncoderR;
}

void setTargetSpeed(int32_t speedL, int32_t speedR)
{
	targetSpeedL = speedL;
	targetSpeedR = speedR;
}

//PWM += Kp * [e(k) - e(k-1)] + Ki * e(k) + Kd * [e(k) - 2 * e(k-1) + e(k-2)]
int32_t incrementalPIDL(int encoderDiffPulseL, int targetSpeedL)
{ 	
	float Kp = 20.0, Ki = 30.0, Kd = 0.0;
	static int32_t Ek = 0, Ek1 = 0, Ek2 = 0, PWM = 0;
	Ek = encoderDiffPulseL - targetSpeedL;
	PWM += Kp * (Ek - Ek1) + Ki * Ek + Kd * (Ek - 2 * Ek1 + Ek2);
	Ek2 = Ek1;
	Ek1 = Ek;
	return PWM;
}

//PWM += Kp * [e(k) - e(k-1)] + Ki * e(k) + Kd * [e(k) - 2 * e(k-1) + e(k-2)]
int32_t incrementalPIDR(int encoderDiffPulseR, int targetSpeedR)
{ 	
	float Kp = 20.0, Ki = 30.0, Kd = 0.0;
	static int32_t Ek = 0, Ek1 = 0, Ek2 = 0, PWM = 0;
	Ek = encoderDiffPulseR - targetSpeedR;
	PWM += Kp * (Ek - Ek1) + Ki * Ek + Kd * (Ek - 2 * Ek1 + Ek2);
	Ek2 = Ek1;
	Ek1 = Ek;
	return PWM;
}

void movementPIDCont(void) //Be called in every 10ms.
{
	static int32_t encoderDiffL, encoderDiffR;
	static int32_t pidSpeedL, pidSpeedR;
	encoderDiffL = getEncoderDiffL();
	encoderDiffR = getEncoderDiffR();
	pidSpeedL = incrementalPIDL(encoderDiffL, targetSpeedL);
	pidSpeedR = incrementalPIDR(encoderDiffR, targetSpeedR);
	car_SetSpeedL(pidSpeedL);
	car_SetSpeedR(pidSpeedR);
}