#ifndef COM_H_
#define COM_H_

#include "stm32f4xx.h"

enum COMMAND {
	COMMAND_MOVE = 0, COMMAND_IDEN_WHEELS = 66
};

extern uint8_t bFlagSendData;

#include "stm32f4xx.h"
#include "tm_stm32f4_usb_vcp.h"

uint8_t readUSB();

#endif
