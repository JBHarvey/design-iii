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

void handle_recv_packet(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    switch(data[0]) {
        case PACKET_START:
            printf("start packet\n");
            break;

        case PACKET_CONTINUE:
            printf("continue packet\n");
            break;
    }
}
