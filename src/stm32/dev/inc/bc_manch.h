#ifndef BC_MANCH
#define BC_MANCH

#include "stm32f4xx.h"
#include "defines.h"
#include "manchester.h"
#include "com.h"

// Taille du buffer d'échantillonnage
#define BC_MANCH_BUFFER_SIZE 2000
// Nombre d'échantillon pour 2 chiffres consécutifs
#define BC_MANCH_SEUIL_TOP 33
// Nombre d'échantillon pour un seul chiffre
#define BC_MANCH_SEUIL_BOTTOM 23

enum MANCH_BC_STATE {
	MANCH_BC_ACQUIRE = 0, MANCH_BC_DECODE = 1, MANCH_BC_DONE = 2
};

// Permet d'indiquer si un buffer est prêt à être décodé
extern uint8_t BcManchFlag;
// Index du buffer du manchester
extern uint16_t bufferBcManchIndex;
// Buffer contenant les échantillons du code Manchester
extern uint8_t bufferBcManch[BC_MANCH_BUFFER_SIZE];
// Permet de compter le nombre de fois que le même code à été décodé
extern uint8_t manchChecked;
// Garde en mémoire le dernier code manchester lu
extern char messageToDisplayHold[MESSAGE_TO_DISPLAY_LENGTH];

// Permet d'initialiser le timer qui sert à échantillonner le signal Manchester
void initBCManch();

// Permet d'obtenir le code Manchester en 01 10 à partir du buffer d'échantillons
void obtenirCodeManch(uint8_t* bufferCodeManch);

void decodeManchBC();

#endif
