#ifndef ROBOTRECEIVER_H_
#define ROBOTRECEIVER_H_

#include "CommunicationStructures.h"
#include "Robot.h"

struct __attribute__((__packed__)) Mesurements
{
    struct Communication_World world;
};


void RobotReceiver_updateMesurements(struct Robot *robot);
void RobotReceiver_updateWorld(struct WorldCamera *world_camera, struct Communication_World world);
struct Mesurements RobotReceiver_fetchInputs(struct Mesurements (*communication_callback)(void));

#endif // ROBOTRECEIVER_H_