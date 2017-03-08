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

void sendStationData(struct PackedStationData station_data)
{
    uint8_t data[1 + sizeof(struct PackedStationData)];
    data[0] = PACKET_STATION_DATA;
    memcpy(data + 1, &station_data, sizeof(struct PackedStationData));
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
            if(length != (sizeof(struct PackedStationData) + 1)) {
                printf("wrong struct PackedStationData length\n");
                break;
            }

            struct PackedStationData station_data;

            memcpy(&station_data, data + 1, sizeof(struct PackedStationData));

            callbackStationData(station_data);

            break;
    }
}
