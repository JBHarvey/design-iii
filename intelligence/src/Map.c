#include <stdlib.h>
#include "Map.h"

struct Map *Map_new()
{
    struct Object *new_object = Object_new();
    struct Pose *new_northEasternCorner = Pose_zero();
    struct Pose *new_northWesternCorner = Pose_zero();
    struct Pose *new_southEasternCorner = Pose_zero();
    struct Pose *new_southWesternCorner = Pose_zero();
    int initialNumberOfObstacles = 0;
    int i;
    struct Pose *new_antennaZoneStart = Pose_zero();
    struct Pose *new_antennaZoneStop = Pose_zero();

    //    struct Obstacles *new_obstacles[3] = {Obstacles_new};
    for(i = 0; i < 3; ++i) {
    }

    struct Pose *new_paintingZones[8];

    for(i = 0; i < 8; ++i) {
        new_paintingZones[i] = Pose_zero();
    }

    struct Map *pointer = (struct Map *) malloc(sizeof(struct Map));

    pointer->object = new_object;

    pointer->northEasternCorner = new_northEasternCorner;

    pointer->northWesternCorner = new_northWesternCorner;

    pointer->southEasternCorner = new_southEasternCorner;

    pointer->southWesternCorner = new_southWesternCorner;

    pointer->numberOfObstacles = initialNumberOfObstacles;

    pointer->antennaZoneStart = new_antennaZoneStart;

    pointer->antennaZoneStop = new_antennaZoneStop;

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
        Pose_delete(map->northEasternCorner);
        Pose_delete(map->northWesternCorner);
        Pose_delete(map->southEasternCorner);
        Pose_delete(map->southWesternCorner);
        Pose_delete(map->antennaZoneStart);
        Pose_delete(map->antennaZoneStop);
        int i;

        for(i = 0; i < 3; ++i) {
        }

        for(i = 0; i < 8; ++i) {
            Pose_delete(map->paintingZones[i]);
        }

        free(map);
    }
}
