#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "network.h"

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

    struct ev_loop *loop = ev_default_loop(0);
    int sd, server;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);

    struct ev_io w_accept;

    if(pid) {
        // Create server socket
        if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket error");
            return -1;
        }

        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT_NO);
        addr.sin_addr.s_addr = INADDR_ANY;

        // Bind socket to address
        if(bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
            perror("bind error");
        }

        server = sd;

        // Start listing on the socket
        if(listen(sd, 2) < 0) {
            perror("listen error");
            return -1;
        }

        // Initialize and start a watcher to accepts client requests
        ev_io_init(&w_accept, acceptCallback, sd, EV_READ);
        ev_io_start(loop, &w_accept);
    } else {
        struct ev_io *w_client = malloc(sizeof(struct ev_io));

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
    }

    uint8_t *test_large = malloc(1 << 17);
    memset(test_large, 'X', 1 << 17);

    addPacket(test_large, 1 << 17);
    unsigned int i;

    for(i = 0; i < 1000; ++i) {
        ev_loop(loop, 10);

        if(test_passed) {
            break;
        }
    }

    free(test_large);
    close(sd);

    if(test_passed) {
        return 0;
    }

    return -1;
}

