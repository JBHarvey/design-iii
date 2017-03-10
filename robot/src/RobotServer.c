#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>

#include "RobotServer.h"

extern struct RobotServer *robot_server;
static struct ev_io *watcher;

static _Bool initTCPServer(struct RobotServer *robot_server, unsigned short port)
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
    watcher = malloc(sizeof(struct ev_io));
    ev_io_init(watcher, acceptCallback, sd, EV_READ);
    ev_io_start(robot_server->loop, watcher);

    return 1;
}

struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port)
{
    struct Object *new_object = Object_new();
    struct RobotServer *pointer = (struct RobotServer *) malloc(sizeof(struct RobotServer));
    pointer->object = new_object;
    pointer->robot = new_robot;
    pointer->loop = ev_default_loop(0);
    pointer->port = new_port;

    if(!pointer->loop) {
        perror("Error in initializing libev. Bad $LIBEV_FLAGS in the environment?");
    }

    while(initTCPServer(pointer, pointer->port) != 1);

    Object_addOneReference(new_robot->object);

    return pointer;
}

void RobotServer_delete(struct RobotServer *robot_server)
{
    Object_removeOneReference(robot_server->object);

    if(Object_canBeDeleted(robot_server->object)) {
        Object_delete(robot_server->object);
        Robot_delete(robot_server->robot);

        free(robot_server);

    }
}

/*


static _Bool initTCPCLient(struct RobotServer *robot_server, char *ip, unsigned short port)
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

    ev_io_start(robot_server->loop, w_client);

    return 1;
}

struct RobotServer *RobotServer_initClient(char *ip, unsigned short port)
{
    struct RobotServer *robot_server = calloc(sizeof(struct RobotServer), 1);
    robot_server->loop = ev_default_loop(0);

    if(!initTCPCLient(robot_server, ip, port)) {
        free(robot_server);
        return 0;
    }

    return robot_server;
}

void RobotServer_do(struct RobotServer *robot_server, unsigned int milliseconds)
{
    ev_loop(robot_server->loop, milliseconds);
}

void RobotServer_close(struct RobotServer *robot_server)
{
    //TODO
    free(robot_server);
}
*/

static void callbackStartPacket()
{
}

static void callbackContinuePacket()
{
}

static void callbackWorld(struct Communication_World communication_world)
{

    RobotReceiver_updateWorld(robot_server->robot->world_camera, communication_world);
}
/*
from protocol:

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

            callbackWorld(communication_world);

            break;
    }
}
