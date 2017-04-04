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
    struct Pose *paintingZone0 = Pose_new(4060, 2500, 0);
    struct Pose *paintingZone1 = Pose_new(1620, 2500, 0);
    struct Pose *paintingZone2 = Pose_new(2500, 1710, MINUS_HALF_PI);
    struct Pose *paintingZone3 = Pose_new(2500, 4240, MINUS_HALF_PI);
    struct Pose *paintingZone4 = Pose_new(2500, 6770, MINUS_HALF_PI);
    struct Pose *paintingZone5 = Pose_new(2500, 9300, MINUS_HALF_PI);
    struct Pose *paintingZone6 = Pose_new(1620, 8500, PI);
    struct Pose *paintingZone7 = Pose_new(4060, 8500, PI);

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

Test(Map, given_aMapAndARobotRadius_when_askedToFetchNavigableMap_then_theTableCornersAreBroughtTowardsTheCenterOfTheMapByTheRobotRadiusInXAndY
     , .init = setup_Map
     , .fini = teardown_Map)
{
    struct Map *navigable_map = Map_fetchNavigableMap(map, THEORICAL_ROBOT_RADIUS);

    int expected_south_western_table_corner_x = map->south_western_table_corner->x + THEORICAL_ROBOT_RADIUS;
    int expected_south_western_table_corner_y = map->south_western_table_corner->y + THEORICAL_ROBOT_RADIUS;
    int expected_south_eastern_table_corner_x = map->south_eastern_table_corner->x - THEORICAL_ROBOT_RADIUS;
    int expected_south_eastern_table_corner_y = map->south_eastern_table_corner->y + THEORICAL_ROBOT_RADIUS;
    int expected_north_western_table_corner_x = map->north_western_table_corner->x + THEORICAL_ROBOT_RADIUS;
    int expected_north_western_table_corner_y = map->north_western_table_corner->y - THEORICAL_ROBOT_RADIUS;
    int expected_north_eastern_table_corner_x = map->north_eastern_table_corner->x - THEORICAL_ROBOT_RADIUS;
    int expected_north_eastern_table_corner_y = map->north_eastern_table_corner->y - THEORICAL_ROBOT_RADIUS;

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

struct Coordinates *center_west_coordinates;
struct Coordinates *center_center_coordinates;
struct Coordinates *center_east_coordinates;
Test(Map, given_aMapAndARobotRadius_when_askedToFetchNavigableMap_then_theRobotRadiusIsAddedToTheObstaclesRadius
     , .init = setup_Map
     , .fini = teardown_Map)
{
    enum CardinalDirection cardinal_direction_0 = NORTH;
    enum CardinalDirection cardinal_direction_1 = SOUTH;
    enum CardinalDirection cardinal_direction_2 = CENTER;
    center_west_coordinates = Coordinates_new(4775, 3550);
    center_center_coordinates = Coordinates_new(9550, 3550);
    center_east_coordinates = Coordinates_new(14325, 3550);
    Map_updateObstacle(map, center_west_coordinates, cardinal_direction_0, 0);
    Map_updateObstacle(map, center_center_coordinates, cardinal_direction_1, 1);
    Map_updateObstacle(map, center_east_coordinates, cardinal_direction_2, 2);

    struct Map *navigable_map = Map_fetchNavigableMap(map, THEORICAL_ROBOT_RADIUS);

    int expected_radius_0 = map->obstacles[0]->radius + THEORICAL_ROBOT_RADIUS;
    int expected_radius_1 = map->obstacles[1]->radius + THEORICAL_ROBOT_RADIUS;
    int expected_radius_2 = map->obstacles[2]->radius + THEORICAL_ROBOT_RADIUS;

    cr_assert_eq(navigable_map->obstacles[0]->radius, expected_radius_0);
    cr_assert_eq(navigable_map->obstacles[1]->radius, expected_radius_1);
    cr_assert_eq(navigable_map->obstacles[2]->radius, expected_radius_2);
    cr_assert(Coordinates_haveTheSameValues(navigable_map->obstacles[0]->coordinates, center_west_coordinates));
    cr_assert(Coordinates_haveTheSameValues(navigable_map->obstacles[1]->coordinates, center_center_coordinates));
    cr_assert(Coordinates_haveTheSameValues(navigable_map->obstacles[2]->coordinates, center_east_coordinates));
    cr_assert_eq(navigable_map->obstacles[0]->orientation, cardinal_direction_0);
    cr_assert_eq(navigable_map->obstacles[1]->orientation, cardinal_direction_1);
    cr_assert_eq(navigable_map->obstacles[2]->orientation, cardinal_direction_2);

    Map_delete(navigable_map);
    Coordinates_delete(center_west_coordinates);
    Coordinates_delete(center_center_coordinates);
    Coordinates_delete(center_east_coordinates);
}

Test(Map, given_aMapAndARobot_when_askedToFetchNavigableMap_then_theRobotRadiusIsSubstractedInYToTheAntennaZoneCoordinates
     , .init = setup_Map
     , .fini = teardown_Map)
{
    int antenna_zone_y = THEORICAL_WORLD_HEIGHT - THEORICAL_ROBOT_RADIUS;
    struct Coordinates *antenna_zone_start = Coordinates_new(THEORICAL_ANTENNA_ZONE_START_X, THEORICAL_ANTENNA_ZONE_Y);
    struct Coordinates *antenna_zone_stop = Coordinates_new(THEORICAL_ANTENNA_ZONE_STOP_X, THEORICAL_ANTENNA_ZONE_Y);
    Map_updateAntennaZone(map, antenna_zone_start, antenna_zone_stop);

    struct Map *navigable_map = Map_fetchNavigableMap(map, THEORICAL_ROBOT_RADIUS);

    struct Coordinates *expected_start = Coordinates_new(THEORICAL_ANTENNA_ZONE_START_X,
                                         THEORICAL_ANTENNA_ZONE_Y - THEORICAL_ROBOT_RADIUS);
    struct Coordinates *expected_stop = Coordinates_new(THEORICAL_ANTENNA_ZONE_STOP_X,
                                        THEORICAL_ANTENNA_ZONE_Y - THEORICAL_ROBOT_RADIUS);
    cr_assert(Coordinates_haveTheSameValues(expected_start, navigable_map->antenna_zone_start));
    cr_assert(Coordinates_haveTheSameValues(expected_stop, navigable_map->antenna_zone_stop));

    Coordinates_delete(expected_start);
    Coordinates_delete(expected_stop);
    Coordinates_delete(antenna_zone_start);
    Coordinates_delete(antenna_zone_stop);
    Map_delete(navigable_map);
}

Test(Map, given_aMapAndARobot_when_askedToFetchNavigableMap_then_thePaintingZonesAreTransfered
     , .init = setup_Map
     , .fini = teardown_Map)
{
    //TODO: ADD PAINTINGZONE VALUES
    struct Map *navigable_map = Map_fetchNavigableMap(map, THEORICAL_ROBOT_RADIUS);

    struct Pose *original_painting_pose;
    struct Pose *navigable_painting_pose;

    for(int i = 0; i < NUMBER_OF_PAINTING; ++i) {
        original_painting_pose = map->painting_zones[i];
        navigable_painting_pose = navigable_map->painting_zones[i];
        cr_assert(Pose_haveTheSameValues(original_painting_pose, navigable_painting_pose));
    }

    Map_delete(navigable_map);
}

void setObstacleCoordinates(struct Map *map, int index, struct Coordinates *new_coordinates)
{
    Coordinates_copyValuesFrom(map->obstacles[index]->coordinates, new_coordinates);
}

struct Map *navigable_map;
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
    navigable_map = Map_fetchNavigableMap(map, THEORICAL_ROBOT_RADIUS);
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

Test(Map, given_aFirstAndSecondObstacleAndNoOverlap_when_askedToRetrieveTheMiddleOne_then_theMiddleOneIsRetrieved
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, center_east_coordinates);
    setObstacleCoordinates(map, 1, center_center_coordinates);
    setObstacleCoordinates(map, 2, center_west_coordinates);

    struct Obstacle *first = Map_retrieveFirstObstacle(map);
    struct Obstacle *last = Map_retrieveLastObstacle(map);
    struct Obstacle *middle = Map_retrieveMiddleObstacle(map, first, last);

    cr_assert(Coordinates_haveTheSameValues(middle->coordinates, center_center_coordinates));
}

