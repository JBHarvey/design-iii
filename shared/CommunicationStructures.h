#ifndef COMMUNICATIONSTRUCTURES_H_
#define COMMUNICATIONSTRUCTURES_H_

enum {
    INVALID_PACKET,
    COMMAND_START_CYCLE,
    COMMAND_STOP_EXECUTION,
    DATA_MANCHESTER_CODE,
    DATA_WORLD,
    DATA_ESTIMATED_ROBOT_POSITION,
    DATA_IMAGE,
    DATA_PLANNED_TRAJECTORY,
    SIGNAL_READY_TO_START,
    SIGNAL_READY_TO_DRAW,
    SIGNAL_END_OF_CYCLE,
    ACK_IMAGE_RECEIVED,
    ACK_PLANNED_TRAJECTORY_RECEIVED,
    ACK_READY_TO_START_RECEIVED,
    ACK_READY_TO_DRAW_RECEIVED,
    ACK_END_OF_CYCLE_RECEIVED,
    DEBUG_TTY_ACM_SEND,
    DEBUG_CAMERA_GET_FIGURE,
    DEBUG_SEND_MANCHESTER
};

void sendStartPacket();

struct __attribute__((__packed__)) Communication_Coordinates {
    int x;
    int y;
};

struct __attribute__((__packed__)) Communication_Path {
    int number_of_points;
    struct Communication_Coordinates path[];
};

struct __attribute__((__packed__)) Communication_Translation {
    struct Communication_Coordinates movement;
    struct Communication_Coordinates speeds;
};

struct __attribute__((__packed__)) Communication_Rotation {
    int theta;
    int gamma;
};

struct __attribute__((__packed__)) Communication_ManchesterSignal {
    int intensity;
};

struct __attribute__((__packed__)) Communication_ManchesterCode {
    int painting_number;
    int scale_factor;
    char orientation; // Possibilities 'N' 'E' 'S' 'W'
};

struct __attribute__((__packed__)) Communication_Pose {
    struct Communication_Coordinates coordinates;
    int theta;
};

struct __attribute__((__packed__)) Communication_Zone {
    struct Communication_Pose pose;
    int index;
};

struct __attribute__((__packed__)) Communication_Object {
    struct Communication_Zone zone;
    int radius;
};

struct __attribute__((__packed__)) Communication_Environment {
    struct Communication_Coordinates north_eastern_table_corner;
    struct Communication_Coordinates north_western_table_corner;
    struct Communication_Coordinates south_eastern_table_corner;
    struct Communication_Coordinates south_western_table_corner;
    struct Communication_Coordinates north_eastern_drawing_corner;
    struct Communication_Coordinates north_western_drawing_corner;
    struct Communication_Coordinates south_eastern_drawing_corner;
    struct Communication_Coordinates south_western_drawing_corner;
    struct Communication_Coordinates antenna_zone_start;
    struct Communication_Coordinates antenna_zone_stop;
    struct Communication_Object obstacles[3];
    struct Communication_Zone painting_zone[8];
};

struct __attribute__((__packed__)) Communication_World {
    struct Communication_Environment environment;
    int environment_has_changed;
    struct Communication_Object robot;
};

struct __attribute__((__packed__)) Communication_Flags {
    //TODO Put robot's state in current cycle
};

struct Command_Translate {
    int x;
    int y;
};

struct Command_Speeds {
    int x;
    int y;
};

struct Command_Rotate {
    int theta;
};

#endif // COMMUNICATIONSTRUCTURES_H_
