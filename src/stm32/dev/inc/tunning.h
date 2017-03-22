#ifndef TUNNING_H_
#define TUNNING_H_

#include "tm_stm32f4_disco.h"
#include "com.h"
#include "stm32f4xx.h"
#include "motor.h"
#include "PID_v1.h"
#include <math.h>
#include "prehenseur.h"

/***************************************************
 * Les buffers qui suivent servent ï¿½ l'acquisition pour
 * bien faire l'asservissement des roues
 ***************************************************/
//#define ENABLE_ACQUIS
#ifdef ENABLE_ACQUIS
#define MAX_WHEEL_INDEX 1000
#define PREMIER_ECHELON 25
#define DEUXIEME_ECHELON 45
#define DUREE_ECHELON_1 5500
#define DUREE_ECHELON_2 4000
extern uint16_t bufferWheelIndex1;
extern uint16_t bufferWheelIndex2;
extern uint16_t bufferWheelIndex3;
extern uint16_t bufferWheelIndex4;
extern uint16_t bufferWheel1[MAX_WHEEL_INDEX];
extern uint16_t bufferWheel2[MAX_WHEEL_INDEX];
extern uint16_t bufferWheel3[MAX_WHEEL_INDEX];
extern uint16_t bufferWheel4[MAX_WHEEL_INDEX];
extern uint8_t bSendData;
#endif

/******************************************************
 * Variables pour tester le PID de vitesse
 * ****************************************************/
//#define ENABLE_SPEED_PI
#ifdef ENABLE_SPEED_PI
#define PID_SPEED
#define MAX_SPEED_PID_INDEX 1400
#define CONSIGNE_SPEED 0.08
extern uint16_t bufferSpeedPIDIndex1;
extern uint16_t bufferSpeedPIDIndex2;
extern uint16_t bufferSpeedPIDIndex3;
extern uint16_t bufferSpeedPIDIndex4;
extern uint16_t bufferSpeedPID1[MAX_SPEED_PID_INDEX];
extern uint16_t bufferSpeedPID2[MAX_SPEED_PID_INDEX];
extern uint16_t bufferSpeedPID3[MAX_SPEED_PID_INDEX];
extern uint16_t bufferSpeedPID4[MAX_SPEED_PID_INDEX];

extern PidType tunningPI1;
extern PidType tunningPI2;
extern PidType tunningPI3;
extern PidType tunningPI4;

extern uint8_t bSendDataSpeed;
extern uint8_t bFlagSendDataSpeed;
extern uint8_t bTunningSpeedDone;
extern uint8_t bFlagTunningSpeedDone;
extern uint8_t bFlagStartTunningSpeed;

#endif

/********************************************************
 * Variables pour trouver la zone morte de chacune des roues
 ********************************************************/
#define ENABLE_DEAD_ZONE
#ifdef ENABLE_DEAD_ZONE
#define DEAD_START_PWM 0
#define DEAD_STOP_PWM 35
#define MAX_DEAD_ZONE 100
extern uint8_t bufferDeadZoneIndex;
extern uint8_t bufferDeadZoneCmd[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone1[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone2[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone3[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone4[MAX_DEAD_ZONE];

// Flag levé par la com
extern uint8_t bFlagTunningDeadZone;

// Détermine le buffer qui doit être rempli
extern uint8_t bufferDeadZoneToFill;

#endif

/*********************************************************
 * Variables pour tester le PID de position
 *********************************************************/
#define ENABLE_POSITION_PID
#ifdef ENABLE_POSITION_PID

#define MAX_POSITION_INDEX 1400
#define CONSIGNE_POSITION_X 0.00
#define CONSIGNE_POSITION_Y -0.30

extern uint16_t bufferPositionPIDIndex1;
extern float bufferPositionPID1[MAX_POSITION_INDEX];

extern uint16_t bufferSpeedPI1[MAX_POSITION_INDEX];
extern uint16_t bufferSpeedPI2[MAX_POSITION_INDEX];
extern uint16_t bufferSpeedPI3[MAX_POSITION_INDEX];
extern uint16_t bufferSpeedPI4[MAX_POSITION_INDEX];

extern uint16_t bufferSpeedPIIndex1;
extern uint16_t bufferSpeedPIIndex2;
extern uint16_t bufferSpeedPIIndex3;
extern uint16_t bufferSpeedPIIndex4;

extern PidType tunningSpeedPI1;
extern PidType tunningSpeedPI2;
extern PidType tunningSpeedPI3;
extern PidType tunningSpeedPI4;

extern PidType tunningPositionPID1;
extern PidType tunningPositionPID2;
extern PidType tunningPositionPID3;
extern PidType tunningPositionPID4;

extern uint8_t bSendDataPosition;
extern uint8_t bFlagSendDataPosition;
extern uint8_t bTunningPositionDone;
extern uint8_t bFlagTunningPositionDone;
extern uint8_t bFlagStartTunningPosition;

extern uint8_t wheelsStartedX1;
extern uint8_t wheelsStartedX2;
extern uint8_t wheelsStartedY1;
extern uint8_t wheelsStartedY2;

#endif

/******************** IDENTIFICATION ****************************/
// Méthode pour effetuer l'identifications des roues à partir de deux échelons de consigne
void tunningSendIdenWheels();

/******************** PI VITESSE ********************************/
// Permet de comparer la consigne avec la réponse d'un système à une consigne
void tunningSpeedPI();
// PErmet d'Envoyer les données relatives à l'analyse de la réponse du PID
void tunningSendSpeedPI();

/******************** PID POSITION ******************************/
#ifdef ENABLE_POSITION_PID
// Permet de tester le PID de position
void tunningPositionPID();
// Permet d'envoyer les données du test de PID de position
void tunningSendPositionPID();
#endif

/******************** DEAD ZONE **********************************/
#ifdef ENABLE_DEAD_ZONE
// Permet d'identifier la zone mort de chacune des roues
void tunningDeadZone();
void tunningSendDeadZone();
#endif

#endif
