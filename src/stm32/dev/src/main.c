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
#include <PID_v1.h>
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

enum SpeedDirection {
	SPEED_DIRECTION_FORWARD, SPEED_DIRECTION_BACKWARD, SPEED_DIRECTION_NONE
};

enum AmountOfCharacters {
	ONE_CHARACTER, MANY_CHARACTERS
};

enum COMMUNICATION_STATUS {
	READ_USB_SUCCESS, READ_USB_FAIL
};

enum COMMAND {
	COMMAND_MOVE
};

#define INTERNAL_SYSTICK_FREQUENCY 500
#define TIME_DELAY 1/(float) INTERNAL_SYSTICK_FREQUENCY
#define MAX_SPEED_INDEX 2000
#define TICKS_BUFFER_SIZE 100
#define MAXIMUM_CHARACTERS_BUFFER_SIZE 100

// buffer ticks for debug
volatile int ticksIndex4 = 0;
volatile int ticksBuffer4[TICKS_BUFFER_SIZE];

// Variables avec le nombre de Ticks pour la vitesse
volatile int numberOfSpeedEdges1 = 0;
volatile int numberOfSpeedEdges2 = 0;
volatile int numberOfSpeedEdges3 = 0;
volatile int numberOfSpeedEdges4 = 0;

// Variables avec le nombre de Ticks pour la position
volatile int numberOfPositionEdges1 = 0;
volatile int numberOfPositionEdges2 = 0;
volatile int numberOfPositionEdges3 = 0;
volatile int numberOfPositionEdges4 = 0;

// speed direction variables
volatile int speedDirection1 = SPEED_DIRECTION_NONE;
volatile int speedDirection2 = SPEED_DIRECTION_NONE;
volatile int speedDirection3 = SPEED_DIRECTION_NONE;
volatile int speedDirection4 = SPEED_DIRECTION_NONE;

// Initialization of speed motor variables
volatile int speedMotor1 = 0;
volatile int speedMotor2 = 0;
volatile int speedMotor3 = 0;
volatile int speedMotor4 = 0;

volatile int speedIndex = 0;
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

volatile int mainState = MAIN_IDLE;

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
		*state = MAIN_MOVE;
		TM_HD44780_Puts(0, 0, "MOVE");
		break;
	case MAIN_PID:
		*state = newState;
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

		uint8_t readCharacter;

		if (TM_USB_VCP_Getc(&readCharacter) == TM_USB_VCP_DATA_OK) {
			// Pour le echo TM_USB_VCP_Putc(c);
			return readCharacter;
		}
	} else {
		/* USB not OK */
		TM_DISCO_LedOff (LED_GREEN);
		TM_DISCO_LedOn (LED_RED);

		return 0;
	}
	return 0;
}

float getXMoveFromBuffer(uint8_t *data) {
	float xMove = *(float *) data;

	return xMove;
}

float getYMoveFromBuffer(uint8_t *data) {
	float yMove = *(float *) (data + Y_MOVE_DATA_BUFFER_INDEX);

	return yMove;
}

