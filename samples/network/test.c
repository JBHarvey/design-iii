#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "network.h"
#include "ev_init.h"

#define PORT_NO 3033
#define BUFFER_SIZE 1024

_Bool test_passed;

void handleReceivedPacket(uint8_t *data, uint32_t length)
{
    if(length == (1 << 17)) {
        unsigned int i;

        for(i = 0; i < length; ++i) {
            if(data[i] != 'X') {
                return;
            }
        }

        test_passed = 1;
    }
}


int main()
{
    int pid = fork();
    struct Communication *communication;

    if(pid) {
        communication = communication_initServer(PORT_NO);
    } else {
        communication = communication_initClient("127.0.0.1", PORT_NO);
    }

    uint8_t *test_large = malloc(1 << 17);
    memset(test_large, 'X', 1 << 17);

    addPacket(test_large, 1 << 17);
    unsigned int i;

    for(i = 0; i < 1000; ++i) {
        communication_do(communication, 10);

        if(test_passed) {
            break;
        }
    }

    free(test_large);
    communication_close(communication);

    if(test_passed) {
        return 0;
    }

    return -1;
}

