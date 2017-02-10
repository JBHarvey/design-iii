/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "tm_stm32f4_gpio.h"

#define INTERNAL_SYSTICK_FREQUENCY 100
#define TIME_DELAY 1/(float) INTERNAL_SYSTICK_FREQUENCY

volatile int numberOfEdges1 = 0;
volatile float timeDelay;
volatile int direction1;

void intializeInputPins() {
	/* Enable clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

}

void intializePinsD() {
	/* Set pins as input */
	intializePinsD();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void intializeExternalInterrupts() {
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine5();
	initializeExternalInterruptLine6();
}

void initializeExternalInterruptLine5() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD0 for EXTI_Line0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource5);

	/* PD0 is connected to EXTI_Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;
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

void initializeExternalInterruptLine6() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD0 for EXTI_Line0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);

	/* PD0 is connected to EXTI_Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line6;
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

/* Configure pins to be interrupts */
void configurePins(void) {
	intializeInputPins();
	intializeExternalInterrupts();
	intializeLeds();
}

void intializeLeds() {
	STM_EVAL_LEDInit (LED6);
}

/* frequency is in hertz*/
void systickInit(uint16_t frequency) {
	NVIC_SetPriority(SysTick_IRQn, 0);
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	(void) SysTick_Config(RCC_Clocks.HCLK_Frequency / frequency);

}

float calculateSpeed(int edges) {
	return edges / (1600 * 4 * timeDelay);
}

int calculateDirection() {
	if (TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_5) == 1) {
		if (TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_6) == 0) {
			return 1;
		} else {
			return 0;
		}
	} else {
		if (TM_GPIO_GetInputPinValue(GPIOD, GPIO_PIN_6) == 1) {
			return 1;
		} else {
			return 0;
		}
	}
}

void initializeSystem() {
	timeDelay = 1 / (float) INTERNAL_SYSTICK_FREQUENCY;

	/* initialize Clocks */
	SystemInit();

	/* set internal interrupt*/
	systickInit(INTERNAL_SYSTICK_FREQUENCY);
}
int main() {
	initializeSystem();

	configurePins();

	while (1) {
	}
}

extern void SysTick_Handler(void) {
	float speed1 = calculateSpeed(numberOfEdges1);
	numberOfEdges1 = 0;
}

extern void EXTI0_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

		/* Toggle RED led */
		STM_EVAL_LEDToggle (LED6);

		/* increase ticks */
		numberOfEdges1++;

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line0);
	}
}

extern void EXTI9_5_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {

		STM_EVAL_LEDToggle (LED6);

		/* increase ticks */
		numberOfEdges1++;

		direction1 = calculateDirection();

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line5);
	}
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {

		STM_EVAL_LEDToggle (LED6);

		/* increase ticks */
		numberOfEdges1++;

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line6);
	}
}

