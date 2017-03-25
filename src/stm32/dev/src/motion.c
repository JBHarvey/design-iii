/*
 * motion.c
 *
 *  Created on: Mar 20, 2017
 *      Author: jesussoldier
 */

#include "motion.h"

uint8_t isRobotRotating = 0;

float getXMoveFromBuffer(uint8_t *data) {
	float xMove = *(float *) data;
	return xMove;
}

float getYMoveFromBuffer(uint8_t *data) {
	float yMove = *(float *) (data + Y_MOVE_DATA_BUFFER_INDEX);
	return yMove;
}

float getRadianMoveFromBuffer(uint8_t *data) {
	float radian = *(float *) data;
	return radian;
}

float getMoveFromRadian(float radian) {
	return radian * WHEEL_RADIUS;
}

void setMoveSettings(uint8_t *data) {

	/* get X Y distances */
	float xMove = getXMoveFromBuffer(data);
	float yMove = getYMoveFromBuffer(data);

	/*set PID setpoints (xMove, yMove);*/
	PID_POSITION1.mySetpoint = xMove;
	PID_POSITION2.mySetpoint = yMove;
	PID_POSITION3.mySetpoint = xMove;
	PID_POSITION4.mySetpoint = yMove;

	// reset pid values
	PID_POSITION1.ITerm = 0;
	PID_SPEED1.ITerm = 0;
	PID_POSITION2.ITerm = 0;
	PID_SPEED2.ITerm = 0;
	PID_POSITION3.ITerm = 0;
	PID_SPEED3.ITerm = 0;
	PID_POSITION4.ITerm = 0;
	PID_SPEED4.ITerm = 0;

	PID_POSITION1.lastInput = 0;
	PID_SPEED1.lastInput = 0;
	PID_POSITION2.lastInput = 0;
	PID_SPEED2.lastInput = 0;
	PID_POSITION3.lastInput = 0;
	PID_SPEED3.lastInput = 0;
	PID_POSITION4.lastInput = 0;
	PID_SPEED4.lastInput = 0;
}

void setRotateSettings(uint8_t *data) {
	float radianToRotate = getRadianMoveFromBuffer(data);
	float move = getMoveFromRadian(radianToRotate);

	/*setPIDCoeficient(xMove, yMove);*/

	PID_POSITION1.mySetpoint = -move;
	PID_POSITION2.mySetpoint = move;
	PID_POSITION3.mySetpoint = -move;
	PID_POSITION4.mySetpoint = move;

	PID_POSITION1.ITerm = 0;
	PID_SPEED1.ITerm = 0;
	PID_POSITION2.ITerm = 0;
	PID_SPEED2.ITerm = 0;
	PID_POSITION3.ITerm = 0;
	PID_SPEED3.ITerm = 0;
	PID_POSITION4.ITerm = 0;
	PID_SPEED4.ITerm = 0;

	PID_POSITION1.lastInput = 0;
	PID_SPEED1.lastInput = 0;
	PID_POSITION2.lastInput = 0;
	PID_SPEED2.lastInput = 0;
	PID_POSITION3.lastInput = 0;
	PID_SPEED3.lastInput = 0;
	PID_POSITION4.lastInput = 0;
	PID_SPEED4.lastInput = 0;
}

void setCustomMoveSettings(uint8_t *data) {

	isRobotRotating = 0;

	/* get X Y distances */
	xMoveSetpoint = getXMoveFromBuffer(data);
	yMoveSetpoint = getYMoveFromBuffer(data);

	// reset pid values
	PID_SPEED1.ITerm = 0;
	PID_SPEED2.ITerm = 0;
	PID_SPEED3.ITerm = 0;
	PID_SPEED4.ITerm = 0;

	PID_SPEED1.lastInput = 0;
	PID_SPEED2.lastInput = 0;
	PID_SPEED3.lastInput = 0;
	PID_SPEED4.lastInput = 0;

	// On defini la direction des roues
	if (xMoveSetpoint > 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(3, CLOCK);
	} else if (xMoveSetpoint < 0) {
		MotorSetDirection(1, CLOCK);
		MotorSetDirection(3, COUNTER_CLOCK);
	} else {
		MotorSetDirection(1, BRAKE_G);
		MotorSetDirection(3, BRAKE_G);
	}
	//
	if (yMoveSetpoint > 0) {
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(4, CLOCK);
	} else if (yMoveSetpoint < 0) {
		MotorSetDirection(2, CLOCK);
		MotorSetDirection(4, COUNTER_CLOCK);
	} else {
		MotorSetDirection(2, BRAKE_G);
		MotorSetDirection(4, BRAKE_G);
	}
}

