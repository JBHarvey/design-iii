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

static TIM_HandleTypeDef TIM_Handler = {
	.Instance = TIM2
};

extern void TIM2_IRQHandler() {
	HAL_TIM_IRQHandler (&TIM_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		BSP_LED_Toggle (LED6);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
	}
}

void initializeTims() {
	__TIM2_CLK_ENABLE();
	TIM_Handler.Init.Prescaler = 42000 - 1;
	TIM_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Handler.Init.Period = 250 - 1;
	TIM_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_Handler.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init (&TIM_Handler);
	HAL_TIM_Base_Start_IT(&TIM_Handler);
}

void initializeLeds() {
	BSP_LED_Init (LED6);
}

void initializeInterrupts() {
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ (TIM2_IRQn);
}

void initializeOutputs() {
	GPIO_InitTypeDef GPIO_InitDef;
	GPIO_InitDef.Pin = GPIO_PIN_0;
	GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitDef.Pull = GPIO_NOPULL;
	GPIO_InitDef.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitDef);
}

int main(void) {
	HAL_Init();
	initializeLeds();
	initializeTims();
	initializeInterrupts();

	for (;;) {

		//int timerValue = __HAL_TIM_GET_COUNTER(&TIM_Handler);
		//if (timerValue == 500)
		//BSP_LED_Toggle (LED6);
		//if (timerValue == 500)
		//BSP_LED_Toggle (LED6);

		//HAL_TIM_PWM_Init(&TIM_Handler);
		//HAL_TIM_Base_Start(&TIM_Handler);
	}
}
