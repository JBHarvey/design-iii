
#include "protocol.h"

static void *globalPointer;

void setPointer(void *pointer)
{
    globalPointer = pointer;
}

void callbackStartPacket()
{
    printf("start packet\n");
}

void callbackContinuePacket()
{
    printf("continue packet\n");
}

void callbackStationData(struct Communication_World station_data)
{
    printf("station data packet\n");
}
