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
	MAIN_IDLE,
	MAIN_MANCH,
	MAIN_ACQUIS,
	MAIN_ACQUIS_ALL,
	MAIN_TEST_SPEED_PID,
	MAIN_DEAD_ZONE,
	MAIN_MOVE,
	MAIN_PID
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

/***************************************************
 *  Les buffers qui suivent servent � l'acquisition pour
 * bien faire l'asservissement des roues
 ***************************************************/
#define ACQUIS
#define MAX_WHEEL_INDEX 1000
#define PREMIER_ECHELON 40
#define DEUXIEME_ECHELON 80
#define DUREE_ECHELON_1 2000
#define DUREE_ECHELON_2 2000
volatile uint16_t bufferWheelIndex1 = 0;
volatile uint16_t bufferWheelIndex2 = 0;
volatile uint16_t bufferWheelIndex3 = 0;
volatile uint16_t bufferWheelIndex4 = 0;
volatile uint16_t bufferWheel1[MAX_WHEEL_INDEX];
volatile uint16_t bufferWheel2[MAX_WHEEL_INDEX];
volatile uint16_t bufferWheel3[MAX_WHEEL_INDEX];
volatile uint16_t bufferWheel4[MAX_WHEEL_INDEX];

/******************************************************
 * Variables pour tester le PID de vitesse
 * ****************************************************/
#define TEST_SPEED_PID
#ifdef TEST_SPEED_PID
#define PID_SPEED
#define MAX_SPEED_PID_INDEX 500
#define CONSIGNE_SPEED 0.2
volatile uint16_t bufferSpeedPIDIndex1 = 0;
volatile uint16_t bufferSpeedPIDIndex2 = 0;
volatile uint16_t bufferSpeedPIDIndex3 = 0;
volatile uint16_t bufferSpeedPIDIndex4 = 0;
volatile uint16_t bufferSpeedPID1[MAX_SPEED_PID_INDEX];
volatile uint16_t bufferSpeedPID2[MAX_SPEED_PID_INDEX];
volatile uint16_t bufferSpeedPID3[MAX_SPEED_PID_INDEX];
volatile uint16_t bufferSpeedPID4[MAX_SPEED_PID_INDEX];
#endif

/********************************************************
 * Variables pour trouver la zone morte de chacune des roues
 ********************************************************/
#define FIND_DEAD_ZONE
#ifdef FIND_DEAD_ZONE
#define DEAD_START_PWM 20
#define DEAD_STOP_PWM 100
#define MAX_DEAD_ZONE 100
volatile uint8_t bufferDeadZoneIndex1 = 0;
volatile uint8_t bufferDeadZoneIndex2 = 0;
volatile uint8_t bufferDeadZoneIndex3 = 0;
volatile uint8_t bufferDeadZoneIndex4 = 0;
volatile uint16_t bufferDeadZone1[MAX_DEAD_ZONE];
volatile uint16_t bufferDeadZone2[MAX_DEAD_ZONE];
volatile uint16_t bufferDeadZone3[MAX_DEAD_ZONE];
volatile uint16_t bufferDeadZone4[MAX_DEAD_ZONE];
#endif

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
			//TM_USB_VCP_Putc(c);
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

void processReceivedData(uint8_t *receivedData, int *mainState) {
	switch (receivedData[0]) {
	case COMMAND_MOVE:
		setState(mainState, MAIN_MOVE);
		break;
	default:
		break;
	}
}

