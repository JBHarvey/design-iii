#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/* Pin pour les moteurs
 * DIR1 : Premi�re pin pour la direction
 * DIR2 : Deuxi�me pin pour la direction
 * PWM : Pin qui g�n�re le PWM
 * */

// Configuration PWM pour tous les moteurs
// Fr�quence = (84 000 000 / (PWM_PRESCALER + 1)) / PWM_PERIOD
#define PWM_PRESCALER 3 // 0 = 1
#define PWM_PERIOD 52500

// Moteur 1
#define MOTEUR1_PWM_RCC RCC_AHB1Periph_GPIOB
#define MOTEUR1_PWM_GPIO GPIOB
#define MOTEUR1_PWM_PIN GPIO_PinSource6
#define MOTEUR1_PWM_AF GPIO_AF_TIM4
#define MOTEUR1_PWM

#define MOTOR1_DIR_GPIO 	GPIOE
#define MOTOR1_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR1_DIR1_PIN 	GPIO_Pin_8
#define MOTOR1_DIR2_PIN 	GPIO_Pin_7

// Moteur 2
#define MOTOR2_DIR_GPIO 	GPIOE
#define MOTOR2_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR2_DIR1_PIN 	GPIO_Pin_10
#define MOTOR2_DIR2_PIN 	GPIO_Pin_9

// Moteur 3
#define MOTOR3_DIR_GPIO 	GPIOE
#define MOTOR3_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR3_DIR1_PIN 	GPIO_Pin_12
#define MOTOR3_DIR2_PIN 	GPIO_Pin_11

// Moteur 4
#define MOTOR4_DIR_GPIO 	GPIOE
#define MOTOR4_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR4_DIR1_PIN 	GPIO_Pin_14
#define MOTOR4_DIR2_PIN 	GPIO_Pin_13

/* Les messages */
#define MSG_IDLE "IDLE"
#define MSG_MANCH "Manchester"
#define MSG_ERR "Etat invalide"

/* PID1 */
// Parametres en vitesse
#define PID_SPEED1_KP 4.87
#define PID_SPEED1_KI 107.6
#define PID_SPEED1_KD 0.0
#define PID_SPEED1_N 0.0

// Parametres en position
#define PID_POSITION1_KP  800.33
#define PID_POSITION1_KI 0.001
#define PID_POSITION1_KD 100.48
#define PID_POSITION1_N 170.0

/* PID2 */
// Parametres en vitesse
#define PID_SPEED2_KP 4.87
#define PID_SPEED2_KI 107.6
#define PID_SPEED2_KD 0.0
#define PID_SPEED2_N 0.0

// Parametres en position
#define PID_POSITION2_KP  800.33
#define PID_POSITION2_KI 0.001
#define PID_POSITION2_KD 100.48
#define PID_POSITION2_N 170.0

/* PID3 */
// Parametres en vitesse
#define PID_SPEED3_KP 4.87
#define PID_SPEED3_KI 107.6
#define PID_SPEED3_KD 0.0
#define PID_SPEED3_N 0.0

// Parametres en position
#define PID_POSITION3_KP  800.33
#define PID_POSITION3_KI 0.001
#define PID_POSITION3_KD 100.48
#define PID_POSITION3_N 170.0

/* PID4 */
// Parametres en vitesse
#define PID_SPEED4_KP 4.87
#define PID_SPEED4_KI 107.6
#define PID_SPEED4_KD 0.0
#define PID_SPEED4_N 0.0

// Parametres en position
#define PID_POSITION4_KP  800.33
#define PID_POSITION4_KI 0.001
#define PID_POSITION4_KD 100.48
#define PID_POSITION4_N 170.0

#define MAX_DISPLAY_CHARACTERS 8
#define PI 3.14159
#define SPEED_CALC_PERIOD 10000
#define SPEED_CALC_TIME_DELAY 0.01
#define DOUBLE_CHANNEL_TICKS 6400
#define METERS_PER_TICK 0.00003387
#define MAX_SPEED_COMMAND 60
#define MIN_SPEED_COMMAND -60
#define MIN_POS_COMMAND -1
#define MAX_POS_COMMAND 1

#define Y_MOVE_DATA_BUFFER_INDEX 4

// Pour utiliser la librairie sans que sa plante
#define ARM_CORTEX_M3
#define __FPU_PRESENT 1

#endif
