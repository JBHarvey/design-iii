#include <com.h>

uint8_t bFlagSendData = 0;

uint8_t readUSB() {
	/* USB configured OK, drivers OK */
	if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
		/* Turn on GREEN led */
		//TM_DISCO_LedOn (LED_GREEN);
		//TM_DISCO_LedOff (LED_RED);
		uint8_t readCharacter;

		if (TM_USB_VCP_Getc(&readCharacter) == TM_USB_VCP_DATA_OK) {
			//TM_USB_VCP_Putc(readCharacter);
			return readCharacter;
		}
	} else {
		/* USB not OK */
		//TM_DISCO_LedOff (LED_GREEN);
		//TM_DISCO_LedOn (LED_RED);
		return 0;
	}
	return 0;
}

void sendMoveMeasures(int numberOfPositionEdges1, int numberOfPositionEdges2,
		int numberOfPositionEdges3, int numberOfPositionEdges4,
		int numberOfSpeedEdges1, int numberOfSpeedEdges2,
		int numberOfSpeedEdges3, int numberOfSpeedEdges4) {
	uint8_t dataToSend[34];
	uint8_t *dataToSendPointer = dataToSend;

	if (isRobotRotating) {
		dataToSend[0] = COMMAND_SEND_ROTATION_MEASURES;
	} else {
		dataToSend[0] = COMMAND_SEND_MOVE_MEASURES;
	}

	dataToSend[1] = 32;
	dataToSendPointer += 2;

	memcpy(dataToSendPointer, &numberOfPositionEdges1, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfPositionEdges2, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfPositionEdges3, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfPositionEdges4, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfSpeedEdges1, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfSpeedEdges2, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfSpeedEdges3, 4);
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &numberOfSpeedEdges4, 4);
	dataToSendPointer += 4;

	VCP_DataTx(dataToSend, 34);
}

