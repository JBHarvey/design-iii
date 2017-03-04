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
    cr_assert(worldCamera->sensor->hasReceivedNewData == 0);
}
