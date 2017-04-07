#include <criterion/criterion.h>
#include <stdio.h>
#include "WorldCamera.h"

const int ROBOT_X_VALUE = 10000;
const int ROBOT_Y_VALUE = 20000;
const int ROBOT_THETA_VALUE = 21000;
const int ROBOT_RADIUS = 16000;

struct WorldCamera *world_camera;

void setup_world_camera(void)
{
    world_camera = WorldCamera_new();
}

void teardown_world_camera(void)
{
    WorldCamera_delete(world_camera);
}

Test(WorldCamera, creation_destruction
     , .init = setup_world_camera
     , .fini = teardown_world_camera)
{
    struct Pose *pose_zero = Pose_zero();
    cr_assert(world_camera->map_sensor->has_received_new_data == 0);
    cr_assert(world_camera->robot_sensor->has_received_new_data == 0);
    cr_assert(world_camera->robot_radius == 0);
    cr_assert(Pose_haveTheSameValues(world_camera->robot_pose, pose_zero));
    Pose_delete(pose_zero);
}

Test(WorldCamera, given_receivedData_when_readsPoseData_then_thePoseIsUpdatedToTheRobotPose
     , .init = setup_world_camera
     , .fini = teardown_world_camera)
{
    int x = ROBOT_X_VALUE;
    int y = ROBOT_Y_VALUE;
    int theta = ROBOT_THETA_VALUE;
    int radius = ROBOT_RADIUS;
    struct Pose *pose = Pose_new(x, y, theta);

    Pose_copyValuesFrom(world_camera->robot_pose, pose);
    world_camera->robot_radius = radius;

    Pose_delete(pose);

    Sensor_receivesData(world_camera->robot_sensor);

    struct Pose *read_pose = Pose_zero();
    WorldCamera_readPoseData(world_camera, read_pose);

    cr_assert(Pose_haveTheSameValues(world_camera->robot_pose, read_pose));

    Pose_delete(read_pose);

}

Test(Wheels, given_receivedData_when_readsPoseData_then_robotSensorHasNoMoreNewData
     , .init = setup_world_camera
     , .fini = teardown_world_camera)
{
    int x = ROBOT_X_VALUE;
    int y = ROBOT_Y_VALUE;
    int theta = ROBOT_THETA_VALUE;
    int radius = ROBOT_RADIUS;
    struct Pose *pose = Pose_new(x, y, theta);

    Pose_copyValuesFrom(world_camera->robot_pose, pose);
    world_camera->robot_radius = radius;

    Pose_delete(pose);

    Sensor_receivesData(world_camera->robot_sensor);

    struct Pose *read_pose = Pose_zero();
    WorldCamera_readPoseData(world_camera, read_pose);

    cr_assert(!world_camera->robot_sensor->has_received_new_data);

    Pose_delete(read_pose);

}
