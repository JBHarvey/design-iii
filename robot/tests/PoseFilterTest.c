#include <criterion/criterion.h>
#include <stdio.h>
#include "PoseFilter.h"

struct Robot *robot;
struct PoseFilter *pose_filter;

Test(PoseFilter, creation_destruction)
{
    struct Robot *robot = Robot_new();
    struct PoseFilter *pose_filter = PoseFilter_new(robot);

    cr_assert_eq(pose_filter->robot, robot);

    PoseFilter_delete(pose_filter);
    Robot_delete(robot);
}

Test(PoseFilter, given__when_fetchPoseFilterCallbacks_then_theCorrectStructureIsReturned)
{
    void (*updateFromCameraOnly)(struct PoseFilter *) = &PoseFilter_updateFromCameraOnly;

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();
    cr_assert_eq(callbacks.updateFromCameraOnly, updateFromCameraOnly);
}

int POSEFILTER_X = 9001;
int POSEFILTER_Y = 4200;
struct Coordinates *coordinates;
void setup_PoseFilter(void)
{
    coordinates = Coordinates_new(POSEFILTER_X, POSEFILTER_Y);
    robot = Robot_new();
    pose_filter = PoseFilter_new(robot);
}

void teardown_PoseFilter(void)
{
    PoseFilter_delete(pose_filter);
    Robot_delete(robot);
    Coordinates_delete(coordinates);
}

void PoseFilterTest_updateFromCameraOnlyMock(struct PoseFilter *pose_filter)
{
    Coordinates_copyValuesFrom(pose_filter->robot->current_state->pose->coordinates, coordinates);
}

Test(PoseFilter, given_aPoseFilterCallback_when_askedToExecuteTheCallback_then_theCallbackIsExecuted
     , .init = setup_PoseFilter
     , .fini = teardown_PoseFilter)
{
    void (*updateFromCameraOnly)(struct PoseFilter *) = &PoseFilterTest_updateFromCameraOnlyMock;
    PoseFilter_executeFilter(pose_filter, updateFromCameraOnly);

    cr_assert(Coordinates_haveTheSameValues(robot->current_state->pose->coordinates, coordinates));
}

Test(PoseFilter,
     given_aWorldCameraWithNoNewDataForTheRobotButDifferentRobotPoseData_when_updatesFromCamera_then_theRobotPoseRemainsUnchanged
     , .init = setup_PoseFilter
     , .fini = teardown_PoseFilter)
{
    Coordinates_copyValuesFrom(robot->world_camera->robot_pose->coordinates, coordinates);
    struct Pose *old_robot_pose = Pose_zero();
    Pose_copyValuesFrom(old_robot_pose, robot->current_state->pose);

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();
    PoseFilter_executeFilter(pose_filter, callbacks.updateFromCameraOnly);

    struct Pose *current_robot_pose = Pose_zero();
    Pose_copyValuesFrom(current_robot_pose, robot->current_state->pose);
    cr_assert(Pose_haveTheSameValues(old_robot_pose, current_robot_pose));

    Pose_delete(current_robot_pose);
    Pose_delete(old_robot_pose);
}

Test(PoseFilter,
     given_aWorldCameraWithNewDataForTheRobot_when_updatesFromCamera_then_theRobotPoseRemainsUnchanged
     , .init = setup_PoseFilter
     , .fini = teardown_PoseFilter)
{
    Coordinates_copyValuesFrom(pose_filter->robot->world_camera->robot_pose->coordinates, coordinates);
    Sensor_receivesData(robot->world_camera->robot_sensor);
    struct Pose *old_robot_pose = Pose_zero();
    Pose_copyValuesFrom(old_robot_pose, robot->current_state->pose);

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();
    PoseFilter_executeFilter(pose_filter, callbacks.updateFromCameraOnly);

    struct Pose *current_robot_pose = Pose_zero();
    Pose_copyValuesFrom(current_robot_pose, robot->current_state->pose);
    cr_assert(!Pose_haveTheSameValues(old_robot_pose, current_robot_pose));
    cr_assert(Pose_haveTheSameValues(robot->world_camera->robot_pose, current_robot_pose));

    Pose_delete(current_robot_pose);
    Pose_delete(old_robot_pose);
}

Test(PoseFilter,
     given_aWorldCameraWithNewDataForTheRobot_when_updatesFromCamera_then_theWorldCameraNoLongerHasNewDataForTheRobot
     , .init = setup_PoseFilter
     , .fini = teardown_PoseFilter)
{
    Coordinates_copyValuesFrom(pose_filter->robot->world_camera->robot_pose->coordinates, coordinates);
    Sensor_receivesData(robot->world_camera->robot_sensor);
    struct Pose *old_robot_pose = Pose_zero();
    Pose_copyValuesFrom(old_robot_pose, robot->current_state->pose);

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();
    PoseFilter_executeFilter(pose_filter, callbacks.updateFromCameraOnly);

    struct Pose *current_robot_pose = Pose_zero();
    Pose_copyValuesFrom(current_robot_pose, robot->current_state->pose);

    cr_assert(!robot->world_camera->robot_sensor->has_received_new_data);

    Pose_delete(current_robot_pose);
    Pose_delete(old_robot_pose);
}
