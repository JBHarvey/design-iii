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
#include "motion.h" // here

#define TAILLE 500

// buffer ticks for debug
volatile int ticksIndex4 = 0;
volatile int ticksBuffer4[TICKS_BUFFER_SIZE];

volatile int speedIndex = 0;
volatile uint16_t speedBuffer[MAX_SPEED_INDEX];

volatile int mainState = MAIN_IDLE;

// Buffer contenant tous les bits du manchester
volatile uint8_t manchesterBuffer[MANCHESTER_BUFFER_LENGTH];
volatile uint8_t manchesterIndex = 0;
volatile uint8_t manchesterState = MANCHESTER_WAIT_FOR_DECODING;
volatile uint8_t manchesterFigureVerification = 0;
volatile char manchesterOrientationVerification[ORIENTATION_LENGTH] = { ' ',
		' ', ' ', ' ', ' ' };
volatile uint8_t manchesterFactorVerification = 0;

volatile uint8_t sendMeasureCounter = 0;

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

void initAll(void) {
	// Initialization of STM system
	SystemInit();
	// LCD initialization
	TM_HD44780_Init(16, 2);
	// Intern LEDs initialization
	TM_DISCO_LedInit();
	// COM port initialization
	TM_USB_VCP_Init();
	// Motors initialization
	initMotors();
	// Encoder initialization
	initEncoders();
	// Push button initialization
	initBtn();

// Initialisation des variables
	mainState = MAIN_IDLE;
//setState(&mainState, MAIN_MOVE);

	int state = IDLE;

// Extern LEDs initialization
	InitializeLEDs();
	InitializeTimer6();

// initializations for manchester signal
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

	/* Initialization of PIDs */
	initAllPIDS();

	initPrehensor();

	moveUpPrehensor();
	Delayms(1000);

	/* Test routine LEDs */
	//startLEDsRoutine();
	while (1) {
		/* Main state machine */
		switch (mainState) {
		case MAIN_IDLE:
			break;
		case MAIN_MOVE:
			//motorRoutine();
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
		case MAIN_TEST_POS_PID:
#ifdef ENABLE_POSITION_PID
			tunningPositionPID();
#endif
			break;
		case MAIN_TEST_DEAD_ZONE:
#ifdef ENABLE_DEAD_ZONE
			tunningDeadZone();
#endif
			break;

		case MAIN_PID:
			setSpeedSetpoints();
			computeCustomPIDS();
			break;
		case MAIN_MANCH:

			tryToDecodeManchesterCode(&manchesterState, manchesterBuffer,
					&manchesterFigureVerification,
					manchesterOrientationVerification,
					&manchesterFactorVerification);

			if (manchesterFactorVerification != 0
					&& manchesterOrientationVerification[2] != ' ') {
				sendManchesterCode(manchesterFigureVerification,
						manchesterFactorVerification,
						manchesterOrientationVerification);
			}

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

		// check wheel 1
		if (wheel1Channel1UP == wheel1Channel2UP) {

			numberOfPositionEdges1++;
			incWheel1Canal1EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_FORWARD;

		} else {
			numberOfPositionEdges1--;
			decWheel1Canal1EncoderCounter++;
			speedDirection1 = SPEED_DIRECTION_BACKWARD;
		}

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

		/* Clear interrupt flag */
		EXTI_ClearITPendingBit (EXTI_Line13);
	}
}

extern void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		/* update Speed pids inputs */
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

		sendMeasureCounter++;

		if (sendMeasureCounter == 5) {
			/* envoyer les mesures captees, a remplacer par USART */
			/*sendMoveMeasures(numberOfPositionEdges1, numberOfPositionEdges2,
			 numberOfPositionEdges3, numberOfPositionEdges4,
			 numberOfSpeedEdges1, numberOfSpeedEdges2,
			 numberOfSpeedEdges3, numberOfSpeedEdges4);*/

			sendMeasureCounter = 0;
		}

		resetEncoderSpeedVariables();

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
		// rentrer les valeurs dans la m�me buffer, dans le while on change l'index
		if (bufferDeadZoneIndex < MAX_DEAD_ZONE) {
			if(bufferDeadZoneToFill == 1) {
				bufferDeadZone1[bufferDeadZoneIndex] = numberOfSpeedEdges1;
			}
			if(bufferDeadZoneToFill == 2) {
				bufferDeadZone2[bufferDeadZoneIndex] = numberOfSpeedEdges2;
			}
			if(bufferDeadZoneToFill == 3) {
				bufferDeadZone3[bufferDeadZoneIndex] = numberOfSpeedEdges3;
			}
			if(bufferDeadZoneToFill == 4) {
				bufferDeadZone4[bufferDeadZoneIndex] = numberOfSpeedEdges4;
			}
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
#ifdef ENABLE_POSITION_PID

		// On met � jour l'input du PID de position
		float setPoint = sqroot((numberOfPositionEdges1*numberOfPositionEdges3)
				+ (numberOfPositionEdges2 * numberOfPositionEdges4));
		tunningPositionPID1.myInput = (numberOfPositionEdges2 + numberOfPositionEdges4)/2;

		// On active le PID de position
		PID_SetMode(&tunningPositionPID1, PID_Mode_Automatic);

		// On met � jour les inputs des PI de vitesse
		tunningSpeedPI1.myInput = numberOfSpeedEdges1;
		tunningSpeedPI2.myInput = numberOfSpeedEdges2;
		tunningSpeedPI3.myInput = numberOfSpeedEdges3;
		tunningSpeedPI4.myInput = numberOfSpeedEdges4;

		// On active les PI de vitesse
		PID_SetMode(&tunningSpeedPI1, PID_Mode_Automatic);
		PID_SetMode(&tunningSpeedPI3, PID_Mode_Automatic);
		PID_SetMode(&tunningSpeedPI2, PID_Mode_Automatic);
		PID_SetMode(&tunningSpeedPI4, PID_Mode_Automatic);

		if (bufferPositionPIDIndex1 < MAX_POSITION_INDEX) {
			bufferPositionPID1[bufferPositionPIDIndex1++] =
			calculatePosition((numberOfPositionEdges1 + numberOfPositionEdges3)/2);
		}
		if (bufferSpeedPIIndex1 < MAX_POSITION_INDEX) {
			bufferSpeedPI1[bufferSpeedPIIndex1++] = numberOfSpeedEdges1;
		}
		if (bufferSpeedPIIndex2 < MAX_POSITION_INDEX) {
			bufferSpeedPI2[bufferSpeedPIIndex2++] = numberOfSpeedEdges2;
		}
		if (bufferSpeedPIIndex3 < MAX_POSITION_INDEX) {
			bufferSpeedPI3[bufferSpeedPIIndex3++] = numberOfSpeedEdges3;
		}
		if (bufferSpeedPIIndex4 < MAX_POSITION_INDEX) {
			bufferSpeedPI4[bufferSpeedPIIndex4++] = numberOfSpeedEdges4;
		}
#endif
	}
}

