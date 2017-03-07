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

    cr_assert(Coordinates_haveTheSameValues(map->obstacles[0]->coordinates, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[1]->coordinates, coordinatesZero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[2]->coordinates, coordinatesZero));
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

Test(Map, given_newTableCorners_when_updatesMap_then_theCorrespondingCornersHaveChanged
     , .init = setupMap
     , .fini = teardownMap)
{
    const int MAP_TABLE_HEIGHT = 11000;
    const int MAP_TABLE_LENGTH = 20000;
    struct Coordinates *northEasternTableCorner = Coordinates_new(MAP_TABLE_LENGTH, MAP_TABLE_HEIGHT);
    struct Coordinates *southEasternTableCorner = Coordinates_new(MAP_TABLE_LENGTH, 0);
    struct Coordinates *southWesternTableCorner = Coordinates_new(0, 0);
    struct Coordinates *northWesternTableCorner = Coordinates_new(0, MAP_TABLE_HEIGHT);

    Map_updateTableCorners(map, northEasternTableCorner, southEasternTableCorner,
                           southWesternTableCorner, northWesternTableCorner);

    cr_assert(Coordinates_haveTheSameValues(map->northEasternTableCorner, northEasternTableCorner));
    cr_assert(Coordinates_haveTheSameValues(map->northWesternTableCorner, northWesternTableCorner));
    cr_assert(Coordinates_haveTheSameValues(map->southEasternTableCorner, southEasternTableCorner));
    cr_assert(Coordinates_haveTheSameValues(map->southWesternTableCorner, southWesternTableCorner));

    Coordinates_delete(northEasternTableCorner);
    Coordinates_delete(southEasternTableCorner);
    Coordinates_delete(southWesternTableCorner);
    Coordinates_delete(northWesternTableCorner);
}

Test(Map, given_newDrawingCorners_when_updatesMap_then_theCorrespondingCornersHaveChanged
     , .init = setupMap
     , .fini = teardownMap)
{
    const int MAP_DRAWING_SIDE = 6600;
    struct Coordinates *northEasternDrawingCorner = Coordinates_new(MAP_DRAWING_SIDE, MAP_DRAWING_SIDE);
    struct Coordinates *southEasternDrawingCorner = Coordinates_new(MAP_DRAWING_SIDE, 0);
    struct Coordinates *southWesternDrawingCorner = Coordinates_new(0, 0);
    struct Coordinates *northWesternDrawingCorner = Coordinates_new(0, MAP_DRAWING_SIDE);

    Map_updateDrawingCorners(map, northEasternDrawingCorner, southEasternDrawingCorner,
                             southWesternDrawingCorner, northWesternDrawingCorner);

    cr_assert(Coordinates_haveTheSameValues(map->northEasternDrawingCorner, northEasternDrawingCorner));
    cr_assert(Coordinates_haveTheSameValues(map->northWesternDrawingCorner, northWesternDrawingCorner));
    cr_assert(Coordinates_haveTheSameValues(map->southEasternDrawingCorner, southEasternDrawingCorner));
    cr_assert(Coordinates_haveTheSameValues(map->southWesternDrawingCorner, southWesternDrawingCorner));

    Coordinates_delete(northEasternDrawingCorner);
    Coordinates_delete(southEasternDrawingCorner);
    Coordinates_delete(southWesternDrawingCorner);
    Coordinates_delete(northWesternDrawingCorner);
}

Test(Map, given_newAntennaZone_when_updatesMap_then_theCorrespondingCoordinatesHaveChanged
     , .init = setupMap
     , .fini = teardownMap)
{
    const int ZONE_X = 10000;
    const int ZONE_Y = 10000;
    const int ZONE_LENGTH = 6000;
    struct Coordinates *antennaZoneStart = Coordinates_new(ZONE_X + ZONE_LENGTH, ZONE_Y);
    struct Coordinates *antennaZoneStop = Coordinates_new(ZONE_X, ZONE_Y);

    Map_updateAntennaZone(map, antennaZoneStart, antennaZoneStop);

    cr_assert(Coordinates_haveTheSameValues(map->antennaZoneStart, antennaZoneStart));
    cr_assert(Coordinates_haveTheSameValues(map->antennaZoneStop, antennaZoneStop));

    Coordinates_delete(antennaZoneStart);
    Coordinates_delete(antennaZoneStop);
}

Test(Map, given_newObstacles_when_updatesMap_then_theCorrespondingObstaclesHaveChanged
     , .init = setupMap
     , .fini = teardownMap)
{
    struct Coordinates *coordinates0 = Coordinates_new(1250, 3250);
    struct Coordinates *coordinates1 = Coordinates_new(5500, 4500);
    struct Coordinates *coordinates2 = Coordinates_new(7000, 2500);

    Map_updateObstacle(map, coordinates0, NORTH, 0);
    Map_updateObstacle(map, coordinates1, SOUTH, 1);
    Map_updateObstacle(map, coordinates2, CENTER, 2);

    cr_assert(Coordinates_haveTheSameValues(map->obstacles[0]->coordinates, coordinates0));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[1]->coordinates, coordinates1));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[2]->coordinates, coordinates2));
    cr_assert_eq(map->obstacles[0]->orientation, NORTH);
    cr_assert_eq(map->obstacles[1]->orientation, SOUTH);
    cr_assert_eq(map->obstacles[2]->orientation, CENTER);

    Coordinates_delete(coordinates0);
    Coordinates_delete(coordinates1);
    Coordinates_delete(coordinates2);
}

Test(Map, given_newPaintingZones_when_updatesMap_then_theCorrespondingZonesHaveChanged
     , .init = setupMap
     , .fini = teardownMap)
{
    struct Pose *paintingZone0 = Pose_new(4060, 2500, MINUS_HALF_PI);
    struct Pose *paintingZone1 = Pose_new(1620, 2500, MINUS_HALF_PI);
    struct Pose *paintingZone2 = Pose_new(2500, 1710, PI);
    struct Pose *paintingZone3 = Pose_new(2500, 4240, PI);
    struct Pose *paintingZone4 = Pose_new(2500, 6770, PI);
    struct Pose *paintingZone5 = Pose_new(2500, 9300, PI);
    struct Pose *paintingZone6 = Pose_new(1620, 8500, HALF_PI);
    struct Pose *paintingZone7 = Pose_new(4060, 8500, HALF_PI);

    Map_updatePaintingZone(map, paintingZone0, 0);
    Map_updatePaintingZone(map, paintingZone1, 1);
    Map_updatePaintingZone(map, paintingZone2, 2);
    Map_updatePaintingZone(map, paintingZone3, 3);
    Map_updatePaintingZone(map, paintingZone4, 4);
    Map_updatePaintingZone(map, paintingZone5, 5);
    Map_updatePaintingZone(map, paintingZone6, 6);
    Map_updatePaintingZone(map, paintingZone7, 7);

    cr_assert(Pose_haveTheSameValues(map->paintingZones[0], paintingZone0));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[1], paintingZone1));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[2], paintingZone2));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[3], paintingZone3));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[4], paintingZone4));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[5], paintingZone5));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[6], paintingZone6));
    cr_assert(Pose_haveTheSameValues(map->paintingZones[7], paintingZone7));

    Pose_delete(paintingZone0);
    Pose_delete(paintingZone1);
    Pose_delete(paintingZone2);
    Pose_delete(paintingZone3);
    Pose_delete(paintingZone4);
    Pose_delete(paintingZone5);
    Pose_delete(paintingZone6);
    Pose_delete(paintingZone7);
}
