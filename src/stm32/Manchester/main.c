#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
#include "manchester.h"
#include <misc.h>

// Buffer contenant tous les bits du manchester
volatile uint8_t manchesterState = MANCHESTER_WAIT_FOR_DECODING;
volatile uint8_t manchesterBuffer[MANCHESTER_BUFFER_LENGTH];
volatile uint8_t manchesterIndex = 0;

void InitializeLEDs() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);

	GPIO_WriteBit(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, Bit_RESET);
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

extern void TIM5_IRQHandler() {
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

		switch (manchesterState) {
		case MANCHESTER_FIRST:
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
			manchesterState = MANCHESTER_FILL;
			TIM_SetAutoreload(TIM5, MANCHESTER_PERIOD * 4 - 1);
			break;
		case MANCHESTER_FILL:
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			manchesterBuffer[manchesterIndex] = GPIO_ReadInputDataBit(GPIOD,
					MANCHESTER_PIN);
			manchesterIndex++;
			if (manchesterIndex >= MANCHESTER_BUFFER_LENGTH)
				manchesterState = MANCHESTER_DECODE;
			break;
		default:
			manchesterState = MANCHESTER_WAIT_FOR_DECODING;
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

int main() {
	InitializeLEDs();
	InitializeInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine7();
	manchesterState = MANCHESTER_DECODE;

	while (1) {
		if (manchesterState == MANCHESTER_DECODE) {

			// les 8 bits contenant l'information
			uint8_t informationBits[INFORMATION_BITS_LENGTH];
			if (decodeManchester(informationBits, manchesterBuffer)
					== VALID_INFORMATION) {
				uint8_t figure = getFigureFromInformationBits(informationBits);
				uint8_t orientation = getOrientationFromInformationBits(
						informationBits);
				uint8_t factor = getFactorFromInformationBits(informationBits);
				displayManchesterInformation(figure, orientation, factor);
				manchesterState = MANCHESTER_IDLE;
			}
		}
	}
}
