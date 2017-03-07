#ifndef ROBOTRECEIVER_H_
#define ROBOTRECEIVER_H_

#include "Robot.h"
#include "CommunicationStructures.h"

/*
enum {
    PACKET_WORLD
};

 */
struct __attribute__((__packed__)) Mesurements
{
    struct Communication_World world;
};


struct Mesurements RobotReceiver_fetchInputs(struct Mesurements (*communicationCallback)(void));
void RobotReceiver_updateMesurements(struct Robot *robot);

#endif // ROBOTRECEIVER_H_
