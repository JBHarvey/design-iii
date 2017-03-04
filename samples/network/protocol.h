#include "network.h"

enum {
    PACKET_START,
    PACKET_CONTINUE,
    PACKET_STATION_DATA
};

typedef struct __attribute__((__packed__))
{
    _Bool present;
    int x;
    int y;
    int radius;
}
Obstacle;

typedef struct __attribute__((__packed__))
{
    int x;
    int y;
    int angle;
    int radius;
    Obstacle obstacles[3];
}
Station_Data;

void send_start_packet();
void send_continue_packet();
void send_station_data(Station_Data station_data);


/* Callbacks */
void cb_start_packet();
void cb_continue_packet();
void cb_station_data(Station_Data station_data);
