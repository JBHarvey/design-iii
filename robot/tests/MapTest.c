#include <criterion/criterion.h>
#include <stdio.h>
#include "Map.h"

struct Map *map;

void setup_Map(void)
{
    map = Map_new();
}

void teardown_Map(void)
{
    Map_delete(map);
}

Test(Map, creation_destruction
     , .init = setup_Map
     , .fini = teardown_Map)
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
     , .init = setup_Map
     , .fini = teardown_Map)
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
     , .init = setup_Map
     , .fini = teardown_Map)
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
     , .init = setup_Map
     , .fini = teardown_Map)
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
     , .init = setup_Map
     , .fini = teardown_Map)
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
     , .init = setup_Map
     , .fini = teardown_Map)
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

const int TEST_ROBOT_RADIUS = 2100;
Test(Map, given_aMapAndARobotRadius_when_askedToFetchNavigableMap_then_theTableCornersAreBroughtTowardsTheCenterOfTheMapByTheRobotRadiusInXAndY
     , .init = setup_Map
     , .fini = teardown_Map)
{
    struct Map *navigable_map = Map_fetchNavigableMap(map, TEST_ROBOT_RADIUS);

    int expected_south_western_table_corner_x = map->south_western_table_corner->x + TEST_ROBOT_RADIUS;
    int expected_south_western_table_corner_y = map->south_western_table_corner->y + TEST_ROBOT_RADIUS;
    int expected_south_eastern_table_corner_x = map->south_eastern_table_corner->x - TEST_ROBOT_RADIUS;
    int expected_south_eastern_table_corner_y = map->south_eastern_table_corner->y + TEST_ROBOT_RADIUS;
    int expected_north_western_table_corner_x = map->north_western_table_corner->x + TEST_ROBOT_RADIUS;
    int expected_north_western_table_corner_y = map->north_western_table_corner->y - TEST_ROBOT_RADIUS;
    int expected_north_eastern_table_corner_x = map->north_eastern_table_corner->x - TEST_ROBOT_RADIUS;
    int expected_north_eastern_table_corner_y = map->north_eastern_table_corner->y - TEST_ROBOT_RADIUS;

    cr_assert_eq(navigable_map->south_western_table_corner->x, expected_south_western_table_corner_x);
    cr_assert_eq(navigable_map->south_western_table_corner->y, expected_south_western_table_corner_y);
    cr_assert_eq(navigable_map->south_eastern_table_corner->x, expected_south_eastern_table_corner_x);
    cr_assert_eq(navigable_map->south_eastern_table_corner->y, expected_south_eastern_table_corner_y);
    cr_assert_eq(navigable_map->north_western_table_corner->x, expected_north_western_table_corner_x);
    cr_assert_eq(navigable_map->north_western_table_corner->y, expected_north_western_table_corner_y);
    cr_assert_eq(navigable_map->north_eastern_table_corner->x, expected_north_eastern_table_corner_x);
    cr_assert_eq(navigable_map->north_eastern_table_corner->y, expected_north_eastern_table_corner_y);

    Map_delete(navigable_map);
}

Test(Map, given_aMapAndARobotRadius_when_askedToFetchNavigableMap_then_theRobotRadiusIsAddedToTheObstaclesRadius
     , .init = setup_Map
     , .fini = teardown_Map)
{
    struct Map *navigable_map = Map_fetchNavigableMap(map, TEST_ROBOT_RADIUS);

    int expected_radius_0 = map->obstacles[0]->radius + TEST_ROBOT_RADIUS;
    int expected_radius_1 = map->obstacles[1]->radius + TEST_ROBOT_RADIUS;
    int expected_radius_2 = map->obstacles[2]->radius + TEST_ROBOT_RADIUS;

    cr_assert_eq(navigable_map->obstacles[0]->radius, expected_radius_0);
    cr_assert_eq(navigable_map->obstacles[1]->radius, expected_radius_1);
    cr_assert_eq(navigable_map->obstacles[2]->radius, expected_radius_2);

    Map_delete(navigable_map);
}

void setObstacleCoordinates(struct Map *map, int index, struct Coordinates *new_coordinates)
{
    Coordinates_copyValuesFrom(map->obstacles[index]->coordinates, new_coordinates);
}

