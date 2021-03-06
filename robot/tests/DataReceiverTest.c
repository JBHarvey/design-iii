#include <criterion/criterion.h>
#include <stdio.h>
#include "DataReceiver.h"
#include "Robot.h"

struct Mesurements mesurements_mock;
struct Flags *flags = NULL;
struct Robot *robot = NULL;

Test(DataReceiver, given_when_fetchDataReceiverCallbacks_then_theCorrectSturctureIsReturned)
{
    void (*updateWorld)(struct WorldCamera *, struct Communication_World) = &DataReceiver_updateWorld;
    void (*updateWheelsTranslation)(struct Wheels *,
                                    struct Communication_Translation) = &DataReceiver_updateWheelsTranslation;
    void (*updateWheelsRotation)(struct Wheels *, struct Communication_Rotation) = &DataReceiver_updateWheelsRotation;
    void (*updateManchesterCode)(struct ManchesterCode *, struct Flags *,
                                 struct Communication_ManchesterCode) = &DataReceiver_updateManchesterCode;
    void (*updateFlagsStartCycle)(struct Flags *) = &DataReceiver_updateFlagsStartCycle;
    void (*updateFlagsImageReceivedByStation)(struct Flags *) = &DataReceiver_updateFlagsImageReceivedByStation;
    void (*updateFlagsPlannedTrajectoryReceivedByStation)(struct Flags *) =
        &DataReceiver_updateFlagsPlannedTrajectoryReceivedByStation;
    void (*updateFlagsReadyToStartSignalReceivedByStation)(struct Flags *) =
        &DataReceiver_updateFlagsReadyToStartSignalReceivedByStation;
    void (*updateFlagsReadyToDrawSignalReceivedByStation)(struct Flags *) =
        &DataReceiver_updateFlagsReadyToDrawSignalReceivedByStation;
    void (*updateFlagsEndOfCycleSignalReceivedByStation)(struct Flags *) =
        &DataReceiver_updateFlagsEndOfCycleSignalReceivedByStation;
    void (*updateFlagsManchesterCodeReceived)(struct Flags *) =
        &DataReceiver_updateFlagsManchesterCodeReceived;

    struct DataReceiver_Callbacks callbacks = DataReceiver_fetchCallbacks();
    cr_assert_eq(callbacks.updateWorld, updateWorld);
    cr_assert_eq(callbacks.updateWheelsTranslation, updateWheelsTranslation);
    cr_assert_eq(callbacks.updateWheelsRotation, updateWheelsRotation);
    cr_assert_eq(callbacks.updateManchesterCode, updateManchesterCode);
    cr_assert_eq(callbacks.updateFlagsStartCycle, updateFlagsStartCycle);
    cr_assert_eq(callbacks.updateFlagsImageReceivedByStation, updateFlagsImageReceivedByStation);
    cr_assert_eq(callbacks.updateFlagsPlannedTrajectoryReceivedByStation, updateFlagsPlannedTrajectoryReceivedByStation);
    cr_assert_eq(callbacks.updateFlagsReadyToStartSignalReceivedByStation, updateFlagsReadyToStartSignalReceivedByStation);
    cr_assert_eq(callbacks.updateFlagsReadyToDrawSignalReceivedByStation, updateFlagsReadyToDrawSignalReceivedByStation);
    cr_assert_eq(callbacks.updateFlagsEndOfCycleSignalReceivedByStation, updateFlagsEndOfCycleSignalReceivedByStation);
    cr_assert_eq(callbacks.updateFlagsManchesterCodeReceived, updateFlagsManchesterCodeReceived);
}

void setup_Flags(void)
{
    flags = Flags_new();
}

void teardown_Flags(void)
{
    Flags_delete(flags);
}

Test(DataReceiver, given_aCommandStartCyclePacket_when_updateFlagsStartCycle_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsStartCycle(flags);

    cr_assert(flags->start_cycle_signal_received);
}

Test(DataReceiver,
     given_anAcknowledgementOfImageReceivedByStation_when_updateFlagsImageReceivedByStation_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsImageReceivedByStation(flags);

    cr_assert(flags->image_received_by_station);
}

Test(DataReceiver,
     given_anAcknowledgementOfPlannedTrajectoryReceivedByStation_when_updateFlagsPlannedTrajectoryReceivedByStation_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsPlannedTrajectoryReceivedByStation(flags);

    cr_assert(flags->planned_trajectory_received_by_station);
}

