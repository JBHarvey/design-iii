#ifndef COM_H_
#define COM_H_

#include "stm32f4xx.h"

enum COMMAND {
	COMMAND_MOVE = 0,
	COMMAND_ROTATE = 1,
	COMMAND_PENCIL_UP = 4,
	COMMAND_PENCIL_DOWN = 5,
	COMMAND_DECODE_MANCHESTER = 6,
	COMMAND_STOP_DECODE_MANCHESTER = 7,
	COMMAND_IDEN_WHEELS = 66,
	COMMAND_DECODED_MANCHESTER = 106
};

extern uint8_t bFlagSendData;

#include "stm32f4xx.h"
#include "tm_stm32f4_usb_vcp.h"

uint8_t readUSB();

#endif
