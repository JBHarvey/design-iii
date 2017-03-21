// Libraries STM
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
// Libraries TM
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_delay.h"
#include "defines.h"
#include "misc.h"
// Libraries DESIGN III
#include <com.h>
#include <motor.h>
#include <PID_v1.h>
#include "manchester.h"
#include "encoder.h"
#include "externalInterrupts.h"
#include "timers.h"
#include "pushButtons.h"
#include "leds.h"
#include "adc.h"
#include "tunning.h"
#include "utils.h"
#include "com.h"
#include "prehenseur.h"

#define TAILLE 500

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

volatile int incWheel1Canal1EncoderCounter = 0;
volatile int incWheel1Canal2EncoderCounter = 0;
volatile int decWheel1Canal1EncoderCounter = 0;
volatile int decWheel1Canal2EncoderCounter = 0;

volatile int incWheel2Canal1EncoderCounter = 0;
volatile int incWheel2Canal2EncoderCounter = 0;
volatile int decWheel2Canal1EncoderCounter = 0;
volatile int decWheel2Canal2EncoderCounter = 0;

volatile int incWheel3Canal1EncoderCounter = 0;
volatile int incWheel3Canal2EncoderCounter = 0;
volatile int decWheel3Canal1EncoderCounter = 0;
volatile int decWheel3Canal2EncoderCounter = 0;

volatile int incWheel4Canal1EncoderCounter = 0;
volatile int incWheel4Canal2EncoderCounter = 0;
volatile int decWheel4Canal1EncoderCounter = 0;
volatile int decWheel4Canal2EncoderCounter = 0;

volatile int wheel1Channel1UP = 0;
volatile int wheel1Channel2UP = 0;
volatile int wheel2Channel1UP = 0;
volatile int wheel2Channel2UP = 0;
volatile int wheel3Channel1UP = 0;
volatile int wheel3Channel2UP = 0;
volatile int wheel4Channel1UP = 0;
volatile int wheel4Channel2UP = 0;

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

/* frequency is in hertz*/
void systickInit(uint16_t frequency) {
	NVIC_SetPriority(SysTick_IRQn, 0);
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	(void) SysTick_Config(RCC_Clocks.HCLK_Frequency / frequency);

}

float getXMoveFromBuffer(uint8_t *data) {
	float xMove = *(float *) data;
	return xMove;
}

float getYMoveFromBuffer(uint8_t *data) {
	float yMove = *(float *) (data + Y_MOVE_DATA_BUFFER_INDEX);
	return yMove;
}

float getRadianMoveFromBuffer(uint8_t *data) {
	float radian = *(float *) data;
	return radian;
}