Test(DataReceiver,
     given_anAcknowledgementOfReadyToStartSignalReceivedByStation_when_updateFlagsReadyToStartSignalReceivedByStation_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsReadyToStartSignalReceivedByStation(flags);

    cr_assert(flags->ready_to_start_received_by_station);
}

Test(DataReceiver,
     given_anAcknowledgementOfReadyToDrawSignalReceivedByStation_when_updateFlagsReadyToDrawSignalReceivedByStation_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsReadyToDrawSignalReceivedByStation(flags);

    cr_assert(flags->ready_to_draw_received_by_station);
}

Test(DataReceiver,
     given_anAcknowledgementOfEndOfCycleSignalReceivedByStation_when_updateFlagsEndOfCycleSignalReceivedByStation_then_itsValueIsSetToOne,
     .init = setup_Flags,
     .fini = teardown_Flags)
{
    DataReceiver_updateFlagsEndOfCycleSignalReceivedByStation(flags);

    cr_assert(flags->end_of_cycle_received_by_station);
}

// World data
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
const int TEST_PAINTING_0_ORIENTATION = 0;
const int TEST_PAINTING_1_X = 1620;
const int TEST_PAINTING_1_Y = 2500;
const int TEST_PAINTING_1_ORIENTATION = 0;
const int TEST_PAINTING_2_X = 2500;
const int TEST_PAINTING_2_Y = 1710;
const int TEST_PAINTING_2_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_3_X = 2500;
const int TEST_PAINTING_3_Y = 4240;
const int TEST_PAINTING_3_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_4_X = 2500;
const int TEST_PAINTING_4_Y = 6770;
const int TEST_PAINTING_4_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_5_X = 2500;
const int TEST_PAINTING_5_Y = 9300;
const int TEST_PAINTING_5_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_6_X = 1620;
const int TEST_PAINTING_6_Y = 8500;
const int TEST_PAINTING_6_ORIENTATION = PI;
const int TEST_PAINTING_7_X = 4060;
const int TEST_PAINTING_7_Y = 8500;
const int TEST_PAINTING_7_ORIENTATION = PI;

// Wheels data
const int RECEIVER_TRANSLATION_X = 40;
const int RECEIVER_TRANSLATION_X_SPEED = 400;
const int RECEIVER_TRANSLATION_Y = -10;
const int RECEIVER_TRANSLATION_Y_SPEED = -100;
const int RECEIVER_ROTATION = 31416;
const int RECEIVER_ROTATION_SPEED = 314160;

