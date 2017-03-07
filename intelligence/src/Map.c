#include <stdlib.h>
#include "Map.h"

struct Map *Map_new()
{
    struct Object *new_object = Object_new();
    struct Coordinates *new_northEasternTableCorner = Coordinates_zero();
    struct Coordinates *new_northWesternTableCorner = Coordinates_zero();
    struct Coordinates *new_southEasternTableCorner = Coordinates_zero();
    struct Coordinates *new_southWesternTableCorner = Coordinates_zero();
    struct Coordinates *new_northEasternDrawingCorner = Coordinates_zero();
    struct Coordinates *new_northWesternDrawingCorner = Coordinates_zero();
    struct Coordinates *new_southEasternDrawingCorner = Coordinates_zero();
    struct Coordinates *new_southWesternDrawingCorner = Coordinates_zero();
    struct Coordinates *new_antennaZoneStart = Coordinates_zero();
    struct Coordinates *new_antennaZoneStop = Coordinates_zero();

    int initialNumberOfObstacles = 0;
    int i;
    struct Obstacle *new_obstacles[3];

    for(i = 0; i < 3; ++i) {
        new_obstacles[i] = Obstacle_new(0, 0, THEORICAL_OBSTACLE_RADIUS, CENTER);
    }

    struct Pose *new_paintingZones[8];

    for(i = 0; i < 8; ++i) {
        new_paintingZones[i] = Pose_zero();
    }

    struct Map *pointer = (struct Map *) malloc(sizeof(struct Map));

    pointer->object = new_object;

    pointer->northEasternTableCorner = new_northEasternTableCorner;

    pointer->northWesternTableCorner = new_northWesternTableCorner;

    pointer->southEasternTableCorner = new_southEasternTableCorner;

    pointer->southWesternTableCorner = new_southWesternTableCorner;

    pointer->northEasternDrawingCorner = new_northEasternDrawingCorner;

    pointer->northWesternDrawingCorner = new_northWesternDrawingCorner;

    pointer->southEasternDrawingCorner = new_southEasternDrawingCorner;

    pointer->southWesternDrawingCorner = new_southWesternDrawingCorner;

    pointer->antennaZoneStart = new_antennaZoneStart;

    pointer->antennaZoneStop = new_antennaZoneStop;

    pointer->numberOfObstacles = initialNumberOfObstacles;

    for(i = 0; i < 3; ++i) {
        pointer->obstacles[i] = new_obstacles[i];
    }

    for(i = 0; i < 8; ++i) {
        pointer->paintingZones[i] = new_paintingZones[i];
    }

    return pointer;
}

void Map_delete(struct Map *map)
{
    Object_removeOneReference(map->object);

    if(Object_canBeDeleted(map->object)) {
        Object_delete(map->object);
        Coordinates_delete(map->northEasternTableCorner);
        Coordinates_delete(map->northWesternTableCorner);
        Coordinates_delete(map->southEasternTableCorner);
        Coordinates_delete(map->southWesternTableCorner);
        Coordinates_delete(map->northEasternDrawingCorner);
        Coordinates_delete(map->northWesternDrawingCorner);
        Coordinates_delete(map->southEasternDrawingCorner);
        Coordinates_delete(map->southWesternDrawingCorner);
        Coordinates_delete(map->antennaZoneStart);
        Coordinates_delete(map->antennaZoneStop);
        int i;

        for(i = 0; i < 3; ++i) {
            Obstacle_delete(map->obstacles[i]);
        }

        for(i = 0; i < 8; ++i) {
            Pose_delete(map->paintingZones[i]);
        }

        free(map);
    }
}
