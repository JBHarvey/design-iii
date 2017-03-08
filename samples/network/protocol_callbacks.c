
#include "protocol.h"


void callbackStartPacket()
{
    printf("start packet\n");
}

void callbackContinuePacket()
{
    printf("continue packet\n");
}

void callbackStationData(struct PackedStationData station_data)
{
    printf("station data packet\n");
}
