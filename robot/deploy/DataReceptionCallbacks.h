#ifndef DATARECEPTIONCALLBACKS_H_
#define DATARECEPTIONCALLBACKS_H_

struct Robot;
static struct Robot *robot;

void DataReceptionCallbacks_setRobotPointer(struct Robot *new_robot);

void callbackStartPacket();

void callbackContinuePacket();

void callbackWorld(struct Communication_World communication_world);

#endif // DATARECEPTIONCALLBACKS_H_
