#include "network.h"
#include "CommunicationStructures.h"

enum {
    PACKET_START,
    PACKET_CONTINUE,
    PACKET_STATION_DATA
};

void sendStartPacket();
void sendContinuePacket();
void sendStationData(struct Communication_World station_data);


/* Callbacks */
void callbackStartPacket();
void callbackContinuePacket();
void callbackStationData(struct Communication_World station_data);
