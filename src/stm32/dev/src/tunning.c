#include "tunning.h"

/***************************************************
 *  Les buffers qui suivent servent ï¿½ l'acquisition pour
 * bien faire l'asservissement des roues
 ***************************************************/
#ifdef ENABLE_ACQUIS
uint16_t bufferWheelIndex1 = 0;
uint16_t bufferWheelIndex2 = 0;
uint16_t bufferWheelIndex3 = 0;
uint16_t bufferWheelIndex4 = 0;
uint16_t bufferWheel1[MAX_WHEEL_INDEX];
uint16_t bufferWheel2[MAX_WHEEL_INDEX];
uint16_t bufferWheel3[MAX_WHEEL_INDEX];
uint16_t bufferWheel4[MAX_WHEEL_INDEX];
uint8_t bSendData = 0;
#endif

/******************************************************
 * Variables pour tester le PID de vitesse
 * ****************************************************/
#ifdef ENABLE_SPEED_PI
uint16_t bufferSpeedPIDIndex1 = 0;
uint16_t bufferSpeedPIDIndex2 = 0;
uint16_t bufferSpeedPIDIndex3 = 0;
uint16_t bufferSpeedPIDIndex4 = 0;
uint16_t bufferSpeedPID1[MAX_SPEED_PID_INDEX];
uint16_t bufferSpeedPID2[MAX_SPEED_PID_INDEX];
uint16_t bufferSpeedPID3[MAX_SPEED_PID_INDEX];
uint16_t bufferSpeedPID4[MAX_SPEED_PID_INDEX];

uint16_t bufferSpeedCmdIndex1 = 0;
uint16_t bufferSpeedCmdIndex2 = 0;
uint16_t bufferSpeedCmdIndex3 = 0;
uint16_t bufferSpeedCmdIndex4 = 0;
uint8_t bufferSpeedCmd1[MAX_SPEED_PID_INDEX];
uint8_t bufferSpeedCmd2[MAX_SPEED_PID_INDEX];
uint8_t bufferSpeedCmd3[MAX_SPEED_PID_INDEX];
uint8_t bufferSpeedCmd4[MAX_SPEED_PID_INDEX];

// Initialization of PID
PidType tunningPI1;
PidType tunningPI2;
PidType tunningPI3;
PidType tunningPI4;

uint8_t bSendDataSpeed = 0;
uint8_t bFlagSendDataSpeed = 0;
uint8_t bTunningSpeedDone = 0;
uint8_t bFlagTunningSpeedDone = 0;
uint8_t bFlagStartTunningSpeed = 0;
#endif

/********************************************************
 * Variables pour trouver la zone morte de chacune des roues
 ********************************************************/
#ifdef ENABLE_DEAD_ZONE
uint8_t bufferDeadZoneIndex = 0;
uint16_t bufferDeadZone1[MAX_DEAD_ZONE];
uint16_t bufferDeadZone2[MAX_DEAD_ZONE];
uint16_t bufferDeadZone3[MAX_DEAD_ZONE];
uint16_t bufferDeadZone4[MAX_DEAD_ZONE];
uint8_t bufferDeadZoneCmd[MAX_DEAD_ZONE];

uint8_t bFlagTunningDeadZone = 0;
extern uint8_t bufferDeadZoneToFill = 0;
#endif

/*********************************************************
 * Variables pour tester le PID de position
 *********************************************************/
#define ENABLE_POSITION_PID
#ifdef ENABLE_POSITION_PID

uint16_t bufferPositionPIDIndex1 = 0;
float bufferPositionPID1[MAX_POSITION_INDEX];

uint16_t bufferSpeedPI1[MAX_POSITION_INDEX];
uint16_t bufferSpeedPI2[MAX_POSITION_INDEX];
uint16_t bufferSpeedPI3[MAX_POSITION_INDEX];
uint16_t bufferSpeedPI4[MAX_POSITION_INDEX];

uint16_t bufferSpeedPIIndex1 = 0;
uint16_t bufferSpeedPIIndex2 = 0;
uint16_t bufferSpeedPIIndex3 = 0;
uint16_t bufferSpeedPIIndex4 = 0;

