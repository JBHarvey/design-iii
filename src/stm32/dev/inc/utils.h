#ifndef UTILS_H_
#define UTILS_H_

#include "defines.h"

enum State {
	IDLE,
	GENERATE_FIRST_PWM,
	BETWEEN_PWM,
	ACQUIRE,
	SEND_DATA,
	WAIT_FOR_SECOND_PWM
};

enum MainState {
	MAIN_IDLE,
	MAIN_MANCH,
	MAIN_ACQUIS,
	MAIN_ACQUIS_ALL,
	MAIN_TEST_SPEED_PID,
	MAIN_DEAD_ZONE,
	MAIN_MOVE,
	MAIN_PID,
	MAIN_PREHENSEUR
};

enum SpeedDirection {
	SPEED_DIRECTION_FORWARD, SPEED_DIRECTION_BACKWARD, SPEED_DIRECTION_NONE
};

enum AmountOfCharacters {
	ONE_CHARACTER, MANY_CHARACTERS
};

enum COMMUNICATION_STATUS {
	READ_USB_SUCCESS, READ_USB_FAIL
};

void cleanNumberString(char *numberString, int size);

void setState(int* state, int newState);

#endif
