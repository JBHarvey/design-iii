/*
 * leds.h
 *
 *  Created on: Mar 7, 2017
 *      Author: jesussoldier
 */

#ifndef LEDS_H_
#define LEDS_H_

#include "stm32f4xx.h"

void InitializeLEDs();
void initLEDsSignals();
void turnOnRedLED();
void turnOnGreenLED();
void turnOffLEDs();
#endif /* LEDS_H_ */
