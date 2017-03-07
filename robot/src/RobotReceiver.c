#include "RobotReceiver.h"


static void updateWorld(struct WorldCamera *worldCamera, struct Communication_World world)
{
    if(world.environmentHasChanged) {
        Sensor_receivesData(worldCamera->mapSensor);
        struct Map *map = worldCamera->map;
        //TODO: call all the update functions
    }

    Sensor_receivesData(worldCamera->robotSensor);



}

void RobotReceiver_updateMesurements(struct Robot *robot)
{
    // TODO: Integrate the handle_recv_packet function to call these callbacks:
    struct Communication_World world;
    updateWorld(robot->worldCamera, world);

    // THESE DON'T EXIST YET
    //updateWheels(robot->wheels);
    //updatePen(robot->pen);
}
