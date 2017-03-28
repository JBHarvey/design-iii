#ifndef COM_H_
#define COM_H_

#include "stm32f4xx.h"
#include "motion.h"

enum COMMAND {
	COMMAND_MOVE = 0,
	COMMAND_ROTATE = 1,
	COMMAND_RED_LED = 2,
	COMMAND_GREEN_LED = 3,
	COMMAND_PREHENSOR_UP = 4,
	COMMAND_PREHENSOR_DOWN = 5,
	COMMAND_DECODE_MANCHESTER = 6,
	COMMAND_STOP_DECODE_MANCHESTER = 7,
	COMMAND_SET_SPEED = 8,
	COMMAND_START_IDEN_WHEELS = 65,
	COMMAND_IDEN_WHEELS = 66,
	COMMAND_START_TEST_PID_SPEED = 67,
	COMMAND_SEND_PID_SPEED = 68,
	COMMAND_START_TEST_PID_POS = 70,
	COMMAND_SEND_PID_POS = 71,
	COMMAND_START_TEST_DEAD_ZONE = 72,
	COMMAND_SEND_DEAD_ZONE = 73,
	COMMAND_SEND_MOVE_MEASURES = 100,
	COMMAND_SEND_ROTATION_MEASURES = 101,
	COMMAND_SEND_RED_LED_CONFIRMATION = 102,
	COMMAND_SEND_GREEN_LED_CONFIRMATION = 103,
	COMMAND_SEND_PREHENSOR_UP_CONFIRMATION = 104,
	COMMAND_SEND_PREHENSOR_DOWN_CONFIRMATION = 105,
	COMMAND_DECODED_MANCHESTER = 106,
	COMMAND_STOP_SENDING_MANCHESTER_SIGNAL_CONFIRMATION = 107

};

extern uint8_t bFlagSendData;

#include "stm32f4xx.h"
#include "tm_stm32f4_usb_vcp.h"

uint8_t readUSB();
void sendMoveMeasures(int numberOfPositionEdges1, int numberOfPositionEdges2,
		int numberOfPositionEdges3, int numberOfPositionEdges4,
		int numberOfSpeedEdges1, int numberOfSpeedEdges2,
		int numberOfSpeedEdges3, int numberOfSpeedEdges4);

void sendRedLightConfirmation();

void sendGreenLightConfirmation();

void sendPrehensorUpConfirmation();

void sendPrehensorDownConfirmation();

void sendStopSendingManchesterSignalConfirmation();

void sendManchesterCode(uint8_t figure, uint8_t factor, uint8_t *orientation);

#endif
