#ifndef BC_MANCH
#define BC_MANCH

#include "stm32f4xx.h"
#include "defines.h"
#include "manchester.h"
#include "com.h"

// Taille du buffer d'�chantillonnage
#define BC_MANCH_BUFFER_SIZE 2000
// Nombre d'�chantillon pour 2 chiffres cons�cutifs
#define BC_MANCH_SEUIL_TOP 33
// Nombre d'�chantillon pour un seul chiffre
#define BC_MANCH_SEUIL_BOTTOM 23

enum MANCH_BC_STATE {
	MANCH_BC_ACQUIRE = 0, MANCH_BC_DECODE = 1, MANCH_BC_DONE = 2
};

// Permet d'indiquer si un buffer est pr�t � �tre d�cod�
extern uint8_t BcManchFlag;
// Index du buffer du manchester
extern uint16_t bufferBcManchIndex;
// Buffer contenant les �chantillons du code Manchester
extern uint8_t bufferBcManch[BC_MANCH_BUFFER_SIZE];
// Permet de compter le nombre de fois que le m�me code � �t� d�cod�
extern uint8_t manchChecked;
// Garde en m�moire le dernier code manchester lu
extern char messageToDisplayHold[MESSAGE_TO_DISPLAY_LENGTH];

// Permet d'initialiser le timer qui sert � �chantillonner le signal Manchester
void initBCManch();

// Permet d'obtenir le code Manchester en 01 10 � partir du buffer d'�chantillons
void obtenirCodeManch(uint8_t* bufferCodeManch);

void decodeManchBC();

#endif
