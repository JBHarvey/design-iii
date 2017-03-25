#include "utils.h"

void cleanNumberString(char *numberString, int size) {
	int arrayPosition = size - 1;
	while (numberString[arrayPosition] != 0) {
		numberString[arrayPosition] = ' ';
		arrayPosition--;
	}

	while (numberString[arrayPosition] == 0) {
		numberString[arrayPosition] = ' ';
		arrayPosition--;
	}
	numberString[size - 1] = 0;
}

/* Change the state of the main
 * IN : the new state
 */
void setState(int* state, int newState) {
	switch (newState) {
	case MAIN_IDLE:
		*state = newState;
		TM_HD44780_Puts(0, 0, MSG_IDLE);
		break;
	case MAIN_MANCH:
		*state = newState;
		TM_HD44780_Puts(0, 0, MSG_MANCH);
		break;
	case MAIN_ACQUIS:
		TM_HD44780_Puts(0, 0, "NOT IMPLEMENTED");
		break;
	case MAIN_ACQUIS_ALL:
		*state = MAIN_ACQUIS_ALL;
		break;
	case MAIN_TEST_SPEED_PID:
		*state = MAIN_TEST_SPEED_PID;
		break;
	case MAIN_TEST_POS_PID:
		*state = MAIN_TEST_POS_PID;
		break;
	case MAIN_TEST_DEAD_ZONE:
		*state = MAIN_TEST_DEAD_ZONE;
		break;
	case MAIN_TEST_ROTATION:
		*state = MAIN_TEST_ROTATION;
		break;
	case MAIN_MOVE:
		*state = MAIN_MOVE;
		TM_HD44780_Puts(0, 0, "MOVE");
		break;
	case MAIN_PID:
		*state = newState;
		TM_HD44780_Puts(0, 0, "NOT IMPLEMENTED");
		break;
	default:
		// Invalid state
		TM_HD44780_Puts(0, 0, MSG_ERR);
		break;
	}
}

float sqroot(float square) {
	float root = square / 3;
	int i;
	if (square <= 0)
		return 0;
	for (i = 0; i < 32; i++)
		root = (root + square / root) / 2;
	return root;
}
