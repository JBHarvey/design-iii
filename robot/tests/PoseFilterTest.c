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

Test(PoseFilter, given__when_fetchPoseFilterCallbacks_then_theCorrectSturctureIsReturned)
{
    void (*updateFromCameraOnly)(struct PoseFilter *) = &PoseFilter_updateFromCameraOnly;

    struct PoseFilter_Callbacks callbacks = PoseFilter_fetchCallbacks();
    cr_assert_eq(callbacks.updateFromCameraOnly, updateFromCameraOnly);
}

void setup_PoseFilter(void)
{
    robot = Robot_new();
    pose_filter = PoseFilter_new(robot);
}

void teardown_PoseFilter(void)
{
    PoseFilter_delete(pose_filter);
    Robot_delete(robot);
}

int POSEFILTER_X = 9001;
int POSEFILTER_Y = 4200;

void PoseFilterTest_updateFromCameraOnlyMock(struct PoseFilter *pose_filter)
{
    struct Coordinates * new_coordinates = Coordinates_new(POSEFILTER_X, POSEFILTER_Y);
    Coordinates_copyValuesFrom(pose_filter->robot->current_state->pose->coordinates, new_coordinates);
    Coordinates_delete(new_coordinates);
}

Test(PoseFilter, given_aPoseFilterCallback_when_askedToExecuteTheCallback_then_theCallbackIsExecuted
     , .init = setup_PoseFilter
     , .fini = teardown_PoseFilter)
{
    void (*updateFromCameraOnly)(struct PoseFilter *) = &PoseFilterTest_updateFromCameraOnlyMock;
    PoseFilter_executeFilter(pose_filter, updateFromCameraOnly);

    struct Coordinates *expected_coordinates = Coordinates_new(POSEFILTER_X, POSEFILTER_Y);
    cr_assert(Coordinates_haveTheSameValues(robot->current_state->pose->coordinates, expected_coordinates));
    Coordinates_delete(expected_coordinates);
}
