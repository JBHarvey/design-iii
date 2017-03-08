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

static _Bool initTCPServer(struct Communication *communication, unsigned short port)
{
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);

    // Create server socket
    if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if(bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        close(sd);
        perror("bind error");
        return 0;
    }

    // Start listing on the socket
    if(listen(sd, 2) < 0) {
        close(sd);
        perror("listen error");
        return 0;
    }

    // Initialize and start a watcher to accepts client requests
    struct ev_io *w_accept = malloc(sizeof(struct ev_io));
    ev_io_init(w_accept, acceptCallback, sd, EV_READ);
    ev_io_start(communication->loop, w_accept);

    return 1;
}

struct Communication *communication_initServer(unsigned short port)
{
    struct Communication *communication = calloc(sizeof(struct Communication), 1);
    communication->loop = ev_default_loop(0);

    if(!initTCPServer(communication, port)) {
        free(communication);
        return 0;
    }

    return communication;
}


static _Bool initTCPCLient(struct Communication *communication, char *ip, unsigned short port)
{
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return 0;
    }

    // Create server socket
    if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 0;
    }

    // Bind socket to address
    if(connect(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sd);
        perror("connect error");
        return 0;
    }

    int flags = fcntl(sd, F_GETFL, 0);

    if(fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(sd);
        perror("fcntl error");
        return 0;
    }

    struct ev_io *w_client = malloc(sizeof(struct ev_io));

    ev_io_init(w_client, readWriteCallback, sd, EV_READ | EV_WRITE);

    ev_io_start(communication->loop, w_client);

    return 1;
}

struct Communication *communication_initClient(char *ip, unsigned short port)
{
    struct Communication *communication = calloc(sizeof(struct Communication), 1);
    communication->loop = ev_default_loop(0);

    if(!initTCPCLient(communication, ip, port)) {
        free(communication);
        return 0;
    }

    return communication;
}

void communication_do(struct Communication *communication, unsigned int milliseconds)
{
    ev_loop(communication->loop, 1);
}

void communication_close(struct Communication *communication)
{
    //TODO
    free(communication);
}
