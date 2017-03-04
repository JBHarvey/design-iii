#ifndef MAP_H_
#define MAP_H_

#include "Pose.h"

struct MapInitalValues; 

struct Map {
    struct Object *object;
    struct Pose *northEasternCorner;
    struct Pose *northWesternCorner;
    struct Pose *southEasternCorner;
    struct Pose *southWesternCorner;
};

struct Map *Map_new(void);
void Map_delete(struct Map *map);

#endif // MAP_H_
