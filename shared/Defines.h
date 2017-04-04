#ifndef DEFINES_H_
#define DEFINES_H_

#include <float.h>

// Flags / boolean related

#define TRUE 1
#define FALSE 0
#define IRRELEVANT -1


// World related

#define MAXIMUM_OBSTACLE_NUMBER 3
#define THEORICAL_OBSTACLE_RADIUS 625
#define THEORICAL_ROBOT_RADIUS 1700
#define THEORICAL_WORLD_LENGTH 23300
#define THEORICAL_WORLD_HEIGHT 11300
#define THEORICAL_DRAWING_ZONE_SIDE 6600
#define THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X 14350
#define THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y 2350
#define THEORICAL_ANTENNA_ZONE_LENGTH 7200
#define THEORICAL_ANTENNA_ZONE_START_X 19100
#define THEORICAL_ANTENNA_ZONE_STOP_X (THEORICAL_ANTENNA_ZONE_START_X - THEORICAL_ANTENNA_ZONE_LENGTH)
#define THEORICAL_ANTENNA_ZONE_Y (THEORICAL_WORLD_HEIGHT - THEORICAL_ROBOT_RADIUS)
#define ONE_CENTIMETER_GAP 100
#define MINIMAL_GAP 50

// Graph and pathfinding related
#define MAX_NUMBER_OF_NODE 22
#define MAX_VALUE_OF_FLOAT FLT_MAX

// Navigation related
#define SPEEDS_BASE_UNIT 0.001
#define MEDIUM_SPEED_VALUE 80

#define LOW_SPEED_VALUE 20
#define LOW_SPEED_DISTANCE 750

#define STOP_VALUE 0
#define STOP_DISTANCE 80

#define ANTENNA_MARK_DISTANCE 750

// Units and mesurements related

#define ANGLE_BASE_UNIT 0.00001
#define TWENTIETH_PI 15708
#define FIFTH_PI 62832
#define QUARTER_PI 78540
#define HALF_PI 157080
#define THREE_QUARTER_PI 235620
#define PI 314160
#define MINUS_PI -314160
#define MINUS_THREE_QUARTER_PI -235620
#define MINUS_HALF_PI -157080
#define MINUS_FIFTH_PI 62832
#define MINUS_QUARTER_PI -78540
#define MINUS_TWENTIETH_PI -15708

// Manchester related
#define TIMES_TWO 2
#define TIMES_FOUR 4
#define SIGNAL_FORCE_ROUNDING_FACTOR 100


// Behavior related

#define DEFAULT_GOAL_X 0
#define DEFAULT_GOAL_Y 0
#define DEFAULT_GOAL_THETA 0

#define X_TOLERANCE_MIN 0                   // 0 mm
#define X_TOLERANCE_DEFAULT 100             // 4 mm
#define X_TOLERANCE_MAX 50000               // 5 m

#define Y_TOLERANCE_MIN 0                   // 0 mm
#define Y_TOLERANCE_DEFAULT 100             // 4 mm
#define Y_TOLERANCE_MAX 50000               // 5 m

#define THETA_TOLERANCE_MIN 0               // 0 rad
#define THETA_TOLERANCE_DEFAULT 15708       // Pi/20 rad
#define THETA_TOLERANCE_MAX 314160          // Pi rad


// Time related
#define NUMBER_OF_NANOSECONDS_IN_A_SECOND 1.0e9
#define NUMBER_OF_NANOSECONDS_IN_A_MILLISECOND 1.0e6
#define ONE_NANOSECOND 1.0e6
#define ONE_MILLISECOND 1
#define ONE_HUNDREDTH_OF_A_SECOND 10
#define ONE_TENTH_OF_A_SECOND 100
#define ONE_SECOND 1000
#define ONE_SECOND_AND_AN_HALF 1500
#define TWO_SECONDS 2000
#define THREE_SECONDS 3000

#endif // DEFINES_H_
