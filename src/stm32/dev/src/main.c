/**
 *    USB VCP for STM32F4xx example.
 *
 *    @author     Tilen Majerle
 *    @email        tilen@majerle.eu
 *    @website    http://stm32f4-discovery.com
 *    @ide        Keil uVision
 *
 * Add line below to use this example with F429 Discovery board (in defines.h file)
 *
 * #define USE_USB_OTG_HS
 */
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_disco.h"
#include "defines.h"

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "tm_stm32f4_gpio.h"

#define TAILLE 500

enum State {
	IDLE,
	GENERATE_FIRST_PWM,
	BETWEEN_PWM,
	ACQUIRE,
	SEND_DATA,
	WAIT_FOR_SECOND_PWM
};

#define INTERNAL_SYSTICK_FREQUENCY 500
#define TIME_DELAY 1/(float) INTERNAL_SYSTICK_FREQUENCY
#define MAX_SPEED_INDEX 2000

// Variables avec le nombre de Ticks
volatile int numberOfEdges1 = 0;
volatile int numberOfEdges2 = 0;
volatile int numberOfEdges3 = 0;
volatile int numberOfEdges4 = 0;

// L'intégration du déplacement
volatile int speedMoteur1 = 0;
volatile int speedMoteur2 = 0;
volatile int speedMoteur3 = 0;
volatile int speedMoteur4 = 0;

volatile int speedIndex = 0;
volatile float timeDelay;
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