float getMoveFromRadian(float radian) {
	return radian * WHEEL_RADIUS;
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

void initAll(void) {
	// Initialization of STM system
	SystemInit();
	// Motors initialization
	initMotors();
	// Encoder initialization
	initEncoders();
	// LCD initialization
	TM_HD44780_Init(16, 2);
	// Intern LEDs initialization
	TM_DISCO_LedInit();
	// COM port initialization
	TM_USB_VCP_Init();
	// Push button initialization
	initBtn();
	// Extern LEDs initialization
	InitializeLEDs();
	// ADC antenne initialization
	initAdcAntenne();
	// Manchester initialization
	InitializeManchesterInput();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	initializeExternalInterruptLine4();
}

int main(void) {
	initAll();
	// Initialisation des variables
	//mainState = MAIN_ACQUIS_ALL;

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
	PID_SetOutputLimits(&PID_POSITION3, MIN_POS_COMMAND, MAX_POS_COMMAND);

	/* Initialization of wheel 4 PIDs */
	PID_init(&PID_SPEED4, PID_SPEED4_KP, PID_SPEED4_KI, PID_SPEED4_KD,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&PID_SPEED4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	PID_init(&PID_POSITION4, PID_POSITION4_KP, PID_POSITION4_KI,
			PID_POSITION4_KD, PID_Direction_Direct, PID_POSITION4_N);
	PID_SetOutputLimits(&PID_POSITION4, MIN_POS_COMMAND, MAX_POS_COMMAND);

	initPrehensor();

	/* Test routine LEDs */
	startLEDsRoutine();
	while (1) {
		/* Main state machine */
		switch (mainState) {
		case MAIN_IDLE:
			break;
		case MAIN_MOVE:
			motorRoutine();
			break;
		case MAIN_ACQUIS_ALL:
#ifdef ENABLE_ACQUIS
			tunningIdentificationWheels();
#endif
			break;
		case MAIN_TEST_SPEED_PID:
#ifdef ENABLE_SPEED_PI
			tunningSpeedPI();
#endif
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
							MotorSetDirection(1, COUNTER_CLOCK);
						} else if (cmdMotor1 < 0) {
							MotorSetDirection(1, CLOCK);
							cmdMotor1 = -cmdMotor1;
						} else {
							speedDirection1 = SPEED_DIRECTION_NONE;
							MotorSetDirection(1, BRAKE_V);
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
							MotorSetDirection(2, COUNTER_CLOCK);
							//MotorSetDirection(4, CLOCK);
						} else if (cmdMotor2 < 0) {
							MotorSetDirection(2, CLOCK);
							cmdMotor2 = -cmdMotor2;
						} else {
							speedDirection2 = SPEED_DIRECTION_NONE;
							MotorSetDirection(2, BRAKE_V);
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
							MotorSetDirection(3, CLOCK);
						} else if (cmdMotor3 < 0) {
							MotorSetDirection(3, COUNTER_CLOCK);
							cmdMotor3 = -cmdMotor3;
						} else {
							speedDirection3 = SPEED_DIRECTION_NONE;
							MotorSetDirection(3, BRAKE_V);
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
							MotorSetDirection(4, CLOCK);
						} else if (cmdMotor4 < 0) {
							MotorSetDirection(4, COUNTER_CLOCK);
							cmdMotor4 = -cmdMotor4;
						} else {
							speedDirection4 = SPEED_DIRECTION_NONE;
							MotorSetDirection(4, BRAKE_V);

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

			uint8_t dataToSend[4];
			dataToSend[0] = COMMAND_DECODED_MANCHESTER;
			dataToSend[1] = 1;
			dataToSend[2] = 4;
			dataToSend[3] = 'N';

			TM_USB_VCP_Send(dataToSend, 4);
			Delayms(10);
			break;
		case MAIN_PREHENSEUR:
			initPrehensor();
			moveUpPrehensor();
			Delayms(1000);
			moveDownPrehensor();
			Delayms(1000);
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
// wheel 1 channel 1
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges1++;

		wheel1Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
		wheel1Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);

		// check wheel 1 channel 2
		if (wheel1Channel1UP == wheel1Channel2UP) {

			numberOfPositionEdges1++;
			incWheel1Canal1EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_FORWARD;

		} else {
			numberOfPositionEdges1--;
			decWheel1Canal1EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection1 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges1++;
		 incWheel1Canal1EncoderCounter++;
		 } else if (speedDirection1 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges1--;
		 decWheel1Canal1EncoderCounter++;
		 }*/

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line5);
	}

	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges1++;

		wheel1Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5);
		wheel1Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6);

		// check wheel 1
		if (wheel1Channel1UP != wheel1Channel2UP) {
			numberOfPositionEdges1++;
			incWheel1Canal2EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_FORWARD;
		} else {
			numberOfPositionEdges1--;
			decWheel1Canal2EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection1 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges1++;
		 incWheel1Canal2EncoderCounter++;
		 } else if (speedDirection1 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges1--;
		 decWheel1Canal2EncoderCounter++;
		 }*/

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line6);
	}

	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges2++;

		wheel2Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
		wheel2Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);

		// check wheel 2
		if (wheel2Channel1UP == wheel2Channel2UP) {

			numberOfPositionEdges2++;
			incWheel2Canal1EncoderCounter++;
			speedDirection2 = SPEED_DIRECTION_FORWARD;

		} else {
			numberOfPositionEdges2--;
			decWheel2Canal1EncoderCounter++;
			speedDirection2 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection2 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges2++;
		 incWheel2Canal1EncoderCounter++;
		 } else if (speedDirection2 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges2--;
		 decWheel2Canal1EncoderCounter++;

		 }*/
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line8);
	}

	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges2++;

		wheel2Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8);
		wheel2Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);

		// check wheel 2
		if (wheel2Channel1UP != wheel2Channel2UP) {
			numberOfPositionEdges2++;
			incWheel2Canal2EncoderCounter++;
			speedDirection2 = SPEED_DIRECTION_FORWARD;

		} else {
			numberOfPositionEdges2--;
			decWheel2Canal2EncoderCounter++;
			speedDirection2 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection2 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges2++;
		 incWheel2Canal2EncoderCounter++;
		 } else if (speedDirection2 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges2--;
		 decWheel2Canal2EncoderCounter++;
		 }*/

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line9);
	}
}