void setup_DataReceiver(void)
{
    struct Mesurements mesurements = {
        .world = {
            .environment = { // WARNING these values are repeated as they are here in other tests in this file
                .north_eastern_table_corner = {.x = MAP_TABLE_LENGTH, .y = MAP_TABLE_HEIGHT},
                .north_western_table_corner = {.x = 0, .y = MAP_TABLE_HEIGHT},
                .south_eastern_table_corner = {.x = MAP_TABLE_LENGTH, .y = 0},
                .south_western_table_corner = {.x = 0, .y = 0},
                .north_eastern_drawing_corner = {.x = MAP_DRAWING_SIDE, .y = MAP_DRAWING_SIDE},
                .north_western_drawing_corner = {.x = 0, .y = MAP_DRAWING_SIDE},
                .south_eastern_drawing_corner = {.x = MAP_DRAWING_SIDE, .y = 0},
                .south_western_drawing_corner = {.x = 0, .y = 0},
                .antenna_zone_start = {.x = ANTENNA_ZONE_X + ANTENNA_ZONE_LENGTH, .y = ANTENNA_ZONE_Y},
                .antenna_zone_stop = {.x = ANTENNA_ZONE_X, .y = ANTENNA_ZONE_Y},
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
                .painting_zone = {
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
            .environment_has_changed = 1,
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
    mesurements_mock = mesurements;
}

struct Mesurements test_callback(void)
{
    return mesurements_mock;
}

Test(DataReceiver, given_aCallbackFunction_when_askedFetchInputs_then_itIsCalled
     , .init = setup_DataReceiver)
{
    struct Mesurements returned_mesure = DataReceiver_fetchInputs(&test_callback);
    int returned_obstacle_zero_orientation = returned_mesure.world.environment.obstacles[0].zone.pose.theta;
    cr_assert_eq(TEST_OBSTACLE_0_ORIENTATION, returned_obstacle_zero_orientation,
                 "Expected %d, Got %d", TEST_OBSTACLE_0_ORIENTATION, returned_obstacle_zero_orientation);
}

Test(DataReceiver, given_aPacket_when_updatesWorld_then_theRobotSensorHasNewData
     , .init = setup_DataReceiver)
{
    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    cr_assert(world_camera->robot_sensor->has_received_new_data);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver, given_aPacket_when_updatesWorld_then_theRobotInformationInWorldCameraAreUpdated
     , .init = setup_DataReceiver)
{
    struct Pose *pose = Pose_new(RECEIVER_TEST_ROBOT_X, RECEIVER_TEST_ROBOT_Y, RECEIVER_TEST_ROBOT_THETA);

    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    cr_assert(Pose_haveTheSameValues(world_camera->robot_pose, pose));
    cr_assert(world_camera->robot_radius == RECEIVER_TEST_ROBOT_RADIUS);
    WorldCamera_delete(world_camera);
    Pose_delete(pose);
}

Test(DataReceiver, given_environmentChanges_when_updatesWorld_then_theMapSenorHasNewData
     , .init = setup_DataReceiver)
{
    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    cr_assert(world_camera->map_sensor->has_received_new_data);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver, given_noEnvironmentChanges_when_updatesWorld_then_theMapSensorHasNoNewData
     , .init = setup_DataReceiver)
{
    struct WorldCamera *world_camera = WorldCamera_new();
    mesurements_mock.world.environment_has_changed = 0;
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    cr_assert(world_camera->map_sensor->has_received_new_data == 0);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver, given_noEnvironmentChanges_when_updatesWorld_then_onlyTheWorldCameraMapIsntChanged
     , .init = setup_DataReceiver)
{
    struct WorldCamera *world_camera = WorldCamera_new();
    mesurements_mock.world.environment_has_changed = 0;
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    cr_assert(world_camera->map_sensor->has_received_new_data == 0);

    struct Map *map = world_camera->map;

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
    WorldCamera_delete(world_camera);
}


Test(DataReceiver, given_aPredeterminedMesurementPacket_when_updatesMesurements_then_theTableCoordinatesAreChanged
     , .init = setup_DataReceiver)
{
    struct Coordinates *north_eastern_table_corner = Coordinates_new(MAP_TABLE_LENGTH, MAP_TABLE_HEIGHT);
    struct Coordinates *south_eastern_table_corner = Coordinates_new(MAP_TABLE_LENGTH, 0);
    struct Coordinates *south_western_table_corner = Coordinates_new(0, 0);
    struct Coordinates *north_western_table_corner = Coordinates_new(0, MAP_TABLE_HEIGHT);

    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    struct Map *map = world_camera->map;

    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_table_corner, north_eastern_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_table_corner, north_western_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_table_corner, south_eastern_table_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_table_corner, south_western_table_corner));

    Coordinates_delete(north_eastern_table_corner);
    Coordinates_delete(south_eastern_table_corner);
    Coordinates_delete(south_western_table_corner);
    Coordinates_delete(north_western_table_corner);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver,
     given_aPredeterminedMesurementPacket_when_updatesMesurements_then_theDrawingZoneCoordinatesAreChanged
     , .init = setup_DataReceiver)
{
    struct Coordinates *north_eastern_drawing_corner = Coordinates_new(MAP_DRAWING_SIDE, MAP_DRAWING_SIDE);
    struct Coordinates *south_eastern_drawing_corner = Coordinates_new(MAP_DRAWING_SIDE, 0);
    struct Coordinates *south_western_drawing_corner = Coordinates_new(0, 0);
    struct Coordinates *north_western_drawing_corner = Coordinates_new(0, MAP_DRAWING_SIDE);

    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    struct Map *map = world_camera->map;

    cr_assert(Coordinates_haveTheSameValues(map->north_eastern_drawing_corner, north_eastern_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->north_western_drawing_corner, north_western_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_eastern_drawing_corner, south_eastern_drawing_corner));
    cr_assert(Coordinates_haveTheSameValues(map->south_western_drawing_corner, south_western_drawing_corner));

    Coordinates_delete(north_eastern_drawing_corner);
    Coordinates_delete(south_eastern_drawing_corner);
    Coordinates_delete(south_western_drawing_corner);
    Coordinates_delete(north_western_drawing_corner);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver,
     given_aPredeterminedMesurementPacket_when_updatesMesurements_then_theAntennaZoneCoordinatesAreChanged
     , .init = setup_DataReceiver)
{
    struct Coordinates * antenna_zone_start = Coordinates_new(ANTENNA_ZONE_X + ANTENNA_ZONE_LENGTH, ANTENNA_ZONE_Y);
    struct Coordinates *antenna_zone_stop = Coordinates_new(ANTENNA_ZONE_X, ANTENNA_ZONE_Y);
    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    struct Map *map = world_camera->map;

    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_start, antenna_zone_start));
    cr_assert(Coordinates_haveTheSameValues(map->antenna_zone_stop, antenna_zone_stop));

    Coordinates_delete(antenna_zone_start);
    Coordinates_delete(antenna_zone_stop);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver,
     given_aPredeterminedMesurementPacket_when_updatesMesurements_then_theObstaclesAreChanged
     , .init = setup_DataReceiver)
{
    struct Coordinates *coordinates0 = Coordinates_new(TEST_OBSTACLE_0_X, TEST_OBSTACLE_0_Y);
    struct Coordinates *coordinates1 = Coordinates_new(TEST_OBSTACLE_1_X, TEST_OBSTACLE_1_Y);
    struct Coordinates *coordinates2 = Coordinates_new(TEST_OBSTACLE_2_X, TEST_OBSTACLE_2_Y);

    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    struct Map *map = world_camera->map;

    cr_assert(Coordinates_haveTheSameValues(map->obstacles[0]->coordinates, coordinates0));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[1]->coordinates, coordinates1));
    cr_assert(Coordinates_haveTheSameValues(map->obstacles[2]->coordinates, coordinates2));
    cr_assert_eq(map->obstacles[0]->orientation, NORTH);
    cr_assert_eq(map->obstacles[1]->orientation, SOUTH);
    cr_assert_eq(map->obstacles[2]->orientation, CENTER);

    Coordinates_delete(coordinates0);
    Coordinates_delete(coordinates1);
    Coordinates_delete(coordinates2);
    WorldCamera_delete(world_camera);
}