void intializePinsD() {
	/* Set pins as input */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void intializeInputPins() {
	/* Enable clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	intializePinsD();
}

void initBtn() {
	//Enable clock for GPOIA
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitDef;
	//Pin 0
	GPIO_InitDef.GPIO_Pin = GPIO_Pin_0;
	//Mode output
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
	//Output type push-pull
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	//With pull down resistor
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN;
	//50MHz pin speed
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;

	//Initialize pin on GPIOA port
	GPIO_Init(GPIOA, &GPIO_InitDef);
}

void initH() {

	GPIO_InitTypeDef GPIO_InitDef;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOD, &GPIO_InitDef);

	GPIO_ResetBits(GPIOD, GPIO_Pin_2);
	GPIO_SetBits(GPIOD, GPIO_Pin_3);
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

void intializeExternalInterrupts() {
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine5();
	initializeExternalInterruptLine6();
}

/* Configure pins to be interrupts */
void configurePins(void) {
	intializeInputPins();
	intializeExternalInterrupts();
}

/* frequency is in hertz*/
void systickInit(uint16_t frequency) {
	NVIC_SetPriority(SysTick_IRQn, 0);
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	(void) SysTick_Config(RCC_Clocks.HCLK_Frequency / frequency);

}

uint16_t calculateSpeed(int edges) {
	return edges / (16000 * 2 * timeDelay);
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
}

int isThereNewSerialData() {
	return 0;
}

int readCommand() {
// read serial port communication
	return 0;
}

int updateState(uint8_t command) {
	return command;
}

void configurePWMPinsD() {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Clock for GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Alternating functions for pins */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

	/* Set pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; //| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void initializeTim4() {
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;

	/* Enable clock for TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/*
	 TIM4 is connected to APB1 bus, which has on F407 device 42MHz clock
	 But, timer has internal PLL, which double this frequency for timer, up to 84MHz
	 Remember: Not each timer is connected to APB1, there are also timers connected
	 on APB2, which works at 84MHz by default, and internal PLL increase
	 this to up to 168MHz

	 Set timer prescaller
	 Timer count frequency is set with

	 timer_tick_frequency = Timer_default_frequency / (prescaller_set + 1)

	 In our case, we want a max frequency for timer, so we set prescaller to 0
	 And our timer will have tick frequency

	 timer_tick_frequency = 84000000 / (0 + 1) = 84000000
	 */
	TIM_BaseStruct.TIM_Prescaler = PWM_PRESCALER;
	/* Count up */
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	/*
	 Set timer period when it have reset
	 First you have to know max value for timer
	 In our case it is 16bit = 65535
	 To get your frequency for PWM, equation is simple

	 PWM_frequency = timer_tick_frequency / (TIM_Period + 1)

	 If you know your PWM frequency you want to have timer period set correct

	 TIM_Period = timer_tick_frequency / PWM_frequency - 1

	 In our case, for 10Khz PWM_frequency, set Period to

	 TIM_Period = 84000000 / 10000 - 1 = 8399

	 If you get TIM_Period larger than max timer value (in our case 65535),
	 you have to choose larger prescaler and slow down timer tick frequency
	 */

	TIM_BaseStruct.TIM_Period = PWM_PERIOD; /* 10kHz PWM */
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	/* Initialize TIM4 */
	TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
	/* Start count on TIM4 */
	TIM_Cmd(TIM4, ENABLE);
}

void generatePWM1(int percent) {
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
	TIM_OC1Init(TIM4, &TIM_OCStruct);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}
void generateFirstPWM() {
	configurePWMPinsD();
	initializeTim4();
	generatePWM1(40);

}

void generateSecondPWM() {
	generatePWM1(90);
}

void startFillingBuffer() {
	systickInit(INTERNAL_SYSTICK_FREQUENCY);
}

void sendData() {

}

/* Fin MARTIN */

void fillBuffer(uint16_t* buffer, int length) {
	for (int i = 0; i < length; i++) {
		buffer[i] = 579;
	}
}

int main(void) {
	// Initialisation des variables
	int state = IDLE;
	uint8_t c;
	/* System Init */
	SystemInit();

	/* Martin */
	configurePins();

	initH();
	initBtn();

	configurePWMPinsD();

	initializeTim4();

	intializeInputPins();

	intializeExternalInterrupts();
	/* Martin */

	/* Initialize LED's. Make sure to check settings for your board in tm_stm32f4_disco.h file */
	TM_DISCO_LedInit();

	/* Initialize USB VCP */
	TM_USB_VCP_Init();

	uint16_t arr[TAILLE];
	fillBuffer(arr, TAILLE);
	int nbFoisEnvoyer = 0; // Le nombre de fois que le buffer a été envoyé
	while (1) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
			int i;
			for (i = 0; i < 9000000; i++)
				;
			TM_DISCO_LedOn (LED_BLUE);
			switch (state) {
			case IDLE:
				state = GENERATE_FIRST_PWM;
				break;
			case BETWEEN_PWM:
				state = ACQUIRE;
				break;
			}
		}
		/* USB configured OK, drivers OK */
		if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
			/* Turn on GREEN led */
			TM_DISCO_LedOn (LED_GREEN);
			TM_DISCO_LedOff (LED_RED);
			/* If something arrived at VCP */
			if (TM_USB_VCP_Getc(&c) == TM_USB_VCP_DATA_OK) {
				/* Return data back */
				state = updateState(c);
				// Pour le echo TM_USB_VCP_Putc(c);
			}

		}

		else {
			/* USB not OK */
			TM_DISCO_LedOff (LED_GREEN);
			TM_DISCO_LedOn (LED_RED);
		}

		if (state == GENERATE_FIRST_PWM) {
			generateFirstPWM();

			state = BETWEEN_PWM;
		} else if (state == ACQUIRE) {
			startFillingBuffer();
			state = WAIT_FOR_SECOND_PWM;
		} else if (state == SEND_DATA) {
			sendData();
			for (int i = 0; i < TAILLE; i++) {
				TM_USB_VCP_Putc(speedBuffer[i] & 0x00ff);
				TM_USB_VCP_Putc((speedBuffer[i] & 0xff00) >> 8);
			}
			/*if (nbFoisEnvoyer < 5) {

			 for (int i = TAILLE * nbFoisEnvoyer;
			 i < TAILLE + nbFoisEnvoyer * TAILLE; i++) {
			 TM_USB_VCP_Putc(speedBuffer[i] & 0x00ff);
			 TM_USB_VCP_Putc((speedBuffer[i] & 0xff00) >> 8);
			 }
			 nbFoisEnvoyer++;
			 }*/

			state = IDLE;
		} else if (state == WAIT_FOR_SECOND_PWM && speedIndex > 200) {
			generateSecondPWM();
			state = IDLE;
		}
	}
}

/* MARTIN START */
extern void SysTick_Handler(void) {
	if (speedIndex < MAX_SPEED_INDEX) {

		speedBuffer[speedIndex] = numberOfEdges1; //calculateSpeed(numberOfEdges1);
		speedIndex++;
		numberOfEdges1 = 0;
	} else {
		TM_DISCO_LedOn (LED_ORANGE);
		generatePWM1(0);
	}
}

extern void EXTI0_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

		/* Toggle RED led */
		//STM_EVAL_LEDToggle (LED6);
		/* increase ticks */
		numberOfEdges1++;

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line0);
	}
}

extern void EXTI9_5_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {

		//STM_EVAL_LEDToggle (LED6);

		/* increase ticks */
		numberOfEdges1++;

		//sdirection1 = calculateDirection();

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line5);
	}
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {

		//STM_EVAL_LEDToggle (LED6);

		/* increase ticks */
		numberOfEdges1++;

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line6);
	}
}

/* MARTIN END */