extern void TIM6_DAC_IRQHandler() {
	turnOffLEDs();
	disableTimer6Interrupt();
}

/* callback when data arrives on USB */
extern void handle_full_packet(uint8_t type, uint8_t *data, uint8_t len) {
	switch (type) {
	case COMMAND_MOVE:
		if (len == 8 && type == 0) {

			// stop the robot before moving again
			stopMove();

			//setMoveSettings(data);
			resetPositionEncoderVariables();

			setCustomMoveSettings(data);

			setState(&mainState, MAIN_PID);
		}
		break;
	case COMMAND_ROTATE:
		if (len == 4 && type == COMMAND_ROTATE) {
			// stop the robot before rotating
			stopMove();

			resetPositionEncoderVariables();

			//setRotateSettings(data);
			setCustomRotateSettings(data);

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
		setState(&mainState, MAIN_TEST_SPEED_PID);
		break;
		case COMMAND_SEND_PID_SPEED:
		bFlagSendDataSpeed = 1;
		break;
#endif
#ifdef ENABLE_POSITION_PID
		case COMMAND_START_TEST_PID_POS:
		setState(&mainState, MAIN_TEST_POS_PID);
		break;
		case COMMAND_SEND_PID_POS:
		bFlagTunningPositionDone = 1;
		break;
#endif
#ifdef ENABLE_DEAD_ZONE
		case COMMAND_START_TEST_DEAD_ZONE:
		setState(&mainState, MAIN_TEST_DEAD_ZONE);
		break;
		case COMMAND_SEND_DEAD_ZONE:
		bFlagTunningDeadZone = 1;
		break;
#endif
	case COMMAND_PREHENSOR_UP:
		moveUpPrehensor();
		sendPrehensorUpConfirmation();
		break;
	case COMMAND_PREHENSOR_DOWN:
		moveDownPrehensor();
		sendPrehensorDownConfirmation();
		break;
	case COMMAND_DECODE_MANCHESTER:
		setState(&mainState, MAIN_MANCH);
		break;
	case COMMAND_STOP_DECODE_MANCHESTER:
		// stop reading and sending adc signal
		setState(&mainState, MAIN_IDLE);
		sendStopSendingManchesterSignalConfirmation();
		break;
	case COMMAND_RED_LED:
		turnOnRedLED();
		EnableTimer6Interrupt();
		sendRedLightConfirmation();
		break;
	case COMMAND_GREEN_LED:
		turnOnGreenLED();
		EnableTimer6Interrupt();
		sendGreenLightConfirmation();
		break;
	}
}
