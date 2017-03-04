#include "protocol.h"

void send_start_packet()
{
    uint8_t data = PACKET_START;
    add_packet(&data, sizeof(data));
}

void send_continue_packet()
{
    uint8_t data = PACKET_CONTINUE;
    add_packet(&data, sizeof(data));
}

void send_station_data(Station_Data station_data)
{
    uint8_t data[1 + sizeof(Station_Data)];
    data[0] = PACKET_STATION_DATA;
    memcpy(data + 1, &station_data, sizeof(Station_Data));
    add_packet(data, sizeof(data));
}

void handle_recv_packet(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    switch(data[0]) {
        case PACKET_START:
            cb_start_packet();
            break;

        case PACKET_CONTINUE:
            cb_continue_packet();
            break;

        case PACKET_STATION_DATA:
            if(length != (sizeof(Station_Data) + 1)) {
                printf("wrong Station_Data length\n");
                break;
            }

            Station_Data station_data;
            memcpy(&station_data, data + 1, sizeof(Station_Data));
            cb_station_data(station_data);
            break;
    }
}
