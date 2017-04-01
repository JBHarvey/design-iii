#include "station_client_sender.h"
#include "station_client.h"
#include "station_interface.h"
#include "Defines.h"

#define PERFECT_DISTANCE_FROM_PAINTING 4000

const int TEST_PAINTING_0_X = 4060;
const int TEST_PAINTING_0_Y = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_0_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_1_X = 1620;
const int TEST_PAINTING_1_Y = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_1_ORIENTATION = MINUS_HALF_PI;
const int TEST_PAINTING_2_X = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_2_Y = 1710;
const int TEST_PAINTING_2_ORIENTATION = PI;
const int TEST_PAINTING_3_X = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_3_Y = 4240;
const int TEST_PAINTING_3_ORIENTATION = PI;
const int TEST_PAINTING_4_X = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_4_Y = 6770;
const int TEST_PAINTING_4_ORIENTATION = PI;
const int TEST_PAINTING_5_X = PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_5_Y = 9300;
const int TEST_PAINTING_5_ORIENTATION = PI;
const int TEST_PAINTING_6_X = 1620;
const int TEST_PAINTING_6_Y = THEORICAL_WORLD_HEIGHT - PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_6_ORIENTATION = HALF_PI;
const int TEST_PAINTING_7_X = 4060;
const int TEST_PAINTING_7_Y = THEORICAL_WORLD_HEIGHT - PERFECT_DISTANCE_FROM_PAINTING;
const int TEST_PAINTING_7_ORIENTATION = HALF_PI;

GMutex add_packet_mutex;
GMutex communicate_mutex;

static int force_environment_has_changed = 1;

static void addPacketToQueue(uint8_t *data, uint32_t length)
{
    if(StationInterface_isConnectedToRobot) {
        g_mutex_lock(&add_packet_mutex);
        addPacket(data, length);
        g_mutex_unlock(&add_packet_mutex);
    }
}

void StationClientSender_sendStartCycleCommand(void)
{
    uint8_t data[1];
    data[0] = COMMAND_START_CYCLE;
    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendReceiveData(struct StationClient *station_client)
{
    g_mutex_lock(&communicate_mutex);
    StationClient_communicate(station_client);
    g_mutex_unlock(&communicate_mutex);
}

void StationClientSender_sendWorldInformationsToRobot(struct Communication_Object *obstacles,
        unsigned int num_obstacles, struct Communication_Object robot, int environment_has_changed)
{
    uint8_t data[1 + sizeof(struct Communication_World)];
    data[0] = DATA_WORLD;

    struct Communication_World communication_world = {
        .environment = {
            .north_eastern_table_corner = {.x = THEORICAL_WORLD_LENGTH, .y = THEORICAL_WORLD_HEIGHT},
            .north_western_table_corner = {.x = 0, .y = THEORICAL_WORLD_HEIGHT},
            .south_eastern_table_corner = {.x = THEORICAL_WORLD_LENGTH, .y = 0},
            .south_western_table_corner = {.x = 0, .y = 0},
            .north_eastern_drawing_corner = {
                .x = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
                .y = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE
            },
            .north_western_drawing_corner = {
                .x = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X - THEORICAL_DRAWING_ZONE_SIDE,
                .y = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE
            },
            .south_eastern_drawing_corner = {
                .x = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
                .y = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y
            },
            .south_western_drawing_corner = {
                .x = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X - THEORICAL_DRAWING_ZONE_SIDE,
                .y = THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y
            },
            .antenna_zone_start = {.x = THEORICAL_ANTENNA_ZONE_START_X, .y = THEORICAL_ANTENNA_ZONE_Y},
            .antenna_zone_stop = {.x = THEORICAL_ANTENNA_ZONE_STOP_X, .y = THEORICAL_ANTENNA_ZONE_Y},
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
    };

    if(force_environment_has_changed) {
        communication_world.environment_has_changed = 1;
    } else {
        communication_world.environment_has_changed = environment_has_changed;
    }

    communication_world.robot = robot;
    memcpy(communication_world.environment.obstacles, obstacles, num_obstacles * sizeof(struct Communication_Object));
    memcpy(data + 1, &communication_world, sizeof(struct Communication_World));

    Logger_startMessageSectionAndAppend("World send!");
    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendImageReceivedAck(void)
{
    uint8_t data[1];
    data[0] = ACK_IMAGE_RECEIVED;

    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendPlannedTrajectoryReceivedAck(void)
{
    uint8_t data[1];
    data[0] = ACK_PLANNED_TRAJECTORY_RECEIVED;

    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendReadyToStartSignalReceivedAck(void)
{
    uint8_t data[1];
    data[0] = ACK_READY_TO_START_RECEIVED;

    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendReadyToDrawSignalReceivedAck(void)
{
    uint8_t data[1];
    data[0] = ACK_READY_TO_DRAW_RECEIVED;

    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_sendEndOfCycleSignalReceivedAck(void)
{
    uint8_t data[1];
    data[0] = ACK_END_OF_CYCLE_RECEIVED;

    addPacketToQueue(data, sizeof(data));
}

void StationClientSender_removeForceEnvironmentHasChanged(void)
{
    force_environment_has_changed = 0;
}
