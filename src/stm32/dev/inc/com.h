#ifndef COM_H_
#define COM_H_

#include "stm32f4xx.h"

enum COMMAND {
	COMMAND_MOVE = 0,
	COMMAND_START_IDEN_WHEELS = 65,
	COMMAND_IDEN_WHEELS = 66,
	COMMAND_START_TEST_PID_SPEED = 67,
	COMMAND_SEND_PID_SPEED = 68
};

extern uint8_t bFlagSendData;

#include "stm32f4xx.h"
#include "tm_stm32f4_usb_vcp.h"

uint8_t readUSB();

#endif
