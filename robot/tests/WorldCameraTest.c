#include <criterion/criterion.h>
#include <stdio.h>
#include "WorldCamera.h"

struct WorldCamera *worldCamera;

void setupWorldCamera(void)
{
    worldCamera = WorldCamera_new();
}

void teardownWorldCamera(void)
{
    WorldCamera_delete(worldCamera);
}

Test(WorldCamera, creation_destruction
     , .init = setupWorldCamera
     , .fini = teardownWorldCamera)
{
    struct Pose *poseZero = Pose_zero();
    cr_assert(worldCamera->mapSensor->hasReceivedNewData == 0);
    cr_assert(worldCamera->robotSensor->hasReceivedNewData == 0);
    cr_assert(worldCamera->robotRadius == 0);
    cr_assert(Pose_haveTheSameValues(worldCamera->robotPose, poseZero));
    Pose_delete(poseZero);
}