Test(Map, given_threeOverlappingOverlappingObstacle_when_askedToRetrieveTheFirstOne_then_theNorthernOneIsReturned
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, south_south_west_coordinates);
    setObstacleCoordinates(map, 1, center_west_coordinates);
    setObstacleCoordinates(map, 2, north_north_west_coordinates);

    struct Obstacle *retrieved = Map_retrieveFirstOverlappingObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, north_north_west_coordinates));
}

Test(Map, given_threeOverlappingOverlappingObstacle_when_askedToRetrieveTheLastOne_then_theSouthernOneIsReturned
     , .init = setup_NavigableMap
     , .fini = teardown_NavigableMap)
{
    setObstacleCoordinates(map, 0, south_south_west_coordinates);
    setObstacleCoordinates(map, 1, center_west_coordinates);
    setObstacleCoordinates(map, 2, north_north_west_coordinates);

    struct Obstacle *retrieved = Map_retrieveLastOverlappingObstacle(map);

    cr_assert(Coordinates_haveTheSameValues(retrieved->coordinates, south_south_west_coordinates));
}

void setup_FreeMap(void)
{
    setup_Map();
    struct Coordinates *south_western_table_corner = Coordinates_zero();
    struct Coordinates *south_eastern_table_corner = Coordinates_new(THEORICAL_WORLD_LENGTH, 0);
    struct Coordinates *north_western_table_corner = Coordinates_new(0, THEORICAL_WORLD_HEIGHT);
    struct Coordinates *north_eastern_table_corner = Coordinates_new(THEORICAL_WORLD_LENGTH, THEORICAL_WORLD_HEIGHT);
    Map_updateTableCorners(map, north_eastern_table_corner, south_eastern_table_corner, south_western_table_corner,
                           north_western_table_corner);

    Coordinates_delete(south_western_table_corner);
    Coordinates_delete(south_eastern_table_corner);
    Coordinates_delete(north_western_table_corner);
    Coordinates_delete(north_eastern_table_corner);
}

