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

/********************************************************
 * param: manchesterBuffer contenant les bits doubles
 * return l'index d'un début de cycle.
 *
 *********************************************************/
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

/*******************************************************************
 * param: informationBits : buffer contenant les bits simples
 * param: manchesterBuffer: buffer contenant les bits doubles captés par la pin.
 * return: VALID_INFORMATION OU BAD_INFORMATION
 *
 *******************************************************************/

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

void setOrientationFromInformationBits(uint8_t * informationBits,
		char *orientation) {
	uint8_t orientationNumber = informationBits[4] * 2 + informationBits[5];
	switch (orientationNumber) {
	case 0:
		orientation[0] = ' ';
		orientation[1] = 'N';
		orientation[2] = 'o';
		orientation[3] = 'r';
		orientation[4] = 'd';
		break;
	case 1:
		orientation[0] = ' ';
		orientation[1] = ' ';
		orientation[2] = 'E';
		orientation[3] = 's';
		orientation[4] = 't';
		break;
	case 2:
		orientation[0] = ' ';
		orientation[1] = ' ';
		orientation[2] = 'S';
		orientation[3] = 'u';
		orientation[4] = 'd';
		break;

	case 3:
		orientation[0] = 'O';
		orientation[1] = 'u';
		orientation[2] = 'e';
		orientation[3] = 's';
		orientation[4] = 't';
		break;
	default:
		orientation[0] = ' ';
		orientation[1] = ' ';
		orientation[2] = ' ';
		orientation[3] = ' ';
		orientation[4] = ' ';
	}
}

uint8_t getFactorFromInformationBits(uint8_t * informationBits) {
	if (informationBits[6] == 0) {
		return FACTOR_2X;
	} else {
		return FACTOR_4X;
	}
}

void setMessageToDisplay(uint8_t figure, char *orientation, uint8_t factor,
		char *messageToDisplay) {
	messageToDisplay[0] = '0' + figure;
	messageToDisplay[1] = ' ';
	messageToDisplay[2] = orientation[0];
	messageToDisplay[3] = orientation[1];
	messageToDisplay[4] = orientation[2];
	messageToDisplay[5] = orientation[3];
	messageToDisplay[6] = orientation[4];
	messageToDisplay[7] = ' ';
	messageToDisplay[8] = '0' + factor;
	messageToDisplay[9] = 'X';
	messageToDisplay[10] = ' ';
	messageToDisplay[11] = ' ';
	messageToDisplay[12] = ' ';
	messageToDisplay[13] = ' ';
	messageToDisplay[14] = ' ';
	messageToDisplay[15] = ' ';
}

void displayManchesterMessage(char *messageToDisplay) {
	TM_HD44780_Init(16, 2);
	TM_HD44780_Clear();
	TM_HD44780_Puts(0, 1, messageToDisplay);
	TM_HD44780_Puts(0, 0, "Manchester code");
}

uint8_t isFigureEqual(uint8_t *figure, uint8_t *figureVerification) {
	return *figure == *figureVerification;
}

uint8_t isOrientationEqual(char *orientation, char *orientationVerification) {
	for (int i = 0; i < ORIENTATION_LENGTH; i++) {
		if (orientation[i] != orientationVerification[i]) {
			return 0;
		}
	}
	return 1;
}

uint8_t isFactorEqual(uint8_t *factor, uint8_t *factorVerification) {
	return *factor == *factorVerification;
}

uint8_t isSameDataThanPreviousIteration(uint8_t *figure,
		uint8_t *figureVerification, char *orientation,
		char *orientationVerification, uint8_t *factor,
		uint8_t *factorVerification) {
	return isFigureEqual(figure, figureVerification)
			&& isOrientationEqual(orientation, orientationVerification)
			&& isFactorEqual(factor, factorVerification);
}

void tryToDecodeManchesterCode(uint8_t *manchesterState,
		uint8_t *manchesterBuffer, uint8_t *manchesterFigureVerification,
		char *manchesterOrientationVerification,
		uint8_t *manchesterFactorVerification) {

	// check if state is ready to decode
	if (*manchesterState == MANCHESTER_DECODE) {

		// buffer contenant l'information compressée 10 devient 1 et 01 devient 0
		uint8_t informationBits[INFORMATION_BITS_LENGTH];

		// décode le code manchester et retourne VALID_INFORMATION si valide
		if (decodeManchester(informationBits, manchesterBuffer)
				== VALID_INFORMATION) {

			*manchesterFigureVerification = getFigureFromInformationBits(
					informationBits);

			setOrientationFromInformationBits(informationBits,
					manchesterOrientationVerification);

			*manchesterFactorVerification = getFactorFromInformationBits(
					informationBits);

			char messageToDisplay[MESSAGE_TO_DISPLAY_LENGTH];

			setMessageToDisplay(*manchesterFigureVerification,
					manchesterOrientationVerification,
					*manchesterFactorVerification, messageToDisplay);

			displayManchesterMessage(messageToDisplay);
			disableExternalInterruptLine4();
			disableTimer5Interrupt();
			*manchesterState = MANCHESTER_IDLE;
			GPIO_SetBits(GPIOD, GPIO_Pin_13);

			// recommence l'acquisition si le code décodé est non-valide
		} else {
			// On désactive l'interruption pour qu'il recommence
			disableTimer5Interrupt();
			*manchesterState = MANCHESTER_WAIT_FOR_DECODING;
			initializeExternalInterruptLine4();
		}
	}
}

// Initiliase l'input pour l'entrée du manchester
void InitializeManchesterInput() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = MANCHESTER_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}