PidType tunningSpeedPI1;
PidType tunningSpeedPI2;
PidType tunningSpeedPI3;
PidType tunningSpeedPI4;

PidType tunningPositionPID1;
PidType tunningPositionPID2;
PidType tunningPositionPID3;
PidType tunningPositionPID4;

uint8_t bSendDataPosition = 0;
uint8_t bFlagSendDataPosition = 0;
uint8_t bTunningPositionDone = 0;
uint8_t bFlagTunningPositionDone = 0;
uint8_t bFlagStartTunningPosition = 0;

uint8_t wheelsStartedX1 = 0;
uint8_t wheelsStartedX2 = 0;
uint8_t wheelsStartedY1 = 0;
uint8_t wheelsStartedY2 = 0;

#endif

#ifdef ENABLE_ACQUIS
void tunningIdentificationWheels() {
	/***** Motor 1 *****/
	// On dï¿½fini la direction
	MotorSetDirection(1, COUNTER_CLOCK);
	MotorSetDirection(2, COUNTER_CLOCK);
	MotorSetDirection(3, CLOCK);
	MotorSetDirection(4, CLOCK);

	// On initialise le dï¿½but du remplissage du buffer
	bufferWheelIndex1 = 0;
	bufferWheelIndex2 = 0;
	bufferWheelIndex3 = 0;
	bufferWheelIndex4 = 0;

	// Tous les moteurs dï¿½butent ï¿½ la mï¿½me vitesse
	MotorSetSpeed(1, PREMIER_ECHELON);
	MotorSetSpeed(2, PREMIER_ECHELON);
	MotorSetSpeed(3, PREMIER_ECHELON);
	MotorSetSpeed(4, PREMIER_ECHELON);

	// On attend le rï¿½gime permanent
	Delayms (DUREE_ECHELON_1);
	// On change la vitesse du moteur 1 seulement
	MotorSetSpeed(1, DEUXIEME_ECHELON);
	MotorSetSpeed(2, DEUXIEME_ECHELON);
	MotorSetSpeed(3, DEUXIEME_ECHELON);
	MotorSetSpeed(4, DEUXIEME_ECHELON);

	// On attend le rï¿½gume permanent
	Delayms (DUREE_ECHELON_2);
	/*
	 // On arrï¿½te les moteurs
	 MotorSetSpeed(1, 0);
	 MotorSetSpeed(2, 0);
	 MotorSetSpeed(3, 0);
	 MotorSetSpeed(4, 0);

	 /***** Motor 2 *****
	 MotorSetDirection(1, COUNTER_CLOCK);
	 MotorSetDirection(2, COUNTER_CLOCK);
	 MotorSetDirection(3, CLOCK);
	 MotorSetDirection(4, CLOCK);

	 // On initialise le dï¿½but du remplissage du buffer
	 bufferWheelIndex2 = 0;

	 // Tous les moteurs dï¿½butent ï¿½ la mï¿½me vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le rï¿½gime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(2, DEUXIEME_ECHELON);

	 // On attend le rï¿½gume permanent
	 Delayms (DUREE_ECHELON_2);

	 // On arrï¿½te les moteurs
	 MotorSetSpeed(1, 0);
	 MotorSetSpeed(2, 0);
	 MotorSetSpeed(3, 0);
	 MotorSetSpeed(4, 0);

	 /**********************************************************
	 * MOTEUR 3
	 **********************************************************
	 MotorSetDirection(1, CLOCK);
	 MotorSetDirection(2, CLOCK);
	 MotorSetDirection(3, COUNTER_CLOCK);
	 MotorSetDirection(4, COUNTER_CLOCK);

	 // On initialise le dï¿½but du remplissage du buffer
	 bufferWheelIndex3 = 0;

	 // Tous les moteurs dï¿½butent ï¿½ la mï¿½me vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le rï¿½gime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(3, DEUXIEME_ECHELON);

	 // On attend le rï¿½gume permanent
	 Delayms (DUREE_ECHELON_2);

	 // On arrï¿½te les moteurs
	 MotorSetSpeed(1, 0);
	 MotorSetSpeed(2, 0);
	 MotorSetSpeed(3, 0);
	 MotorSetSpeed(4, 0);

	 /**********************************************************
	 * MOTEUR 4
	 **********************************************************
	 MotorSetDirection(1, COUNTER_CLOCK);
	 MotorSetDirection(2, COUNTER_CLOCK);
	 MotorSetDirection(3, CLOCK);
	 MotorSetDirection(4, CLOCK);

	 // On initialise le dï¿½but du remplissage du buffer
	 bufferWheelIndex4 = 0;

	 // Tous les moteurs dï¿½butent ï¿½ la mï¿½me vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le rï¿½gime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(4, DEUXIEME_ECHELON);

	 // On attend le rï¿½gume permanent
	 Delayms (DUREE_ECHELON_2);
	 */
	// On arrï¿½te les moteurs
	MotorSetSpeed(1, 0);
	MotorSetSpeed(2, 0);
	MotorSetSpeed(3, 0);
	MotorSetSpeed(4, 0);

	/*** FIN de l'identification ***/

	// On attends que l'ordinateur demande des donnï¿½es
	TM_DISCO_LedOn (LED_ORANGE);
	while (bSendData == 0) {
		// Intermï¿½diaire nï¿½cessaire pour la synchonisation des donnï¿½es
		bSendData = bFlagSendData;
	}
	tunningSendIdenWheels();
}

