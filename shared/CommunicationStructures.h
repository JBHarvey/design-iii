#ifndef COMMUNICATIONSTRUCTURES_H_
#define COMMUNICATIONSTRUCTURES_H_

struct __attribute__((__packed__)) Communication_Coordinates {
    int x;
    int y;
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

#endif // COMMUNICATIONSTRUCTURES_H_
