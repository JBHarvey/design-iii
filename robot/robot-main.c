#include <stdio.h>

#include "Logger.h"

static struct Robot *robot;
struct Logger *logger;
struct RobotServer *robot_server;
const int port = 35794;
//char *ttyACM = "/dev/null";
char *ttyACM = "/dev/ttyACM0";

static void testSendRobotPoseAsWheelcommands(struct WorldCamera *world_camera, struct Communication_World world)
{
    struct Command_Translate command = {
        .x = world.robot.zone.pose.coordinates.x,
        .y = world.robot.zone.pose.coordinates.y
    };
    RobotServer_sendTranslateCommand(command);
}

int main(int argc, char *argv[])
{
    robot = Robot_new();
    robot_server = RobotServer_new(robot, port, ttyACM);

    logger = Logger_new();

    struct DataReceiver_Callbacks test_callbacks = DataReceiver_fetchCallbacks();
    test_callbacks.updateWorld = &testSendRobotPoseAsWheelcommands;
    test_callbacks = Logger_startLoggingDataReceiverAndReturnCallbacks(test_callbacks);
    RobotServer_updateDataReceiverCallbacks(test_callbacks);


    struct Communication_Rotation rotation = { .theta = 855, .gamma = 5};
    struct Communication_Translation translation = {
        .movement = { .x = 1531, .y = 13513},
        .speeds = { .x = 55, .y = 42}
    };

    /*
    (*(test_callbacks.updateFlagsStartCycle))(robot->current_state->flags, 1);
    (*(test_callbacks.updateFlagsStartCycle))(robot->current_state->flags, 0);
    (*(test_callbacks.updateWheelsRotation))(robot->wheels, rotation);
    (*(test_callbacks.updateWheelsTranslation))(robot->wheels, translation);
    */

    while(1) {
        RobotServer_communicate(robot_server);
    }

    test_callbacks = Logger_stopLoggingDataReceiverAndReturnCallbacks(logger);

    Logger_delete(logger);
    RobotServer_delete(robot_server);
    Robot_delete(robot);

    return 0;
}
