#include "tunning.h"

/***************************************************
 *  Les buffers qui suivent servent à l'acquisition pour
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

/*************** VARIABLES POUR ROTATION*****************/
#ifdef ENABLE_ROTATION
#define MAX_ROTATION_INDEX 1400
#define CONSIGNE_ANGLE 30
#define ROTATION_RAYON 10

#define CONSIGNE_SPEED_MEDIUM 0.08
#define CONSIGNE_SPEED_LOW 0.02

uint16_t bufferRotationIndex = 0;
float bufferRotation[MAX_ROTATION_INDEX];

uint16_t bufferRotationPI1[MAX_ROTATION_INDEX];
uint16_t bufferRotationPI2[MAX_ROTATION_INDEX];
uint16_t bufferRotationPI3[MAX_ROTATION_INDEX];
uint16_t bufferRotationPI4[MAX_ROTATION_INDEX];

uint16_t bufferRotationPIIndex1 = 0;
uint16_t bufferRotationPIIndex2 = 0;
uint16_t bufferRotationPIIndex3 = 0;
uint16_t bufferRotationPIIndex4 = 0;

PidType tunningRotationPI1;
PidType tunningRotationPI2;
PidType tunningRotationPI3;
PidType tunningRotationPI4;

PidType tunningRotationPID1;
PidType tunningRotationPID2;
PidType tunningRotationPID3;
PidType tunningRotationPID4;

uint8_t bSendDataRotation = 0;
uint8_t bFlagSendDataRotation = 0;
uint8_t bTunningRotationDone = 0;
uint8_t bFlagTunningRotationDone = 0;
uint8_t bFlagStartTunningRotation = 0;
#endif

#ifdef ENABLE_ACQUIS
void tunningIdentificationWheels() {
	/***** Motor 1 *****/
	// On défini la direction
	MotorSetDirection(1, COUNTER_CLOCK);
	MotorSetDirection(2, COUNTER_CLOCK);
	MotorSetDirection(3, CLOCK);
	MotorSetDirection(4, CLOCK);

	// On initialise le début du remplissage du buffer
	bufferWheelIndex1 = 0;
	bufferWheelIndex2 = 0;
	bufferWheelIndex3 = 0;
	bufferWheelIndex4 = 0;

	// Tous les moteurs débutent à la même vitesse
	MotorSetSpeed(1, PREMIER_ECHELON);
	MotorSetSpeed(2, PREMIER_ECHELON);
	MotorSetSpeed(3, PREMIER_ECHELON);
	MotorSetSpeed(4, PREMIER_ECHELON);

	// On attend le régime permanent
	Delayms (DUREE_ECHELON_1);
	// On change la vitesse du moteur 1 seulement
	MotorSetSpeed(1, DEUXIEME_ECHELON);
	MotorSetSpeed(2, DEUXIEME_ECHELON);
	MotorSetSpeed(3, DEUXIEME_ECHELON);
	MotorSetSpeed(4, DEUXIEME_ECHELON);

	// On attend le régume permanent
	Delayms (DUREE_ECHELON_2);
	/*
	 // On arrête les moteurs
	 MotorSetSpeed(1, 0);
	 MotorSetSpeed(2, 0);
	 MotorSetSpeed(3, 0);
	 MotorSetSpeed(4, 0);

	 /***** Motor 2 *****
	 MotorSetDirection(1, COUNTER_CLOCK);
	 MotorSetDirection(2, COUNTER_CLOCK);
	 MotorSetDirection(3, CLOCK);
	 MotorSetDirection(4, CLOCK);

	 // On initialise le début du remplissage du buffer
	 bufferWheelIndex2 = 0;

	 // Tous les moteurs débutent à la même vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le régime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(2, DEUXIEME_ECHELON);

	 // On attend le régume permanent
	 Delayms (DUREE_ECHELON_2);

	 // On arrête les moteurs
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

	 // On initialise le début du remplissage du buffer
	 bufferWheelIndex3 = 0;

	 // Tous les moteurs débutent à la même vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le régime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(3, DEUXIEME_ECHELON);

	 // On attend le régume permanent
	 Delayms (DUREE_ECHELON_2);

	 // On arrête les moteurs
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

	 // On initialise le début du remplissage du buffer
	 bufferWheelIndex4 = 0;

	 // Tous les moteurs débutent à la même vitesse
	 MotorSetSpeed(1, PREMIER_ECHELON);
	 MotorSetSpeed(2, PREMIER_ECHELON);
	 MotorSetSpeed(3, PREMIER_ECHELON);
	 MotorSetSpeed(4, PREMIER_ECHELON);

	 // On attend le régime permanent
	 Delayms (DUREE_ECHELON_1);
	 // On change la vitesse du moteur 1 seulement
	 MotorSetSpeed(4, DEUXIEME_ECHELON);

	 // On attend le régume permanent
	 Delayms (DUREE_ECHELON_2);
	 */
	// On arrête les moteurs
	MotorSetSpeed(1, 0);
	MotorSetSpeed(2, 0);
	MotorSetSpeed(3, 0);
	MotorSetSpeed(4, 0);

	/*** FIN de l'identification ***/

	// On attends que l'ordinateur demande des données
	TM_DISCO_LedOn (LED_ORANGE);
	while (bSendData == 0) {
		// Intermédiaire nécessaire pour la synchonisation des données
		bSendData = bFlagSendData;
	}
	tunningSendIdenWheels();
}

