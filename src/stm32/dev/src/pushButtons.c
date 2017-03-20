/*
 * pushButtons.c
 *
 *  Created on: Mar 7, 2017
 *      Author: jesussoldier
 */
#include "pushButtons.h"

void initBtn() {
	//Enable clock for GPOIA
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitDef;
	//Pin 0
	GPIO_InitDef.GPIO_Pin = GPIO_Pin_0;
	//Mode output
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
	//Output type push-pull
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	//With pull down resistor
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN;
	//50MHz pin speed
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;

	//Initialize pin on GPIOA port
	GPIO_Init(GPIOA, &GPIO_InitDef);
}

/* Return the value of the push button blue with a debounce */
uint8_t getPushBtnStatus() {
	// Vite fait debounce pour le bouton bleu
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
		int i;
		for (i = 0; i < 9000000; i++)
			;
		return 1;
	}
	return 0;
}

