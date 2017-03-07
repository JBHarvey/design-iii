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
// TEST
#include <motor.h>
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_disco.h"
#include "defines.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_delay.h"
#include "manchester.h"
#include "PID.h"

#include <encoder.h>

#define TAILLE 500

enum State {
	IDLE,
	GENERATE_FIRST_PWM,
	BETWEEN_PWM,
	ACQUIRE,
	SEND_DATA,
	WAIT_FOR_SECOND_PWM
};

enum MainState {
	MAIN_IDLE, MAIN_MANCH, MAIN_ACQUIS, MAIN_MOVE, MAIN_PID
};

#define INTERNAL_SYSTICK_FREQUENCY 500
#define TIME_DELAY 1/(float) INTERNAL_SYSTICK_FREQUENCY
#define MAX_SPEED_INDEX 2000

// Variables avec le nombre de Ticks
volatile int numberOfEdges1 = 0;
volatile int numberOfEdges2 = 0;
volatile int numberOfEdges3 = 0;
volatile int numberOfEdges4 = 0;

// Initialization of speed motor variables
volatile int speedMotor1 = 0;
volatile int speedMotor2 = 0;
volatile int speedMotor3 = 0;
volatile int speedMotor4 = 0;

volatile int speedIndex = 0;
volatile float timeDelay;
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

// Buffer contenant tous les bits du manchester
volatile uint8_t manchesterBuffer[MANCHESTER_BUFFER_LENGTH];
volatile uint8_t manchesterIndex = 0;
volatile uint8_t manchesterState = MANCHESTER_WAIT_FOR_DECODING;
volatile uint8_t manchesterFigureVerification = 0;
volatile char manchesterOrientationVerification[ORIENTATION_LENGTH] = { ' ',
		' ', ' ', ' ', ' ' };
volatile uint8_t manchesterFactorVerification = 0;

void InitializeLEDs() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef gpioStructure;
	gpioStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructure.GPIO_Mode = GPIO_Mode_OUT;
	gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStructure);

	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

void SetTimer5(uint16_t period) {
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = MANCHESTER_PRESCALER;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = period - 1;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &timerInitStructure);
}

void InitializeTimer5() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	SetTimer5(MANCHESTER_PERIOD - 1);
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}

