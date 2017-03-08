#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "protocol.h"

#define PORT_NO 3033
#define BUFFER_SIZE 1024

int main()
{
    struct ev_loop *loop = ev_default_loop(0);
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    struct ev_io *w_client = malloc(sizeof(struct ev_io));

    // Create server socket
    if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);

    if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return -1;
    }

    // Bind socket to address
    if(connect(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect error");
        return -1;
    }

    int flags = fcntl(sd, F_GETFL, 0);

    if(fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl error");
        return -1;
    }

    ev_io_init(w_client, readWriteCallback, sd, EV_READ | EV_WRITE);
    ev_io_start(loop, w_client);

    // Start infinite loop
    uint8_t *test_large = malloc(1 << 17);
    memset(test_large, 'X', 1 << 17);
    test_large[(1 << 17) - 1] = 0;
    sendStartPacket();

    while(1) {
        //addPacket(test_large, 1 << 17);
        ev_loop(loop, 10);
    }

    free(test_large);

    return 0;
}
