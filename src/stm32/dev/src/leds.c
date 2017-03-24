/*
 * leds.c
 *
 *  Created on: Mar 7, 2017
 *      Author: jesussoldier
 */

#include "leds.h"

void InitializeLEDs() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);

	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);

	initLEDsSignals();
}

/* Initialize LEDs Red and Green for signals */
/* Red LED 		: PC4
 * Green LED 	: PC5
 */
void initLEDsSignals() {
	GPIO_InitTypeDef GPIO_InitDef;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOC, &GPIO_InitDef);
	turnOffLEDs();
}

/* Turn on the RED LED for signal
 * and asset that all the light are off before
 */
void turnOnRedLED() {
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

/* Turn on the Green LED for signal
 * and asset that all the LEDs are off before
 */
void turnOnGreenLED() {
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}

// Turn off the LEDs for signals
void turnOffLEDs() {
	GPIO_ResetBits(GPIOC, GPIO_Pin_5 | GPIO_Pin_4);
}

/* Routine de test pour les LEDs */
void startLEDsRoutine() {
	Delayms(1000);
	turnOnRedLED();
	Delayms(1000);
	turnOffLEDs();
	Delayms(1000);
	turnOnGreenLED();
	Delayms(1000);
	turnOnRedLED();
	Delayms(1000);
	turnOffLEDs();
}
