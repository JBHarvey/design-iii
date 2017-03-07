#include <criterion/criterion.h>
#include <stdio.h>
#include "Map.h"

struct Map *map;

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

const int NUMBER_OF_OBSTACLES = 3;

struct Obstacles *obstacle0;
struct Obstacles *obstacle1;
struct Obstacles *obstacle2;

struct Pose *paintingZone0;
struct Pose *paintingZone1;
struct Pose *paintingZone2;
struct Pose *paintingZone3;
struct Pose *paintingZone4;
struct Pose *paintingZone5;
struct Pose *paintingZone6;
struct Pose *paintingZone7;

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
    struct Coordinates *coordinatesZero = Coordinates_zero();
    struct Pose *poseZero = Pose_zero();
    cr_assert(Coordinates_haveTheSameValues(map->northEasternTableCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->northWesternTableCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->southEasternTableCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->southWesternTableCorner, coordinatesZero));

    cr_assert(Coordinates_haveTheSameValues(map->northEasternDrawingCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->northWesternDrawingCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->southEasternDrawingCorner, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->southWesternDrawingCorner, coordinatesZero));

    cr_assert(Coordinates_haveTheSameValues(map->antennaZoneStart, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->antennaZoneStop, coordinatesZero));

    cr_assert(map->numberOfObstacles == 0);
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[0]->coordinates, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[2]->coordinates, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[1]->coordinates, coordinatesZero));
    cr_assert_eq(map->obstacles[0]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[1]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[2]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[0]->orientation, CENTER);
    cr_assert_eq(map->obstacles[1]->orientation, CENTER);
    cr_assert_eq(map->obstacles[2]->orientation, CENTER);

    cr_assert(Pose_haveTheSameValues(map->paintingZones[0], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[1], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[2], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[3], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[4], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[5], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[6], poseZero));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[7], poseZero));
    Coordinates_delete(coordinatesZero);
    Pose_delete(poseZero);
}
