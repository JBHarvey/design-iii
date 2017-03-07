#ifndef MAP_H_
#define MAP_H_

#include "Obstacle.h"
#include "Pose.h"

struct MapInitalValues; 

struct Map {
    struct Object *object;
    struct Coordinates *northEasternTableCorner;
    struct Coordinates *northWesternTableCorner;
    struct Coordinates *southEasternTableCorner;
    struct Coordinates *southWesternTableCorner;
    struct Coordinates *northEasternDrawingCorner;
    struct Coordinates *northWesternDrawingCorner;
    struct Coordinates *southEasternDrawingCorner;
    struct Coordinates *southWesternDrawingCorner;
    struct Coordinates *antennaZoneStart;
    struct Coordinates *antennaZoneStop;
    int numberOfObstacles;
    struct Obstacle *obstacles[3];
    struct Pose *paintingZones[8];
};

struct Map *Map_new(void);
void Map_delete(struct Map *map);

#endif // MAP_H_
