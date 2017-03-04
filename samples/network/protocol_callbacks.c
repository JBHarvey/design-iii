
#include "protocol.h"


void cb_start_packet()
{
    printf("start packet\n");
}

void cb_continue_packet()
{
    printf("continue packet\n");
}

void cb_station_data(Station_Data station_data)
{
    printf("station data packet\n");
}