Test(Map, given_coordinatesToTheNorthOfTheNorthernNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_north = Coordinates_new(0, THEORICAL_WORLD_HEIGHT * 2);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_north);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_north);
}

Test(Map, given_coordinatesOnTheNorthernNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_north = Coordinates_new(0, THEORICAL_WORLD_HEIGHT);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_north);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_north);
}

Test(Map, given_coordinatesToTheSouthOfTheSouthernNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_south = Coordinates_new(0, -1 * THEORICAL_WORLD_HEIGHT);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_south);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_south);
}

Test(Map, given_coordinatesOnTheSouthernNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_south = Coordinates_new(0, 0);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_south);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_south);
}

Test(Map, given_coordinatesToTheEastOfTheEasternNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_east = Coordinates_new(THEORICAL_WORLD_LENGTH * 2, 0);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_east);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_east);
}

Test(Map, given_coordinatesOnTheEasternNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_east = Coordinates_new(THEORICAL_WORLD_LENGTH, 0);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_east);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_east);
}

Test(Map, given_coordinatesToTheWestOfTheWesternNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_west = Coordinates_new(-1 * THEORICAL_WORLD_HEIGHT, 0);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_west);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_west);
}

Test(Map, given_coordinatesOnTheWesternNavigableY_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *too_to_the_west = Coordinates_new(0, 0);

    int is_within_navigable_space = Map_isCoordinateFree(map, too_to_the_west);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(too_to_the_west);
}

Test(Map, given_coordinatesNotFreeOfAnObstacle_when_askedIfItIsFree_then_itIsNot
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *coordinates = Coordinates_new(THEORICAL_WORLD_LENGTH / 2, THEORICAL_WORLD_HEIGHT / 2);
    setObstacleCoordinates(map, 1, coordinates);
    int is_within_navigable_space = Map_isCoordinateFree(map, coordinates);
    cr_assert(!is_within_navigable_space);

    Coordinates_delete(coordinates);
}

Test(Map, given_coordinatesFreeOfTheWallsAndObstacles_when_askedIfItIsFree_then_itIs
     , .init = setup_FreeMap
     , .fini = teardown_Map)
{
    struct Coordinates *coordinates = Coordinates_new(THEORICAL_WORLD_LENGTH / 2, THEORICAL_WORLD_HEIGHT / 2);
    int is_within_navigable_space = Map_isCoordinateFree(map, coordinates);
    cr_assert(is_within_navigable_space);

    Coordinates_delete(coordinates);
}

const int MAP_MANCHESTER_PAINTING_NUMBER = 0;
struct ManchesterCode *manchester_code;
const int DRAWING_SIZE = 666;
struct Coordinates *upper_right;
struct Coordinates *lower_right;
struct Coordinates *lower_left;
struct Coordinates *upper_left;
struct CoordinatesSequence *sequence;