extern void EXTI15_10_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges3++;

		wheel3Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
		wheel3Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);

		// check wheel 3
		if (wheel3Channel1UP != wheel3Channel2UP) {
			numberOfPositionEdges3++;
			incWheel3Canal1EncoderCounter++;
			speedDirection3 = SPEED_DIRECTION_FORWARD;
		} else {
			numberOfPositionEdges3--;
			decWheel3Canal1EncoderCounter++;
			speedDirection3 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection3 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges3++;
		 incWheel3Canal1EncoderCounter++;
		 } else if (speedDirection3 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges3--;
		 decWheel3Canal1EncoderCounter++;

		 }*/
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line10);
	}

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges3++;

		wheel3Channel1UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10);
		wheel3Channel2UP = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11);

		// check wheel 3
		if (wheel3Channel1UP == wheel3Channel2UP) {
			numberOfPositionEdges3++;
			incWheel3Canal2EncoderCounter++;
			speedDirection3 = SPEED_DIRECTION_FORWARD;
		} else {
			numberOfPositionEdges3--;
			decWheel3Canal2EncoderCounter++;
			speedDirection3 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection3 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges3++;
		 incWheel3Canal2EncoderCounter++;
		 }

		 else if (speedDirection3 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges3--;
		 decWheel3Canal2EncoderCounter++;
		 }*/
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line11);
	}

	if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges4++;

		wheel4Channel1UP = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
		wheel4Channel2UP = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

		// check wheel 4
		if (wheel4Channel1UP != wheel4Channel2UP) {
			numberOfPositionEdges4++;
			incWheel4Canal1EncoderCounter++;
			speedDirection4 = SPEED_DIRECTION_FORWARD;
		} else {
			numberOfPositionEdges4--;
			decWheel4Canal1EncoderCounter++;
			speedDirection4 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection4 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges4++;
		 incWheel4Canal1EncoderCounter++;
		 } else if (speedDirection4 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges4--;
		 decWheel4Canal1EncoderCounter++;
		 }*/
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line12);
	}

	if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
		/* increase ticks */
		numberOfSpeedEdges4++;

		wheel4Channel1UP = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
		wheel4Channel2UP = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

		// check wheel 4
		if (wheel4Channel1UP == wheel4Channel2UP) {
			numberOfPositionEdges4++;
			incWheel4Canal2EncoderCounter++;
			speedDirection4 = SPEED_DIRECTION_FORWARD;
		} else {
			numberOfPositionEdges4--;
			decWheel4Canal2EncoderCounter++;
			speedDirection4 = SPEED_DIRECTION_BACKWARD;
		}

		/*if (speedDirection4 == SPEED_DIRECTION_FORWARD) {
		 numberOfPositionEdges4++;
		 incWheel4Canal2EncoderCounter++;
		 } else if (speedDirection4 == SPEED_DIRECTION_BACKWARD) {
		 numberOfPositionEdges4--;
		 decWheel4Canal2EncoderCounter++;
		 }*/
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

#ifdef ENABLE_ACQUIS
		if (bufferWheelIndex1 < MAX_WHEEL_INDEX) {
			bufferWheel1[bufferWheelIndex1++] = numberOfSpeedEdges1;
		}
		if (bufferWheelIndex2 < MAX_WHEEL_INDEX) {
			bufferWheel2[bufferWheelIndex2++] = numberOfSpeedEdges2;
		}
		if (bufferWheelIndex3 < MAX_WHEEL_INDEX) {
			bufferWheel3[bufferWheelIndex3++] = numberOfSpeedEdges3;
		}
		if (bufferWheelIndex4 < MAX_WHEEL_INDEX) {
			bufferWheel4[bufferWheelIndex4++] = numberOfSpeedEdges4;
		}
#endif
#ifdef ENABLE_DEAD_ZONE
		// rentrer les valeurs dans la même buffer, dans le while on change l'index
		if (bufferDeadZoneIndex1 < MAX_DEAD_ZONE) {
			bufferDeadZone1[bufferDeadZoneIndex1] = numberOfSpeedEdges1;
		}
		if (bufferDeadZoneIndex2 < MAX_DEAD_ZONE) {
			bufferDeadZone2[bufferDeadZoneIndex2] = numberOfSpeedEdges2;
		}
		if (bufferDeadZoneIndex3 < MAX_DEAD_ZONE) {
			bufferDeadZone3[bufferDeadZoneIndex3] = numberOfSpeedEdges3;
		}
		if (bufferDeadZoneIndex4 < MAX_DEAD_ZONE) {
			bufferDeadZone4[bufferDeadZoneIndex4] = numberOfSpeedEdges4;
		}
