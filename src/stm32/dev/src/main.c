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
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

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
	int mainState = MAIN_IDLE;
	//setState(&mainState, MAIN_MOVE);

	int state = IDLE;

	// initializations for manchester signal
	InitializeLEDs();
	InitializeManchesterInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine7();

	int consigneX;
	int consigneY;

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

		int consigneX = 10;
		int consigneY = 22;
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

			mainState = MAIN_IDLE;

			break;
		case MAIN_PID:
			consigneX = 10;
			consigneY = 22;
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

				int cmdMotor1 = getPID(&PID_SPEED, speedMotor1, consigneX);
				int cmdMotor2 = getPID(&PID_SPEED, speedMotor2, consigneY);
				int cmdMotor3 = getPID(&PID_SPEED, speedMotor3, consigneX);
				int cmdMotor4 = getPID(&PID_SPEED, speedMotor4, consigneY);

				MotorSetSpeed(1, cmdMotor1);
				MotorSetSpeed(2, cmdMotor2);
				MotorSetSpeed(3, cmdMotor3);
				MotorSetSpeed(4, cmdMotor4);
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

	if (EXTI_GetITStatus(EXTI_Line7) != RESET) {
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line7);
		manchesterState = MANCHESTER_FIRST;
		InitializeTimer5();
		EnableTimer5Interrupt();
		disableExternalInterruptLine7();
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

		speedMotor1 = calculateSpeed(numberOfEdges1);
		//numberOfEdges1 = 0;
		speedMotor2 = calculateSpeed(numberOfEdges2);
		//numberOfEdges2 = 0;
		speedMotor3 = calculateSpeed(numberOfEdges3);
		//numberOfEdges3 = 0;
		speedMotor4 = calculateSpeed(numberOfEdges4);
		//numberOfEdges4 = 0;

		TM_HD44780_Puts(0, 0, numberOfEdges1);
		TM_HD44780_Puts(0, 8, numberOfEdges2);
		TM_HD44780_Puts(1, 0, numberOfEdges3);
		TM_HD44780_Puts(1, 8, numberOfEdges4);
	}
}
