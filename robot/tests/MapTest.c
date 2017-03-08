#include <criterion/criterion.h>
#include <stdio.h>
#include "Map.h"

struct Map *map;

void setup_map(void)
{
    map = Map_new();
}

void teardown_map(void)
{
    Map_delete(map);
}

Test(Map, creation_destruction
     , .init = setup_map
     , .fini = teardown_map)
{
    struct Coordinates *coordinates_zero = Coordinates_zero();
    struct Pose *pose_zero = Pose_zero();
    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_table_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_table_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_table_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_table_corner, coordinates_zero));

    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_drawing_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_drawing_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_drawing_corner, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_drawing_corner, coordinates_zero));

    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_start, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_stop, coordinates_zero));

    cr_assert(Coordinates_haveTheSameValues(map->obstacles[0]->coordinates, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[1]->coordinates, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[2]->coordinates, coordinates_zero));
    cr_assert_eq(map->obstacles[0]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[1]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[2]->radius, THEORICAL_OBSTACLE_RADIUS);
    cr_assert_eq(map->obstacles[0]->orientation, CENTER);
    cr_assert_eq(map->obstacles[1]->orientation, CENTER);
    cr_assert_eq(map->obstacles[2]->orientation, CENTER);

    cr_assert(Pose_haveTheSameValues(map->painting_zones[0], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[1], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[2], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[3], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[4], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[5], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[6], pose_zero));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[7], pose_zero));
    Coordinates_delete(coordinates_zero);
    Pose_delete(pose_zero);
}

Test(Map, given_newTableCorners_when_updatesMap_then_theCorrespondingCornersHaveChanged
     , .init = setup_map
     , .fini = teardown_map)
{
    const int MAP_TABLE_HEIGHT = 11000;
    const int MAP_TABLE_LENGTH = 20000;
    struct Coordinates *north_eastern_table_corner = Coordinates_new(MAP_TABLE_LENGTH, MAP_TABLE_HEIGHT);
    struct Coordinates *south_eastern_table_corner = Coordinates_new(MAP_TABLE_LENGTH, 0);
    struct Coordinates *south_western_table_corner = Coordinates_new(0, 0);
    struct Coordinates *north_western_table_corner = Coordinates_new(0, MAP_TABLE_HEIGHT);

    Map_updateTableCorners(map, north_eastern_table_corner, south_eastern_table_corner,
                           south_western_table_corner, north_western_table_corner);

    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_table_corner, north_eastern_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_table_corner, north_western_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_table_corner, south_eastern_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_table_corner, south_western_table_corner));

    Coordinates_delete(north_eastern_table_corner);
    Coordinates_delete(south_eastern_table_corner);
    Coordinates_delete(south_western_table_corner);
    Coordinates_delete(north_western_table_corner);
}

Test(Map, given_newDrawingCorners_when_updatesMap_then_theCorrespondingCornersHaveChanged
     , .init = setup_map
     , .fini = teardown_map)
{
    const int MAP_DRAWING_SIDE = 6600;
    struct Coordinates *north_eastern_drawing_corner = Coordinates_new(MAP_DRAWING_SIDE, MAP_DRAWING_SIDE);
    struct Coordinates *south_eastern_drawing_corner = Coordinates_new(MAP_DRAWING_SIDE, 0);
    struct Coordinates *south_western_drawing_corner = Coordinates_new(0, 0);
    struct Coordinates *north_western_drawing_corner = Coordinates_new(0, MAP_DRAWING_SIDE);

    Map_updateDrawingCorners(map, north_eastern_drawing_corner, south_eastern_drawing_corner,
                             south_western_drawing_corner, north_western_drawing_corner);

    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_drawing_corner, north_eastern_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_drawing_corner, north_western_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_drawing_corner, south_eastern_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_drawing_corner, south_western_drawing_corner));

    Coordinates_delete(north_eastern_drawing_corner);
    Coordinates_delete(south_eastern_drawing_corner);
    Coordinates_delete(south_western_drawing_corner);
    Coordinates_delete(north_western_drawing_corner);
}

Test(Map, given_newAntennaZone_when_updatesMap_then_theCorrespondingCoordinatesHaveChanged
     , .init = setup_map
     , .fini = teardown_map)
{
    const int ZONE_X = 10000;
    const int ZONE_Y = 10000;
    const int ZONE_LENGTH = 6000;
    struct Coordinates *antenna_zone_start = Coordinates_new(ZONE_X + ZONE_LENGTH, ZONE_Y);
    struct Coordinates *antenna_zone_stop = Coordinates_new(ZONE_X, ZONE_Y);

    Map_updateAntennaZone(map, antenna_zone_start, antenna_zone_stop);

    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_start, antenna_zone_start));
    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_stop, antenna_zone_stop));

    Coordinates_delete(antenna_zone_start);
    Coordinates_delete(antenna_zone_stop);
}

Test(Map, given_newObstacles_when_updatesMap_then_theCorrespondingObstaclesHaveChanged
     , .init = setup_map
     , .fini = teardown_map)
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
     , .init = setup_map
     , .fini = teardown_map)
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

    cr_assert(Pose_haveTheSameValues(map->painting_zones[0], paintingZone0));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[1], paintingZone1));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[2], paintingZone2));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[3], paintingZone3));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[4], paintingZone4));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[5], paintingZone5));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[6], paintingZone6));
    cr_assert(Pose_haveTheSameValues(map->painting_zones[7], paintingZone7));

    Pose_delete(paintingZone0);
    Pose_delete(paintingZone1);
    Pose_delete(paintingZone2);
    Pose_delete(paintingZone3);
    Pose_delete(paintingZone4);
    Pose_delete(paintingZone5);
    Pose_delete(paintingZone6);
    Pose_delete(paintingZone7);
}
