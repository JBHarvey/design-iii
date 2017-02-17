/*
 * manchester.c
 *
 *  Created on: Feb 17, 2017
 *      Author: jesussoldier
 */

#include "manchester.h"

uint8_t isNextTwoBitsTheBeginningOfNewCycle(uint8_t index,
		uint8_t *manchesterBuffer) {
	return manchesterBuffer[index] == 0 && manchesterBuffer[index + 1] == 1;
}

uint8_t findIndexOfBeginningCycle(uint16_t analyserIndex,
		uint8_t *manchesterBuffer) {

	uint8_t i;
	for (i = 0; i < NUMBER_OF_END_CYCLE_BITS; i++) {
		if (manchesterBuffer[analyserIndex + i] == i % 2) {
			return 0;
		}
	}

	if (isNextTwoBitsTheBeginningOfNewCycle(analyserIndex + i,
			manchesterBuffer)) {
		return analyserIndex + i;
	}
	return 0;
}

uint8_t findAValidCycle(uint8_t *manchesterBuffer) {
	uint8_t analyserIndex = 0;
	uint8_t beginningCycleIndex = 0;
	while (analyserIndex < MANCHESTER_BUFFER_LENGTH && beginningCycleIndex == 0) {
		beginningCycleIndex = findIndexOfBeginningCycle(analyserIndex,
				manchesterBuffer);
		analyserIndex++;
	}
	return beginningCycleIndex;
}

uint8_t isNextBitsEqualToZero(uint8_t index, uint8_t *manchesterBuffer) {
	return manchesterBuffer[index] == 0 && manchesterBuffer[index + 1] == 1;
}

uint8_t isNextBitsEqualToOne(uint8_t index, uint8_t *manchesterBuffer) {
	return manchesterBuffer[index] == 1 && manchesterBuffer[index + 1] == 0;
}

uint8_t decodeInformationBits(uint8_t indexOfBeginningCycle,
		uint8_t *informationBits, uint8_t *manchesterBuffer) {
	for (uint8_t i = 0; i < INFORMATION_BITS_LENGTH; i++) {
		if (isNextBitsEqualToZero(indexOfBeginningCycle + 2 * i,
				manchesterBuffer)) {
			informationBits[i] = 0;
		} else if (isNextBitsEqualToOne(indexOfBeginningCycle + 2 * i,
				manchesterBuffer)) {
			informationBits[i] = 1;
		} else {
			return BAD_INFORMATION;
		}
	}
	return VALID_INFORMATION;
}

uint8_t decodeManchester(uint8_t * informationBits, uint8_t *manchesterBuffer) {
	uint8_t indexOfBeginningCycle = findAValidCycle(manchesterBuffer);
	if (indexOfBeginningCycle != 0) {
		uint8_t informationState = decodeInformationBits(indexOfBeginningCycle,
				informationBits, manchesterBuffer);
		return informationState;
	}
	return BAD_INFORMATION;
}

uint8_t getFigureFromInformationBits(uint8_t * informationBits) {
	return informationBits[1] * 4 + informationBits[2] * 2 + informationBits[3];
}

uint8_t getOrientationFromInformationBits(uint8_t * informationBits) {
	return informationBits[4] * 2 + informationBits[5]; //  en char
}

uint8_t getFactorFromInformationBits(uint8_t * informationBits) {
	if (informationBits[6] == 0) {
		return FACTOR_2X;
	} else {
		return FACTOR_4X;
	}
}

void displayManchesterInformation(uint8_t figure, uint8_t orientation,
		uint8_t factor) {
}
