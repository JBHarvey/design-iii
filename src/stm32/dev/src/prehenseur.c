#include "prehenseur.h"

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void initPrehensor() {
	initPrehensorTIM();
	initPrehensorPWM();
}

void initPrehensorTIM(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* GPIOB Configuration: TIM4 PB6 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM4 pins to AF2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
}

void initPrehensorPWM() {
	//uint16_t PrescalerValue = 0;
	/* -----------------------------------------------------------------------
	 TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.

	 In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1),
	 since APB1 prescaler is different from 1.
	 TIM3CLK = 2 * PCLK1
	 PCLK1 = HCLK / 4
	 => TIM3CLK = HCLK / 2 = SystemCoreClock /2

	 To get TIM3 counter clock at 28 MHz, the prescaler is computed as follows:
	 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	 Prescaler = ((SystemCoreClock /2) /28 MHz) - 1

	 To get TIM3 output clock at 30 KHz, the period (ARR)) is computed as follows:
	 ARR = (TIM3 counter clock / TIM3 output clock) - 1
	 = 665

	 Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	 function to update SystemCoreClock variable value. Otherwise, any configuration
	 based on this variable will be incorrect.
	 ----------------------------------------------------------------------- */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	/* Compute the prescaler value */
	//PrescalerValue = (uint16_t)((SystemCoreClock / 2) / 28000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PREHENSOR_PERIOD - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PREHENSOR_PRESCALER - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM4, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}

// Move up the prehensor
void moveUpPrehensor() {
	TIM4->CCR1 = (uint32_t) PREHENSOR_UP_PWM * PREHENSOR_PERIOD / 100;
}

// Move down the prehensor
void moveDownPrehensor() {
	TIM4->CCR1 = (uint32_t) PREHENSOR_DOWN_PWM * PREHENSOR_PERIOD / 100;
}
