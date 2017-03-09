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
	// Enable the clock for GPIOC
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// Init the struct to initialize the pin
	GPIO_InitTypeDef gpioStructure;
	// Set the pin number to be used
	gpioStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	// Set the direction of the pin, OUTPUT in this case
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	// Set the refresh speed, speed doesn't matter in this case
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// Call the initialization
	GPIO_Init(GPIOC, &gpioStructure);
	// Set the BIT to zero to have LED turned down by default
	turnOffLEDs();
}

/* Turn on the RED LED for signal
 * and asset that all the light are off before
 */
void turnOnRedLED() {
	turnOffLEDs();
GPIO_SetBits(GPIOC GPIO_Pin_4);
}

/* Turn on the Green LED for signal
 * and asset that all the LEDs are off before
 */
void turnOnGreenLED() {
turnOffLEDs();
GPIO_SetBits(GPIOC, GPIO_Pin_5);
}

// Turn off the LEDs for signals
void turnOffLEDs() {
GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}
