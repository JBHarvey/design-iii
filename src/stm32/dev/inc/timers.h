#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f4xx.h"

void SetTimer5(uint16_t period);
void InitializeTimer5();
void EnableTimer5Interrupt();
void disableTimer5Interrupt();

#endif
