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

void initCanal(uint32_t RCCx, uint32_t GPIO_Pinx, GPIO_TypeDef* GPIOx);
void initEncoders();
void initializeExternalInterruptLine5();
void initializeExternalInterruptLine6();

#endif /* ENCODER_H_ */
