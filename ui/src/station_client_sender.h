#ifndef STATION_CLIENT_SENDER_H_
#define STATION_CLIENT_SENDER_H_

#include "CommunicationStructures.h"
#include "station_client.h"

void StationClientSender_sendStartCycleCommand(void);

void StationClientSender_sendWorldInformationsToRobot(struct Communication_Object *obstacles, unsigned num_obstacles, struct Communication_Object robot);

void StationClientSender_sendReceiveData(struct StationClient *station_client);

void StationClientSender_sendImageReceivedAck(void);

#endif // STATION_CLIENT_SENDER_H_