// Initiliase l'input pour l'entrée du manchester
void InitializeManchesterInput() {
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

void EnableTimer5Interrupt() {
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

void disableTimer5Interrupt() {
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

/* Change the state of the main
 * IN : the new state
 */
void setState(int* state, int newState) {
	switch (newState) {
	case MAIN_IDLE:
		*state = newState;
		TM_HD44780_Puts(0, 0, MSG_IDLE);
		break;
	case MAIN_MANCH:
		*state = newState;
		TM_HD44780_Puts(0, 0, MSG_MANCH);
		break;
	case MAIN_ACQUIS:
		TM_HD44780_Puts(0, 0, "NOT IMPLEMENTED");
		break;
	case MAIN_MOVE:
		TM_HD44780_Puts(0, 0, "NOT IMPLEMENTED");
		break;
	default:
		// Invalid state
		TM_HD44780_Puts(0, 0, MSG_ERR);
		break;
	}
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

void generateFirstPWM() {
	MotorSetSpeed(1, 40);
}

void generateSecondPWM() {
	MotorSetSpeed(1, 90);
}

void startFillingBuffer() {
	systickInit(INTERNAL_SYSTICK_FREQUENCY);
}

uint8_t readUSB() {
	/* USB configured OK, drivers OK */
	if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
		/* Turn on GREEN led */
		TM_DISCO_LedOn (LED_GREEN);
		TM_DISCO_LedOff (LED_RED);
		uint8_t c;
		/* If something arrived at VCP */
		if (TM_USB_VCP_Getc(&c) == TM_USB_VCP_DATA_OK) {
			/* Return data back */
			return c;
			// Pour le echo TM_USB_VCP_Putc(c);
		}
		return 0;

	} else {
		/* USB not OK */
		TM_DISCO_LedOff (LED_GREEN);
		TM_DISCO_LedOn (LED_RED);
		return 0;
	}
}

void tryToDecodeManchesterCode() {
	if (manchesterState == MANCHESTER_DECODE) {

		// les 8 bits contenant l'information
		uint8_t informationBits[INFORMATION_BITS_LENGTH];
		if (decodeManchester(informationBits, manchesterBuffer)
				== VALID_INFORMATION) {

			uint8_t figure = getFigureFromInformationBits(informationBits);
			char orientation[ORIENTATION_LENGTH];
			setOrientationFromInformationBits(informationBits, orientation);
			uint8_t factor = getFactorFromInformationBits(informationBits);

			if (isSameDataThanPreviousIteration(figure,
					manchesterFigureVerification, orientation,
					manchesterOrientationVerification, factor,
					manchesterFactorVerification)) {
				char messageToDisplay[MESSAGE_TO_DISPLAY_LENGTH];
				setMessageToDisplay(figure, orientation, factor,
						messageToDisplay);
				displayManchesterMessage(messageToDisplay);
				disableExternalInterruptLine7();
				disableTimer5Interrupt();
				manchesterState = MANCHESTER_IDLE;
				GPIO_SetBits(GPIOD, GPIO_Pin_13);
			} else {
				manchesterFigureVerification = figure;
				strcpy(manchesterOrientationVerification, orientation);
				manchesterFactorVerification = factor;
				disableTimer5Interrupt();
				manchesterState = MANCHESTER_WAIT_FOR_DECODING;
				initializeExternalInterruptLine7();
			}

		} else {
			disableTimer5Interrupt();
			manchesterState = MANCHESTER_WAIT_FOR_DECODING;
			initializeExternalInterruptLine7();
		}
	}
}
int main(void) {
	/* System Init */
	SystemInit();

	// Motors initialization
	initMotors();

	// Encoder initialization
	initEncoders();
	// LCD initialization
	TM_HD44780_Init(16, 2);

	/* Initialize LED's. Make sure to check settings for your board in tm_stm32f4_disco.h file */
	TM_DISCO_LedInit();

	/* Initialize USB VCP */
	TM_USB_VCP_Init();

	initBtn();

	// Initialisation des variables
	int mainState = MAIN_IDLE;
	//setState(&mainState, MAIN_MOVE);

	int state = IDLE;

	// initializations for manchester signal
	InitializeLEDs();
	InitializeManchesterInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine7();

	while (1) {

		// Vite fait debounce pour le bouton bleu
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
			int i;
			for (i = 0; i < 9000000; i++)
				;
			TM_DISCO_LedOn (LED_BLUE);
			switch (mainState) {
			case MAIN_IDLE:
				mainState = MAIN_MOVE;
				//setState(&mainState, MAIN_MOVE);
				break;
			}
		}
		uint8_t consigne = readUSB();
		/* Main state machine */
		switch (mainState) {
		case MAIN_IDLE:
			//if (consigne != 0)
			//setState(&mainState, consigne);
			break;
		case MAIN_ACQUIS:
			/* Action lors d'une acquisition */
			state = readUSB();
			if (state == GENERATE_FIRST_PWM) {
				generateFirstPWM();
				state = BETWEEN_PWM;
			} else if (state == ACQUIRE) {
				startFillingBuffer();
				state = WAIT_FOR_SECOND_PWM;
			} else if (state == SEND_DATA) {
				for (int i = 0; i < TAILLE; i++) {
					TM_USB_VCP_Putc(speedBuffer[i] & 0x00ff);
					TM_USB_VCP_Putc((speedBuffer[i] & 0xff00) >> 8);
				}
				state = IDLE;
			} else if (state == WAIT_FOR_SECOND_PWM && speedIndex > 200) {
				generateSecondPWM();
				state = IDLE;
			}
			break;
		case MAIN_MOVE:
			//Initialize Systick timer
			TM_DELAY_Init();
			TM_DISCO_LedOn (LED_BLUE);
			TM_DISCO_LedOn (LED_ORANGE);
			// Example of direction setting of 4 motors
			//MotorSetDirection(1, CLOCK);
			//MotorSetDirection(2, CLOCK);
			//MotorSetDirection(3, CLOCK);
			//MotorSetDirection(4, CLOCK);

			// Example of speed setting of 4 motors
			//MotorSetSpeed(1, 50);
			//MotorSetSpeed(2, 50);
			//MotorSetSpeed(3, 50);
			//MotorSetSpeed(4, 50);

			//Delayms(2000);

			//MotorSetSpeed(1, 0);
			//MotorSetSpeed(2, 0);
			//MotorSetSpeed(3, 0);
			//MotorSetSpeed(4, 0);

			//Delayms(2000);

			MotorSetDirection(1, COUNTER_CLOCK);
			MotorSetDirection(2, BRAKE_G);
			MotorSetDirection(3, CLOCK);
			MotorSetDirection(4, BRAKE_G);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 0);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 0);

			Delayms(2000);

			MotorSetDirection(1, CLOCK);
			MotorSetDirection(2, BRAKE_G);
			MotorSetDirection(3, COUNTER_CLOCK);
			MotorSetDirection(4, BRAKE_G);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 0);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 0);

			Delayms(2000);

			MotorSetDirection(1, CLOCK);
			MotorSetDirection(2, CLOCK);
			MotorSetDirection(3, COUNTER_CLOCK);
			MotorSetDirection(4, COUNTER_CLOCK);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 75);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 75);

			Delayms(1000);

			MotorSetDirection(1, CLOCK);
			MotorSetDirection(2, COUNTER_CLOCK);
			MotorSetDirection(3, COUNTER_CLOCK);
			MotorSetDirection(4, CLOCK);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 75);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 75);

			Delayms(1000);

			MotorSetDirection(1, COUNTER_CLOCK);
			MotorSetDirection(2, COUNTER_CLOCK);
			MotorSetDirection(3, CLOCK);
			MotorSetDirection(4, CLOCK);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 75);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 75);

			Delayms(1000);

			MotorSetDirection(1, COUNTER_CLOCK);
			MotorSetDirection(2, CLOCK);
			MotorSetDirection(3, CLOCK);
			MotorSetDirection(4, COUNTER_CLOCK);

			MotorSetSpeed(1, 75);
			MotorSetSpeed(2, 75);
			MotorSetSpeed(3, 75);
			MotorSetSpeed(4, 75);

			Delayms(1000);

			MotorSetDirection(1, BRAKE_G);
			MotorSetDirection(2, BRAKE_G);
			MotorSetDirection(3, BRAKE_G);
			MotorSetDirection(4, BRAKE_G);

			MotorSetSpeed(1, 0);
			MotorSetSpeed(2, 0);
			MotorSetSpeed(3, 0);
			MotorSetSpeed(4, 0);

			mainState = MAIN_IDLE;

			break;
		case MAIN_PID:
			int consigneX = 10;
			int consigneY = 22;
			while (1) {

				/* Initialization of PIDs */
				arm_pid_instance_f32 PID_SPEED;
				arm_pid_instance_f32 PID_POS;
				initPID(&PID_SPEED, &PID_POS);

				if (consigneX > 0) {
					MotorSetDirection(1, COUNTER_CLOCK);
					MotorSetDirection(3, CLOCK);
				} else if (consigneX < 0) {
					MotorSetDirection(1, CLOCK);
					MotorSetDirection(3, COUNTER_CLOCK);
				} else {
					MotorSetDirection(1, BRAKE_G);
					MotorSetDirection(3, BRAKE_G);
				}

				if (consigneY > 0) {
					MotorSetDirection(2, COUNTER_CLOCK);
					MotorSetDirection(4, CLOCK);
				} else if (consigneY < 0) {
					MotorSetDirection(2, CLOCK);
					MotorSetDirection(4, COUNTER_CLOCK);
				} else {
					MotorSetDirection(2, BRAKE_G);
					MotorSetDirection(4, BRAKE_G);
				}

				int cmdMotor1 = getPID(&PID_SPEED, speedMotor1, 2);
				int cmdMotor2 = getPID(&PID_SPEED, speedMotor2, 2);
				int cmdMotor3 = getPID(&PID_SPEED, speedMotor3, 2);
				int cmdMotor4 = getPID(&PID_SPEED, speedMotor4, 2);

				MotorSetSpeed(1, cmdMotor1);
				MotorSetSpeed(2, cmdMotor2);
				MotorSetSpeed(3, cmdMotor3);
				MotorSetSpeed(4, cmdMotor4);
			}
			break;
		case MAIN_MANCH:
			tryToDecodeManchesterCode();
			break;
		default:
			setState(&mainState, MAIN_IDLE);
		}
	}
}

/*extern void SysTick_Handler(void) {
 if (speedIndex < MAX_SPEED_INDEX) {

 speedBuffer[speedIndex] = numberOfEdges1; //calculateSpeed(numberOfEdges1);
 speedIndex++;
 numberOfEdges1 = 0;
 } else {
 TM_DISCO_LedOn (LED_ORANGE);
 MotorSetSpeed(1, 0);
 }
 }*/

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

	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line7);
		manchesterState = MANCHESTER_FIRST;
		InitializeTimer5();
		EnableTimer5Interrupt();
		disableExternalInterruptLine7();
	}
}
