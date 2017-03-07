#ifndef ROBOTRECEIVER_H_
#define ROBOTRECEIVER_H_

#include "Robot.h"
#include "CommunicationStructures.h"

/*
enum {
    PACKET_WORLD
};

 */
void RobotReceiver_updateMesurements(struct Robot *robot);

#endif // ROBOTRECEIVER_H_
