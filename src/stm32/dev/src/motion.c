/*
 * motion.c
 *
 *  Created on: Mar 20, 2017
 *      Author: jesussoldier
 */

#include "motion.h"

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

void setMoveSettings(PidType *pidPosition1, PidType *pidPosition2,
		PidType *pidPosition3, PidType *pidPosition4, PidType *pidSpeed1,
		PidType *pidSpeed2, PidType *pidSpeed3, PidType *pidSpeed4,
		uint8_t *data) {

	/* get X Y distances */
	float xMove = getXMoveFromBuffer(data);
	float yMove = getYMoveFromBuffer(data);

	/*set PID setpoints (xMove, yMove);*/
	pidPosition1->mySetpoint = xMove;
	pidPosition2->mySetpoint = xMove;
	pidPosition3->mySetpoint = yMove;
	pidPosition4->mySetpoint = yMove;

	// reset pid values
	pidPosition1->ITerm = 0;
	pidSpeed1->ITerm = 0;
	pidPosition2->ITerm = 0;
	pidSpeed2->ITerm = 0;
	pidPosition3->ITerm = 0;
	pidSpeed3->ITerm = 0;
	pidPosition4->ITerm = 0;
	pidSpeed4->ITerm = 0;

	pidPosition1->lastInput = 0;
	pidSpeed1->lastInput = 0;
	pidPosition2->lastInput = 0;
	pidSpeed2->lastInput = 0;
	pidPosition3->lastInput = 0;
	pidSpeed3->lastInput = 0;
	pidPosition4->lastInput = 0;
	pidSpeed4->lastInput = 0;
}

void setRotateSettings(PidType *pidPosition1, PidType *pidPosition2,
		PidType *pidPosition3, PidType *pidPosition4, PidType *pidSpeed1,
		PidType *pidSpeed2, PidType *pidSpeed3, PidType *pidSpeed4,
		uint8_t *data) {
	float radianToRotate = getRadianMoveFromBuffer(data);
	float move = getMoveFromRadian(radianToRotate);

	/*setPIDCoeficient(xMove, yMove);*/

	pidPosition1->mySetpoint = -move;
	pidPosition2->mySetpoint = move;
	pidPosition3->mySetpoint = -move;
	pidPosition4->mySetpoint = move;

	pidPosition1->ITerm = 0;
	pidSpeed1->ITerm = 0;
	pidPosition2->ITerm = 0;
	pidSpeed2->ITerm = 0;
	pidPosition3->ITerm = 0;
	pidSpeed3->ITerm = 0;
	pidPosition4->ITerm = 0;
	pidSpeed4->ITerm = 0;

	pidPosition1->lastInput = 0;
	pidSpeed1->lastInput = 0;
	pidPosition1->lastInput = 0;
	pidSpeed2->lastInput = 0;
	pidPosition1->lastInput = 0;
	pidSpeed3->lastInput = 0;
	pidPosition1->lastInput = 0;
	pidSpeed4->lastInput = 0;
}
