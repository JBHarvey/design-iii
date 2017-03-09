#include "network.h"
#include "DataReceptionCallbacks.h"

enum {
    PACKET_START,
    PACKET_CONTINUE,
    PACKET_STATION_DATA
};

void sendStartPacket();
void sendContinuePacket();
void sendWorldToRobot(struct Communication_World communication_world);

