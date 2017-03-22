#ifndef COM_H_
#define COM_H_

#include "stm32f4xx.h"
#include "tm_stm32f4_usb_vcp.h"

enum COMMAND {
	COMMAND_MOVE = 0,
	COMMAND_START_IDEN_WHEELS = 65,
	COMMAND_IDEN_WHEELS = 66,
	COMMAND_START_TEST_PID_SPEED = 67,
	COMMAND_SEND_PID_SPEED = 68,
	COMMAND_ROTATE = 1,
	COMMAND_TURN_RED_LED_ON = 2,
	COMMAND_TURN_GREEN_LED_ON = 3,
	COMMAND_PENCIL_UP = 4,
	COMMAND_PENCIL_DOWN = 5,
	COMMAND_DECODE_MANCHESTER = 6,
	COMMAND_STOP_DECODE_MANCHESTER = 7,
	COMMAND_IDLE = 8,
	COMMAND_SEND_POSITION_TICKS = 100,
	COMMAND_RED_LIGHT_CONFIRMATION = 102,
	COMMAND_GREEN_LIGHT_CONFIRMATION = 103,
	COMMAND_MOVE_UP_CONFIRMATION = 104,
	COMMAND_MOVE_DOWN_CONFIRMATION = 105,
	COMMAND_DECODED_MANCHESTER = 106,
	COMMAND_STOP_DECODE_MANCHESTER_CONFIRMATION = 107
};

extern uint8_t bFlagSendData;

uint8_t readUSB();
void sendRedLightConfirmation();
void sendGreenLightConfirmation();
void sendMoveUpConfirmation();
void sendMoveDownConfirmation();
void sendStopDecodeManchesterConfirmation();

void sendPositionEncoderTicks(int numberOfPositionEdges1,
		int numberOfPositionEdges2, int numberOfPositionEdges3,
		int numberOfPositionEdges4, int numberOfSpeedEdges1,
		int numberOfSpeedEdges2, int numberOfSpeedEdges3,
		int numberOfSpeedEdges4);

#endif
