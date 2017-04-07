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
	*state = newState;

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
