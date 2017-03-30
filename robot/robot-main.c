#include <stdio.h>
#include <unistd.h>
#include "Logger.h"
#include "PoseFilter.h"


struct PoseFilter *pose_filter;
struct Robot *robot;
struct RobotServer *robot_server;
int main(int argc, char *argv[])
{
    const int port = 35794;
    //char *ttyACM = "/dev/null";
    char *ttyACM = "/dev/ttyACM0";

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();

    robot = Robot_new();
    pose_filter = PoseFilter_new(robot);
    robot_server = RobotServer_new(robot, port, ttyACM);

    Logger_startLoggingRobot(robot);

    while(1) {
        RobotServer_communicate(robot_server);
        Robot_updateBehaviorIfNeeded(robot);
        Robot_act(robot);
        PoseFilter_executeFilter(pose_filter, callbacks.updateFromCameraOnly);
        //Robot_sendPoseEstimate(robot);
    }


    RobotServer_delete(robot_server);
    Robot_delete(robot);
    PoseFilter_delete(pose_filter);

    return 0;

    /*
    for(int i = 0; i < 15; ++i) {
        CommandSender_sendLightRedLEDCommand(command_sender);
        waitASecond();
        CommandSender_sendLightGreenLEDCommand(command_sender);
        waitASecond();
    }


    sendTranslate(0, 2000);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(0, 2000);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(0, 2000);
    sendRotate(MINUS_HALF_PI);
    sendTranslate(0, 2000);
    sendRotate(MINUS_HALF_PI);
    // MANCHESTER ASK + LOG RETURN TEST
    CommandSender_sendFetchManchesterCode(command_sender);

    // TEST OF CAMERA AND PATH
    // Initialise the camera
    // The camera will have to be initialized and freed in the main
    OnboardCamera_init();

    IplImage *test_image;
    struct CoordinatesSequence *image_trajectory;

    image_trajectory = OnboardCamera_extractTrajectoryFromImage(&test_image);

    struct ManchesterCode *code = ManchesterCode_new();
    ManchesterCode_updateCodeValues(code, 0, TIMES_TWO, WEST);

    RobotServer_sendImageToStation(test_image);
    RobotServer_sendPlannedTrajectoryToStation(image_trajectory);
    // After communication:
    // Releases Camera
    OnboardCamera_deleteImage(&test_image);
    OnboardCamera_freeCamera();
    */


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

}