Test(DataReceiver,
     given_aPredeterminedMesurementPacket_when_updatesMesurements_then_thePaintingZonesAreChanged
     , .init = setup_DataReceiver)
{
    struct Pose *paintingZone0 = Pose_new(TEST_PAINTING_0_X, TEST_PAINTING_0_Y, TEST_PAINTING_0_ORIENTATION);
    struct Pose *paintingZone1 = Pose_new(TEST_PAINTING_1_X, TEST_PAINTING_1_Y, TEST_PAINTING_1_ORIENTATION);
    struct Pose *paintingZone2 = Pose_new(TEST_PAINTING_2_X, TEST_PAINTING_2_Y, TEST_PAINTING_2_ORIENTATION);
    struct Pose *paintingZone3 = Pose_new(TEST_PAINTING_3_X, TEST_PAINTING_3_Y, TEST_PAINTING_3_ORIENTATION);
    struct Pose *paintingZone4 = Pose_new(TEST_PAINTING_4_X, TEST_PAINTING_4_Y, TEST_PAINTING_4_ORIENTATION);
    struct Pose *paintingZone5 = Pose_new(TEST_PAINTING_5_X, TEST_PAINTING_5_Y, TEST_PAINTING_5_ORIENTATION);
    struct Pose *paintingZone6 = Pose_new(TEST_PAINTING_6_X, TEST_PAINTING_6_Y, TEST_PAINTING_6_ORIENTATION);
    struct Pose *paintingZone7 = Pose_new(TEST_PAINTING_7_X, TEST_PAINTING_7_Y, TEST_PAINTING_7_ORIENTATION);

    struct WorldCamera *world_camera = WorldCamera_new();
    DataReceiver_updateWorld(world_camera, mesurements_mock.world);

    struct Map *map = world_camera->map;

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
    WorldCamera_delete(world_camera);
}

Test(DataReceiver, given_aTranslationDataPacket_when_updatesWheels_then_wheelsHaveNewTranslationAndItsValueIsCorrect)
{
    struct Coordinates *translation_coordinates = Coordinates_new(RECEIVER_TRANSLATION_X, RECEIVER_TRANSLATION_Y);
    struct Coordinates *translation_speed = Coordinates_new(RECEIVER_TRANSLATION_X_SPEED, RECEIVER_TRANSLATION_Y_SPEED);
    struct Communication_Translation translation_mock = {
        .movement = {
            .x = RECEIVER_TRANSLATION_X,
            .y = RECEIVER_TRANSLATION_Y
        },
        .speeds = {
            .x = RECEIVER_TRANSLATION_X_SPEED,
            .y = RECEIVER_TRANSLATION_Y_SPEED
        }
    };

    struct Wheels *wheels = Wheels_new();
    DataReceiver_updateWheelsTranslation(wheels, translation_mock);

    cr_assert(wheels->translation_sensor->has_received_new_data);
    cr_assert(Coordinates_haveTheSameValues(translation_coordinates, wheels->translation_data_movement));
    cr_assert(Coordinates_haveTheSameValues(translation_speed, wheels->translation_data_speed));

    Coordinates_delete(translation_coordinates);
    Coordinates_delete(translation_speed);
    Wheels_delete(wheels);
}

