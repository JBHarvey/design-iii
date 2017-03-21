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

void setMoveSettings(uint8_t *data) {

	/* get X Y distances */
	float xMove = getXMoveFromBuffer(data);
	float yMove = getYMoveFromBuffer(data);

	/*set PID setpoints (xMove, yMove);*/
	PID_POSITION1.mySetpoint = xMove;
	PID_POSITION2.mySetpoint = xMove;
	PID_POSITION3.mySetpoint = yMove;
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
