#include "network.h"

enum {
    PACKET_START,
    PACKET_CONTINUE,
    PACKET_STATION_DATA
};

struct __attribute__((__packed__)) PackedObstacle {
    _Bool present;
    int x;
    int y;
    int radius;
};

struct __attribute__((__packed__)) PackedStationData {
    int x;
    int y;
    int angle;
    int radius;
    struct PackedObstacle obstacles[3];
};

void sendStartPacket();
void sendContinuePacket();
void sendStationData(struct PackedStationData station_data);


/* Callbacks */
void callbackStartPacket();
void callbackContinuePacket();
void callbackStationData(struct PackedStationData station_data);