void tunningSendIdenWheels() {
	/****** ENVOIE DONNÉES MOTEUR 1 ******/
	VCP_DataTx((uint8_t*) bufferWheel1, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNÉES MOTEUR 2 ******/
	VCP_DataTx((uint8_t*) bufferWheel2, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNÉES MOTEUR 3 ******/
	VCP_DataTx((uint8_t*) bufferWheel3, MAX_WHEEL_INDEX * 2);
	Delayms(2000);
	/****** ENVOIE DONNÉES MOTEUR 4 ******/
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

	// Initialise l'état
	bTunningSpeedDone = 0;
	bFlagTunningSpeedDone = 0;
	bFlagStartTunningSpeed = 1;

	// Redémarre les buffers
	bufferSpeedPIDIndex1 = 0;
	bufferSpeedPIDIndex2 = 0;
	bufferSpeedPIDIndex3 = 0;
	bufferSpeedPIDIndex4 = 0;

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

	// On arrête les roues
	MotorSetSpeed(1, 0);
	MotorSetSpeed(2, 0);
	MotorSetSpeed(3, 0);
	MotorSetSpeed(4, 0);

	// On attends que l'ordinateur demande des données
	TM_DISCO_LedOn (LED_ORANGE);
	while (bSendDataSpeed == 0) {
		// Intermédiaire nécessaire pour la synchonisation des données
		bSendDataSpeed = bFlagSendDataSpeed;
	}
	tunningSendSpeedPI();
}

void tunningSendSpeedPI() {
	/****** ENVOIE DONNÉES MOTEUR 1 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID1, MAX_SPEED_PID_INDEX * 2);

	//VCP_DataTx((uint8_t*) bufferSpeedCmd1, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 2 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID2, MAX_SPEED_PID_INDEX * 2);

	//VCP_DataTx((uint8_t*) bufferSpeedCmd2, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 3 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID3, MAX_SPEED_PID_INDEX * 2);

	//VCP_DataTx((uint8_t*) bufferSpeedCmd3, MAX_SPEED_PID_INDEX);
	Delayms(1000);
	/****** ENVOIE DONNÉES MOTEUR 4 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID4, MAX_SPEED_PID_INDEX * 2);
	//VCP_DataTx((uint8_t*) bufferSpeedCmd4, MAX_SPEED_PID_INDEX);
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
	moveDownPrehensor();
	Delayms(2500);
	/*************** Initialisation des PI de vitesse *******************/
	// Initialization of wheel 1 PIDs
	PID_init(&tunningSpeedPI1, PID_SPEED1_KP, PID_SPEED1_KI, 0,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&tunningSpeedPI1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 2 PIDs
	PID_init(&tunningSpeedPI2, PID_SPEED2_KP, PID_SPEED2_KI, 0,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&tunningSpeedPI2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 3 PIDs
	PID_init(&tunningSpeedPI3, PID_SPEED3_KP, PID_SPEED3_KI, 0,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&tunningSpeedPI3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 4 PIDs
	PID_init(&tunningSpeedPI4, PID_SPEED4_KP, PID_SPEED4_KI, 0,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&tunningSpeedPI4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// On défini la direction des roues
	if(CONSIGNE_POSITION_X > 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(3, CLOCK);
	}
	else if(CONSIGNE_POSITION_X < 0) {
		MotorSetDirection(1, CLOCK);
		MotorSetDirection(3, COUNTER_CLOCK);
	}
	else {
		MotorSetDirection(1, BRAKE_G);
		MotorSetDirection(3, BRAKE_G);
	}
	//
	if(CONSIGNE_POSITION_Y > 0) {
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(4, CLOCK);
	}
	else if(CONSIGNE_POSITION_Y < 0) {
		MotorSetDirection(2, CLOCK);
		MotorSetDirection(4, COUNTER_CLOCK);
	}
	else {
		MotorSetDirection(2, BRAKE_G);
		MotorSetDirection(4, BRAKE_G);
	}

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
	// On boucle tant qu'on a pas fini
	while (bTunningPositionDone == 0) {
		float deplacementY = calculatePosition((numberOfPositionEdges2+numberOfPositionEdges4));
		float deplacementX = calculatePosition((numberOfPositionEdges1+numberOfPositionEdges3));
		if ((CONSIGNE_POSITION_Y != 0 && deplacementY < (CONSIGNE_POSITION_Y/2))
				|| (CONSIGNE_POSITION_X != 0 && deplacementX < (CONSIGNE_POSITION_X/2))) {
			tunningSpeedPI1.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI2.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI3.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI4.mySetpoint = CONSIGNE_SPEED_MEDIUM;
		}
		else {
			tunningSpeedPI1.mySetpoint = CONSIGNE_SPEED_LOW;
			tunningSpeedPI2.mySetpoint = CONSIGNE_SPEED_LOW;
			tunningSpeedPI3.mySetpoint = CONSIGNE_SPEED_LOW;
			tunningSpeedPI4.mySetpoint = CONSIGNE_SPEED_LOW;
		}

		if((deplacementX+0.001 >= CONSIGNE_POSITION_X && CONSIGNE_POSITION_X != 0)
				|| (deplacementY+0.001 >= CONSIGNE_POSITION_Y && CONSIGNE_POSITION_Y != 0)) {
			motorStopAll();
		}

		// Apply command for motor 1
		if (PID_Compute_Speed(&tunningSpeedPI1)) {
			FloatType cmdMotor = tunningSpeedPI1.myOutput;
			MotorSetSpeed(1, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI1, PID_Mode_Manual);
		}
		// Apply command for motor 2
		if (PID_Compute_Speed(&tunningSpeedPI2)) {
			FloatType cmdMotor = tunningSpeedPI2.myOutput;
			uint8_t cmdPWM = (uint8_t) cmdMotor;
			MotorSetSpeed(2, (uint8_t) cmdPWM);
			PID_SetMode(&tunningSpeedPI2, PID_Mode_Manual);
		}
		// Apply command for motor 3
		if (PID_Compute_Speed(&tunningSpeedPI3)) {
			FloatType cmdMotor = tunningSpeedPI3.myOutput;
			MotorSetSpeed(3, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI3, PID_Mode_Manual);
		}
		// Apply command for motor 4
		if (PID_Compute_Speed(&tunningSpeedPI4)) {
			FloatType cmdMotor = tunningSpeedPI4.myOutput;
			MotorSetSpeed(4, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI4, PID_Mode_Manual);
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

#ifdef ENABLE_ROTATION
void tunningRotation() {
	/**************** Direction des moteurs *********************/
	moveDownPrehensor();
	Delayms(2500);
	/*************** Initialisation des PI de vitesse *******************/
	// Initialization of wheel 1 PIDs
	PID_init(&tunningSpeedPI1, PID_SPEED1_KP, PID_SPEED1_KI, 0,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&tunningSpeedPI1, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 2 PIDs
	PID_init(&tunningSpeedPI2, PID_SPEED2_KP, PID_SPEED2_KI, 0,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&tunningSpeedPI2, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 3 PIDs
	PID_init(&tunningSpeedPI3, PID_SPEED3_KP, PID_SPEED3_KI, 0,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&tunningSpeedPI3, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// Initialization of wheel 4 PIDs
	PID_init(&tunningSpeedPI4, PID_SPEED4_KP, PID_SPEED4_KI, 0,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&tunningSpeedPI4, MIN_SPEED_COMMAND, MAX_SPEED_COMMAND);

	// On défini la direction des roues
	if(CONSIGNE_ANGLE > 0) {
		MotorSetDirection(1, COUNTER_CLOCK);
		MotorSetDirection(3, CLOCK);
	}
	else if(CONSIGNE_POSITION_X < 0) {
		MotorSetDirection(1, CLOCK);
		MotorSetDirection(3, COUNTER_CLOCK);
	}
	else {
		MotorSetDirection(1, BRAKE_G);
		MotorSetDirection(3, BRAKE_G);
	}
	//
	if(CONSIGNE_POSITION_Y > 0) {
		MotorSetDirection(2, COUNTER_CLOCK);
		MotorSetDirection(4, CLOCK);
	}
	else if(CONSIGNE_POSITION_Y < 0) {
		MotorSetDirection(2, CLOCK);
		MotorSetDirection(4, COUNTER_CLOCK);
	}
	else {
		MotorSetDirection(2, BRAKE_G);
		MotorSetDirection(4, BRAKE_G);
	}

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
	// On boucle tant qu'on a pas fini
	while (bTunningPositionDone == 0) {
		float deplacementY = calculatePosition((numberOfPositionEdges2+numberOfPositionEdges4)/2);
		float deplacementX = calculatePosition((numberOfPositionEdges1+numberOfPositionEdges3)/2);
		if ((CONSIGNE_POSITION_Y != 0 && deplacementY < (CONSIGNE_POSITION_Y/2))
				|| (CONSIGNE_POSITION_X != 0 && deplacementX < (CONSIGNE_POSITION_X/2))) {
			tunningSpeedPI1.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI2.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI3.mySetpoint = CONSIGNE_SPEED_MEDIUM;
			tunningSpeedPI4.mySetpoint = CONSIGNE_SPEED_MEDIUM;
		}
		else {
			tunningSpeedPI1.mySetpoint = 0;
			tunningSpeedPI2.mySetpoint = 0;
			tunningSpeedPI3.mySetpoint = 0;
			tunningSpeedPI4.mySetpoint = 0;
		}

		// Apply command for motor 1
		if (PID_Compute_Speed(&tunningSpeedPI1)) {
			FloatType cmdMotor = tunningSpeedPI1.myOutput;
			MotorSetSpeed(1, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI1, PID_Mode_Manual);
		}
		// Apply command for motor 2
		if (PID_Compute_Speed(&tunningSpeedPI2)) {
			FloatType cmdMotor = tunningSpeedPI2.myOutput;
			uint8_t cmdPWM = (uint8_t) cmdMotor;
			MotorSetSpeed(2, (uint8_t) cmdPWM);
			PID_SetMode(&tunningSpeedPI2, PID_Mode_Manual);
		}
		// Apply command for motor 3
		if (PID_Compute_Speed(&tunningSpeedPI3)) {
			FloatType cmdMotor = tunningSpeedPI3.myOutput;
			MotorSetSpeed(3, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI3, PID_Mode_Manual);
		}
		// Apply command for motor 4
		if (PID_Compute_Speed(&tunningSpeedPI4)) {
			FloatType cmdMotor = tunningSpeedPI4.myOutput;
			MotorSetSpeed(4, (uint8_t) cmdMotor);
			PID_SetMode(&tunningSpeedPI4, PID_Mode_Manual);
		}

		bTunningPositionDone = bFlagTunningPositionDone;
	}
	moveUpPrehensor();
	tunningSendPositionPID();
	while (1) {

	}
}
#endif
