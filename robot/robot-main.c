#include <stdio.h>
#include <unistd.h>
#include "Logger.h"

static struct Robot *robot;
struct Logger *logger;
struct RobotServer *robot_server;
const int port = 35794;
//char *ttyACM = "/dev/null";
char *ttyACM = "/dev/ttyACM0";

static void waitASecond()
{
    usleep(1000000);
}

static void waitASecondAndAHalf()
{
    usleep(1500000);
}

int main(int argc, char *argv[])
{
    robot = Robot_new();
    robot_server = RobotServer_new(robot, port, ttyACM);

    logger = Logger_new();

    struct DataReceiver_Callbacks test_callbacks = DataReceiver_fetchCallbacks();
    test_callbacks = Logger_startLoggingDataReceiverAndReturnCallbacks(logger, test_callbacks);
    RobotServer_updateDataReceiverCallbacks(test_callbacks);


    RobotServer_sendLowerPenCommand();
    waitASecondAndAHalf();

    //First Triangle
    struct Command_Translate command = { .x = 1000, .y = 0 };
    RobotServer_sendTranslateCommand(command);
    waitASecondAndAHalf();
    struct Command_Translate command1 = { .x = 0, .y = -1500 };
    RobotServer_sendTranslateCommand(command1);
    waitASecondAndAHalf();
    struct Command_Translate command2 = { .x = -1000, .y = 1500 };
    RobotServer_sendTranslateCommand(command2);
    waitASecondAndAHalf();

    struct Command_Translate command3 = { .x = -1000, .y = 0 };
    RobotServer_sendTranslateCommand(command3);
    waitASecondAndAHalf();
    struct Command_Translate command4 = { .x = 0, .y =  1500 };
    RobotServer_sendTranslateCommand(command4);
    waitASecondAndAHalf();
    struct Command_Translate command5 = { .x =  1000, .y = -1500 };
    RobotServer_sendTranslateCommand(command5);
    waitASecondAndAHalf();

    RobotServer_sendRisePenCommand();
    /*
    */
    /*
    struct Communication_Rotation rotation = { .theta = 855, .gamma = 5};
    struct Communication_Translation translation = {
        .movement = { .x = 1531, .y = 13513},
        .speeds = { .x = 55, .y = 42}
    };
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
