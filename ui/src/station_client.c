#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "station_client.h"

/* Constants */

const int MAX_IP_ADDRESS_LENGTH = 45;

static struct ev_io *watcher;

struct StationClient *StationClient_new(int new_port, const char *server_ip)
{
    struct StationClient *station_client = malloc(sizeof(struct StationClient));
    station_client->loop = ev_default_loop(0);
    station_client->port = new_port;
    station_client->server_ip = malloc(sizeof(char) * (MAX_IP_ADDRESS_LENGTH + 1));
    strcpy(station_client->server_ip, server_ip);

    if(!station_client->loop) {
        perror("Error in initializing libev. Bad $LIBEV_FLAGS in the environment?");
    }

    return station_client;
}

void StationClient_delete(struct StationClient *station_client)
{
    free(station_client->server_ip);
    free(station_client);
    station_client = NULL;
}

static _Bool initTCPClient(struct StationClient *station_client)
{
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(station_client->port);

    if(inet_pton(AF_INET, station_client->server_ip, &addr.sin_addr) <= 0) {
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

    ev_io_start(station_client->loop, w_client);

    return 1;
}

gboolean StationClient_init(struct StationClient *station_client)
{
    if(!initTCPClient(station_client)) {
        printf("\nConnection to robot failed. Retry.\n");
        g_idle_add((GSourceFunc) StationClient_init, (gpointer) station_client);
        return FALSE;
    }

    printf("\nConnected !\n");
    return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
}

gboolean StationClient_communicate(struct StationClient *station_client, unsigned int milliseconds)
{
    ev_loop(station_client->loop, milliseconds);

}


static void callbackStartPacket()
{
}

static void callbackContinuePacket()
{
}

void sendStartPacket()
{
    uint8_t data = PACKET_START;
    addPacket(&data, sizeof(data));
}

void sendContinuePacket()
{
    uint8_t data = PACKET_CONTINUE;
    addPacket(&data, sizeof(data));
}

/*
void sendWorldToRobot(struct Communication_World communication_world)
{
    uint8_t data[1 + sizeof(struct Communication_World)];
    data[0] = PACKET_WORLD;
    memcpy(data + 1, &communication_world, sizeof(struct Communication_World));
    addPacket(data, sizeof(data));
}
*/

void handleReceivedPacket(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    switch(data[0]) {
        case PACKET_START:
            callbackStartPacket();
            break;

        case PACKET_CONTINUE:
            callbackContinuePacket();
            break;

        case PACKET_WORLD:
            if(length != (sizeof(struct Communication_World) + 1)) {
                printf("wrong struct Communication_World length\n");
                break;
            }

            struct Communication_World communication_world;

            memcpy(&communication_world, data + 1, sizeof(struct Communication_World));

            break;
    }
}
