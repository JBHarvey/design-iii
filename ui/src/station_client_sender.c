#include "station_client_sender.h"
#include "station_client.h"
#include "station_interface.h"

extern enum ThreadStatus main_loop_status;

// Fake data
#define ANGLE_BASE_UNIT 0.00001
#define QUARTER_PI 78540
#define HALF_PI 157080
#define THREE_QUARTER_PI 235620
#define PI 314160
#define MINUS_PI -314160
#define MINUS_THREE_QUARTER_PI -235620
#define MINUS_HALF_PI -157080
#define MINUS_QUARTER_PI -78540
#define THEORICAL_OBSTACLE_RADIUS 625
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

static void cleanExitIfMainLoopTerminated(void)
{
    if(main_loop_status == TERMINATED) {
        g_thread_exit((gpointer) TRUE);
    }
}

void StationClientSender_startSendingWorldInformationsToRobot(struct StationClient *station_client)
{
    while(1) {

        cleanExitIfMainLoopTerminated();

        uint8_t data[1 + sizeof(struct Communication_World)];
        data[0] = DATA_WORLD;

        struct Communication_World communication_world = {
            .environment = {
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
        };

        memcpy(data + 1, &communication_world, sizeof(struct Communication_World));
        addPacket(data, sizeof(data));

        StationClient_communicate(station_client, 1);
    }
}

