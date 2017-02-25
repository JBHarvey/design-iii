#include "network.h"

void send_start_packet();
void send_continue_packet();

enum {
    PACKET_START,
    PACKET_CONTINUE
};
