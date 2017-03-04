#include <stdlib.h>
#include "Map.h"

struct Map *Map_new()
{
    struct Object *new_object = Object_new();
    struct Pose *new_northEasternCorner = Pose_new(0, 0, 0);
    struct Pose *new_northWesternCorner = Pose_new(0, 0, 0);
    struct Pose *new_southEasternCorner = Pose_new(0, 0, 0);
    struct Pose *new_southWesternCorner = Pose_new(0, 0, 0);

    struct Map *pointer = (struct Map *) malloc(sizeof(struct Map));

    pointer->object = new_object;
    pointer->northEasternCorner = new_northEasternCorner;
    pointer->northWesternCorner = new_northWesternCorner;
    pointer->southEasternCorner = new_southEasternCorner;
    pointer->southWesternCorner = new_southWesternCorner;

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
        free(map);
    }
}
