/**********************************************************************************************
 * C PID Library - Version 1.0.1
 * modified my Matthew Blythe <mblythester@gmail.com> mjblythe.com/hacks
 * originally by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include "PID_v1.h"
#include "motion.h"

void PID_Initialize(PidType* pid);

/*************************************
 * Design 3 PIDS
 *************************************/
PidType PID_SPEED1;
PidType PID_SPEED2;
PidType PID_SPEED3;
PidType PID_SPEED4;
PidType PID_POSITION1;
PidType PID_POSITION2;
PidType PID_POSITION3;
PidType PID_POSITION4;

float xMoveSetpoint = 0;
float yMoveSetpoint = 0;
float rotateMoveSetpoint = 0;
uint8_t isMovingX = 0;
uint8_t isMovingY = 0;
uint8_t isMovingRotate = 0;
uint8_t isMoveDone = 0;

volatile int ticksIndex5 = 0;
volatile FloatType ticksBuffer5[100];

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
void PID_init(PidType* pid, FloatType Kp, FloatType Ki, FloatType Kd,
		PidDirectionType ControllerDirection, FloatType N) {
	pid->myInput = 0;
	pid->myOutput = 0;
	pid->mySetpoint = 0;
	pid->ITerm = 0;
	pid->lastInput = 0;
	pid->inAuto = false;
	pid->N = N;
	pid->offset = 0;

	PID_SetOutputLimits(pid, 0, 0xffff);

	//default Controller Sample Time is 0.1 seconds
	pid->SampleTime = PID_SAMPLE_TIME;

	PID_SetControllerDirection(pid, ControllerDirection);
	PID_SetTunings(pid, Kp, Ki, Kd);

//  pid->lastTime = millis() - pid->SampleTime;
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/
bool PID_Compute_Speed(PidType* pid) {
	if (!pid->inAuto) {
		return false;
	}
//  unsigned long now = millis();
//  unsigned long timeChange = (now - pid->lastTime);
//  if (timeChange >= pid->SampleTime) {
	/*Compute all the working error variables*/
	float input = pid->myInput;
	//FloatType input = calculateSpeed(preInput);
	pid->error = pid->mySetpoint - input;
	pid->ITerm += (pid->ki * pid->error);
	if (pid->ITerm > pid->outMax)
		pid->ITerm = pid->outMax;
	else if (pid->ITerm < pid->outMin)
		pid->ITerm = pid->outMin;
	FloatType dInput = (input - pid->lastInput);

	/*Compute PID Output*/
	FloatType output = pid->kp * pid->error + pid->ITerm - pid->kd * dInput;

	if (output > pid->outMax)
		output = pid->outMax;
	else if (output < pid->outMin)
		output = pid->outMin;
	pid->myOutput = output;

	/*Remember some variables for next time*/
	pid->lastInput = pid->myInput;
//    pid->lastTime = now;
	return true;
//  } else {
//    return false;
//  }
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/
bool PID_Compute_Position(PidType* pid) {
	if (!pid->inAuto) {
		return false;
	}
//  unsigned long now = millis();
//  unsigned long timeChange = (now - pid->lastTime);
//  if (timeChange >= pid->SampleTime) {
	/*Compute all the working error variables*/
	float preInput = pid->myInput;
	FloatType input = calculatePosition(preInput);
	pid->error = pid->mySetpoint - input;
	if (pid->error > -PID_POSITION_TOLERANCE
			&& pid->error < PID_POSITION_TOLERANCE) {
		pid->error = 0;
	}

	pid->ITerm += (pid->ki * pid->error);
	if (pid->ITerm > pid->outMax)
		pid->ITerm = pid->outMax;
	else if (pid->ITerm < pid->outMin)
		pid->ITerm = pid->outMin;
	FloatType dInput = (input - calculatePosition(pid->lastInput));

	/*Compute PID Output*/
	//FloatType output = pid->kp * pid->error + pid->ITerm - pid->kd * dInput;
	FloatType output;
	if (pid->error == 0) {
		output = 0;
	} else {
		output = (pid->kp * pid->error + pid->ITerm - pid->kd * dInput);
		//* pid->coef;
	}

	if (output > pid->outMax) {
		output = pid->outMax;
	} else if (output < pid->outMin) {
		output = pid->outMin;
	}
	pid->myOutput = output;

	/*Remember some variables for next time*/
	pid->lastInput = pid->myInput;
//    pid->lastTime = now;
	return true;
//  } else {
//    return false;
//  }
}

/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted.
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/

void PID_SetTunings(PidType* pid, FloatType Kp, FloatType Ki, FloatType Kd) {
	if (Kp < 0 || Ki < 0 || Kd < 0) {
		return;
	}

	pid->dispKp = Kp;
	pid->dispKi = Ki;
	pid->dispKd = Kd;

	FloatType SampleTimeInSec = ((FloatType) pid->SampleTime) / 1000;
	pid->kp = Kp;
	pid->ki = Ki * SampleTimeInSec;
	pid->kd = Kd / SampleTimeInSec;
	//pid->kd = Kd / (SampleTimeInSec * pid->N + 1);

	if (pid->controllerDirection == PID_Direction_Reverse) {
		pid->kp = (0 - pid->kp);
		pid->ki = (0 - pid->ki);
		pid->kd = (0 - pid->kd);
	}
}

/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed
 ******************************************************************************/
void PID_SetSampleTime(PidType* pid, int NewSampleTime) {
	if (NewSampleTime > 0) {
		FloatType ratio = (FloatType) NewSampleTime
				/ (FloatType) pid->SampleTime;
		pid->ki *= ratio;
		pid->kd /= ratio;
		pid->SampleTime = (unsigned long) NewSampleTime;
	}
}

/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void PID_SetOutputLimits(PidType* pid, FloatType Min, FloatType Max) {
	if (Min >= Max) {
		return;
	}
	pid->outMin = Min;
	pid->outMax = Max;

	if (pid->inAuto) {
		if (pid->myOutput > pid->outMax) {
			pid->myOutput = pid->outMax;
		} else if (pid->myOutput < pid->outMin) {
			pid->myOutput = pid->outMin;
		}

		if (pid->ITerm > pid->outMax) {
			pid->ITerm = pid->outMax;
		} else if (pid->ITerm < pid->outMin) {
			pid->ITerm = pid->outMin;
		}
	}
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/
void PID_SetMode(PidType* pid, PidModeType Mode) {
	bool newAuto = (Mode == PID_Mode_Automatic);
	if (newAuto == !pid->inAuto) { /*we just went from manual to auto*/
		//PID_Initialize(pid);
	}
	pid->inAuto = newAuto;
}

/* Initialize()****************************************************************
 *  does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/
void PID_Initialize(PidType* pid) {
	pid->ITerm = pid->myOutput;
	pid->lastInput = pid->myInput;
	if (pid->ITerm > pid->outMax) {
		pid->ITerm = pid->outMax;
	} else if (pid->ITerm < pid->outMin) {
		pid->ITerm = pid->outMin;
	}
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID_SetControllerDirection(PidType* pid, PidDirectionType Direction) {
	if (pid->inAuto && Direction != pid->controllerDirection) {
		pid->kp = (0 - pid->kp);
		pid->ki = (0 - pid->ki);
		pid->kd = (0 - pid->kd);
	}
	pid->controllerDirection = Direction;
}

/* Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
FloatType PID_GetKp(PidType* pid) {
	return pid->dispKp;
}
FloatType PID_GetKi(PidType* pid) {
	return pid->dispKi;
}
FloatType PID_GetKd(PidType* pid) {
	return pid->dispKd;
}
PidModeType PID_GetMode(PidType* pid) {
	return pid->inAuto ? PID_Mode_Automatic : PID_Mode_Manual;
}
PidDirectionType PID_GetDirection(PidType* pid) {
	return pid->controllerDirection;
}

FloatType calculateSpeed(FloatType speedEdges) {

	FloatType speedResult = (speedEdges * METERS_PER_TICK)
			/ SPEED_CALC_TIME_DELAY;

	return speedResult;
}

uint16_t calculateSpeedToTicks(FloatType speedInMeters) {
	if (speedInMeters < 0.0) {
		speedInMeters = -speedInMeters;
	}
	uint16_t ticksResult = (speedInMeters * SPEED_CALC_TIME_DELAY)
			/ METERS_PER_TICK;

	return ticksResult;
}

FloatType calculatePosition(FloatType positionEdges) {
	FloatType positionResult = (positionEdges * METERS_PER_TICK);
	return positionResult;
}

/*******************************************************
 * Design 3 Functions
 *******************************************************/
void initAllPIDS() {
	PID_init(&PID_SPEED1, PID_SPEED1_KP, PID_SPEED1_KI, PID_SPEED1_KD,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&PID_SPEED1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION1, PID_POSITION1_KP, PID_POSITION1_KI,
			PID_POSITION1_KD, PID_Direction_Direct, PID_POSITION1_N);
	PID_SetOutputLimits(&PID_POSITION1, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 2 PIDs */
	PID_init(&PID_SPEED2, PID_SPEED2_KP, PID_SPEED2_KI, PID_SPEED2_KD,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&PID_SPEED2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION2, PID_POSITION2_KP, PID_POSITION2_KI,
			PID_POSITION2_KD, PID_Direction_Direct, PID_POSITION2_N);
	PID_SetOutputLimits(&PID_POSITION2, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 3 PIDs */
	PID_init(&PID_SPEED3, PID_SPEED3_KP, PID_SPEED3_KI, PID_SPEED3_KD,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&PID_SPEED3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION3, PID_POSITION3_KP, PID_POSITION3_KI,
			PID_POSITION3_KD, PID_Direction_Direct, PID_POSITION3_N);
	PID_SetOutputLimits(&PID_POSITION3, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 4 PIDs */
	PID_init(&PID_SPEED4, PID_SPEED4_KP, PID_SPEED4_KI, PID_SPEED4_KD,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&PID_SPEED4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION4, PID_POSITION4_KP, PID_POSITION4_KI,
			PID_POSITION4_KD, PID_Direction_Direct, PID_POSITION4_N);
	PID_SetOutputLimits(&PID_POSITION4, MIN_POS_COMMAND, MAX_POS_COMMAND);
}

void computeAllPIDS() {
	if (PID_Compute_Position(&PID_POSITION1)) {
		float positionOutput1 = PID_POSITION1.myOutput;
		int cmdMotor1;
		PID_SPEED1.mySetpoint = positionOutput1;

		if (PID_Compute_Speed(&PID_SPEED1)) {
			cmdMotor1 = PID_SPEED1.myOutput;
			if (cmdMotor1 > 0) {
				MotorSetDirection(1, COUNTER_CLOCK);
			} else if (cmdMotor1 < 0) {
				MotorSetDirection(1, CLOCK);
				cmdMotor1 = -cmdMotor1;
			} else {
				speedDirection1 = SPEED_DIRECTION_NONE;
				MotorSetDirection(1, BRAKE_V);
			}

			MotorSetSpeed(1, cmdMotor1);
			PID_SetMode(&PID_SPEED1, PID_Mode_Manual);
			PID_SetMode(&PID_POSITION1, PID_Mode_Manual);

			/*char numberString[MAX_DISPLAY_CHARACTERS];
			 sprintf(numberString, "%d", numberOfSpeedEdges1);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(0, 0, numberString);
			 sprintf(numberString, "%d", cmdMotor1);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(3, 0, numberString);*/

			/* send position to USB */
			/*TM_USB_VCP_Putc(2);
			 TM_USB_VCP_Putc(8);
			 VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));
			 VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));*/
		}
	}

	if (PID_Compute_Position(&PID_POSITION2)) {
		float positionOutput2 = PID_POSITION2.myOutput;
		PID_SPEED2.mySetpoint = positionOutput2;
		int cmdMotor2;

		if (PID_Compute_Speed(&PID_SPEED2)) {
			cmdMotor2 = PID_SPEED2.myOutput;
			if (cmdMotor2 > 0) {
				MotorSetDirection(2, COUNTER_CLOCK);
			} else if (cmdMotor2 < 0) {
				MotorSetDirection(2, CLOCK);
				cmdMotor2 = -cmdMotor2;
			} else {
				speedDirection2 = SPEED_DIRECTION_NONE;
				MotorSetDirection(2, BRAKE_V);
			}

			MotorSetSpeed(2, cmdMotor2);
			PID_SetMode(&PID_SPEED2, PID_Mode_Manual);
			PID_SetMode(&PID_POSITION2, PID_Mode_Manual);

			/*char numberString[MAX_DISPLAY_CHARACTERS];
			 sprintf(numberString, "%d", numberOfSpeedEdges2);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(0, 0, numberString);
			 sprintf(numberString, "%d", cmdMotor2);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(3, 0, numberString);*/
		}
	}

	if (PID_Compute_Position(&PID_POSITION3)) {
		float positionOutput3 = PID_POSITION3.myOutput;
		PID_SPEED3.mySetpoint = positionOutput3;

		int cmdMotor3;

		if (PID_Compute_Speed(&PID_SPEED3)) {
			cmdMotor3 = PID_SPEED3.myOutput;
			if (cmdMotor3 > 0) {
				MotorSetDirection(3, CLOCK);
			} else if (cmdMotor3 < 0) {
				MotorSetDirection(3, COUNTER_CLOCK);
				cmdMotor3 = -cmdMotor3;
			} else {
				speedDirection3 = SPEED_DIRECTION_NONE;
				MotorSetDirection(3, BRAKE_V);
			}

			MotorSetSpeed(3, cmdMotor3);
			PID_SetMode(&PID_SPEED3, PID_Mode_Manual);
			PID_SetMode(&PID_POSITION3, PID_Mode_Manual);

			/*char numberString[MAX_DISPLAY_CHARACTERS];
			 sprintf(numberString, "%d", numberOfSpeedEdges3);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(0, 0, numberString);
			 sprintf(numberString, "%d", cmdMotor3);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(3, 0, numberString);*/
		}
	}

	if (PID_Compute_Position(&PID_POSITION4)) {
		float positionOutput4 = PID_POSITION4.myOutput;
		PID_SPEED4.mySetpoint = positionOutput4;

		int cmdMotor4;

		if (PID_Compute_Speed(&PID_SPEED4)) {
			cmdMotor4 = PID_SPEED4.myOutput;
			if (cmdMotor4 > 0) {
				MotorSetDirection(4, CLOCK);
			} else if (cmdMotor4 < 0) {
				MotorSetDirection(4, COUNTER_CLOCK);
				cmdMotor4 = -cmdMotor4;
			} else {
				speedDirection4 = SPEED_DIRECTION_NONE;
				MotorSetDirection(4, BRAKE_V);

			}

			MotorSetSpeed(4, cmdMotor4);
			PID_SetMode(&PID_SPEED4, PID_Mode_Manual);
			PID_SetMode(&PID_POSITION4, PID_Mode_Manual);

			/*char numberString[MAX_DISPLAY_CHARACTERS];
			 sprintf(numberString, "%d", numberOfSpeedEdges4);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(0, 0, numberString);
			 sprintf(numberString, "%d", cmdMotor4);
			 cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
			 TM_HD44780_Puts(3, 0, numberString);*/
		}
	}
}

void computeCustomPIDS(uint8_t *mainState) {
	if (isMoveDone == 0) {

		// Apply command for motor 1
		if (PID_Compute_Speed(&PID_SPEED1)) {
			FloatType cmdMotor = PID_SPEED1.myOutput;
			MotorSetSpeed(1, (uint8_t) cmdMotor);
			PID_SetMode(&PID_SPEED1, PID_Mode_Manual);
		}
		// Apply command for motor 2
		if (PID_Compute_Speed(&PID_SPEED2)) {
			FloatType cmdMotor = PID_SPEED2.myOutput;
			MotorSetSpeed(2, (uint8_t) cmdMotor);
			PID_SetMode(&PID_SPEED2, PID_Mode_Manual);
		}
		// Apply command for motor 3
		if (PID_Compute_Speed(&PID_SPEED3)) {
			FloatType cmdMotor = PID_SPEED3.myOutput;
			MotorSetSpeed(3, (uint8_t) cmdMotor);
			PID_SetMode(&PID_SPEED3, PID_Mode_Manual);
		}
		// Apply command for motor 4
		if (PID_Compute_Speed(&PID_SPEED4)) {
			FloatType cmdMotor = PID_SPEED4.myOutput;
			MotorSetSpeed(4, (uint8_t) cmdMotor);
			PID_SetMode(&PID_SPEED4, PID_Mode_Manual);
		}

		FloatType xTravelledDistance = calculatePosition(
				(numberOfPositionEdges1 + numberOfPositionEdges3) / 2);

		if (isMovingX
				&& (isSetpointReached(xMoveSetpoint, xTravelledDistance)
						|| areWheelsStopped())) {
			isMovingX = 0;
			isMoveDone = 1;
			stopMove();
			setState(mainState, MAIN_IDLE);
		}

		FloatType yTravelledDistance = calculatePosition(
				(numberOfPositionEdges2 + numberOfPositionEdges4) / 2);

		if (isMovingY
				&& (isSetpointReached(yMoveSetpoint, yTravelledDistance)
						|| areWheelsStopped())) {
			isMovingY = 0;
			isMoveDone = 1;
			stopMove();
			setState(mainState, MAIN_IDLE);
		}

		FloatType rotateTravelledDistance = calculatePosition(
				(-numberOfPositionEdges1 - numberOfPositionEdges2
						+ numberOfPositionEdges3 + numberOfPositionEdges4) / 4);

		if (isMovingRotate
				&& (isSetpointReached(rotateMoveSetpoint,
						rotateTravelledDistance) || areWheelsStopped())) {
			isMovingRotate = 0;
			isMoveDone = 1;
			stopMove();
			setState(mainState, MAIN_IDLE);
		}
	}
}

void computeSpeedPIDS() {

	if (PID_Compute_Speed(&PID_SPEED1)) {
		int cmdMotor1 = PID_SPEED1.myOutput;

		MotorSetSpeed(1, cmdMotor1);
		PID_SetMode(&PID_SPEED1, PID_Mode_Manual);
	}

	if (PID_Compute_Speed(&PID_SPEED2)) {
		int cmdMotor2 = PID_SPEED2.myOutput;

		MotorSetSpeed(2, cmdMotor2);
		PID_SetMode(&PID_SPEED2, PID_Mode_Manual);
	}

	if (PID_Compute_Speed(&PID_SPEED3)) {
		int cmdMotor3 = PID_SPEED3.myOutput;

		MotorSetSpeed(3, cmdMotor3);
		PID_SetMode(&PID_SPEED3, PID_Mode_Manual);
	}

	if (PID_Compute_Speed(&PID_SPEED4)) {
		int cmdMotor4 = PID_SPEED4.myOutput;
		MotorSetSpeed(4, cmdMotor4);
		PID_SetMode(&PID_SPEED4, PID_Mode_Manual);
	}
}

uint8_t isSetpointReached(float setPoint, FloatType travelledDistance) {
	return setPoint - travelledDistance < 0.001
			|| travelledDistance - setPoint < 0.001;
}

uint8_t areWheelsStopped() {
	return ((PID_SPEED1.myInput == 0 && PID_SPEED2.myInput == 0)
			&& (PID_SPEED3.myInput == 0 && PID_SPEED4.myInput == 0));
}