struct Map *navigable_map;
struct Coordinates *center_west_coordinates;
struct Coordinates *center_center_coordinates;
struct Coordinates *center_east_coordinates;
struct Coordinates *north_west_coordinates;
struct Coordinates *south_west_coordinates;
struct Coordinates *south_south_south_west_coordinates;
struct Coordinates *south_south_west_coordinates;
struct Coordinates *north_north_north_west_coordinates;
struct Coordinates *north_north_west_coordinates;
struct Coordinates *reset_coordinates;

void setup_NavigableMap(void)
{
    setup_Map();
    navigable_map = Map_fetchNavigableMap(map, TEST_ROBOT_RADIUS);
    center_west_coordinates = Coordinates_new(4775, 3550);
    center_center_coordinates = Coordinates_new(9550, 3550);
    center_east_coordinates = Coordinates_new(14325, 3550);
    north_west_coordinates = Coordinates_new(4775, 5325);
    south_west_coordinates = Coordinates_new(4775, 1775);
    south_south_south_west_coordinates = Coordinates_new(7640, 1775);
    south_south_west_coordinates = Coordinates_new(6367, 1775);
    north_north_north_west_coordinates = Coordinates_new(7640, 5325);
    north_north_west_coordinates = Coordinates_new(6367, 5325);
    reset_coordinates = Coordinates_new(-1, -1);
}

void teardown_NavigableMap(void)
{
    teardown_Map();
    Map_delete(navigable_map);

    Coordinates_delete(reset_coordinates);
    Coordinates_delete(center_west_coordinates);
    Coordinates_delete(center_center_coordinates);
    Coordinates_delete(center_east_coordinates);
    Coordinates_delete(north_west_coordinates);
    Coordinates_delete(south_west_coordinates);
    Coordinates_delete(south_south_south_west_coordinates);
    Coordinates_delete(south_south_west_coordinates);
    Coordinates_delete(north_north_north_west_coordinates);
    Coordinates_delete(north_north_west_coordinates);
}


Test(Map, given_aMapWithObstacles_when_coordinatesOfAnObstacleAreMinusOneInXAndY_then_theObstacleIsNotConsidered
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    cr_assert_eq(Map_fetchNumberOfObstacles(map), 3);
    setObstacleCoordinates(map, 0, reset_coordinates);
    cr_assert_eq(Map_fetchNumberOfObstacles(map), 2);
    setObstacleCoordinates(map, 1, reset_coordinates);

    cr_assert_eq(Map_fetchNumberOfObstacles(map), 1);
    setObstacleCoordinates(map, 2, reset_coordinates);
    cr_assert_eq(Map_fetchNumberOfObstacles(map), 0);
}

Test(Map, given_aNavigableMapWithOneObstacle_when_askedToRetrieveTheFirstOne_then_theEasternOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, reset_coordinates);
    setObstacleCoordinates(map, 1, reset_coordinates);
    setObstacleCoordinates(map, 2, center_west_coordinates);

    struct Obstacle *retrieved = Map_retrieveFirstObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, center_west_coordinates));
}

Test(Map, given_aNavigableMapWithTwoObstacles_when_askedToRetrieveTheFirstOne_then_theEasternOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, center_east_coordinates);
    setObstacleCoordinates(map, 1, center_west_coordinates);
    setObstacleCoordinates(map, 2, reset_coordinates);

    struct Obstacle *retrieved = Map_retrieveFirstObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, center_east_coordinates));
}

Test(Map, given_aNavigableMapWithTwoObstaclesWithSameXCoordinates_when_askedToRetrieveTheFirstOne_then_theNorthEasternOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, north_west_coordinates);
    setObstacleCoordinates(map, 1, center_west_coordinates);
    setObstacleCoordinates(map, 2, reset_coordinates);

    struct Obstacle *retrieved = Map_retrieveFirstObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, north_west_coordinates));
}

Test(Map, given_aNavigableMapWithThreeObstacles_when_askedToRetrieveTheLastOne_then_theWesternOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, center_east_coordinates);
    setObstacleCoordinates(map, 1, center_center_coordinates);
    setObstacleCoordinates(map, 2, center_west_coordinates);

    struct Obstacle *retrieved = Map_retrieveLastObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, center_west_coordinates));
}

Test(Map, given_aNavigableMapWithThreeObstaclesAndTheTwoWesternOnesWithSameXCoordinates_when_askedToRetrieveTheLastOne_then_theSouthWesternOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, south_south_west_coordinates);
    setObstacleCoordinates(map, 1, center_east_coordinates);
    setObstacleCoordinates(map, 2, north_north_west_coordinates);

    struct Obstacle *retrieved = Map_retrieveLastObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, south_south_west_coordinates));
}
