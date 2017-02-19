/*
 * encoder.c
 *
 *  Created on: 2017-02-19
 *      Author: Admin
 */

#include "motors.h"

void initCanal(uint32_t RCCx, uint32_t GPIO_Pinx, GPIO_TypeDef* GPIOx) {

	RCC_AHB1PeriphClockCmd(RCCx, ENABLE);
	/* Set pins as input */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pinx;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void initEncoder() {
	initCanal(RCC_AHB1Periph_GPIOD, GPIO_Pin_5 | GPIO_Pin_6, GPIOD);
}
