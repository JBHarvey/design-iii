#ifndef STATION_CLIENT_SENDER_H_
#define STATION_CLIENT_SENDER_H_

#include "CommunicationStructures.h"
#include "station_client.h"

void StationClientSender_sendStartCycleCommand(void);

void StationClientSender_sendWorldInformationsToRobot(struct Communication_Object *obstacles, unsigned int num_obstacles, 
        struct Communication_Object robot, int environment_has_changed);

void StationClientSender_sendReceiveData(struct StationClient *station_client);

void StationClientSender_sendImageReceivedAck(void);

void StationClientSender_sendPlannedTrajectoryReceivedAck(void);

void StationClientSender_sendReadyToStartSignalReceivedAck(void);

void StationClientSender_sendReadyToDrawSignalReceivedAck(void);

void StationClientSender_sendEndOfCycleSignalReceivedAck(void);

void StationClientSender_removeForceEnvironmentHasChanged(void);

#endif // STATION_CLIENT_SENDER_H_