void fillDummiesBuffers() {
	for (int i = 0; i < MAX_WHEEL_INDEX; i++) {
		bufferWheel1[i] = i + 1;
		bufferWheel2[i] = i + 2;
		bufferWheel3[i] = i + 3;
		bufferWheel4[i] = i + 4;
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

//Initialize Systick timer
	TM_DELAY_Init();

// Initialisation des variables
	int mainState = MAIN_ACQUIS_ALL;
//setState(&mainState, MAIN_MOVE);

	int state = IDLE;
	char stateInput = 0;
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
		char receivedData[MAXIMUM_CHARACTERS_BUFFER_SIZE];
		if (readUSB() == 1) {
			setState(&mainState, MAIN_PID);
		}

		/* Main state machine */
		switch (mainState) {
		case MAIN_IDLE:
			//if (consigne != 0)
			//setState(&mainState, consigne);
			break;
		case MAIN_ACQUIS:
			/* Action lors d'une acquisition */
			stateInput = readUSB();
			if (stateInput != 0)
				state = stateInput;

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
		case MAIN_ACQUIS_ALL:
			/***** Motor 1 *****/
			// Seulement le moteur 1 op�rationnel
			MotorSetDirection(1, CLOCK);
			MotorSetDirection(2, BRAKE_G);
			MotorSetDirection(3, BRAKE_G);
			MotorSetDirection(4, BRAKE_G);
			// Pour d�buter le remplissage du buffer
			bufferWheelIndex1 = 0;
			// Premi�re vitesse du moteur
			MotorSetSpeed(1, PREMIER_ECHELON);
			// D�lai avant le prochain PWM
			Delayms(DUREE_ECHELON_1);
			MotorSetSpeed(1, DEUXIEME_ECHELON);
			// D�lai de la fin de l'asservissement
			Delayms(DUREE_ECHELON_2);
			/***** Motor 2 *****/
			MotorSetDirection(1, BRAKE_G);
			MotorSetDirection(2, CLOCK);
			MotorSetSpeed(1, 0);
			// Pour d�buter le remplissage du buffer
			bufferWheelIndex2 = 0;
			MotorSetSpeed(2, PREMIER_ECHELON);
			// Delai du 1er �chelon
			Delayms(DUREE_ECHELON_1);
			// 2e �chelon
			MotorSetSpeed(2, DEUXIEME_ECHELON);
			Delayms(DUREE_ECHELON_2);
			/***** Motor 3 *****/
			MotorSetDirection(2, BRAKE_G);
			MotorSetDirection(3, CLOCK);
			MotorSetSpeed(2, 0);
			// Pour d�buter le remplissage du buffer
			bufferWheelIndex3 = 0;
			MotorSetSpeed(3, PREMIER_ECHELON);
			// Delai du 1er �chelon
			Delayms(DUREE_ECHELON_1);
			// 2e �chelon
			MotorSetSpeed(3, DEUXIEME_ECHELON);
			Delayms(DUREE_ECHELON_2);
			/***** Motor 4 *****/
			MotorSetDirection(3, BRAKE_G);
			MotorSetDirection(4, CLOCK);
			MotorSetSpeed(3, 0);
			// Pour d�buter le remplissage du buffer
			bufferWheelIndex4 = 0;
			MotorSetSpeed(4, PREMIER_ECHELON);
			// Delai du 1er �chelon
			Delayms(DUREE_ECHELON_1);
			// 2e �chelon
			MotorSetSpeed(4, DEUXIEME_ECHELON);
			Delayms(DUREE_ECHELON_2);
			// On stop le moteur
			MotorSetSpeed(4, 0);

			//fillDummiesBuffers();

			/****** ENVOIE DONN�ES MOTEUR 1 ******/
			// On attends que l'ordinateur demande des donn�es
			char cmdAcquis = 0;
			while (cmdAcquis == 0) {
				cmdAcquis = readUSB();
			}
			VCP_DataTx((uint8_t*) bufferWheel1, MAX_WHEEL_INDEX * 2);
			/****** ENVOIE DONN�ES MOTEUR 2 ******/
			cmdAcquis = 0;
			while (cmdAcquis == 0) {
				cmdAcquis = readUSB();
			}
			VCP_DataTx((uint8_t*) bufferWheel2, MAX_WHEEL_INDEX * 2);
			/****** ENVOIE DONN�ES MOTEUR 3 ******/
			cmdAcquis = 0;
			while (cmdAcquis == 0) {
				cmdAcquis = readUSB();
			}
			VCP_DataTx((uint8_t*) bufferWheel3, MAX_WHEEL_INDEX * 2);
			/****** ENVOIE DONN�ES MOTEUR 4 ******/
			cmdAcquis = 0;
			while (cmdAcquis == 0) {
				cmdAcquis = readUSB();
			}
			VCP_DataTx((uint8_t*) bufferWheel4, MAX_WHEEL_INDEX * 2);

			break;
		case MAIN_TEST_SPEED_PID:
			/* Initialization of wheel 1 PIDs */
			PID_init(&PID_SPEED1, PID_SPEED1_KP, PID_SPEED1_KI, PID_SPEED1_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED1, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);

			break;
		case MAIN_DEAD_ZONE:
			// On fait tourner tous les moteurs dans la m�me direction
			MotorSetDirection(1, CLOCK);
			MotorSetDirection(2, CLOCK);
			MotorSetDirection(3, CLOCK);
			MotorSetDirection(4, CLOCK);
			// On fait passer les vitesses de moteurs par toute les PWM
			for (int i = DEAD_START_PWM; i <= DEAD_STOP_PWM; i++) {
				MotorSetSpeed(1, i);
				MotorSetSpeed(2, i);
				MotorSetSpeed(3, i);
				MotorSetSpeed(4, i);
				// On change l'index pour que dans l'interruption
				// ce soit le bon qui soit rempli
				bufferDeadZoneIndex1 = i;
				bufferDeadZoneIndex2 = i;
				bufferDeadZoneIndex3 = i;
				bufferDeadZoneIndex4 = i;
				// On attends un peu pour avoir une valeue repr�sentative
				Delayms(100);
			}
			// On attends que l'ordinateur demande des donn�es
			char cmdDeadZone = 0;
			while (cmdDeadZone == 0) {
				cmdDeadZone = readUSB();
			}
			VCP_DataTx((uint8_t*) bufferDeadZone1, MAX_DEAD_ZONE * 2);
			break;
		case MAIN_MOVE:
			motorRoutine();
			break;
		case MAIN_PID:
			consigneX = 0.24;
			consigneY = 0.24;

			/* Initialization of wheel 1 PIDs */
			PID_init(&PID_SPEED1, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED1, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION1, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);
			PID_SetOutputLimits(&PID_POSITION1, MIN_POS_COMMAND,
					MAX_POS_COMMAND);

			/* Initialization of wheel 2 PIDs */
			PID_init(&PID_SPEED2, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED2, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION2, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			/* Initialization of wheel 3 PIDs */
			PID_init(&PID_SPEED3, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED3, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION3, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			/* Initialization of wheel 4 PIDs */
			PID_init(&PID_SPEED4, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD,
					PID_Direction_Direct);
			PID_SetOutputLimits(&PID_SPEED4, MIN_SPEED_COMMAND,
					MAX_SPEED_COMMAND);
			PID_init(&PID_POSITION4, PID_POSITION_KP, PID_POSITION_KI,
					PID_POSITION_KD, PID_Direction_Direct);

			PID_POSITION1.mySetpoint = 1;
			PID_POSITION2.mySetpoint = 1;
			PID_POSITION3.mySetpoint = 1;
			PID_POSITION4.mySetpoint = 1;

			while (1) {

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

				if (PID_Compute_Position(&PID_POSITION1)) {
					float positionOutput = PID_POSITION1.myOutput;
					PID_SPEED1.mySetpoint = positionOutput;

					if (PID_Compute_Speed(&PID_SPEED1)) {
						int cmdMotor1 = PID_SPEED1.myOutput;
						if (cmdMotor1 > 0) {
							speedDirection1 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(1, CLOCK);
							//MotorSetDirection(3, CLOCK);
						} else if (cmdMotor1 < 0) {

							speedDirection1 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(1, COUNTER_CLOCK);
							//MotorSetDirection(3, COUNTER_CLOCK);
							cmdMotor1 = -cmdMotor1;
						} else {
							speedDirection1 = SPEED_DIRECTION_NONE;
							MotorSetDirection(1, BRAKE_G);
							//MotorSetDirection(3, BRAKE_G);

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
						TM_USB_VCP_Putc(2);
						TM_USB_VCP_Putc(8);
						VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));
						VCP_DataTx((uint8_t*) &positionOutput, sizeof(float));

					}
				}

				if (PID_Compute_Position(&PID_POSITION2)) {
					float positionOutput = PID_POSITION2.myOutput;
					PID_SPEED2.mySetpoint = positionOutput;

					if (PID_Compute_Speed(&PID_SPEED2)) {
						int cmdMotor2 = PID_SPEED2.myOutput;
						if (cmdMotor2 > 0) {
							speedDirection2 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(2, CLOCK);
							//MotorSetDirection(4, CLOCK);
						} else if (cmdMotor2 < 0) {

							speedDirection2 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(2, COUNTER_CLOCK);
							//MotorSetDirection(4, CLOCK);
							cmdMotor2 = -cmdMotor2;
						} else {
							speedDirection2 = SPEED_DIRECTION_NONE;
							MotorSetDirection(2, BRAKE_G);
							//MotorSetDirection(4, BRAKE_G);

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

				if (PID_Compute_Position(&PID_POSITION3)) {
					float positionOutput = PID_POSITION3.myOutput;
					PID_SPEED3.mySetpoint = positionOutput;

					if (PID_Compute_Speed(&PID_SPEED3)) {
						int cmdMotor3 = PID_SPEED3.myOutput;
						if (cmdMotor3 > 0) {
							speedDirection3 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(3, COUNTER_CLOCK);
							//MotorSetDirection(1, COUNTER_CLOCK);
						} else if (cmdMotor3 < 0) {

							speedDirection3 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(3, CLOCK);
							//MotorSetDirection(1, CLOCK);
							cmdMotor3 = -cmdMotor3;
						} else {
							speedDirection3 = SPEED_DIRECTION_NONE;
							MotorSetDirection(3, BRAKE_G);
							//MotorSetDirection(1, BRAKE_G);

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
					float positionOutput = PID_POSITION4.myOutput;
					PID_SPEED4.mySetpoint = positionOutput;

					if (PID_Compute_Speed(&PID_SPEED4)) {
						int cmdMotor4 = PID_SPEED4.myOutput;
						if (cmdMotor4 > 0) {
							speedDirection4 = SPEED_DIRECTION_FORWARD;
							MotorSetDirection(4, COUNTER_CLOCK);
							//MotorSetDirection(2, COUNTER_CLOCK);
						} else if (cmdMotor4 < 0) {

							speedDirection4 = SPEED_DIRECTION_BACKWARD;
							MotorSetDirection(4, CLOCK);
							//MotorSetDirection(2, CLOCK);
							cmdMotor4 = -cmdMotor4;
						} else {
							speedDirection4 = SPEED_DIRECTION_NONE;
							MotorSetDirection(4, BRAKE_G);
							//MotorSetDirection(2, BRAKE_G);

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
		PID_SPEED1.myInput = numberOfSpeedEdges1;
		PID_SPEED2.myInput = numberOfSpeedEdges2;
		PID_SPEED3.myInput = numberOfSpeedEdges3;
		PID_SPEED4.myInput = numberOfSpeedEdges4;

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

#ifdef ACQUIS
		if (bufferWheelIndex1 < MAX_WHEEL_INDEX)
			bufferWheel1[bufferWheelIndex1++] = numberOfSpeedEdges1;
		if (bufferWheelIndex2 < MAX_WHEEL_INDEX)
			bufferWheel2[bufferWheelIndex2++] = numberOfSpeedEdges2;
		if (bufferWheelIndex3 < MAX_WHEEL_INDEX)
			bufferWheel3[bufferWheelIndex3++] = numberOfSpeedEdges3;
		if (bufferWheelIndex4 < MAX_WHEEL_INDEX)
			bufferWheel4[bufferWheelIndex4++] = numberOfSpeedEdges4;
#endif
#ifdef FIND_DEAD_ZONE
		// rentrer les valeurs dans la m�me buffer, dans le while on change l'index
		if (bufferDeadZoneIndex1 < MAX_DEAD_ZONE)
			bufferDeadZone1[bufferDeadZoneIndex1] = numberOfSpeedEdges1;
		if (bufferDeadZoneIndex2 < MAX_DEAD_ZONE)
			bufferDeadZone2[bufferDeadZoneIndex2] = numberOfSpeedEdges2;
		if (bufferDeadZoneIndex3 < MAX_DEAD_ZONE)
			bufferDeadZone3[bufferDeadZoneIndex3] = numberOfSpeedEdges3;
		if (bufferDeadZoneIndex4 < MAX_DEAD_ZONE)
			bufferDeadZone4[bufferDeadZoneIndex4] = numberOfSpeedEdges4;
#endif
		numberOfSpeedEdges1 = 0;
		numberOfSpeedEdges2 = 0;
		numberOfSpeedEdges3 = 0;
		numberOfSpeedEdges4 = 0;

	}
}
