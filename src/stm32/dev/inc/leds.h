#ifndef LEDS_H_
#define LEDS_H_

#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"

#define LED_PRESCALER 8400-1
#define LED_PERIOD 50000

void InitializeLEDs();
void initLEDsSignals();
void turnOnRedLED();
void turnOnGreenLED();
void turnOffLEDs();
void startLEDsRoutine();

#endif /* LEDS_H_ */
