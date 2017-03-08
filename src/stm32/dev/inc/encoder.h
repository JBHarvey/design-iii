/*
 * encoder.h
 *
 *  Created on: 2017-02-19
 *      Author: Admin
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "defines.h"

void intializePinsD();
void initializeExternalInterruptLine5();
void initializeExternalInterruptLine6();
void intializeExternalInterrupts();
void initEncoders();
uint16_t calculateSpeed(int edges);

#endif /* ENCODER_H_ */
