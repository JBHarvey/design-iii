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

void sendMoveMeasures(float positionX, float positionY, float speedX,
		float speedY, float radian, float radianSpeed) {
	uint8_t dataToSend[18];
	uint8_t *dataToSendPointer = dataToSend;

	if (isRobotRotating) {

		dataToSend[0] = COMMAND_SEND_ROTATION_MEASURES;

		dataToSend[1] = 8;
		dataToSendPointer += 2;

		memcpy(dataToSendPointer, &radian, 4);
		dataToSendPointer += 4;

		memcpy(dataToSendPointer, &radianSpeed, 4);
		dataToSendPointer += 4;

		/* il faut envoyer les données par USART, sinon, ca crash ici. */
		VCP_DataTx(dataToSend, 10);

	} else {
		dataToSend[0] = COMMAND_SEND_MOVE_MEASURES;

		dataToSend[1] = 16;
		dataToSendPointer += 2;

		memcpy(dataToSendPointer, &positionX, 4);
		dataToSendPointer += 4;

		memcpy(dataToSendPointer, &positionY, 4);
		dataToSendPointer += 4;

		memcpy(dataToSendPointer, &speedX, 4);
		dataToSendPointer += 4;

		memcpy(dataToSendPointer, &speedY, 4);
		dataToSendPointer += 4;

		/* il faut envoyer les données par USART, sinon, ca crash ici. */
		VCP_DataTx(dataToSend, 18);
	}
}

void sendRedLightConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_SEND_RED_LED_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendGreenLightConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_SEND_GREEN_LED_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendPrehensorUpConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_SEND_PREHENSOR_UP_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendPrehensorDownConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_SEND_PREHENSOR_DOWN_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendStopSendingManchesterSignalConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_SEND_PREHENSOR_DOWN_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendManchesterCode(uint8_t figure, uint8_t factor, uint8_t *orientation) {
	uint8_t dataToSend[5];
	dataToSend[0] = COMMAND_DECODED_MANCHESTER;
	dataToSend[1] = 3;
	dataToSend[2] = figure;
	dataToSend[3] = factor;

	switch (orientation[2]) {
	case 'o':
		dataToSend[4] = 'N';
		break;
	case 'E':
		dataToSend[4] = 'E';
		break;
	case 'S':
		dataToSend[4] = 'S';
		break;
	case 'e':
		dataToSend[4] = 'W';
		break;
	}

	//dataToSend[4] = 'N';

	VCP_DataTx(dataToSend, 5);
}

void sendInfraredMesasures(float infraredCaptor1, float infraredCaptor2,
		float infraredCaptor3) {

	uint16_t dataToSend[14];
	uint16_t *dataToSendPointer = dataToSend;

	dataToSend[0] = COMMAND_SEND_INFRARED_CAPTORS;

	dataToSend[1] = 12;
	dataToSendPointer += 2;

	memcpy(dataToSendPointer, &infraredCaptor1, sizeof(float));
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &infraredCaptor2, sizeof(float));
	dataToSendPointer += 4;

	memcpy(dataToSendPointer, &infraredCaptor3, sizeof(float));
	dataToSendPointer += 4;

	/* il faut envoyer les données par USART, sinon, ca crash ici. */
	VCP_DataTx(dataToSend, 14);

}

