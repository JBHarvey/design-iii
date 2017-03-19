#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "station_client.h"
#include "station_interface.h"
#include "station_client_sender.h"
#include "logger.h"

/* Constants */

const int MAX_IP_ADDRESS_LENGTH = 45;

/* Flag definitions */

extern enum ThreadStatus main_loop_status;
extern enum ConnectionStatus robot_connection_status;

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

gpointer StationClient_init(struct StationClient *station_client)
{
    Logger_startRobotConnectionHandlerSectionAndAppend("Trying to connect to the robot...");

    while(!initTCPClient(station_client)) {
        Logger_startRobotConnectionHandlerSectionAndAppend("Connection to robot failed, trying again");
        usleep(100000);

        if(main_loop_status == TERMINATED) {
            return (gpointer) FALSE;
        }
    }

    robot_connection_status = CONNECTED;
    Logger_startRobotConnectionHandlerSectionAndAppend("Connected !");
    StationClientSender_startSendingWorldInformationsToRobot(station_client);
    return (gpointer) TRUE;
}

gboolean StationClient_communicate(struct StationClient *station_client, unsigned int milliseconds)
{
    ev_loop(station_client->loop, milliseconds);
    return FALSE;
}


static void startPacketCallback()
{
}

static void continuePacketCallback()
{
}

void handleReceivedPacket(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    switch(data[0]) {
        case DATA_MANCHESTER:
            break;

        case DATA_IMAGE:
            break;

        case DATA_PLANNED_TRAJECTORY:
            break;

        case DATA_ESTIMATED_ROBOT_POSITION:
            break;
            /*
                case DATA_WORLD:
                    if(length != (sizeof(struct Communication_World) + 1)) {
                        printf("wrong struct Communication_World length\n");
                        break;
                    }

                    struct Communication_World communication_world;

                    memcpy(&communication_world, data + 1, sizeof(struct Communication_World));

                    break;
            */
    }
}