#endif
#ifdef ENABLE_SPEED_PI
		if(bFlagStartTunningSpeed == 1) {
			/* update position pids */
			tunningPI1.myInput = numberOfSpeedEdges1;
			tunningPI2.myInput = numberOfSpeedEdges2;
			tunningPI3.myInput = numberOfSpeedEdges3;
			tunningPI4.myInput = numberOfSpeedEdges4;

			/* activate speed pids */
			PID_SetMode(&tunningPI1, PID_Mode_Automatic);
			PID_SetMode(&tunningPI2, PID_Mode_Automatic);
			PID_SetMode(&tunningPI3, PID_Mode_Automatic);
			PID_SetMode(&tunningPI4, PID_Mode_Automatic);

			if (bufferSpeedPIDIndex1 < MAX_SPEED_PID_INDEX) {
				bufferSpeedPID1[bufferSpeedPIDIndex1++] = numberOfSpeedEdges1;
			}
			if (bufferSpeedPIDIndex2 < MAX_SPEED_PID_INDEX) {
				bufferSpeedPID2[bufferSpeedPIDIndex2++] = numberOfSpeedEdges2;
			}
			if (bufferSpeedPIDIndex3 < MAX_SPEED_PID_INDEX) {
				bufferSpeedPID3[bufferSpeedPIDIndex3++] = numberOfSpeedEdges3;
			}
			if (bufferSpeedPIDIndex4 < MAX_SPEED_PID_INDEX) {
				bufferSpeedPID4[bufferSpeedPIDIndex4++] = numberOfSpeedEdges4;
			}
			else
			{
				bFlagTunningSpeedDone = 1;
			}
		}
#endif

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
/*
 void setPIDCoeficient(float xMove, float yMove) {
 float xMoveAbsolute;
 float yMoveAbsolute;

 if (xMove >= 0) {
 xMoveAbsolute = xMove;
 } else {
 xMoveAbsolute = -xMove;
 }

 if (yMove >= 0) {
 yMoveAbsolute = yMove;
 } else {
 yMoveAbsolute = -yMove;
 }

 float max;
 float min;
 uint8_t xAxisMax;

 if (xMoveAbsolute >= yMoveAbsolute) {
 xAxisMax = 1;
 max = xMoveAbsolute;
 min = yMoveAbsolute;
 } else {
 xAxisMax = 0;
 max = yMoveAbsolute;
 min = xMoveAbsolute;
 }


 float coef = 1 - ((max - min) / max);

 if (xAxisMax == 1) {
 PID_POSITION1.coef = coef;
 PID_POSITION3.coef = coef;
 PID_POSITION2.coef = 1;
 PID_POSITION4.coef = 1;
 } else {
 PID_POSITION1.coef = 1;
 PID_POSITION3.coef = 1;
 PID_POSITION2.coef = coef;
 PID_POSITION4.coef = coef;
 }
 }*/

