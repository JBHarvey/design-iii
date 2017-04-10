#include "bc_manch.h"

// Variables utilisées conjointement dans l'Handler
uint8_t BcManchFlag = 0;
uint16_t bufferBcManchIndex;
uint8_t bufferBcManch[BC_MANCH_BUFFER_SIZE];
uint8_t manchChecked = 0;
char messageToDisplayHold[MESSAGE_TO_DISPLAY_LENGTH];

void initBCManch() {
	/* TIM7 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 100 - 1; // 1 MHz down to 10 KHz (0.1 ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // Down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	/* TIM7 enable counter */
	TIM_Cmd(TIM7, ENABLE);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM7_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

void obtenirCodeManch(uint8_t* bufferCodeManch) {
	uint8_t bufferCodeManchIndex = 0;
	bufferBcManchIndex = 0;
	uint8_t valeurCourante = bufferBcManch[0];
	int i;
	for (i = 0; i < BC_MANCH_BUFFER_SIZE; i++) {
		int iNbOccurence = 0;
		while (bufferBcManch[i++] == valeurCourante) {
			iNbOccurence++;
		}
		if (iNbOccurence > BC_MANCH_SEUIL_TOP) {
			bufferCodeManch[bufferCodeManchIndex++] = valeurCourante;
			bufferCodeManch[bufferCodeManchIndex++] = valeurCourante;
		} else if (iNbOccurence < BC_MANCH_SEUIL_BOTTOM) {
			bufferCodeManch[bufferCodeManchIndex++] = valeurCourante;
		}

		valeurCourante = bufferBcManch[i];
	}
}

void decodeManchBC() {
	uint8_t manchesterFactorVerification = 0;
	uint8_t codeDecoder[BC_MANCH_BUFFER_SIZE];
	uint8_t informationBits[INFORMATION_BITS_LENGTH];
	char manchesterOrientationVerification[ORIENTATION_LENGTH] = { ' ', ' ',
			' ', ' ', ' ' };
	uint8_t manchesterFigureVerification = getFigureFromInformationBits(
			informationBits);

	if (BcManchFlag == MANCH_BC_DECODE) {

		obtenirCodeManch(codeDecoder);
		if (decodeManchester(informationBits, codeDecoder)
				== VALID_INFORMATION) {

			setOrientationFromInformationBits(informationBits,
					manchesterOrientationVerification);
			manchesterFactorVerification = getFactorFromInformationBits(
					informationBits);

			sendManchesterCode(manchesterFigureVerification,
					manchesterFactorVerification,
					manchesterOrientationVerification);

			char messageToDisplay[MESSAGE_TO_DISPLAY_LENGTH];

			setMessageToDisplay(manchesterFigureVerification,
					manchesterOrientationVerification,
					manchesterFactorVerification, messageToDisplay);

			/*if (manchChecked == 0) {
			 memcpy(messageToDisplayHold, messageToDisplay,
			 MESSAGE_TO_DISPLAY_LENGTH);
			 manchChecked = 1;
			 } else if (manchChecked == 1) {
			 if (strcmp(messageToDisplayHold, messageToDisplay) == 0) {
			 displayManchesterMessage(messageToDisplay);
			 GPIO_SetBits(GPIOD, GPIO_Pin_13);

			 sendManchesterCode(manchesterFigureVerification,
			 manchesterFactorVerification,
			 manchesterOrientationVerification);
			 }

			 manchChecked = 0;
			 }
			 BcManchFlag = MANCH_BC_DONE;
			 }*/
			displayManchesterMessage(messageToDisplay);
			GPIO_SetBits(GPIOD, GPIO_Pin_13);

			BcManchFlag = MANCH_BC_DONE;

		} else {
			BcManchFlag = MANCH_BC_ACQUIRE;
		}
	}
}

/* Interruption du TIM7 qui permet d'échantillonner le signal Manchester */
extern void TIM7_IRQHandler() {
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		// 0 indique que l'on est entrain d'échantillonner
		if (BcManchFlag == MANCH_BC_ACQUIRE) {
			if (bufferBcManchIndex < BC_MANCH_BUFFER_SIZE) {
				bufferBcManch[bufferBcManchIndex++] = GPIO_ReadInputDataBit(
						GPIOD, MANCHESTER_PIN);
			} else {
				// Le buffer est plein, on est prêt à le traiter
				BcManchFlag = 1;
			}
		}
	}
}