void tunningSendIdenWheels() {
	/****** ENVOIE DONNï¿½ES MOTEUR 1 ******/
	VCP_DataTx((uint8_t*) bufferWheel1, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNï¿½ES MOTEUR 2 ******/
	VCP_DataTx((uint8_t*) bufferWheel2, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNï¿½ES MOTEUR 3 ******/
	VCP_DataTx((uint8_t*) bufferWheel3, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNï¿½ES MOTEUR 4 ******/
	VCP_DataTx((uint8_t*) bufferWheel4, MAX_WHEEL_INDEX * 2);
	bSendData = 0;
	while(1)
	{
// Meurt
	}
}
#endif

#ifdef ENABLE_SPEED_PI
void tunningSpeedPI() {

	// Initialization of wheel 1 PIDs
	PID_init(&tunningPI1, PID_SPEED1_KP, PID_SPEED1_KI, 0,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&tunningPI1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningPI1.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 2 PIDs
	PID_init(&tunningPI2, PID_SPEED2_KP, PID_SPEED2_KI, 0,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&tunningPI2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningPI2.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 3 PIDs
	PID_init(&tunningPI3, PID_SPEED3_KP, PID_SPEED3_KI, 0,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&tunningPI3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningPI3.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 4 PIDs
	PID_init(&tunningPI4, PID_SPEED4_KP, PID_SPEED4_KI, 0,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&tunningPI4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningPI4.mySetpoint = CONSIGNE_SPEED;

	// Setting of direction for wheels
	if (CONSIGNE_SPEED > 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(3, CLOCK);
		MotorSetDirection(4, CLOCK);
	} else if (CONSIGNE_SPEED < 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(3, CLOCK);
		MotorSetDirection(4, CLOCK);
	} else {
		MotorSetDirection(1, BRAKE_G);
		MotorSetDirection(2, BRAKE_G);
		MotorSetDirection(3, BRAKE_G);
		MotorSetDirection(4, BRAKE_G);
	}

	// Redï¿½marre les buffers
	bufferSpeedPIDIndex1 = 0;
	bufferSpeedPIDIndex2 = 0;
	bufferSpeedPIDIndex3 = 0;
	bufferSpeedPIDIndex4 = 0;

	// Initialise l'ï¿½tat
	bTunningSpeedDone = 0;
	bFlagTunningSpeedDone = 0;
	bFlagStartTunningSpeed = 1;

	while (bTunningSpeedDone == 0) {

		// Apply command for motor 1
		if (PID_Compute_Speed (&tunningPI1)) {
			uint8_t cmdMotor = (int) tunningPI1.myOutput;
			if(bufferSpeedCmdIndex1 < MAX_SPEED_PID_INDEX) {
				bufferSpeedCmd1[bufferSpeedCmdIndex1++] = cmdMotor;
			}
			MotorSetSpeed(1, cmdMotor);
			PID_SetMode(&tunningPI1, PID_Mode_Manual);
		}
		// Apply command for motor 2
		if (PID_Compute_Speed (&tunningPI2)) {
			uint8_t cmdMotor = (int) tunningPI2.myOutput;
			if(bufferSpeedCmdIndex2 < MAX_SPEED_PID_INDEX) {
				bufferSpeedCmd2[bufferSpeedCmdIndex2++] = cmdMotor;
			}
			MotorSetSpeed(2, cmdMotor);
			PID_SetMode(&tunningPI2, PID_Mode_Manual);
		}
		// Apply command for motor 3
		if (PID_Compute_Speed (&tunningPI3)) {
			uint8_t cmdMotor = (int) tunningPI3.myOutput;
			if(bufferSpeedCmdIndex3 < MAX_SPEED_PID_INDEX) {
				bufferSpeedCmd3[bufferSpeedCmdIndex3++] = cmdMotor;
			}
			MotorSetSpeed(3, cmdMotor);
			PID_SetMode(&tunningPI3, PID_Mode_Manual);
		}
		// Apply command for motor 4
		if (PID_Compute_Speed (&tunningPI4)) {
			uint8_t cmdMotor = (int) tunningPI4.myOutput;
			if(bufferSpeedCmdIndex4 < MAX_SPEED_PID_INDEX) {
				bufferSpeedCmd4[bufferSpeedCmdIndex4++] = cmdMotor;
			}
			MotorSetSpeed(4, cmdMotor);
			PID_SetMode(&tunningPI4, PID_Mode_Manual);
		}
		bTunningSpeedDone = bFlagTunningSpeedDone;
	}
	bFlagStartTunningSpeed = 0;

	// On arrï¿½te les roues
	MotorSetSpeed(1, 0);
	MotorSetSpeed(2, 0);
	MotorSetSpeed(3, 0);
	MotorSetSpeed(4, 0);

	// On attends que l'ordinateur demande des donnï¿½es
	TM_DISCO_LedOn (LED_ORANGE);
	while (bSendDataSpeed == 0) {
		// Intermï¿½diaire nï¿½cessaire pour la synchonisation des donnï¿½es
		bSendDataSpeed = bFlagSendDataSpeed;
	}
	tunningSendSpeedPI();
}

void tunningSendSpeedPI() {
	/****** ENVOIE DONNï¿½ES MOTEUR 1 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID1, MAX_SPEED_PID_INDEX * 2);
	VCP_DataTx((uint8_t*) bufferSpeedCmd1, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 2 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID2, MAX_SPEED_PID_INDEX * 2);

	VCP_DataTx((uint8_t*) bufferSpeedCmd2, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 3 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID3, MAX_SPEED_PID_INDEX * 2);

	VCP_DataTx((uint8_t*) bufferSpeedCmd3, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 4 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID4, MAX_SPEED_PID_INDEX * 2);
	VCP_DataTx((uint8_t*) bufferSpeedCmd4, MAX_SPEED_PID_INDEX);
	bFlagSendDataSpeed = 0;
	bSendDataSpeed = 0;
	while(1)
	{
// MEURT
	}
}

#endif

#ifdef ENABLE_POSITION_PID

void tunningPositionPID() {
	/**************** Direction des moteurs *********************/
	MotorSetDirection(1, COUNTER_CLOCK);
	MotorSetDirection(2, COUNTER_CLOCK);
	MotorSetDirection(3, CLOCK);
	MotorSetDirection(4, CLOCK);
	moveDownPrehensor();
	Delayms(2500);
	/*************** Initialisation des PI de vitesse *******************/
	// Initialization of wheel 1 PIDs
	PID_init(&tunningSpeedPI1, PID_SPEED1_KP, PID_SPEED1_KI, 0,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&tunningSpeedPI1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningSpeedPI1.offset = PID_SPEED1_OFFSET;

	// Initialization of wheel 2 PIDs
	PID_init(&tunningSpeedPI2, PID_SPEED2_KP, PID_SPEED2_KI, 0,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&tunningSpeedPI2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningSpeedPI2.offset = PID_SPEED2_OFFSET;

	// Initialization of wheel 3 PIDs
	PID_init(&tunningSpeedPI3, PID_SPEED3_KP, PID_SPEED3_KI, 0,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&tunningSpeedPI3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningSpeedPI3.offset = PID_SPEED3_OFFSET;

	// Initialization of wheel 4 PIDs
	PID_init(&tunningSpeedPI4, PID_SPEED4_KP, PID_SPEED4_KI, 0,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&tunningSpeedPI4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);
	tunningSpeedPI4.offset = PID_SPEED4_OFFSET;

	/************ Initialisation des PID de position *******************/
	PID_init(&tunningPositionPID1, PID_POSITION1_KP, PID_POSITION1_KI,
			PID_POSITION1_KD, PID_Direction_Direct, PID_POSITION1_KD);
	PID_SetOutputLimits(&tunningPositionPID1, MIN_POS_COMMAND, MAX_POS_COMMAND);

	float toSquare = (CONSIGNE_POSITION_X * CONSIGNE_POSITION_X)
			+ (CONSIGNE_POSITION_Y * CONSIGNE_POSITION_Y);

	float squared = sqroot(toSquare);

	tunningPositionPID1.mySetpoint = squared;

	float ratioX = CONSIGNE_POSITION_X
			/ (CONSIGNE_POSITION_X + CONSIGNE_POSITION_Y);
	float ratioY = CONSIGNE_POSITION_Y
			/ (CONSIGNE_POSITION_X + CONSIGNE_POSITION_Y);

	// Redémarre les buffers
	bufferPositionPIDIndex1 = 0;

	// Initialise l'état
	bTunningPositionDone = 0;
	bFlagTunningPositionDone = 0;
	bFlagStartTunningPosition = 1;
	bufferPositionPIDIndex1 = 0;
	bufferSpeedPIIndex1 = 0;
	bufferSpeedPIIndex2 = 0;
	bufferSpeedPIIndex3 = 0;
	bufferSpeedPIIndex4 = 0;
	while (bTunningPositionDone == 0) {

		if (PID_Compute_Position(&tunningPositionPID1)) {
			float positionOutput1 = tunningPositionPID1.myOutput;

			tunningSpeedPI1.mySetpoint = ratioX * positionOutput1;
			tunningSpeedPI2.mySetpoint = ratioY * positionOutput1;
			tunningSpeedPI3.mySetpoint = ratioX * positionOutput1;
			tunningSpeedPI4.mySetpoint = ratioY * positionOutput1;

			// Apply command for motor 1
			if (PID_Compute_Speed(&tunningSpeedPI1)) {
				uint8_t cmdMotor = tunningSpeedPI1.myOutput;
				if (cmdMotor > 0) {
					MotorSetDirection(1, COUNTER_CLOCK);
				} else if (cmdMotor < 0) {
					MotorSetDirection(1, CLOCK);
					cmdMotor = -cmdMotor;
				} else {
					speedDirection2 = SPEED_DIRECTION_NONE;
					MotorSetDirection(1, BRAKE_V);
				}

				MotorSetSpeed(1, cmdMotor);
				PID_SetMode(&tunningSpeedPI1, PID_Mode_Manual);
			}
			// Apply command for motor 2
			if (PID_Compute_Speed(&tunningSpeedPI2)) {
				uint8_t cmdMotor = tunningSpeedPI2.myOutput;
				if (cmdMotor > 0) {
					MotorSetDirection(2, COUNTER_CLOCK);
				} else if (cmdMotor < 0) {
					MotorSetDirection(2, CLOCK);
					cmdMotor = -cmdMotor;
				} else {
					speedDirection2 = SPEED_DIRECTION_NONE;
					MotorSetDirection(1, BRAKE_V);
				}
				MotorSetSpeed(2, cmdMotor);
				PID_SetMode(&tunningSpeedPI2, PID_Mode_Manual);
			}
			// Apply command for motor 3
			if (PID_Compute_Speed(&tunningSpeedPI3)) {
				uint8_t cmdMotor = tunningSpeedPI3.myOutput;
				if (cmdMotor > 0) {
					MotorSetDirection(3, CLOCK);
				} else if (cmdMotor < 0) {
					MotorSetDirection(3, COUNTER_CLOCK);
					cmdMotor = -cmdMotor;
				} else {
					speedDirection3 = SPEED_DIRECTION_NONE;
					MotorSetDirection(3, BRAKE_V);
				}
				MotorSetSpeed(3, cmdMotor);
				PID_SetMode(&tunningSpeedPI3, PID_Mode_Manual);
			}
			// Apply command for motor 4
			if (PID_Compute_Speed(&tunningSpeedPI4)) {
				uint8_t cmdMotor = tunningSpeedPI4.myOutput;
				if (cmdMotor > 0) {
					MotorSetDirection(4, CLOCK);
				} else if (cmdMotor < 0) {
					MotorSetDirection(4, COUNTER_CLOCK);
					cmdMotor = -cmdMotor;
				} else {
					speedDirection4 = SPEED_DIRECTION_NONE;
					MotorSetDirection(4, BRAKE_V);
				}

				MotorSetSpeed(4, cmdMotor);
				PID_SetMode(&tunningSpeedPI4, PID_Mode_Manual);
			}
		}
		bTunningPositionDone = bFlagTunningPositionDone;
	}
	moveUpPrehensor();
	tunningSendPositionPID();
	while (1) {

	}
}

void tunningSendPositionPID() {
	/****** ENVOIE DONNÉES ******/
	VCP_DataTx((uint8_t*) bufferPositionPID1, MAX_POSITION_INDEX * 4);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferSpeedPI1, MAX_POSITION_INDEX * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferSpeedPI2, MAX_POSITION_INDEX * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferSpeedPI3, MAX_POSITION_INDEX * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferSpeedPI4, MAX_POSITION_INDEX * 2);
	Delayms(1000);
}

#endif

#ifdef ENABLE_DEAD_ZONE

void tunningDeadZone() {
	// On arrêt tous les moteurs
	for(int k = 1; k < 5; k++) {
		MotorSetDirection(k, BRAKE_G);
	}

	for(int i = 1; i < 5; i++)
	{
		// On active seulement le moteur en action
		MotorSetDirection(i, CLOCK);
		// On remet les itérations à 0
		bufferDeadZoneIndex = 0;
		// On sélectionne le buffer à remplir
		bufferDeadZoneToFill = i;
		for(int j = DEAD_START_PWM; j <= DEAD_STOP_PWM; j++) {
			MotorSetSpeed(i, j);
			Delayms(1000);
			bufferDeadZoneCmd[bufferDeadZoneIndex] = j;
			bufferDeadZoneIndex++;
		}
		// On arrête le moteur en action
		MotorSetDirection(i, BRAKE_G);
	}
	uint8_t bSendDataDeadZone = 0;
	while (bSendDataDeadZone == 0) {
		// Intermédiaire nécessaire pour la synchonisation des données
		bSendDataDeadZone = bFlagTunningDeadZone;
	}
	tunningSendDeadZone();
}

// Permet d'envoyé les données relatives à l'identification de la zone morte
void tunningSendDeadZone() {
	/****** ENVOIE DES DONNÉES ******/
	VCP_DataTx((uint8_t*) bufferDeadZoneCmd, MAX_DEAD_ZONE);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferDeadZone1, MAX_DEAD_ZONE * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferDeadZone2, MAX_DEAD_ZONE * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferDeadZone3, MAX_DEAD_ZONE * 2);
	Delayms(1000);
	VCP_DataTx((uint8_t*) bufferDeadZone4, MAX_DEAD_ZONE * 2);
	Delayms(1000);
}

#endif
