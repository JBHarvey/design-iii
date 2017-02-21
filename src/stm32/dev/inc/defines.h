/* Pin pour les moteurs
 * DIR1 : Première pin pour la direction
 * DIR2 : Deuxième pin pour la direction
 * PWM : Pin qui génère le PWM
 * */

// Configuration PWM pour tous les moteurs
// Fréquence = (84 000 000 / (PWM_PRESCALER + 1)) / PWM_PERIOD
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
#define MOTOR1_DIR1_PIN 	GPIO_Pin_7
#define MOTOR1_DIR2_PIN 	GPIO_Pin_8

// Moteur 2
#define MOTOR2_DIR_GPIO 	GPIOE
#define MOTOR2_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR2_DIR1_PIN 	GPIO_Pin_9
#define MOTOR2_DIR2_PIN 	GPIO_Pin_10

// Moteur 3
#define MOTOR3_DIR_GPIO 	GPIOE
#define MOTOR3_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR3_DIR1_PIN 	GPIO_Pin_11
#define MOTOR3_DIR2_PIN 	GPIO_Pin_12

// Moteur 4
#define MOTOR4_DIR_GPIO 	GPIOE
#define MOTOR4_DIR_RCC		RCC_AHB1Periph_GPIOE
#define MOTOR4_DIR1_PIN 	GPIO_Pin_13
#define MOTOR4_DIR2_PIN 	GPIO_Pin_14

/* Les messages */
#define MSG_IDLE "IDLE"
#define MSG_MANCH "Manchester"
#define MSG_ERR "Etat invalide"

#ifndef TM_DEFINES_H
#define TM_DEFINES_H

#endif