void setup_DrawingMap(void)
{
    setup_Map();
    upper_right = Coordinates_new(DRAWING_SIZE, DRAWING_SIZE);
    lower_right = Coordinates_new(DRAWING_SIZE, -DRAWING_SIZE);
    lower_left = Coordinates_new(-DRAWING_SIZE, -DRAWING_SIZE);
    upper_left = Coordinates_new(-DRAWING_SIZE, DRAWING_SIZE);
    sequence = CoordinatesSequence_new(upper_right);
    CoordinatesSequence_append(sequence, lower_right);
    CoordinatesSequence_append(sequence, lower_left);
    CoordinatesSequence_append(sequence, upper_left);

    manchester_code = ManchesterCode_new();
    struct Coordinates *south_western_drawing_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X -
            THEORICAL_DRAWING_ZONE_SIDE, THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y);
    struct Coordinates *south_eastern_drawing_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
            THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y);
    struct Coordinates *north_western_drawing_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X -
            THEORICAL_DRAWING_ZONE_SIDE, THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE);
    struct Coordinates *north_eastern_drawing_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
            THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE);

    Map_updateDrawingCorners(map, north_eastern_drawing_corner, south_eastern_drawing_corner, south_western_drawing_corner,
                             north_western_drawing_corner);
    Coordinates_delete(south_western_drawing_corner);
    Coordinates_delete(south_eastern_drawing_corner);
    Coordinates_delete(north_western_drawing_corner);
    Coordinates_delete(north_eastern_drawing_corner);
}

void teardown_DrawingMap(void)
{
    Coordinates_delete(upper_right);
    Coordinates_delete(lower_right);
    Coordinates_delete(lower_left);
    Coordinates_delete(upper_left);
    ManchesterCode_delete(manchester_code);
    CoordinatesSequence_delete(sequence);
    teardown_Map();
}

void assertCoordinatesHaveTheSameValuesWithTolerance(struct Coordinates *received_coordinates,
        struct Coordinates *expected_coordinates, int tolerance)
{
    int expected_x = expected_coordinates->x;
    int expected_y = expected_coordinates->y;
    int received_x = received_coordinates->x;
    int received_y = received_coordinates->y;
    cr_assert((expected_x == received_x ||
               expected_x == received_x + tolerance ||
               expected_x == received_x - tolerance)
              &&
              (expected_y == received_y ||
               expected_y == received_y + tolerance ||
               expected_y == received_y - tolerance)
              ,
              "ManchesterCode transformation. \n Expected : (%d,%d) ± %d\nReceived : (%d,%d)\n",
              expected_x, expected_y,
              tolerance,
              received_x, received_y
             );
}


Test(Map, given_aCoordinatesSequenceAndAManchesterCodeWithEastOrientation_when_askedToCreateTheDrawingTrajectory_then_itIsRotatedToTheMinusHalfPi
     , .init = setup_DrawingMap
     , .fini = teardown_DrawingMap)
{
    struct Coordinates *zero = Coordinates_zero();
    Map_updateDrawingCorners(map, zero, zero, zero, zero);
    ManchesterCode_updateCodeValues(manchester_code, MAP_MANCHESTER_PAINTING_NUMBER, 1, EAST);
    struct Angle *angle = ManchesterCode_retrieveOrientationAngle(manchester_code);
    struct Coordinates *expected_coordinates_upper_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_upper_left = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_left = Coordinates_zero();

    Coordinates_copyValuesFrom(expected_coordinates_upper_right, upper_right);
    Coordinates_copyValuesFrom(expected_coordinates_upper_left, upper_left);
    Coordinates_copyValuesFrom(expected_coordinates_lower_right, lower_right);
    Coordinates_copyValuesFrom(expected_coordinates_lower_left, lower_left);

    Map_createDrawingTrajectory(map, manchester_code, sequence);

    struct CoordinatesSequence *sequence_head = sequence;

    Coordinates_rotateOfAngle(expected_coordinates_lower_left, angle);
    Coordinates_rotateOfAngle(expected_coordinates_upper_left, angle);
    Coordinates_rotateOfAngle(expected_coordinates_upper_right, angle);
    Coordinates_rotateOfAngle(expected_coordinates_lower_right, angle);

    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_left, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_left, 1);
    sequence = sequence_head;

    Angle_delete(angle);
    Coordinates_delete(zero);
    Coordinates_delete(expected_coordinates_upper_right);
    Coordinates_delete(expected_coordinates_upper_left);
    Coordinates_delete(expected_coordinates_lower_right);
    Coordinates_delete(expected_coordinates_lower_left);
}

