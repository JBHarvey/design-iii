/*
 * manchester.h
 *
 *  Created on: Feb 17, 2017
 *      Author: jesussoldier
 */

#ifndef MANCHESTER_H_
#define MANCHESTER_H_

#include "misc.h"

enum ManchState {
	MANCHESTER_IDLE,
	MANCHESTER_WAIT_FOR_DECODING,
	MANCHESTER_FIRST,
	MANCHESTER_FILL,
	MANCHESTER_DECODE,
	MANCHESTER_VERIFICATION,
	MANCHESTER_ERREUR
};

enum InformationBitsState {
	BAD_INFORMATION, VALID_INFORMATION
};

// Taille du buffer en bit (2 bits par bit manchester)
#define MANCHESTER_BUFFER_LENGTH 64
#define MANCHESTER_PERIOD 125
#define MANCHESTER_PRESCALER 84-1 // Donne 500k
#define MANCHESTER_PIN GPIO_Pin_7
#define NUMBER_OF_END_CYCLE_BITS 16
#define MANCHESTER_CYCLE_LENGTH 32
#define INFORMATION_BITS_LENGTH 8
#define FACTOR_2X 2
#define FACTOR_4X 4
#define ORIENTATION_LENGTH 5
#define MESSAGE_TO_DISPLAY_LENGTH 10

uint8_t isNextTwoBitsTheBeginningOfNewCycle(uint8_t index,
		uint8_t *manchesterBuffer);

uint8_t findIndexOfBeginningCycle(uint16_t analyserIndex,
		uint8_t *manchesterBuffer);

uint8_t findAValidCycle(uint8_t *manchesterBuffer);

uint8_t isNextBitsEqualToZero(uint8_t index, uint8_t *manchesterBuffer);

uint8_t isNextBitsEqualToOne(uint8_t index, uint8_t *manchesterBuffer);

uint8_t decodeInformationBits(uint8_t indexOfBeginningCycle,
		uint8_t *informationBits, uint8_t *manchesterBuffer);

uint8_t decodeManchester(uint8_t * informationBits, uint8_t *manchesterBuffer);

uint8_t getFigureFromInformationBits(uint8_t * informationBits);

void setOrientationFromInformationBits(uint8_t * informationBits,
		char *orientation);

uint8_t getFactorFromInformationBits(uint8_t * informationBits);

void setMessageToDisplay(uint8_t figure, char *orientation, uint8_t factor,
		char *messageToDisplay);

void displayManchesterMessage(char *messageToDisplay);

#endif /* MANCHESTER_H_ */
