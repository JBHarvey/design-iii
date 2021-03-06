#include <stdio.h>
#include <unistd.h>
#include "Logger.h"
#include "PoseFilter.h"
#include "Timer.h"

struct PoseFilter *pose_filter;
struct Robot *robot;
struct RobotServer *robot_server;
struct Timer *timer;

int main(int argc, char *argv[])
{
    const int port = 35794;
    //char *ttyACM = "/dev/null";
    char *ttyACM = "/dev/ttyACM0";

    struct PoseFilter_Callbacks filters = PoseFilter_fetchCallbacks();

    OnboardCamera_init();
    robot = Robot_new();
    pose_filter = PoseFilter_new(robot);
    robot_server = RobotServer_new(robot, port, ttyACM);

    timer = Timer_new();

    Logger_startLoggingRobot(robot);

    while(1) {

        RobotServer_communicate(robot_server);

        if(pose_filter->robot->current_state->flags->navigable_map_is_ready) {
            PoseFilter_executeFilter(pose_filter, filters.particlesFilterUsingWorldCameraAndWheels);
            Robot_resetAllActuators(robot);
        }

        Robot_updateBehaviorIfNeeded(robot);
        Robot_act(robot);

        if(Timer_hasTimePassed(timer, ONE_TENTH_OF_A_SECOND)) {
            Robot_sendPoseEstimate(robot);
            Timer_reset(timer);
        }

        //ask station to send : [stop signal] on exit

        /*

        initializeRobot();

        executeRobot();

        freeRobot();
        */
    }

    Timer_delete(timer);
    RobotServer_delete(robot_server);
    Robot_delete(robot);
    PoseFilter_delete(pose_filter);
    OnboardCamera_freeCamera();


    return 0;
}
/*

void initializeRobot(void)
{
    const int port = 35794;
    //char *ttyACM = "/dev/null";
    char *ttyACM = "/dev/ttyACM0";

    filters = PoseFilter_fetchCallbacks();

    OnboardCamera_init();
    robot = Robot_new();
    pose_filter = PoseFilter_new(robot);
    robot_server = RobotServer_new(robot, port, ttyACM);

    timer = Timer_new();

    Logger_startLoggingRobot(robot);

    while(!Timer_hasTimePassed(timer, THREE_SECONDS));
}

void executeRobot(void)
{
    while(!robot->current_state->flags->stop_execution_signal_received)  {
        RobotServer_communicate(robot_server);

        if(pose_filter->robot->current_state->flags->navigable_map_is_ready) {
            PoseFilter_executeFilter(pose_filter, filters.particlesFilterUsingWorldCameraAndWheels);
            Robot_resetAllActuators(robot);
        }

        Robot_updateBehaviorIfNeeded(robot);
        Robot_act(robot);

        if(Timer_hasTimePassed(timer, ONE_TENTH_OF_A_SECOND)) {
            Robot_sendPoseEstimate(robot);
            Timer_reset(timer);
        }
    }
}

void freeRobot(void)
{
    Timer_delete(timer);
    RobotServer_delete(robot_server);
    Robot_delete(robot);
    PoseFilter_delete(pose_filter);
    OnboardCamera_freeCamera();
}
*/
