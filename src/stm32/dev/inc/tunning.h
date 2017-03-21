#ifndef TUNNING_H_
#define TUNNING_H_

#include "tm_stm32f4_disco.h"
#include "com.h"
#include "stm32f4xx.h"
#include "motor.h"
#include "PID_v1.h"

/***************************************************
 * Les buffers qui suivent servent à l'acquisition pour
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
#define ENABLE_SPEED_PI
#ifdef ENABLE_SPEED_PI
#define PID_SPEED
#define MAX_SPEED_PID_INDEX 1400
#define CONSIGNE_SPEED 0.10
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

extern FloatType tunningSpeedKP1;
extern FloatType tunningSpeedKP2;
extern FloatType tunningSpeedKP3;
extern FloatType tunningSpeedKP4;

extern FloatType tunningSpeedKI1;
extern FloatType tunningSpeedKI2;
extern FloatType tunningSpeedKI3;
extern FloatType tunningSpeedKI4;
#endif

/********************************************************
 * Variables pour trouver la zone morte de chacune des roues
 ********************************************************/
//#define ENABLE_DEAD_ZONE
#ifdef ENABLE_DEAD_ZONE
#define DEAD_START_PWM 20
#define DEAD_STOP_PWM 100
#define MAX_DEAD_ZONE 100
extern uint8_t bufferDeadZoneIndex1;
extern uint8_t bufferDeadZoneIndex2;
extern uint8_t bufferDeadZoneIndex3;
extern uint8_t bufferDeadZoneIndex4;
extern uint16_t bufferDeadZone1[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone2[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone3[MAX_DEAD_ZONE];
extern uint16_t bufferDeadZone4[MAX_DEAD_ZONE];
#endif

// Méthode pour effetuer l'identifications des roues à partir de deux échelons de consigne
void tunningIdentificationWheels();

void tunningSendIdenWheels();

// Permet de comparer la consigne avec la réponse d'un système à une consigne
void tunningSpeedPI();

// PErmet d'Envoyer les données relatives à l'analyse de la réponse du PID
void tunningSendSpeedPI();

void getPIFromBuffer(uint8_t *data);

#endif
