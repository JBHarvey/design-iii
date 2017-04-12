#ifndef DEFINES_H_
#define DEFINES_H_

#include <float.h>

// Flags / boolean related

#ifndef TRUE
#define TRUE 1
#endif // TRUE (Avoid Gtk TRUE redefinition)

#ifndef FALSE
#define FALSE 0
#endif // FALSE (Avoid Gtk FALSE redefinition)

#define IRRELEVANT -1

// World related

#define MAXIMUM_OBSTACLE_NUMBER 3
#define NUMBER_OF_PAINTING 8
#define THEORICAL_OBSTACLE_RADIUS 650
#define THEORICAL_ROBOT_RADIUS 1700
#define OVERLAPPING_DISTANCE 2800
#define THEORICAL_WORLD_LENGTH 23300
#define THEORICAL_WORLD_HEIGHT 11300
#define THEORICAL_DRAWING_ZONE_SIDE 6600
#define THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X 14350
#define THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y 2350
#define THEORICAL_ANTENNA_ZONE_LENGTH 4600
#define THEORICAL_ANTENNA_ZONE_START_X 17200
#define THEORICAL_ANTENNA_ZONE_STOP_X (THEORICAL_ANTENNA_ZONE_START_X - THEORICAL_ANTENNA_ZONE_LENGTH)
#define THEORICAL_ANTENNA_ZONE_Y (THEORICAL_WORLD_HEIGHT - THEORICAL_ROBOT_RADIUS / 2)
#define ONE_CENTIMETER_GAP 100
#define MINIMAL_GAP 50

// Graph and pathfinding related
#define MAX_NUMBER_OF_NODE 15
#define MAX_VALUE_OF_FLOAT FLT_MAX

// Navigation related
#define SPEEDS_BASE_UNIT 0.0001
#define MEDIUM_SPEED_VALUE 2000

#define LOW_SPEED_VALUE 300
#define LOW_SPEED_DISTANCE 800

#define STOP_VALUE 0
#define STOP_DISTANCE 75

#define ANTENNA_MARK_DISTANCE 85

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
#define MINUS_FIFTH_PI -62832
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
#define X_TOLERANCE_DRAWING 50             // 5 mm
#define X_TOLERANCE_MOVING 100             // 10 mm
#define X_TOLERANCE_MAX 50000               // 5 m

#define Y_TOLERANCE_MIN 0                   // 0 mm
#define Y_TOLERANCE_DRAWING 50             // 5 mm
#define Y_TOLERANCE_MOVING 100             // 10 mm
#define Y_TOLERANCE_MAX 50000               // 5 m

#define THETA_TOLERANCE_MIN 0               // 0 rad
#define THETA_TOLERANCE_DRAWING 5000 
#define THETA_TOLERANCE_MOVING 10000
#define THETA_TOLERANCE_ORIENTATION 3000
#define THETA_TOLERANCE_MAX 314160         // Pi rad

// Navigation related
#define OMEGA_MEDIUM_SPEED_DRAWING 6000
#define OMEGA_LOW_SPEED_DRAWING 3000
#define OMEGA_MEDIUM_SPEED_MOVING 11000
#define OMEGA_LOW_SPEED_MOVING 5500

#define MEDIUM_DISTANCE_DRAWING 1000
#define MEDIUM_DISTANCE_MOVING 2000
#define MAX_SPEED_DRAWING 1000
#define MAX_SPEED_MOVING 1600
#define SHORT_DISTANCE_DRAWING 100
#define ACCELERATION_FACTOR_DRAWING 1.2
#define SHORT_DISTANCE_MOVING 400
#define ACCELERATION_FACTOR_MOVING 2.0

// Trajectory related
#define MAX_SEGMENT_LENGHT_DRAWING 3500
#define MAX_SEGMENT_LENGHT_OBSTACLE 10000

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
#define FOUR_SECONDS 4000

#endif // DEFINES_H_
