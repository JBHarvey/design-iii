/*
 * PID.c
 *
 *  Created on: 2017-03-07
 *      Author: Admin
 */
#include "PID.h"
#include "arm_math.h"

void initPID(arm_pid_instance_f32* PID_SPEED, arm_pid_instance_f32* PID_POS) {
	/* ARM PID Instance, float_32 format */

	/* Set PID parameters */
	/* Set this for your needs */
	PID_SPEED->Kp = PID_SPEED_KP; /* Proportional */
	PID_SPEED->Ki = PID_SPEED_KI; /* Integral */
	PID_SPEED->Kd = PID_SPEED_KD; /* Derivative */

	/* Initialize PID system, float32_t format */
	//arm_pid_init_f32(PID_SPEED, 1);
	/* Initialize PID for position */
	PID_POS->Kp = PID_POSITION_KP;
	PID_POS->Ki = PID_POSITION_KI;
	PID_POS->Kd = PID_POSITION_KD;
	//arm_pid_init_f32(PID_POS, 1);

}

float getPID(arm_pid_instance_f32* PID, float current, float want) {
	/* Calculate error */
	float pid_error = current - want;

	/* Calculate PID here, argument is error */
	/* Output data will be returned, we will use it as duty cycle parameter */
	return arm_pid_f32(PID, pid_error);
}
