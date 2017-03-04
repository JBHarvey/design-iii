#include <criterion/criterion.h>
#include <stdio.h>
#include "Map.h"

struct Map *map;

void setupMap(void)
{
    map = Map_new();
}

void teardownMap(void)
{
    Map_delete(map);
}

Test(Map, creation_destruction
     , .init = setupMap
     , .fini = teardownMap)
{
    struct Pose *initialPose = Pose_new(0, 0, 0);
    cr_assert(Pose_haveTheSameValues(map->northEasternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->northWesternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->southEasternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->southWesternCorner, initialPose));
    Pose_delete(initialPose);
}
