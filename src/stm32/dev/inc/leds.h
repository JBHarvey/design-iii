#ifndef LEDS_H_
#define LEDS_H_

#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"

#define LED_PRESCALER 4200-1 // Donne 500k
#define LED_PERIOD 10000

void InitializeLEDs();
void initLEDsSignals();
void turnOnRedLED();
void turnOnGreenLED();
void turnOffLEDs();
uint8_t isRedLightOn();
uint8_t isGreenLightOn();
void sendRedLightConfirmation();

void startLEDsRoutine();

#endif /* LEDS_H_ */