void setCustomRotateSettings(uint8_t *data) {
	isRobotRotating = 1;

	/* get X Y distances */
	float radianToRotate = getRadianMoveFromBuffer(data);
	rotateMoveSetpoint = getMoveFromRadian(radianToRotate);

	// reset pid values
	PID_SPEED1.ITerm = 0;
	PID_SPEED2.ITerm = 0;
	PID_SPEED3.ITerm = 0;
	PID_SPEED4.ITerm = 0;

	PID_SPEED1.lastInput = 0;
	PID_SPEED2.lastInput = 0;
	PID_SPEED3.lastInput = 0;
	PID_SPEED4.lastInput = 0;

	// On defini la direction des roues 1 et 3
	if (rotateMoveSetpoint > 0) {
		MotorSetDirection(1, CLOCK);
		MotorSetDirection(3, CLOCK);
	} else if (rotateMoveSetpoint < 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(3, COUNTER_CLOCK);
	} else {
		MotorSetDirection(1, BRAKE_G);
		MotorSetDirection(3, BRAKE_G);
	}
	// On defini la direction des roues 2 et 4
	if (rotateMoveSetpoint > 0) {
		MotorSetDirection(2, CLOCK);
		MotorSetDirection(4, CLOCK);
	} else if (rotateMoveSetpoint < 0) {
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(4, COUNTER_CLOCK);
	} else {
		MotorSetDirection(2, BRAKE_G);
		MotorSetDirection(4, BRAKE_G);
	}
}

void setSpeedSetpoints() {
	if (isRobotRotating) {

		float deplacementY = calculatePosition(
				(-numberOfPositionEdges2 + numberOfPositionEdges4) / 2);
		float deplacementX = calculatePosition(
				(-numberOfPositionEdges1 + numberOfPositionEdges3) / 2);

		if ((rotateMoveSetpoint != 0 && deplacementY < (rotateMoveSetpoint / 2))
				|| (rotateMoveSetpoint != 0
						&& deplacementX < (rotateMoveSetpoint / 2))) {

			PID_SPEED1.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED2.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED3.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED4.mySetpoint = CONSIGNE_SPEED_MEDIUM;
		} else {

			PID_SPEED1.mySetpoint = 0;
			PID_SPEED2.mySetpoint = 0;
			PID_SPEED3.mySetpoint = 0;
			PID_SPEED4.mySetpoint = 0;
			isMoving = 1;
		}

	} else {

		float deplacementY = calculatePosition(
				(numberOfPositionEdges2 + numberOfPositionEdges4) / 2);
		float deplacementX = calculatePosition(
				(numberOfPositionEdges1 + numberOfPositionEdges3) / 2);

		if ((yMoveSetpoint != 0 && deplacementY < (yMoveSetpoint / 2))
				|| (xMoveSetpoint != 0 && deplacementX < (xMoveSetpoint / 2))) {

			PID_SPEED1.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED2.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED3.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			PID_SPEED4.mySetpoint = CONSIGNE_SPEED_MEDIUM;
		} else {

			PID_SPEED1.mySetpoint = 0;
			PID_SPEED2.mySetpoint = 0;
			PID_SPEED3.mySetpoint = 0;
			PID_SPEED4.mySetpoint = 0;
			isMoving = 1;
		}
	}
}

void stopMove() {
	MotorSetSpeed(1, 0);
	MotorSetSpeed(2, 0);
	MotorSetSpeed(3, 0);
	MotorSetSpeed(4, 0);
}