void processCommand(uint8_t command, uint8_t dataSize, int* mainState) {
	switch (command) {
	float xMove;
	float yMove;
case COMMAND_MOVE:
	//xMove = getXMoveFromBuffer();
	//yMove = getYMoveFromBuffer();
	//setState(mainState, MAIN_PID);
	break;
default:
	break;
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
	mainState = MAIN_IDLE;
//setState(&mainState, MAIN_MOVE);

	int state = IDLE;

// initializations for manchester signal
	InitializeLEDs();
	InitializeManchesterInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine4();

	/* Initialization of wheel 1 PIDs */
	PID_init(&PID_SPEED1, PID_SPEED1_KP, PID_SPEED1_KI, PID_SPEED1_KD,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&PID_SPEED1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION1, PID_POSITION1_KP, PID_POSITION1_KI,
			PID_POSITION1_KD, PID_Direction_Direct, PID_POSITION1_N);
	PID_SetOutputLimits(&PID_POSITION1, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 2 PIDs */
	PID_init(&PID_SPEED2, PID_SPEED2_KP, PID_SPEED2_KI, PID_SPEED2_KD,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&PID_SPEED2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION2, PID_POSITION2_KP, PID_POSITION2_KI,
			PID_POSITION2_KD, PID_Direction_Direct, PID_POSITION2_N);
	PID_SetOutputLimits(&PID_POSITION2, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 3 PIDs */
	PID_init(&PID_SPEED3, PID_SPEED3_KP, PID_SPEED3_KI, PID_SPEED3_KD,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&PID_SPEED3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION3, PID_POSITION3_KP, PID_POSITION3_KI,
			PID_POSITION3_KD, PID_Direction_Direct, PID_POSITION3_N);

	/* Initialization of wheel 4 PIDs */
	PID_init(&PID_SPEED4, PID_SPEED4_KP, PID_SPEED4_KI, PID_SPEED4_KD,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&PID_SPEED4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION4, PID_POSITION4_KP, PID_POSITION4_KI,
			PID_POSITION4_KD, PID_Direction_Direct, PID_POSITION4_N);

	while (1) {

		// Vite fait debounce pour le bouton bleu
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
			int i;
			for (i = 0; i < 9000000; i++)
				;
			TM_DISCO_LedOn (LED_BLUE);
			switch (mainState) {
			case MAIN_IDLE:
				//mainState = MAIN_PID;
				//setState(&mainState, MAIN_MOVE);
				break;
			}
		}

		uint8_t command = readUSB();
		if (command != 0) {

			/*uint8_t sizeOfData = readUSB();

			 processCommand(command, sizeOfData, &mainState);

			 setState(&mainState, MAIN_PID);*/
		}

		/* Main state machine */
		switch (mainState) {
		case MAIN_IDLE:
			//if (consigne != 0)
			//setState(&mainState, consigne);
			break;
		case MAIN_ACQUIS:
			/* Action lors d'une acquisition */
			readUSB();
			state = command;

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

			//PID_POSITION1.mySetpoint = 0.00;
			//PID_POSITION2.mySetpoint = 0.00;
			//PID_POSITION3.mySetpoint = 0.00;
			//PID_POSITION4.mySetpoint = 0.00;

			while (1) {

				if (PID_Compute_Position(&PID_POSITION1)) {
					//&& PID_Compute_Position(&PID_POSITION2)
					//&& PID_Compute_Position(&PID_POSITION3)
					//&& PID_Compute_Position(&PID_POSITION4)) {
					float positionOutput1 = PID_POSITION1.myOutput;
					int cmdMotor1;
					PID_SPEED1.mySetpoint = positionOutput1;

					if (PID_Compute_Speed(&PID_SPEED1)) {
						cmdMotor1 = PID_SPEED1.myOutput;
						if (cmdMotor1 > 0) {
							speedDirection1 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(1, COUNTER_CLOCK);
						} else if (cmdMotor1 < 0) {
							speedDirection1 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(1, CLOCK);
							cmdMotor1 = -cmdMotor1;
						} else {
							speedDirection1 = SPEED_DIRECTION_NONE;
							MotorSetDirection(1, BRAKE_G);
						}

						MotorSetSpeed(1, cmdMotor1);
						PID_SetMode(&PID_SPEED1, PID_Mode_Manual);

						char numberString[MAX_DISPLAY_CHARACTERS];
						sprintf(numberString, "%d", numberOfSpeedEdges1);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(0, 0, numberString);
						sprintf(numberString, "%d", cmdMotor1);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(3, 0, numberString);

						/* send position to USB */
						/*TM_USB_VCP_Putc(2);
						 TM_USB_VCP_Putc(8);
						 VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));
						 VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));*/
					}
				}

				/*PID_SPEED3.mySetpoint = positionOutput;

				 if (PID_Compute_Speed(&PID_SPEED3)) {

				 int cmdMotor3 = PID_SPEED3.myOutput;

				 if (cmdMotor3 > 0) {
				 speedDirection3 = SPEED_DIRECTION_FORWARD;
				 MotorSetDirection(3, CLOCK);
				 //MotorSetDirection(1, CLOCK);
				 } else if (cmdMotor3 < 0) {

				 speedDirection3 = SPEED_DIRECTION_BACKWARD;
				 MotorSetDirection(3, COUNTER_CLOCK);
				 //MotorSetDirection(1, CLOCK);
				 cmdMotor3 = -cmdMotor3;
				 } else {
				 speedDirection3 = SPEED_DIRECTION_NONE;
				 MotorSetDirection(3, BRAKE_G);
				 }

				 MotorSetSpeed(3, cmdMotor3);
				 PID_SetMode(&PID_SPEED3, PID_Mode_Manual);
				 }
				 }*/

				if (PID_Compute_Position(&PID_POSITION2)) {
					float positionOutput2 = PID_POSITION2.myOutput;
					PID_SPEED2.mySetpoint = positionOutput2;
					int cmdMotor2;

					if (PID_Compute_Speed(&PID_SPEED2)) {
						cmdMotor2 = PID_SPEED2.myOutput;
						if (cmdMotor2 > 0) {
							speedDirection2 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(2, COUNTER_CLOCK);
							//MotorSetDirection(4, CLOCK);
						} else if (cmdMotor2 < 0) {
							speedDirection2 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(2, CLOCK);
							cmdMotor2 = -cmdMotor2;
						} else {
							speedDirection2 = SPEED_DIRECTION_NONE;
							MotorSetDirection(2, BRAKE_G);
						}

						MotorSetSpeed(2, cmdMotor2);
						PID_SetMode(&PID_SPEED2, PID_Mode_Manual);

						char numberString[MAX_DISPLAY_CHARACTERS];
						sprintf(numberString, "%d", numberOfSpeedEdges2);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(0, 0, numberString);
						sprintf(numberString, "%d", cmdMotor2);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(3, 0, numberString);
					}
				}

				/*PID_SPEED4.mySetpoint = positionOutput;

				 if (PID_Compute_Speed(&PID_SPEED4)) {
				 int cmdMotor4 = PID_SPEED4.myOutput;

				 if (cmdMotor4 > 0) {
				 speedDirection4 = SPEED_DIRECTION_FORWARD;
				 MotorSetDirection(4, CLOCK);
				 //MotorSetDirection(2, COUNTER_CLOCK);
				 } else if (cmdMotor4 < 0) {

				 speedDirection4 = SPEED_DIRECTION_BACKWARD;
				 MotorSetDirection(4, COUNTER_CLOCK);
				 //MotorSetDirection(2, CLOCK);
				 cmdMotor4 = -cmdMotor4;
				 } else {
				 speedDirection4 = SPEED_DIRECTION_NONE;
				 MotorSetDirection(4, BRAKE_G);
				 //MotorSetDirection(2, BRAKE_G);

				 }

				 MotorSetSpeed(4, cmdMotor4);
				 PID_SetMode(&PID_SPEED4, PID_Mode_Manual);
				 }*/

				if (PID_Compute_Position(&PID_POSITION3)) {
					float positionOutput3 = PID_POSITION3.myOutput;
					PID_SPEED3.mySetpoint = positionOutput3;

					int cmdMotor3;

					if (PID_Compute_Speed(&PID_SPEED3)) {
						cmdMotor3 = PID_SPEED3.myOutput;
						if (cmdMotor3 > 0) {
							speedDirection3 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(3, CLOCK);
						} else if (cmdMotor3 < 0) {
							speedDirection3 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(3, COUNTER_CLOCK);
							cmdMotor3 = -cmdMotor3;
						} else {
							speedDirection3 = SPEED_DIRECTION_NONE;
							MotorSetDirection(3, BRAKE_G);
						}

						MotorSetSpeed(3, cmdMotor3);
						PID_SetMode(&PID_SPEED3, PID_Mode_Manual);

						char numberString[MAX_DISPLAY_CHARACTERS];
						sprintf(numberString, "%d", numberOfSpeedEdges3);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(0, 0, numberString);
						sprintf(numberString, "%d", cmdMotor3);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(3, 0, numberString);
					}
				}

				if (PID_Compute_Position(&PID_POSITION4)) {
					float positionOutput4 = PID_POSITION4.myOutput;
					PID_SPEED4.mySetpoint = positionOutput4;

					int cmdMotor4;

					if (PID_Compute_Speed(&PID_SPEED4)) {
						cmdMotor4 = PID_SPEED4.myOutput;
						if (cmdMotor4 > 0) {
							speedDirection4 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(4, CLOCK);
						} else if (cmdMotor4 < 0) {

							speedDirection4 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(4, COUNTER_CLOCK);
							cmdMotor4 = -cmdMotor4;
						} else {
							speedDirection4 = SPEED_DIRECTION_NONE;
							MotorSetDirection(4, BRAKE_G);

						}

						MotorSetSpeed(4, cmdMotor4);
						PID_SetMode(&PID_SPEED4, PID_Mode_Manual);

						char numberString[MAX_DISPLAY_CHARACTERS];
						sprintf(numberString, "%d", numberOfSpeedEdges4);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(0, 0, numberString);
						sprintf(numberString, "%d", cmdMotor4);
						cleanNumberString(numberString, MAX_DISPLAY_CHARACTERS);
						TM_HD44780_Puts(3, 0, numberString);

					}

					//MotorSetSpeed(1, cmdMotor1);
					//PID_SetMode(&PID_SPEED1, PID_Mode_Manual);

					//MotorSetSpeed(2, cmdMotor2);
					//PID_SetMode(&PID_SPEED2, PID_Mode_Manual);

					//MotorSetSpeed(3, cmdMotor3);
					//PID_SetMode(&PID_SPEED3, PID_Mode_Manual);

					//MotorSetSpeed(4, cmdMotor4);
					//PID_SetMode(&PID_SPEED4, PID_Mode_Manual);

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
		numberOfSpeedEdges1++;
		if (speedDirection1 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges1++;
		} else if (speedDirection1 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges1--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line5);
	}

	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges1++;
		if (speedDirection1 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges1++;
		} else if (speedDirection1 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges1--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line6);
	}

	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges2++;
		if (speedDirection2 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges2++;
		} else if (speedDirection2 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges2--;

		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line8);
	}

	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges2++;
		if (speedDirection2 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges2--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line9);
	}
}

extern void EXTI15_10_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges3++;

		if (speedDirection3 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges3++;
		} else if (speedDirection3 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges3--;

		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line10);
	}

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges3++;

		if (speedDirection3 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges3++;
		}

		else if (speedDirection3 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges3--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line11);
	}

	if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges4++;
		if (speedDirection4 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges4++;
		} else if (speedDirection4 == SPEED_DIRECTION_BACKWARD) {
			numberOfPositionEdges4--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line12);
	}

	if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges4++;
		if (speedDirection4 == SPEED_DIRECTION_FORWARD) {
			numberOfPositionEdges4++;
		} else if (speedDirection4 == SPEED_DIRECTION_BACKWARD) {

			numberOfPositionEdges4--;
		}
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line13);
	}
}

