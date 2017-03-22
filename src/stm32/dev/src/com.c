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

void sendRedLightConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_RED_LIGHT_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendGreenLightConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_GREEN_LIGHT_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendMoveUpConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_MOVE_UP_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendMoveDownConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_MOVE_DOWN_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendStopDecodeManchesterConfirmation() {
	uint8_t dataToSend[2];
	dataToSend[0] = COMMAND_STOP_DECODE_MANCHESTER_CONFIRMATION;
	dataToSend[1] = 0;
	VCP_DataTx(dataToSend, 2);
}

void sendDecodedManchesterCode(uint8_t manchesterFigureVerification,
		uint8_t manchesterFactorVerification,
		uint8_t manchesterOrientationVerification) {

	uint8_t dataToSend[5];
	dataToSend[0] = COMMAND_DECODED_MANCHESTER;
	dataToSend[1] = (uint8_t) 3;
	dataToSend[2] = (uint8_t) 1;
	dataToSend[3] = (uint8_t) 4;
	dataToSend[4] = (uint8_t) 'N';

	VCP_DataTx(dataToSend, 5);
}

void sendPositionEncoderTicks(int numberOfPositionEdges1,
		int numberOfPositionEdges2, int numberOfPositionEdges3,
		int numberOfPositionEdges4, int numberOfSpeedEdges1,
		int numberOfSpeedEdges2, int numberOfSpeedEdges3,
		int numberOfSpeedEdges4) {

	uint8_t dataToSend[5];
	dataToSend[0] = COMMAND_SEND_POSITION_TICKS;
	dataToSend[1] = (uint8_t) 33;

	dataToSend[2] = (uint8_t)(numberOfPositionEdges1 >> 0);
	dataToSend[3] = (uint8_t)(numberOfPositionEdges1 >> 8);
	dataToSend[4] = (uint8_t)(numberOfPositionEdges1 >> 16);
	dataToSend[5] = (uint8_t)(numberOfPositionEdges1 >> 24);

	dataToSend[6] = (uint8_t)(numberOfPositionEdges2 >> 0);
	dataToSend[7] = (uint8_t)(numberOfPositionEdges2 >> 8);
	dataToSend[8] = (uint8_t)(numberOfPositionEdges2 >> 16);
	dataToSend[9] = (uint8_t)(numberOfPositionEdges2 >> 24);

	dataToSend[10] = (uint8_t)(numberOfPositionEdges3 >> 0);
	dataToSend[11] = (uint8_t)(numberOfPositionEdges3 >> 8);
	dataToSend[12] = (uint8_t)(numberOfPositionEdges3 >> 16);
	dataToSend[13] = (uint8_t)(numberOfPositionEdges3 >> 24);

	dataToSend[14] = (uint8_t)(numberOfPositionEdges4 >> 0);
	dataToSend[15] = (uint8_t)(numberOfPositionEdges4 >> 8);
	dataToSend[16] = (uint8_t)(numberOfPositionEdges4 >> 16);
	dataToSend[17] = (uint8_t)(numberOfPositionEdges4 >> 24);

	dataToSend[18] = (uint8_t)(numberOfSpeedEdges1 >> 0);
	dataToSend[19] = (uint8_t)(numberOfSpeedEdges1 >> 8);
	dataToSend[20] = (uint8_t)(numberOfSpeedEdges1 >> 16);
	dataToSend[21] = (uint8_t)(numberOfSpeedEdges1 >> 24);

	dataToSend[22] = (uint8_t)(numberOfSpeedEdges2 >> 0);
	dataToSend[23] = (uint8_t)(numberOfSpeedEdges2 >> 8);
	dataToSend[24] = (uint8_t)(numberOfSpeedEdges2 >> 16);
	dataToSend[25] = (uint8_t)(numberOfSpeedEdges2 >> 24);

	dataToSend[26] = (uint8_t)(numberOfSpeedEdges3 >> 0);
	dataToSend[27] = (uint8_t)(numberOfSpeedEdges3 >> 8);
	dataToSend[28] = (uint8_t)(numberOfSpeedEdges3 >> 16);
	dataToSend[29] = (uint8_t)(numberOfSpeedEdges3 >> 24);

	dataToSend[30] = (uint8_t)(numberOfSpeedEdges4 >> 0);
	dataToSend[31] = (uint8_t)(numberOfSpeedEdges4 >> 8);
	dataToSend[32] = (uint8_t)(numberOfSpeedEdges4 >> 16);
	dataToSend[33] = (uint8_t)(numberOfSpeedEdges4 >> 24);

	VCP_DataTx(dataToSend, 35);
}
