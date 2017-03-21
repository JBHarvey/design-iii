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

void setMoveSettings(uint8_t *data);

void setRotateSettings(uint8_t *data);

#endif /* MOTION_H_ */