extern void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		/* update Speed pids */
		if (speedDirection1 == SPEED_DIRECTION_FORWARD) {
			PID_SPEED1.myInput = numberOfSpeedEdges1;
		} else if (speedDirection1 == SPEED_DIRECTION_BACKWARD) {
			PID_SPEED1.myInput = -numberOfSpeedEdges1;
		}
		if (speedDirection2 == SPEED_DIRECTION_FORWARD) {
			PID_SPEED2.myInput = numberOfSpeedEdges2;
		} else if (speedDirection2 == SPEED_DIRECTION_BACKWARD) {
			PID_SPEED2.myInput = -numberOfSpeedEdges2;
		}
		if (speedDirection3 == SPEED_DIRECTION_FORWARD) {
			PID_SPEED3.myInput = numberOfSpeedEdges3; // slave
		} else if (speedDirection3 == SPEED_DIRECTION_BACKWARD) {
			PID_SPEED3.myInput = -numberOfSpeedEdges3; // slave

		}
		if (speedDirection4 == SPEED_DIRECTION_FORWARD) {
			PID_SPEED4.myInput = numberOfSpeedEdges4; // slave
		} else if (speedDirection4 == SPEED_DIRECTION_BACKWARD) {
			PID_SPEED4.myInput = -numberOfSpeedEdges4; // slave
		}

		ticksBuffer4[ticksIndex4] = numberOfSpeedEdges2;
		ticksIndex4++;
		if (ticksIndex4 >= TICKS_BUFFER_SIZE) {
			ticksIndex4 = 0;
		}
		/* update position pids */
		PID_POSITION1.myInput = numberOfPositionEdges1;
		PID_POSITION2.myInput = numberOfPositionEdges2;
		PID_POSITION3.myInput = numberOfPositionEdges3;
		PID_POSITION4.myInput = numberOfPositionEdges4;

		/* activate speed pids */
		PID_SetMode(&PID_SPEED1, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED2, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED3, PID_Mode_Automatic);
		PID_SetMode(&PID_SPEED4, PID_Mode_Automatic);

		/* activate position pids */
		PID_SetMode(&PID_POSITION1, PID_Mode_Automatic);
		PID_SetMode(&PID_POSITION2, PID_Mode_Automatic);
		PID_SetMode(&PID_POSITION3, PID_Mode_Automatic);
		PID_SetMode(&PID_POSITION4, PID_Mode_Automatic);

		numberOfSpeedEdges1 = 0;
		numberOfSpeedEdges2 = 0;
		numberOfSpeedEdges3 = 0;
		numberOfSpeedEdges4 = 0;

		/* reset position for each wheels */
		/*if (-0.01 < PID_POSITION1.mySetpoint - PID_POSITION1.myInput
		 && PID_POSITION1.mySetpoint - PID_POSITION1.myInput < 0.01) {
		 numberOfPositionEdges1 = 0;
		 PID_POSITION1.mySetpoint = 0;
		 PID_POSITION1.myInput = 0;
		 }

		 if (-0.01 < PID_POSITION2.mySetpoint - PID_POSITION2.myInput
		 && PID_POSITION2.mySetpoint - PID_POSITION2.myInput < 0.01) {
		 numberOfPositionEdges2 = 0;
		 PID_POSITION2.mySetpoint = 0;
		 PID_POSITION2.myInput = 0;
		 }

		 if (-0.01 < PID_POSITION3.mySetpoint - PID_POSITION3.myInput
		 && PID_POSITION3.mySetpoint - PID_POSITION3.myInput < 0.01) {
		 numberOfPositionEdges3 = 0;
		 PID_POSITION3.mySetpoint = 0;
		 PID_POSITION3.myInput = 0;
		 }

		 if (-0.01 < PID_POSITION4.mySetpoint - PID_POSITION4.myInput
		 && PID_POSITION4.mySetpoint - PID_POSITION4.myInput < 0.01) {
		 numberOfPositionEdges4 = 0;
		 PID_POSITION4.mySetpoint = 0;
		 PID_POSITION4.myInput = 0;
		 }*/
	}
}

/* callback when data arrives on USB */
extern void handle_full_packet(uint8_t type, uint8_t *data, uint8_t len) {
	switch (type) {
	case COMMAND_MOVE:
		if (len == 8 && type == 0) {
			/* send data to test send feature on Usb */
			//uint8_t dataToSend[10] = { 1, 8, 3, 4, 5, 6, 7, 8, 9, 10 };
			//TM_USB_VCP_Send(dataToSend, 10);
			/* get X Y distances */
			float xMove = getXMoveFromBuffer(data);
			float yMove = getYMoveFromBuffer(data);
			PID_POSITION1.mySetpoint = xMove;
			PID_POSITION3.mySetpoint = xMove;
			PID_POSITION2.mySetpoint = yMove;
			PID_POSITION4.mySetpoint = yMove;

			numberOfPositionEdges1 = 0;
			numberOfPositionEdges2 = 0;
			numberOfPositionEdges3 = 0;
			numberOfPositionEdges4 = 0;

			setState(&mainState, MAIN_PID);
		}
		break;
	}
}
