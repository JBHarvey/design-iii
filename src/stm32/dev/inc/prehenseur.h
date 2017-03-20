/*
 * prehenseur.h
 */

#ifndef PREHENSEUR_H_
#define PREHENSEUR_H_

#include "stm32f4xx.h"

// We need a frequency of 50Hz
#define PREHENSOR_PERIOD 60000
#define PREHENSOR_PRESCALER 28

#define PREHENSOR_UP_PWM 9
#define PREHENSOR_DOWN_PWM 6

void initPrehensor();
void initPrehensorTIM(void);
void initPrehensorPWM();
void moveUpPrehensor();
void moveDownPrehensor();

#endif /* PREHENSEUR_H_ */
