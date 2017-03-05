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
    struct Pose *initialPose = Pose_zero();
    cr_assert(Pose_haveTheSameValues(map->northEasternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->northWesternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->southEasternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->southWesternCorner, initialPose));
    cr_assert(Pose_haveTheSameValues(map->antennaZoneStart, initialPose));
    cr_assert(Pose_haveTheSameValues(map->antennaZoneStop, initialPose));
    cr_assert(map->numberOfObstacles == 0);
    //    cr_assert(Pose_haveTheSameValues(map->obstacles[0], initialPose));
    //    cr_assert(Pose_haveTheSameValues(map->obstacles[1], initialPose));
    //    cr_assert(Pose_haveTheSameValues(map->obstacles[2], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[0], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[1], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[2], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[3], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[4], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[5], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[6], initialPose));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[7], initialPose));
    Pose_delete(initialPose);
}