Test(Map, given_aCoordinatesSequenceAndAManchesterCodeWithTimesTwoScaleFactor_when_askedToCreateTheDrawingTrajectory_then_itScaleCorrectly
     , .init = setup_DrawingMap
     , .fini = teardown_DrawingMap)
{
    struct Coordinates *zero = Coordinates_zero();
    Map_updateDrawingCorners(map, zero, zero, zero, zero);
    ManchesterCode_updateCodeValues(manchester_code, MAP_MANCHESTER_PAINTING_NUMBER, TIMES_TWO, NORTH);
    struct Coordinates *expected_coordinates_upper_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_upper_left = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_left = Coordinates_zero();

    Coordinates_copyValuesFrom(expected_coordinates_upper_right, upper_right);
    Coordinates_copyValuesFrom(expected_coordinates_upper_left, upper_left);
    Coordinates_copyValuesFrom(expected_coordinates_lower_right, lower_right);
    Coordinates_copyValuesFrom(expected_coordinates_lower_left, lower_left);

    Map_createDrawingTrajectory(map, manchester_code, sequence);

    struct CoordinatesSequence *sequence_head = sequence;

    Coordinates_scaleOf(expected_coordinates_lower_left, TIMES_TWO);
    Coordinates_scaleOf(expected_coordinates_upper_left, TIMES_TWO);
    Coordinates_scaleOf(expected_coordinates_upper_right, TIMES_TWO);
    Coordinates_scaleOf(expected_coordinates_lower_right, TIMES_TWO);

    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_left, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_left, 1);
    sequence = sequence_head;

    Coordinates_delete(zero);
    Coordinates_delete(expected_coordinates_upper_right);
    Coordinates_delete(expected_coordinates_upper_left);
    Coordinates_delete(expected_coordinates_lower_right);
    Coordinates_delete(expected_coordinates_lower_left);
}

Test(Map, given_aCoordinatesSequenceAndAManchesterCodeWithNoChangesInRotationAndScaling_when_askedToCreateTheDrawingTrajectory_then_itTranslatedCorrectlyToTheDrawingZone
     , .init = setup_DrawingMap
     , .fini = teardown_DrawingMap)
{
    int x = Coordinates_computeMeanX(map->south_western_drawing_corner, map->north_eastern_drawing_corner);
    int y = Coordinates_computeMeanY(map->south_western_drawing_corner, map->north_eastern_drawing_corner);
    struct Coordinates *new_center = Coordinates_new(x, y);
    ManchesterCode_updateCodeValues(manchester_code, MAP_MANCHESTER_PAINTING_NUMBER, 1, NORTH);
    struct Coordinates *expected_coordinates_upper_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_upper_left = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_right = Coordinates_zero();
    struct Coordinates *expected_coordinates_lower_left = Coordinates_zero();

    Coordinates_copyValuesFrom(expected_coordinates_upper_right, upper_right);
    Coordinates_copyValuesFrom(expected_coordinates_upper_left, upper_left);
    Coordinates_copyValuesFrom(expected_coordinates_lower_right, lower_right);
    Coordinates_copyValuesFrom(expected_coordinates_lower_left, lower_left);

    Map_createDrawingTrajectory(map, manchester_code, sequence);

    struct CoordinatesSequence *sequence_head = sequence;

    Coordinates_translateOf(expected_coordinates_lower_left, new_center);
    Coordinates_translateOf(expected_coordinates_upper_left, new_center);
    Coordinates_translateOf(expected_coordinates_upper_right, new_center);
    Coordinates_translateOf(expected_coordinates_lower_right, new_center);

    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_right, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_lower_left, 1);
    sequence = sequence->next_element;
    assertCoordinatesHaveTheSameValuesWithTolerance(sequence->coordinates, expected_coordinates_upper_left, 1);
    sequence = sequence_head;

    Coordinates_delete(new_center);
    Coordinates_delete(expected_coordinates_upper_right);
    Coordinates_delete(expected_coordinates_upper_left);
    Coordinates_delete(expected_coordinates_lower_right);
    Coordinates_delete(expected_coordinates_lower_left);
}
