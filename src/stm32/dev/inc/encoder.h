/*
 * encoder.h
 *
 *  Created on: 2017-02-19
 *      Author: Admin
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "utils.h"

/******************************************************
 * Variables pour mesurer la position des roues
 * ****************************************************/
extern volatile int numberOfPositionEdges1;
extern volatile int numberOfPositionEdges2;
extern volatile int numberOfPositionEdges3;
extern volatile int numberOfPositionEdges4;

/**************************************************************
 * Variables pour mesurer le nombre de ticks vitesse des roues
 * ************************************************************/
extern volatile int numberOfSpeedEdges1;
extern volatile int numberOfSpeedEdges2;
extern volatile int numberOfSpeedEdges3;
extern volatile int numberOfSpeedEdges4;

/**************************************************************
 * variables pour déterminer la direction des roues
 * ************************************************************/
extern volatile int speedDirection1;
extern volatile int speedDirection2;
extern volatile int speedDirection3;
extern volatile int speedDirection4;

/**************************************************************
 * variables temporaires pour debugger les encodeurs
 * ************************************************************/
extern volatile int incWheel1Canal1EncoderCounter;
extern volatile int incWheel1Canal2EncoderCounter;
extern volatile int decWheel1Canal1EncoderCounter;
extern volatile int decWheel1Canal2EncoderCounter;

extern volatile int incWheel2Canal1EncoderCounter;
extern volatile int incWheel2Canal2EncoderCounter;
extern volatile int decWheel2Canal1EncoderCounter;
extern volatile int decWheel2Canal2EncoderCounter;

extern volatile int incWheel3Canal1EncoderCounter;
extern volatile int incWheel3Canal2EncoderCounter;
extern volatile int decWheel3Canal1EncoderCounter;
extern volatile int decWheel3Canal2EncoderCounter;

extern volatile int incWheel4Canal1EncoderCounter;
extern volatile int incWheel4Canal2EncoderCounter;
extern volatile int decWheel4Canal1EncoderCounter;
extern volatile int decWheel4Canal2EncoderCounter;

/**************************************************************
 * variables pour calculer la direction (déphasage) des roues
 * ************************************************************/
volatile int wheel1Channel1UP;
volatile int wheel1Channel2UP;
volatile int wheel2Channel1UP;
volatile int wheel2Channel2UP;
volatile int wheel3Channel1UP;
volatile int wheel3Channel2UP;
volatile int wheel4Channel1UP;
volatile int wheel4Channel2UP;

void intializePinsD();
void initializeExternalInterruptLine5();
void initializeExternalInterruptLine6();
void intializeExternalInterrupts();
void initEncoders();
void resetPositionEncoderVariables();
void resetEncoderSpeedVariables();

#endif /* ENCODER_H_ */
