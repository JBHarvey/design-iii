#ifndef DATARECEPTIONCALLBACKS_H_
#define DATARECEPTIONCALLBACKS_H_

#include "DataReceptionCallbacks.h"

void DataReceptionCallbacks_setRobotPointer(struct Robot *new_robot)
{
    robot = new_robot;
}

void callbackStartPacket()
{
}

void callbackContinuePacket()
{
}

void callbackWorld(struct Communication_World communication_world)
{
    RobotReceiver_updateWorld(robot->worldCamera, communication_world);
}

#endif // DATARECEPTIONCALLBACKS_H_
