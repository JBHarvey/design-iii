#include <stdio.h>
#include <unistd.h>
#include "Logger.h"
#include "OnboardCamera.h"

static struct Robot *robot;
struct Logger *logger;
struct RobotServer *robot_server;
struct CommandSender *command_sender;
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

static void waitFifteenSeconds()
{
    usleep(15000000);
}

static void sendTranslate(int x, int y)
{
    struct Command_Translate translate = { .x = x, .y = y };
    CommandSender_sendTranslateCommand(command_sender, translate);
    waitASecondAndAHalf();
}

static void sendRotate(int theta)
{
    struct Command_Rotate rotate = { .theta = theta};
    CommandSender_sendRotateCommand(command_sender, rotate);
    waitASecondAndAHalf();
}

int main(int argc, char *argv[])
{

    robot = Robot_new();
    robot_server = RobotServer_new(robot, port, ttyACM);

    logger = Logger_new();

    struct CommandSender *command_sender = CommandSender_new();

    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();
    struct CommandSender_Callbacks command_sender_callbacks = CommandSender_fetchCallbacksForRobot();

    data_receiver_callbacks = Logger_startLoggingDataReceiverAndReturnCallbacks(logger, data_receiver_callbacks);
    command_sender_callbacks = Logger_startLoggingCommandSenderAndReturnCallbacks(logger, command_sender_callbacks);

    CommandSender_changeTarget(command_sender, command_sender_callbacks);
    RobotServer_updateDataReceiverCallbacks(data_receiver_callbacks);

    waitFifteenSeconds();
    waitFifteenSeconds();

    for(int i = 0; i < 15; ++i) {
        CommandSender_sendLightRedLEDCommand(command_sender);
        waitASecond();
        CommandSender_sendLightGreenLEDCommand(command_sender);
        waitASecond();
    }

    // MANCHESTER ASK + LOG RETURN TEST
    CommandSender_sendFetchManchesterCode(command_sender);

    // TEST OF CAMERA AND PATH
    // Initialise the camera
    OnboardCamera_init();

    IplImage *test_image;
    struct CoordinatesSequence *image_trajectory;

    image_trajectory = OnboardCamera_extractTrajectoryFromImage(&test_image);

    struct ManchesterCode *code = ManchesterCode_new();
    ManchesterCode_updateCodeValues(code, 0, TIMES_TWO, WEST);

    RobotServer_sendImageToStation(test_image);
    RobotServer_sendPlannedTrajectoryToStation(image_trajectory);


    /*
    // HOUSE TEST
    RobotServer_sendLowerPenCommand();
    waitASecondAndAHalf();
    sendTranslate(1000, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(1000, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(300, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(300, 0);
    sendRotate(HALF_PI);
    sendTranslate(400, 0);
    sendRotate(HALF_PI);
    sendTranslate(300, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(300, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(1000, 0);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(500, 500);
    sendTranslate(500, -500);

    RobotServer_sendRisePenCommand();
    */
    // STAR TEST
    /*
    RobotServer_sendLowerPenCommand();
    waitASecondAndAHalf();

    sendTranslate(660, -460);
    sendTranslate(-240, 770);
    sendTranslate(660, 500);
    sendTranslate(-830, 0);
    sendTranslate(-250, 760);
    sendTranslate(-250, -760);
    sendTranslate(-830, 0);
    sendTranslate(660, -500);
    sendTranslate(-240, -770);
    sendTranslate(660, 460);

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

    // Releases Camera
    OnboardCamera_deleteImageAndFreeCamera(&test_image);

    CommandSender_delete(command_sender);
    Logger_delete(logger);
    RobotServer_delete(robot_server);
    Robot_delete(robot);

    return 0;
}
