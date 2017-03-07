/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "defines.h"

enum directions {
	BRAKE_G, CLOCK, COUNTER_CLOCK, BRAKE_V
};

void initMotors(void);
void TIM_Config(void);
void PWM_Config();
void MotorSetSpeed(uint16_t channel, uint16_t dutycycle);
void initDir(uint32_t RCCx, uint16_t GPIO_Pin1, uint16_t GPIO_Pin2,
		GPIO_TypeDef* GPIOx);
void setDir(GPIO_TypeDef* GPIOx, uint16_t Pin1, uint16_t Pin2,
		uint8_t direction);
void MotorSetDirection(uint8_t noMotor, uint8_t direction);
