#include <criterion/criterion.h>
#include <stdio.h>
#include "RobotReceiver.h"

struct Mesurements mesurementsMock;

const int RECEIVER_TEST_ROBOT_RADIUS = 1750;
const int RECEIVER_TEST_ROBOT_X = 9000;
const int RECEIVER_TEST_ROBOT_Y = 5000;
const int RECEIVER_TEST_ROBOT_THETA = PI;
const int MAP_TABLE_HEIGHT = 11000;
const int MAP_TABLE_LENGTH = 20000;
const int MAP_DRAWING_SIDE = 6600;
const int ANTENNA_ZONE_X = 10000;
const int ANTENNA_ZONE_Y = 10000;
const int ANTENNA_ZONE_LENGTH = 6000;
const int TEST_OBSTACLE_0_X = 1250;
const int TEST_OBSTACLE_0_Y = 3250;
const int TEST_OBSTACLE_0_ORIENTATION = HALF_PI;
const int TEST_OBSTACLE_1_X = 5500;
const int TEST_OBSTACLE_1_Y = 4500;
const int TEST_OBSTACLE_1_ORIENTATION = MINUS_HALF_PI;
const int TEST_OBSTACLE_2_X = 7000;
const int TEST_OBSTACLE_2_Y = 2500;
const int TEST_OBSTACLE_2_ORIENTATION = 0;
const int TEST_PAINTING_0_X = 4060;
const int TEST_PAINTING_0_Y = 2500;
const int TEST_PAINTING_0_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_1_X = 1620;
const int TEST_PAINTING_1_Y = 2500;
const int TEST_PAINTING_1_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_2_X = 2500;
const int TEST_PAINTING_2_Y = 1710;
const int TEST_PAINTING_2_ORIENTATION = PI;
const int TEST_PAINTING_3_X = 2500;
const int TEST_PAINTING_3_Y = 4240;
const int TEST_PAINTING_3_ORIENTATION = PI;
const int TEST_PAINTING_4_X = 2500;
const int TEST_PAINTING_4_Y = 6770;
const int TEST_PAINTING_4_ORIENTATION = PI;
const int TEST_PAINTING_5_X = 2500;
const int TEST_PAINTING_5_Y = 9300;
const int TEST_PAINTING_5_ORIENTATION = PI;
const int TEST_PAINTING_6_X = 1620;
const int TEST_PAINTING_6_Y = 8500;
const int TEST_PAINTING_6_ORIENTATION = HALF_PI;
const int TEST_PAINTING_7_X = 4060;
const int TEST_PAINTING_7_Y = 8500;
const int TEST_PAINTING_7_ORIENTATION = HALF_PI;
void setupRobotReceiver(void)
{
    struct Mesurements mesurements = {
        .world = {
            .environment = {
                .northEasternTableCorner = {.x = MAP_TABLE_LENGTH, .y = MAP_TABLE_HEIGHT},
                .northWesternTableCorner = {.x = 0, .y = MAP_TABLE_HEIGHT},
                .southEasternTableCorner = {.x = MAP_TABLE_LENGTH, .y = 0},
                .southWesternTableCorner = {.x = 0, .y = 0},
                .northEasternDrawingCorner = {.x = MAP_DRAWING_SIDE, .y = MAP_DRAWING_SIDE},
                .northWesternDrawingCorner = {.x = 0, .y = MAP_DRAWING_SIDE},
                .southEasternDrawingCorner = {.x = MAP_DRAWING_SIDE, .y = 0},
                .southWesternDrawingCorner = {.x = 0, .y = 0},
                .antennaZoneStart = {.x = ANTENNA_ZONE_X + ANTENNA_ZONE_LENGTH, .y = ANTENNA_ZONE_Y},
                .antennaZoneStop = {.x = ANTENNA_ZONE_X, .y = ANTENNA_ZONE_Y},
                .obstacles = {
                    {
                        // Obstacle 0
                        .zone = {
                            .pose = {
                                .coordinates = { .x = TEST_OBSTACLE_0_X, .y = TEST_OBSTACLE_0_Y },
                                .theta = TEST_OBSTACLE_0_ORIENTATION
                            },
                            .index = 0
                        },
                        .radius = THEORICAL_OBSTACLE_RADIUS
                    },
                    {
                        // Obstacle 1
                        .zone = {
                            .pose = {
                                .coordinates = { .x = TEST_OBSTACLE_1_X, .y = TEST_OBSTACLE_1_Y },
                                .theta = TEST_OBSTACLE_1_ORIENTATION
                            },
                            .index = 1
                        },
                        .radius = THEORICAL_OBSTACLE_RADIUS
                    },
                    {
                        // Obstacle 2
                        .zone = {
                            .pose = {
                                .coordinates = { .x = TEST_OBSTACLE_2_X, .y = TEST_OBSTACLE_2_Y },
                                .theta = TEST_OBSTACLE_2_ORIENTATION
                            },
                            .index = 2
                        },
                        .radius = THEORICAL_OBSTACLE_RADIUS
                    },
                },
                .paintingZone = {
                    {
                        // Painting zone 0
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_0_X, .y = TEST_PAINTING_0_Y },
                            .theta = TEST_PAINTING_0_ORIENTATION
                        },
                        .index = 0
                    },
                    {
                        // Painting zone 1
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_1_X, .y = TEST_PAINTING_1_Y },
                            .theta = TEST_PAINTING_1_ORIENTATION
                        },
                        .index = 1
                    },
                    {
                        // Painting zone 2
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_2_X, .y = TEST_PAINTING_2_Y },
                            .theta = TEST_PAINTING_2_ORIENTATION
                        },
                        .index = 2
                    },
                    {
                        // Painting zone 3
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_3_X, .y = TEST_PAINTING_3_Y },
                            .theta = TEST_PAINTING_3_ORIENTATION
                        },
                        .index = 3
                    },
                    {
                        // Painting zone 4
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_4_X, .y = TEST_PAINTING_4_Y },
                            .theta = TEST_PAINTING_4_ORIENTATION
                        },
                        .index = 0
                    },
                    {
                        // Painting zone 5
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_5_X, .y = TEST_PAINTING_5_Y },
                            .theta = TEST_PAINTING_5_ORIENTATION
                        },
                        .index = 5
                    },
                    {
                        // Painting zone 6
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_6_X, .y = TEST_PAINTING_6_Y },
                            .theta = TEST_PAINTING_6_ORIENTATION
                        },
                        .index = 6
                    },
                    {
                        // Painting zone 7
                        .pose = {
                            .coordinates = {.x = TEST_PAINTING_7_X, .y = TEST_PAINTING_7_Y },
                            .theta = TEST_PAINTING_7_ORIENTATION
                        },
                        .index = 7
                    }
                }
            },
            .environmentHasChanged = 1,
            .robot = {
                .zone = {
                    .pose = {
                        .coordinates = { .x = RECEIVER_TEST_ROBOT_X, .y = RECEIVER_TEST_ROBOT_Y },
                        .theta = RECEIVER_TEST_ROBOT_THETA
                    },
                    .index = 0
                },
                .radius = RECEIVER_TEST_ROBOT_RADIUS
            }
        }
    };
    mesurementsMock = mesurements;
}

struct Mesurements testCallback(void)
{
    return mesurementsMock;
}

Test(RobotReceiver, given_aCallbackFunction_when_askedFetchInputs_then_itIsCalled
     , .init = setupRobotReceiver)
{
    struct Mesurements returnedMesure = RobotReceiver_fetchInputs(&testCallback);
    int returnedObstacleZeroOrientation = returnedMesure.world.environment.obstacles[0].zone.pose.theta;
    cr_assert_eq(TEST_OBSTACLE_0_ORIENTATION, returnedObstacleZeroOrientation,
                 "Expected %d, Got %d", TEST_OBSTACLE_0_ORIENTATION, returnedObstacleZeroOrientation);
}

/*
Test(RobotReceiver, given_aNewWorldViewAndCorrespondingData_when_updatesWorldView_then_theWorldIsUpdated)
{
struct Map *map;
map = Map_new();
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
Map_delete(map);
}
*/
