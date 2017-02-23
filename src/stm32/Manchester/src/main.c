#include <misc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
#include "manchester.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_hd44780.h"

// Buffer contenant tous les bits du manchester
volatile uint8_t manchesterState = MANCHESTER_WAIT_FOR_DECODING;
volatile uint8_t manchesterBuffer[MANCHESTER_BUFFER_LENGTH];
volatile uint8_t manchesterIndex = 0;
volatile uint8_t figureVerification = 0;
volatile char orientationVerification[ORIENTATION_LENGTH] = { ' ', ' ', ' ',
		' ', ' ' };
volatile uint8_t factorVerification = 0;

void InitializeLEDs() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);

	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

void SetTimer(uint16_t period) {
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = MANCHESTER_PRESCALER;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = period - 1;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &timerInitStructure);
}

void InitializeTimer() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	SetTimer(MANCHESTER_PERIOD - 1);
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}

// Initiliase l'input pour l'entrée du manchester
void InitializeInput() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = MANCHESTER_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

// Initilise l'interruption pour le font de l'entrée manchester
void initializeExternalInterruptLine7() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD0 for EXTI_Line0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);

	/* PD0 is connected to EXTI_Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line7;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

void EnableTimerInterrupt() {
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

void disableTimerInterrupt() {
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&nvicStructure);
}

extern void TIM5_IRQHandler() {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

		switch (manchesterState) {
		case MANCHESTER_FIRST:
			//GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			manchesterIndex = 0;
			manchesterState = MANCHESTER_FILL;
			TIM_SetAutoreload(TIM5, MANCHESTER_PERIOD * 4 - 1);
			break;
		case MANCHESTER_FILL:
			//GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			manchesterBuffer[manchesterIndex] = GPIO_ReadInputDataBit(GPIOD,
					MANCHESTER_PIN);
			manchesterIndex++;
			if (manchesterIndex >= MANCHESTER_BUFFER_LENGTH)
				manchesterState = MANCHESTER_DECODE;
			break;
		}
	}
}

void disableExternalInterruptLine7() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD7 for EXTI_Line7 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);

	/* PD7 is connected to EXTI_Line7 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line7;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

extern void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line7);
		manchesterState = MANCHESTER_FIRST;
		InitializeTimer();
		EnableTimerInterrupt();
		disableExternalInterruptLine7();
	}
}

uint8_t decodeInformationBits16(uint8_t indexOfBeginningCycle,
		uint8_t *informationBits, uint8_t *manchesterBuffer) {
	for (uint8_t i = 0; i < 16; i++) {
		if (isNextBitsEqualToZero(indexOfBeginningCycle + 2 * i,
				manchesterBuffer)) {
			informationBits[i] = 0;
		} else if (isNextBitsEqualToOne(indexOfBeginningCycle + 2 * i,
				manchesterBuffer)) {
			informationBits[i] = 1;
		} else {
			return BAD_INFORMATION;
		}
	}
	return VALID_INFORMATION;
}

uint8_t decodeManchester16Bits(uint8_t * informationBits,
		uint8_t *manchesterBuffer) {
	uint8_t indexOfBeginningCycle = findAValidCycle(manchesterBuffer);
	if (indexOfBeginningCycle != 0) {
		uint8_t informationState = decodeInformationBits16(
				indexOfBeginningCycle, informationBits, manchesterBuffer);
		return informationState;
	}
	return BAD_INFORMATION;
}

void generateMessageToDisplay16(char *informationBits16,
		char *messageToDisplay16) {

	messageToDisplay16[0] = '0' + informationBits16[0];
	messageToDisplay16[1] = '0' + informationBits16[1];
	messageToDisplay16[2] = '0' + informationBits16[2];
	messageToDisplay16[3] = '0' + informationBits16[3];
	messageToDisplay16[4] = '0' + informationBits16[4];
	messageToDisplay16[5] = '0' + informationBits16[5];
	messageToDisplay16[6] = '0' + informationBits16[6];
	messageToDisplay16[7] = '0' + informationBits16[7];
	messageToDisplay16[8] = '0' + informationBits16[8];
	messageToDisplay16[9] = '0' + informationBits16[9];
	messageToDisplay16[10] = '0' + informationBits16[10];
	messageToDisplay16[11] = '0' + informationBits16[11];
	messageToDisplay16[12] = '0' + informationBits16[12];
	messageToDisplay16[13] = '0' + informationBits16[13];
	messageToDisplay16[14] = '0' + informationBits16[14];
	messageToDisplay16[15] = '0' + informationBits16[15];

}

uint8_t isFigureEqual(uint8_t figure, uint8_t figureVerification) {
	return figure == figureVerification;
}

uint8_t isOrientationEqual(char *orientation, char *orientationVerification) {
	for (int i = 0; i < ORIENTATION_LENGTH; i++) {
		if (orientation[i] != orientationVerification[i]) {
			return 0;
		}
	}
	return 1;
}

uint8_t isFactorEqual(uint8_t factor, uint8_t factorVerification) {
	return factor == factorVerification;
}

uint8_t isSameDataThanPreviousIteration(uint8_t figure,
		uint8_t figureVerification, char *orientation,
		char *orientationVerification, uint8_t factor,
		uint8_t factorVerification) {
	return isFigureEqual(figure, figureVerification)
			&& isOrientationEqual(orientation, orientationVerification)
			&& isFactorEqual(factor, factorVerification);
}

int main() {
	InitializeLEDs();
	InitializeInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine7();
	TM_HD44780_Init(16, 2);
	while (1) {
		if (manchesterState == MANCHESTER_DECODE) {

			// les 8 bits contenant l'information
			uint8_t informationBits[INFORMATION_BITS_LENGTH];
			if (decodeManchester(informationBits, manchesterBuffer)
					== VALID_INFORMATION) {

				uint8_t figure = getFigureFromInformationBits(informationBits);
				char orientation[ORIENTATION_LENGTH];
				setOrientationFromInformationBits(informationBits, orientation);
				uint8_t factor = getFactorFromInformationBits(informationBits);

				if (isSameDataThanPreviousIteration(figure, figureVerification,
						orientation, orientationVerification, factor,
						factorVerification)) {
					char messageToDisplay[MESSAGE_TO_DISPLAY_LENGTH];
					setMessageToDisplay(figure, orientation, factor,
							messageToDisplay);
					displayManchesterMessage(messageToDisplay);
					disableExternalInterruptLine7();
					disableTimerInterrupt();
					manchesterState = MANCHESTER_IDLE;
					GPIO_SetBits(GPIOD, GPIO_Pin_13);
				} else {
					figureVerification = figure;
					strcpy(orientationVerification, orientation);
					factorVerification = factor;
					disableTimerInterrupt();
					manchesterState = MANCHESTER_WAIT_FOR_DECODING;
					initializeExternalInterruptLine7();
				}

			} else {
				disableTimerInterrupt();
				manchesterState = MANCHESTER_WAIT_FOR_DECODING;
				initializeExternalInterruptLine7();
			}
		}
	}
}
