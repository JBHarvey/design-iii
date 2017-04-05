#include <stdlib.h>
#include "Robot.h"
#include "Logger.h"
#include "Timer.h"

struct DefaultValues {
    struct Pose *pose;
};

struct Robot *Robot_new(void)
{
    struct DefaultValues *default_values = malloc(sizeof(struct DefaultValues));
    default_values->pose = Pose_zero();

    struct Object *new_object = Object_new();
    struct State *new_state = State_new(default_values->pose);
    struct ManchesterCode *new_manchester_code = ManchesterCode_new();
    struct Wheels *new_wheels = Wheels_new();
    struct WorldCamera *new_world_camera = WorldCamera_new();
    struct Navigator *new_navigator = Navigator_new();
    struct DataSender *new_data_sender = DataSender_new();
    struct CommandSender *new_command_sender = CommandSender_new();
    struct Logger *new_logger = Logger_new();
    struct Timer *new_timer = Timer_new();
    struct Robot *pointer =  malloc(sizeof(struct Robot));

    pointer->object = new_object;
    pointer->default_values = default_values;
    pointer->current_state = new_state;
    pointer->manchester_code = new_manchester_code;
    pointer->wheels = new_wheels;
    pointer->world_camera = new_world_camera;
    pointer->navigator = new_navigator;
    pointer->data_sender = new_data_sender;
    pointer->command_sender = new_command_sender;
    pointer->logger = new_logger;
    pointer->timer = new_timer;

    RobotBehaviors_prepareInitialBehaviors(pointer);
    pointer->first_behavior = pointer->current_behavior;

    return pointer;
}

void Robot_delete(struct Robot *robot)
{
    Object_removeOneReference(robot->object);

    if(Object_canBeDeleted(robot->object)) {
        Object_delete(robot->object);
        State_delete(robot->current_state);
        ManchesterCode_delete(robot->manchester_code);
        Wheels_delete(robot->wheels);
        WorldCamera_delete(robot->world_camera);
        Behavior_delete(robot->first_behavior);
        Navigator_delete(robot->navigator);
        DataSender_delete(robot->data_sender);
        CommandSender_delete(robot->command_sender);
        Logger_delete(robot->logger);
        Timer_delete(robot->timer);

        /* DefaultValues destruction */
        Pose_delete(robot->default_values->pose);
        free(robot->default_values);

        free(robot);
    }
}

void Robot_updateBehaviorIfNeeded(struct Robot *robot)
{
    robot->current_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(robot->current_behavior, robot->current_state);
}

void Robot_act(struct Robot *robot)
{
    Behavior_act(robot->current_behavior, robot);
}

void Robot_sendReadyToStartSignal(struct Robot *robot)
{
    DataSender_sendSignalReadyToStart(robot->data_sender);
}

void Robot_sendReadyToDrawSignal(struct Robot *robot)
{
    DataSender_sendSignalReadyToDraw(robot->data_sender);
}

void Robot_sendPlannedTrajectory(struct Robot *robot)
{
    DataSender_sendPlannedTrajectory(robot->data_sender, robot->navigator->planned_trajectory);
}

void Robot_sendPoseEstimate(struct Robot *robot)
{
    DataSender_sendRobotPoseEstimate(robot->data_sender, robot->current_state->pose);
}

void Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(struct Robot *robot)
{
    if(Timer_hasTimePassed(robot->timer, ONE_SECOND)) {
        CommandSender_sendFetchManchesterCode(robot->command_sender);
        Timer_reset(robot->timer);
    }
}

void Robot_lowerPenAndWaitASecondAndAHalf(struct Robot *robot)
{
    CommandSender_sendLowerPenCommand(robot->command_sender);
    Timer_reset(robot->timer);

    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND_AND_AN_HALF));
}

void Robot_risePenAndWaitASecondAndAHalf(struct Robot *robot)
{
    CommandSender_sendRisePenCommand(robot->command_sender);
    Timer_reset(robot->timer);

    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND_AND_AN_HALF));
}