Test(DataReceiver, given_aRotationDataPacket_when_updatesWheels_then_wheelsHaveNewRotationAndItsValueIsCorrect)
{
    struct Angle *rotation_movement = Angle_new(RECEIVER_ROTATION);
    struct Angle *rotation_speed = Angle_new(RECEIVER_ROTATION_SPEED);
    struct Communication_Rotation rotation_mock = {
        .theta = RECEIVER_ROTATION,
        .gamma = RECEIVER_ROTATION_SPEED
    };

    struct Wheels *wheels = Wheels_new();
    DataReceiver_updateWheelsRotation(wheels, rotation_mock);

    cr_assert(wheels->rotation_sensor->has_received_new_data);
    cr_assert(Angle_smallestAngleBetween(rotation_movement, wheels->rotation_data_movement) == 0);
    cr_assert(Angle_smallestAngleBetween(rotation_speed, wheels->rotation_data_speed) == 0);

    Angle_delete(rotation_movement);
    Angle_delete(rotation_speed);
    Wheels_delete(wheels);
}

const int RECEIVER_PAINTING_NUMBER = 5;
const int RECEIVER_SCALE_FACTOR = TIMES_FOUR;
const char RECEIVER_MANCHESTER_NORTH = 'N';
const char RECEIVER_MANCHESTER_EAST = 'E';
const char RECEIVER_MANCHESTER_SOUTH = 'S';
const char RECEIVER_MANCHESTER_WEST = 'W';

void setup_Robot(void)
{
    robot = Robot_new();
}

void teardown_Robot(void)
{
    Robot_delete(robot);
}

Test(DataReceiver, given_aManchesterCodePacket_when_updatesManchesterCode_then_theManchesterCodeReceivedFlagIsSetToOne
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_NORTH
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    cr_assert_eq(robot->current_state->flags->manchester_code_received, 1);
}

Test(DataReceiver, given_aManchesterCodePacket_when_updatesManchesterCode_then_thePaintingNumberIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_NORTH
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    cr_assert_eq(robot->manchester_code->painting_number, RECEIVER_PAINTING_NUMBER);
}

Test(DataReceiver, given_aManchesterCodePacket_when_updatesManchesterCode_then_theScaleFactorIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_NORTH
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    cr_assert_eq(robot->manchester_code->scale_factor, RECEIVER_SCALE_FACTOR);
}

Test(DataReceiver,
     given_aManchesterCodePacketWithNorthOrientation_when_updatesManchesterCode_then_theOrientationIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_NORTH
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    enum CardinalDirection expected_orientation = NORTH;
    cr_assert_eq(robot->manchester_code->orientation, expected_orientation);
}

Test(DataReceiver,
     given_aManchesterCodePacketWithEastOrientation_when_updatesManchesterCode_then_theOrientationIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_EAST
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    enum CardinalDirection expected_orientation = EAST;
    cr_assert_eq(robot->manchester_code->orientation, expected_orientation);
}

Test(DataReceiver,
     given_aManchesterCodePacketWithSouthOrientation_when_updatesManchesterCode_then_theOrientationIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_SOUTH
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    enum CardinalDirection expected_orientation = SOUTH;
    cr_assert_eq(robot->manchester_code->orientation, expected_orientation);
}

Test(DataReceiver,
     given_aManchesterCodePacketWithWestOrientation_when_updatesManchesterCode_then_theOrientationIsUpdatedCorrectly
     , .init = setup_Robot
     , .fini = teardown_Robot)
{
    struct Communication_ManchesterCode manchester_code_mock = {
        .painting_number = RECEIVER_PAINTING_NUMBER,
        .scale_factor = RECEIVER_SCALE_FACTOR,
        .orientation = RECEIVER_MANCHESTER_WEST
    };

    DataReceiver_updateManchesterCode(robot->manchester_code, robot->current_state->flags, manchester_code_mock);

    enum CardinalDirection expected_orientation = WEST;
    cr_assert_eq(robot->manchester_code->orientation, expected_orientation);
}
