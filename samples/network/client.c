#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "protocol.h"
#include "ev_init.h"

#define PORT_NO 3033
#define BUFFER_SIZE 1024

int main()
{

    struct Communication *communication = communication_initClient("127.0.0.1", PORT_NO);

    uint8_t *test_large = malloc(1 << 17);
    memset(test_large, 'X', 1 << 17);
    test_large[(1 << 17) - 1] = 0;
    sendStartPacket();

    while(1) {
        addPacket(test_large, 1 << 17);
        communication_do(communication, 10);
    }

    free(test_large);

    return 0;
}
