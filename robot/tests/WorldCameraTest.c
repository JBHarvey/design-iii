#include <criterion/criterion.h>
#include <stdio.h>
#include "WorldCamera.h"

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
