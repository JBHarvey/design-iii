/*
 * externalInterrupts.c
 *
 *  Created on: Mar 7, 2017
 *      Author: jesussoldier
 */

#include "externalInterrupts.h"

// Initilise l'interruption pour le font de l'entrée manchester
void initializeExternalInterruptLine4() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD4 for EXTI_Line4 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);

	/* PD0 is connected to EXTI_Line4 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
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
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

void disableExternalInterruptLine4() {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PD4 for EXTI_Line4 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);

	/* PD7 is connected to EXTI_Line7 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
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
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

