#ifndef DEFINES_H_
#define DEFINES_H_

// Flags / boolean related

#define TRUE 1
#define FALSE 0
#define IRRELEVANT -1


// World related

#define MAXIMIN_NUMBER_OF_OBSTACLE 3

// Units and mesurements related

#define ANGLE_BASE_UNIT 0.00001
#define QUARTER_PI 78540
#define HALF_PI 157080
#define THREE_QUARTER_PI 235620
#define PI 314160
#define MINUS_PI -314160
#define MINUS_THREE_QUARTER_PI -235620
#define MINUS_HALF_PI -157080
#define MINUS_QUARTER_PI -78540



// Behavior related

#define DEFAULT_GOAL_X 0
#define DEFAULT_GOAL_Y 0
#define DEFAULT_GOAL_THETA 0

#define X_TOLERANCE_MIN 0                   // 0 mm
#define X_TOLERANCE_DEFAULT 1500            // 15 cm
#define X_TOLERANCE_MAX 50000               // 5 m

#define Y_TOLERANCE_MIN 0                   // 0 mm
#define Y_TOLERANCE_DEFAULT 1500            // 15 cm
#define Y_TOLERANCE_MAX 50000               // 5 m

#define THETA_TOLERANCE_MIN 0               // 0 rad
#define THETA_TOLERANCE_DEFAULT 31416       // Pi/10 rad
#define THETA_TOLERANCE_MAX 314160          // Pi rad


#endif // DEFINES_H_
