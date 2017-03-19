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

static void sendTranslate(int x, int y)
{
    struct Command_Translate command = { .x = x, .y = y };
    RobotServer_sendTranslateCommand(command);
    waitASecondAndAHalf();
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

    /*
    sendTranslate(500, 675);
    sendTranslate(-500, 675);
    sendTranslate( 800, -200);
    sendTranslate( 480, 580);
    sendTranslate( 0, -820);
    sendTranslate( 820, -235);
    sendTranslate( -820, -275);
    sendTranslate( 0, -800);
    sendTranslate( -480,600);
    sendTranslate( -800, -200);

    RobotServer_sendRisePenCommand();
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
