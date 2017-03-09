#include "protocol.h"

void sendStartPacket()
{
    uint8_t data = PACKET_START;
    addPacket(&data, sizeof(data));
}

void sendContinuePacket()
{
    uint8_t data = PACKET_CONTINUE;
    addPacket(&data, sizeof(data));
}

void sendWorldToRobot(struct Communication_World communication_world)
{
    uint8_t data[1 + sizeof(struct Communication_World)];
    data[0] = PACKET_STATION_DATA;
    memcpy(data + 1, &communication_world, sizeof(struct Communication_World));
    addPacket(data, sizeof(data));
}

void handleReceivedPacket(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    switch(data[0]) {
        case PACKET_START:
            callbackStartPacket();
            break;

        case PACKET_CONTINUE:
            callbackContinuePacket();
            break;

        case PACKET_STATION_DATA:
            if(length != (sizeof(struct Communication_World) + 1)) {
                printf("wrong struct Communication_World length\n");
                break;
            }

            struct Communication_World communication_world;

            memcpy(&communication_world, data + 1, sizeof(struct Communication_World));

            callbackStationData(communication_world);

            break;
    }
}
