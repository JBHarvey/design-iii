#ifndef STATION_CLIENT_SENDER_H_
#define STATION_CLIENT_SENDER_H_

#include "CommunicationStructures.h"
#include "station_client.h"

void StationClientSender_sendStartPacket(void);

void StationClientSender_sendContinuePacket(void);

void StationClientSender_startSendingWorldInformationsToRobot(struct StationClient *station_client);

#endif // STATION_CLIENT_SENDER_H_
