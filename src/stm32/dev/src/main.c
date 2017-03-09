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
#include "PID_v1.h"
#include "encoder.h"
#include "externalInterrupts.h"
#include "timers.h"
#include "pushButtons.h"
#include "leds.h"

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

volatile int lcdCounter = 0;

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
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

// declaration of pids
PidType PID_SPEED1;
PidType PID_SPEED2;
PidType PID_SPEED3;
PidType PID_SPEED4;

PidType PID_POSITION1;
PidType PID_POSITION2;
PidType PID_POSITION3;
PidType PID_POSITION4;

// Buffer contenant tous les bits du manchester
volatile uint8_t manchesterBuffer[MANCHESTER_BUFFER_LENGTH];
volatile uint8_t manchesterIndex = 0;
volatile uint8_t manchesterState = MANCHESTER_WAIT_FOR_DECODING;
volatile uint8_t manchesterFigureVerification = 0;
volatile char manchesterOrientationVerification[ORIENTATION_LENGTH] = { ' ',
		' ', ' ', ' ', ' ' };
volatile uint8_t manchesterFactorVerification = 0;

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

void cleanNumberString(char *numberString, int size) {
	int arrayPosition = size - 1;
	while (numberString[arrayPosition] != 0) {
		numberString[arrayPosition] = ' ';
		arrayPosition--;
	}

	while (numberString[arrayPosition] == 0) {
		numberString[arrayPosition] = ' ';
		arrayPosition--;
	}
	numberString[size - 1] = 0;
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
	int mainState = MAIN_PID;
//setState(&mainState, MAIN_MOVE);

	int state = IDLE;

// initializations for manchester signal
	InitializeLEDs();
	InitializeManchesterInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine4();

	float consigneX;
	float consigneY;

	while (1) {

		// Vite fait debounce pour le bouton bleu
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
			int i;
			for (i = 0; i < 9000000; i++)
				;
			TM_DISCO_LedOn (LED_BLUE);
			switch (mainState) {
			case MAIN_IDLE:
				mainState = MAIN_PID;
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

			//move(100);

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

			mainState = MAIN_MANCH;

			break;
		case MAIN_PID:
			consigneX = 0.01;
			consigneY = 0.01;

			/* Initialization of PIDs */
			PID_init(&PID_SPEED1, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED1, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);

			PID_init(&PID_POSITION1, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			PID_init(&PID_SPEED2, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED2, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION2, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			PID_init(&PID_SPEED3, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED3, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);

			PID_init(&PID_POSITION3, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);
			PID_init(&PID_SPEED4, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);

			PID_SetOutputLimits(&PID_SPEED4, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION4, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			PID_SPEED1.mySetpoint = consigneX;
			PID_SPEED2.mySetpoint = consigneY;
			PID_SPEED3.mySetpoint = consigneX;
			PID_SPEED4.mySetpoint = consigneY;
			while (1) {

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

				if (PID_Compute(&PID_SPEED1)) {
					int cmdMotor1 = PID_SPEED1.myOutput + 0.5;
					MotorSetSpeed(1, cmdMotor1);
					PID_SetMode(&PID_SPEED1, PID_Mode_Manual);

					char numberString[MAX_DISPLAY_CHARACTERS];
					sprintf(numberString, "%d", numberOfEdges1);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(0, 0, numberString);
					sprintf(numberString, "%d", cmdMotor1);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(3, 0, numberString);
				}

				if (PID_Compute(&PID_SPEED2)) {
					int cmdMotor2 = PID_SPEED2.myOutput + 0.5;
					MotorSetSpeed(2, cmdMotor2);
					PID_SetMode(&PID_SPEED2, PID_Mode_Manual);

					char numberString[MAX_DISPLAY_CHARACTERS];
					sprintf(numberString, "%d", numberOfEdges2);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(8, 0, numberString);
					sprintf(numberString, "%d", cmdMotor2);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(11, 0, numberString);
				}
				if (PID_Compute(&PID_SPEED3)) {
					int cmdMotor3 = PID_SPEED3.myOutput + 0.5;
					MotorSetSpeed(3, cmdMotor3);
					PID_SetMode(&PID_SPEED3, PID_Mode_Manual);

					char numberString[MAX_DISPLAY_CHARACTERS];
					sprintf(numberString, "%d", numberOfEdges3);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(0, 1, numberString);
					sprintf(numberString, "%d", cmdMotor3);
					cleanNumberString(numberString, 4);
					TM_HD44780_Puts(3, 1, numberString);
				}
				if (PID_Compute(&PID_SPEED4)) {
					int cmdMotor4 = PID_SPEED4.myOutput + 0.5;
					MotorSetSpeed(4, cmdMotor4);
					PID_SetMode(&PID_SPEED4, PID_Mode_Manual);

					char numberString[MAX_DISPLAY_CHARACTERS];
					sprintf(numberString, "%d", numberOfEdges4);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(8, 1, numberString);
					sprintf(numberString, "%d", cmdMotor4);
					cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
					TM_HD44780_Puts(11, 1, numberString);
				}
			}
			break;
		case MAIN_MANCH:
			tryToDecodeManchesterCode(&manchesterState, manchesterBuffer,
					&manchesterFigureVerification,
					manchesterOrientationVerification,
					&manchesterFactorVerification);
			break;
		default:
			setState(&mainState, MAIN_IDLE);
		}
	}
}

extern void EXTI4_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line4);
		manchesterState = MANCHESTER_FIRST;
		InitializeTimer5();
		EnableTimer5Interrupt();
		disableExternalInterruptLine4();
	}

}

extern void EXTI9_5_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
		/* increase ticks */
		numberOfEdges1++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line5);
	}

	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		/* increase ticks */
		numberOfEdges1++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line6);
	}

	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		/* increase ticks */
		numberOfEdges2++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line8);
	}

	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		/* increase ticks */
		numberOfEdges2++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line9);
	}
}

extern void EXTI15_10_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		/* increase ticks */
		numberOfEdges3++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line10);
	}

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* increase ticks */
		numberOfEdges3++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line11);
	}

	if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
		/* increase ticks */
		numberOfEdges4++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line12);
	}

	if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
		/* increase ticks */
		numberOfEdges4++;
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line13);
	}
}

extern void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		PID_SPEED1.myInput = numberOfEdges1;
		PID_SPEED2.myInput = numberOfEdges2;
		PID_SPEED3.myInput = numberOfEdges3;
		PID_SPEED4.myInput = numberOfEdges4;

		numberOfEdges1 = 0;
		numberOfEdges2 = 0;
		numberOfEdges3 = 0;
		numberOfEdges4 = 0;

		PID_SetMode(&PID_SPEED1, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED2, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED3, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED4, PID_Mode_Automatic);

	}
}
