/*
 * motion.h
 *
 *  Created on: Mar 20, 2017
 *      Author: jesussoldier
 */

#ifndef MOTION_H_
#define MOTION_H_

#include "stm32f4xx.h"
#include "defines.h"
#include "PID_v1.h"

float getXMoveFromBuffer(uint8_t *data);

float getYMoveFromBuffer(uint8_t *data);

float getRadianMoveFromBuffer(uint8_t *data);

float getMoveFromRadian(float radian);

void setMoveSettings(PidType *pidPosition1, PidType *pidPosition2,
		PidType *pidPosition3, PidType *pidPosition4, PidType *pidSpeed1,
		PidType *pidSpeed2, PidType *pidSpeed3, PidType *pidSpeed4,
		uint8_t *data);

#endif /* MOTION_H_ */
