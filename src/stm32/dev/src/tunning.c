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

FloatType tunningSpeedKP1 = 0;
FloatType tunningSpeedKP2 = 0;
FloatType tunningSpeedKP3 = 0;
FloatType tunningSpeedKP4 = 0;

FloatType tunningSpeedKI1 = 0;
FloatType tunningSpeedKI2 = 0;
FloatType tunningSpeedKI3 = 0;
FloatType tunningSpeedKI4 = 0;

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
uint8_t bufferDeadZoneIndex1 = 0;
uint8_t bufferDeadZoneIndex2 = 0;
uint8_t bufferDeadZoneIndex3 = 0;
uint8_t bufferDeadZoneIndex4 = 0;
uint16_t bufferDeadZone1[MAX_DEAD_ZONE];
uint16_t bufferDeadZone2[MAX_DEAD_ZONE];
uint16_t bufferDeadZone3[MAX_DEAD_ZONE];
uint16_t bufferDeadZone4[MAX_DEAD_ZONE];
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
	PID_init(&tunningPI1, tunningSpeedKP1, tunningSpeedKI1, 0,
			PID_Direction_Direct, PID_SPEED1_N);
	PID_SetOutputLimits(&tunningPI1, 25, MAX_SPEED_COMMAND);
	tunningPI1.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 2 PIDs
	PID_init(&tunningPI2, tunningSpeedKP2, tunningSpeedKI2, 0,
			PID_Direction_Direct, PID_SPEED2_N);
	PID_SetOutputLimits(&tunningPI2, 25, MAX_SPEED_COMMAND);
	tunningPI2.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 3 PIDs
	PID_init(&tunningPI3, tunningSpeedKP3, tunningSpeedKI3, 0,
			PID_Direction_Direct, PID_SPEED3_N);
	PID_SetOutputLimits(&tunningPI3, 25, MAX_SPEED_COMMAND);
	tunningPI3.mySetpoint = CONSIGNE_SPEED;

	// Initialization of wheel 4 PIDs
	PID_init(&tunningPI4, tunningSpeedKP4, tunningSpeedKI4, 0,
			PID_Direction_Direct, PID_SPEED4_N);
	PID_SetOutputLimits(&tunningPI4, 25, MAX_SPEED_COMMAND);
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

	// Redémarre les buffers
	bufferSpeedPIDIndex1 = 0;
	bufferSpeedPIDIndex2 = 0;
	bufferSpeedPIDIndex3 = 0;
	bufferSpeedPIDIndex4 = 0;

	// Initialise l'état
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
	VCP_DataTx((uint8_t*) bufferSpeedCmd1, MAX_SPEED_PID_INDEX);
	/****** ENVOIE DONNÉES MOTEUR 2 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID2, MAX_SPEED_PID_INDEX * 2);
	VCP_DataTx((uint8_t*) bufferSpeedCmd2, MAX_SPEED_PID_INDEX);
	/****** ENVOIE DONNÉES MOTEUR 3 ******/
	VCP_DataTx((uint8_t*) bufferSpeedPID3, MAX_SPEED_PID_INDEX * 2);
	VCP_DataTx((uint8_t*) bufferSpeedCmd3, MAX_SPEED_PID_INDEX);
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

void getPIFromBuffer(uint8_t *data) {
	tunningSpeedKP1 = *(float *) data;
	tunningSpeedKP2 = *(float *) (data + 4);
	tunningSpeedKP3 = *(float *) (data + 8);
	tunningSpeedKP4 = *(float *) (data + 12);

	tunningSpeedKI1 = *(float *) (data + 16);
	tunningSpeedKI2 = *(float *) (data + 20);
	tunningSpeedKI3 = *(float *) (data + 24);
	tunningSpeedKI4 = *(float *) (data + 28);
}

#endif
