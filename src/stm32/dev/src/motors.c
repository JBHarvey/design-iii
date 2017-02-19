/*
 * motors.c
 *
 *  Created on: 2017-02-19
 *      Author: Benoit
 */
#include "motors.h"

void InitializeTimer(int period = 500) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 40000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = period;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &timerInitStructure);
	TIM_Cmd(TIM4, ENABLE);
}

void InitializePWMChannel() {
	TIM_OCInitTypeDef outputChannelInit = { 0, };
	outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
	outputChannelInit.TIM_Pulse = 400;
	outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
	outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &outputChannelInit);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
}

void InitializePWMChannel2() {
	TIM_OCInitTypeDef outputChannelInit = { 0, };
	outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
	outputChannelInit.TIM_Pulse = 100;
	outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
	outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM4, &outputChannelInit);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
}

void InitializePin() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	gpioStructure.GPIO_Mode = GPIO_Mode_AF;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);
}

void initPWM(uint32_t RCCx, GPIO_TypeDef* GPIOx, uint8_t GPIO_Pinx,
		uint8_t GPIO_AFx, uint32_t GPIO_Pin_Sourcex) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCCx, ENABLE);

	/* Alternating functions for pins */
	GPIO_PinAFConfig(GPIOx, GPIO_Pinx, GPIO_AFx);

	/* Set pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_Sourcex;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void initTIM(uint32_t RCCx, TIM_TypeDef* TIMx) {
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;

	/* Enable clock for TIM4 */
	RCC_APB1PeriphClockCmd(RCCx, ENABLE);

	TIM_BaseStruct.TIM_Period = PWM_PERIOD;
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_BaseStruct);

	/* Start count on TIM4 */
	TIM_Cmd(TIMx, ENABLE);
}

// Initialise les pins pour la direction
void initDir(uint32_t RCCx, uint16_t GPIO_Pin1, uint16_t GPIO_Pin2,
		GPIO_TypeDef* GPIOx) {

	GPIO_InitTypeDef GPIO_InitDef;
	RCC_AHB1PeriphClockCmd(RCCx, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin1 | GPIO_Pin2;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOx, &GPIO_InitDef);

	// On stop le moteur de base
	GPIO_ResetBits(GPIOx, GPIO_Pin1);
	GPIO_ResetBits(GPIOx, GPIO_Pin2);
}

void setPWM(TIM_TypeDef* TIMx, int percent) {
	TIM_OCInitTypeDef TIM_OCStruct;

	/* Common settings */

	/* PWM mode 2 = Clear on compare match */
	/* PWM mode 1 = Set on compare match */
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	/*
	 To get proper duty cycle, you have simple equation

	 pulse_length = ((TIM_Period + 1) * DutyCycle) / 100 - 1

	 where DutyCycle is in percent, between 0 and 100%

	 25% duty cycle:     pulse_length = ((8399 + 1) * 25) / 100 - 1 = 2099
	 50% duty cycle:     pulse_length = ((8399 + 1) * 50) / 100 - 1 = 4199
	 75% duty cycle:     pulse_length = ((8399 + 1) * 75) / 100 - 1 = 6299
	 100% duty cycle:    pulse_length = ((8399 + 1) * 100) / 100 - 1 = 8399

	 Remember: if pulse_length is larger than TIM_Period, you will have output HIGH all the time
	 */
	TIM_OCStruct.TIM_Pulse = (uint32_t) percent * PWM_PERIOD / 100;
	TIM_OC1Init(TIMx, &TIM_OCStruct);
	TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
}

void setDir(GPIO_TypeDef* GPIOx, uint16_t Pin1, uint16_t Pin2,
		uint8_t direction) {
	switch (direction) {
	case BRAKE_G:
		GPIO_ResetBits(GPIOx, Pin1 | Pin2);
		break;
	case CLOCK:
		GPIO_ResetBits(GPIOx, Pin1);
		GPIO_SetBits(GPIOx, Pin2);
		break;
	case COUNTER_CLOCK:
		GPIO_SetBits(GPIOx, Pin1);
		GPIO_ResetBits(GPIOx, Pin2);
		break;
	case BRAKE_V:
		GPIO_SetBits(GPIOx, Pin1 | Pin2);
		break;
	default:
		GPIO_ResetBits(GPIOx, Pin1);
		GPIO_ResetBits(GPIOx, Pin2);
		break;
	}
}

void initMotors() {
	initPWM(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_PinSource6, GPIO_AF_TIM4,
			GPIO_Pin_6);
	initPWM(RCC_AHB1Periph_GPIOA, GPIOA, GPIO_PinSource6, GPIO_AF_TIM3,
			GPIO_Pin_6);
	initPWM(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_PinSource8, GPIO_AF_TIM10,
			GPIO_Pin_8);
	initPWM(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_PinSource9, GPIO_AF_TIM11,
			GPIO_Pin_9);

	initTIM(RCC_APB1Periph_TIM4, TIM4);
	initTIM(RCC_APB1Periph_TIM3, TIM3);
	initTIM(RCC_APB2Periph_TIM10, TIM10);
	initTIM(RCC_APB2Periph_TIM11, TIM11);

	initDir(RCC_AHB1Periph_GPIOD, GPIO_Pin_2, GPIO_Pin_3, GPIOD);
	setPWM(TIM4, 50);
}
