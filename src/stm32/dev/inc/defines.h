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

#define MOTEUR1_DIR1_GPIO GPIOD
#define MOTEUR1_DIR1_PIN 1
#define MOTEUR1_DIR2_GPIO GPIOD
#define MOTEUR1_DIR2_PIN 2
#define MOTEUR1_TIM TIM3

// Moteur 2
#define MOTEUR2_DIR1_GPIO GPIOD
#define MOTEUR2_DIR1_PIN 1
#define MOTEUR2_DIR2_GPIO GPIOD
#define MOTEUR2_DIR2_PIN 2
#define MOTEUR2_PWM_GPIO GPIOD
#define MOTEUR2_PWM_PIN 3
#define MOTEUR2_TIM TIM3

// Moteur 3
#define MOTEUR3_DIR1_GPIO GPIOD
#define MOTEUR3_DIR1_PIN 1
#define MOTEUR3_DIR2_GPIO GPIOD
#define MOTEUR3_DIR2_PIN 2
#define MOTEUR3_PWM_GPIO GPIOD
#define MOTEUR3_PWM_PIN 3
#define MOTEUR3_TIM TIM3

// Moteur 4
#define MOTEUR4_DIR1_GPIO GPIOD
#define MOTEUR4_DIR1_PIN 1
#define MOTEUR4_DIR2_GPIO GPIOD
#define MOTEUR4_DIR2_PIN 2
#define MOTEUR4_PWM_GPIO GPIOD
#define MOTEUR4_PWM_PIN 3
#define MOTEUR4_TIM TIM3

#ifndef TM_DEFINES_H
#define TM_DEFINES_H

/* Put your global defines for all libraries here used in your project */

#endif
