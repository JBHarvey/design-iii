/* Includes ------------------------------------------------------------------*/
#include "motor.h"
/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/* Private functions ---------------------------------------------------------*/

void initMotors(void) {
	/* TIM Configuration */
	TIM_Config();
	/* PWM Configuration */
	PWM_Config();
	/* Direction Configuration */
	initDir(MOTOR1_DIR_RCC, MOTOR1_DIR1_PIN, MOTOR1_DIR2_PIN, MOTOR1_DIR_GPIO);
	initDir(MOTOR2_DIR_RCC, MOTOR2_DIR1_PIN, MOTOR2_DIR2_PIN, MOTOR1_DIR_GPIO);
	initDir(MOTOR3_DIR_RCC, MOTOR3_DIR1_PIN, MOTOR3_DIR2_PIN, MOTOR1_DIR_GPIO);
	initDir(MOTOR4_DIR_RCC, MOTOR4_DIR1_PIN, MOTOR4_DIR2_PIN, MOTOR1_DIR_GPIO);
}
/**
 * @brief  Configure the TIM3 Ouput Channels.
 * @param  None
 * @retval None
 */
void TIM_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOC and GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);

	/* GPIOC Configuration: TIM3 CH1 (PC6) and TIM3 CH2 (PC7) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOB Configuration:  TIM3 CH3 (PB0) and TIM3 CH4 (PB1) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
}

void PWM_Config() {
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

	/* Compute the prescaler value */
	//PrescalerValue = (uint16_t)((SystemCoreClock / 2) / 28000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
	TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
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

	GPIO_ResetBits(GPIOx, GPIO_Pin1);
	GPIO_SetBits(GPIOx, GPIO_Pin2);
}

void MotorSetSpeed(uint16_t channel, uint16_t percent) {
	if (channel == 1) {
		TIM3->CCR1 = (uint32_t) percent * PWM_PERIOD / 100;
	} else if (channel == 2) {
		TIM3->CCR2 = (uint32_t) percent * PWM_PERIOD / 100;
	} else if (channel == 3) {
		TIM3->CCR3 = (uint32_t) percent * PWM_PERIOD / 100;
	} else {
		TIM3->CCR4 = (uint32_t) percent * PWM_PERIOD / 100;
	}
}

void MotorSetDirection(uint8_t noMotor, uint8_t direction) {
	GPIO_TypeDef* GPIOx;
	uint16_t Pin1;
	uint16_t Pin2;
	switch (noMotor) {
	case 1:
		GPIOx = MOTOR1_DIR_GPIO;
		Pin1 = MOTOR1_DIR1_PIN;
		Pin2 = MOTOR1_DIR2_PIN;
		break;
	case 2:
		GPIOx = MOTOR2_DIR_GPIO;
		Pin1 = MOTOR2_DIR1_PIN;
		Pin2 = MOTOR2_DIR2_PIN;
		break;
	case 3:
		GPIOx = MOTOR3_DIR_GPIO;
		Pin1 = MOTOR3_DIR1_PIN;
		Pin2 = MOTOR3_DIR2_PIN;
		break;
	case 4:
		GPIOx = MOTOR4_DIR_GPIO;
		Pin1 = MOTOR4_DIR1_PIN;
		Pin2 = MOTOR4_DIR2_PIN;
		break;
	default:
		return;
		break;
	}
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

