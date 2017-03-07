#ifndef COMMUNICATIONSTRUCTURES_H_
#define COMMUNICATIONSTRUCTURES_H_

struct __attribute__((__packed__)) Communication_Coordinates 
{
    int x;
    int y;
};

struct __attribute__((__packed__)) Communication_Pose 
{
    struct Communication_Coordinates coordinates;
    int theta;
};

struct __attribute__((__packed__)) Communication_Zone 
{
    struct Communication_Pose pose;
    int index;
};

struct __attribute__((__packed__)) Communication_Object 
{
    struct Communication_Zone zone;
    int radius;
};

struct __attribute__((__packed__)) Communication_Environment 
{
    struct Communication_Coordinates northEasternTableCorner;
    struct Communication_Coordinates northWesternTableCorner;
    struct Communication_Coordinates southEasternTableCorner;
    struct Communication_Coordinates southWesternTableCorner;
    struct Communication_Coordinates northEasternDrawingCorner;
    struct Communication_Coordinates northWesternDrawingCorner;
    struct Communication_Coordinates southEasternDrawingCorner;
    struct Communication_Coordinates southWesternDrawingCorner;
    struct Communication_Coordinates antennaZoneStart;
    struct Communication_Coordinates antennaZoneStop;
    struct Communication_Object obstacles[3];
    struct Communication_Zone paintingZone[8];
};

struct __attribute__((__packed__)) Communication_World
{
    struct Communication_Environment environment;
    int environmentHasChanged;
    struct Communication_Object robot;
};

#endif // COMMUNICATIONSTRUCTURES_H_
