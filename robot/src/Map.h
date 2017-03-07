#ifndef MAP_H_
#define MAP_H_

#include "Obstacle.h"
#include "State.h"

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
    struct Obstacle *obstacles[3];
    struct Pose *paintingZones[8];
};

struct Map *Map_new(void);
void Map_delete(struct Map *map);

void Map_updateTableCorners(struct Map *map, struct Coordinates *northEastern, 
        struct Coordinates *southEastern, struct Coordinates *southWestern, 
        struct Coordinates *northWestern);
void Map_updateDrawingCorners(struct Map *map, struct Coordinates *northEastern, 
        struct Coordinates *southEastern, struct Coordinates *southWestern, 
        struct Coordinates *northWestern);
void Map_updateAntennaZone(struct Map *map, struct Coordinates *start, struct Coordinates *stop);
void Map_updateObstacle(struct Map *map, struct Coordinates *newCoordinates, enum CardinalDirection newOrientation, int index);
void Map_updatePaintingZone(struct Map *map, struct Pose *newPose, int index);
#endif // MAP_H_