/* callback when data arrives on USB */
extern void handle_full_packet(uint8_t type, uint8_t *data, uint8_t len) {
	switch (type) {
	case COMMAND_MOVE:
		if (len == 8 && type == COMMAND_MOVE) {
			/* send data to test send feature on Usb */
			uint8_t dataToSend[10] = { 1, 8, 3, 4, 5, 6, 7, 8, 9, 10 };
			TM_USB_VCP_Send(dataToSend, 10);
			/* get X Y distances */
			float xMove = getXMoveFromBuffer(data);
			float yMove = getYMoveFromBuffer(data);

			/*setPIDCoeficient(xMove, yMove);*/

			PID_POSITION1.mySetpoint = xMove;
			PID_POSITION3.mySetpoint = xMove;
			PID_POSITION2.mySetpoint = yMove;
			PID_POSITION4.mySetpoint = yMove;

			numberOfPositionEdges1 = 0;
			numberOfPositionEdges2 = 0;
			numberOfPositionEdges3 = 0;
			numberOfPositionEdges4 = 0;

			PID_POSITION1.ITerm = 0;
			PID_SPEED1.ITerm = 0;
			PID_POSITION2.ITerm = 0;
			PID_SPEED2.ITerm = 0;
			PID_POSITION3.ITerm = 0;
			PID_SPEED3.ITerm = 0;
			PID_POSITION4.ITerm = 0;
			PID_SPEED4.ITerm = 0;

			PID_POSITION1.lastInput = 0;
			PID_SPEED1.lastInput = 0;
			PID_POSITION2.lastInput = 0;
			PID_SPEED2.lastInput = 0;
			PID_POSITION3.lastInput = 0;
			PID_SPEED3.lastInput = 0;
			PID_POSITION4.lastInput = 0;
			PID_SPEED4.lastInput = 0;

			incWheel1Canal1EncoderCounter = 0;
			incWheel1Canal2EncoderCounter = 0;
			decWheel1Canal1EncoderCounter = 0;
			decWheel1Canal2EncoderCounter = 0;

			incWheel2Canal1EncoderCounter = 0;
			incWheel2Canal2EncoderCounter = 0;
			decWheel2Canal1EncoderCounter = 0;
			decWheel2Canal2EncoderCounter = 0;

			incWheel3Canal1EncoderCounter = 0;
			incWheel3Canal2EncoderCounter = 0;
			decWheel3Canal1EncoderCounter = 0;
			decWheel3Canal2EncoderCounter = 0;

			incWheel4Canal1EncoderCounter = 0;
			incWheel4Canal2EncoderCounter = 0;
			decWheel4Canal1EncoderCounter = 0;
			decWheel4Canal2EncoderCounter = 0;

			setState(&mainState, MAIN_PID);
		}
		break;
	case COMMAND_ROTATE:
		if (len == 4 && type == COMMAND_ROTATE) {
			/* send data to test send feature on Usb */
			//uint8_t dataToSend[10] = { 1, 8, 3, 4, 5, 6, 7, 8, 9, 10 };
			//TM_USB_VCP_Send(dataToSend, 10);
			/* get X Y distances */
			float radianToRotate = getRadianMoveFromBuffer(data);
			float move = getMoveFromRadian(radianToRotate);

			/*setPIDCoeficient(xMove, yMove);*/

			PID_POSITION1.mySetpoint = -move;
			PID_POSITION3.mySetpoint = move;
			PID_POSITION2.mySetpoint = -move;
			PID_POSITION4.mySetpoint = move;

			numberOfPositionEdges1 = 0;
			numberOfPositionEdges2 = 0;
			numberOfPositionEdges3 = 0;
			numberOfPositionEdges4 = 0;

			PID_POSITION1.ITerm = 0;
			PID_SPEED1.ITerm = 0;
			PID_POSITION2.ITerm = 0;
			PID_SPEED2.ITerm = 0;
			PID_POSITION3.ITerm = 0;
			PID_SPEED3.ITerm = 0;
			PID_POSITION4.ITerm = 0;
			PID_SPEED4.ITerm = 0;

			PID_POSITION1.lastInput = 0;
			PID_SPEED1.lastInput = 0;
			PID_POSITION2.lastInput = 0;
			PID_SPEED2.lastInput = 0;
			PID_POSITION3.lastInput = 0;
			PID_SPEED3.lastInput = 0;
			PID_POSITION4.lastInput = 0;
			PID_SPEED4.lastInput = 0;

			incWheel1Canal1EncoderCounter = 0;
			incWheel1Canal2EncoderCounter = 0;
			decWheel1Canal1EncoderCounter = 0;
			decWheel1Canal2EncoderCounter = 0;

			incWheel2Canal1EncoderCounter = 0;
			incWheel2Canal2EncoderCounter = 0;
			decWheel2Canal1EncoderCounter = 0;
			decWheel2Canal2EncoderCounter = 0;

			incWheel3Canal1EncoderCounter = 0;
			incWheel3Canal2EncoderCounter = 0;
			decWheel3Canal1EncoderCounter = 0;
			decWheel3Canal2EncoderCounter = 0;

			incWheel4Canal1EncoderCounter = 0;
			incWheel4Canal2EncoderCounter = 0;
			decWheel4Canal1EncoderCounter = 0;
			decWheel4Canal2EncoderCounter = 0;

			setState(&mainState, MAIN_PID);
		}
		break;
	case COMMAND_START_IDEN_WHEELS:
		setState(&mainState, MAIN_ACQUIS_ALL);
		break;
	case COMMAND_IDEN_WHEELS:
		bFlagSendData = 1;
		break;
#ifdef ENABLE_SPEED_PI
		case COMMAND_START_TEST_PID_SPEED:
		if(len == 32) {
			getPIFromBuffer(data);
			setState(&mainState, MAIN_TEST_SPEED_PID);
		}
		break;
		case COMMAND_SEND_PID_SPEED:
		bFlagSendDataSpeed = 1;
		break;
#endif
	case COMMAND_PENCIL_UP:
		moveUpPrehensor();
		break;
	case COMMAND_PENCIL_DOWN:
		moveDownPrehensor();
		break;
	case COMMAND_DECODE_MANCHESTER:
		setState(&mainState, MAIN_MANCH);
		break;
	case COMMAND_STOP_DECODE_MANCHESTER:
		setState(&mainState, MAIN_IDLE);
		break;
	}
}
