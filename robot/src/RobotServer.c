#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>
#include <termios.h>

#include "RobotServer.h"
#include "CommunicationStructures.h"

extern struct RobotServer *robot_server;

static _Bool initTCPServer(struct ev_loop *loop, unsigned short port)
{
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    struct ev_io *watcher;

    // Create server socket
    if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 0;
    }

    int yes = 1;

    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        close(sd);
        perror("setsockopt");
        return 0;
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
    ev_io_start(loop, watcher);

    return 1;
}

void TTYACMCallback(struct ev_loop *loop, struct ev_io *watcher, int revents);

static int initTTYACM(struct ev_loop *loop, char *ttyacm_path)
{
    int fd = open(ttyacm_path, O_RDWR | O_NOCTTY);

    if(fd < 0) {
        return -1;
    }

    struct termios options;

    tcgetattr(fd, &options);

    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);

    options.c_oflag &= ~(ONLCR | OCRNL);

    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    tcsetattr(fd, TCSANOW, &options);

    uint8_t data[2] = {255, 0};

    if(write(fd, data, sizeof(data)) != sizeof(data)) {
        close(fd);
        printf("error writing begin packet\n");
        return -1;
    }

    struct ev_io *watcher;

    watcher = malloc(sizeof(struct ev_io));

    ev_io_init(watcher, TTYACMCallback, fd, EV_READ);

    ev_io_start(loop, watcher);

    return fd;
}

//This global variable allows the command sender to be output-agnostic
static int physical_robot_file_descriptor;
static struct DataReceiver_Callbacks reception_callbacks;

struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port, char *ttyacm_path)
{
    struct ev_loop *new_loop = ev_default_loop(0);

    if(!new_loop) {
        perror("Error in initializing libev. Bad $LIBEV_FLAGS in the environment?");
        return 0;
    }

    int new_fd_ttyACM = initTTYACM(new_loop, ttyacm_path);

    if(new_fd_ttyACM == -1) {
        printf("error opening %s\n", ttyacm_path);
        return 0;
    }

    physical_robot_file_descriptor = new_fd_ttyACM;

    while(!initTCPServer(new_loop, new_port));

    struct Object *new_object = Object_new();
    struct DataReceiver_Callbacks new_data_receiver_callbacks = DataReceiver_fetchCallbacks();
    struct RobotServer *pointer =  malloc(sizeof(struct RobotServer));
    pointer->object = new_object;
    pointer->robot = new_robot;
    pointer->loop = new_loop;
    reception_callbacks = new_data_receiver_callbacks;

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

void RobotServer_communicate(struct RobotServer *robot_server)
{
    ev_run(robot_server->loop, EVRUN_NOWAIT);
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

    printf("received packet %u of length %u\n", data[0], length);

    switch(data[0]) {
        /*
        case PACKET_START:
            callbackStartPacket();
            break;

        case PACKET_CONTINUE:
            callbackContinuePacket();
            break;

        case DATA_TRANSLATION:
            //TODO: add processing
            reception_callbacks.updateWheelsTranslation(robot_server->robot->wheels, communication_translation);
            break;

        case DATA_ROTATION:
            //TODO: add processing
            reception_callbacks.updateWheelsTranslation(robot_server->robot->wheels, communication_rotation);
            break;

            */

        case DATA_WORLD:
            if(length != (sizeof(struct Communication_World) + 1)) {
                printf("wrong struct Communication_World length\n");
                break;
            }

            struct Communication_World communication_world;

            memcpy(&communication_world, data + 1, sizeof(struct Communication_World));

            reception_callbacks.updateWorld(robot_server->world_camera, communication_world);

            break;
    }
}

static void handleTTYACMPacket(uint8_t type, uint8_t *data, uint8_t length)
{
    printf("packet %hhu of length %hhu: ", type, length);

    unsigned int i;

    for(i = 0; i < length; ++i) {
        printf("%02X", data[i]);
    }

    printf("\n");
}

#include <sys/ioctl.h>

static uint8_t TTYACM_header[2];
static _Bool TTYACM_header_stored;

static _Bool readTTYACMPacket(int fd)
{
    int bytes_available = 0;
    ioctl(fd, FIONREAD, &bytes_available);

    if(!TTYACM_header_stored) {
        if(bytes_available >= 2 && read(fd, TTYACM_header, sizeof(TTYACM_header)) == 2) {
            bytes_available -= 2;
            TTYACM_header_stored = 1;
        }
    }

    if(TTYACM_header_stored) {
        uint8_t data[TTYACM_header[1]];

        if(bytes_available >= sizeof(data) && read(fd, data, sizeof(data)) == sizeof(data)) {
            handleTTYACMPacket(TTYACM_header[0], data, sizeof(data));
            TTYACM_header_stored = 0;
            return 1;
        }
    }

    return 0;
}

_Bool writeTTYACMPacket(uint8_t type, uint8_t *data, unsigned int length)
{
    if(length > UINT8_MAX) {
        return 0;
    }

    uint8_t packet[2 + length];

    packet[0] = type;
    packet[1] = length;
    memcpy(packet + 2, data, length);

    return write(physical_robot_file_descriptor, packet, sizeof(packet)) == sizeof(packet);
}

void TTYACMCallback(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    if(EV_ERROR & revents) {
        perror("got invalid event");
        return;
    }

    if(EV_READ & revents) {
        printf("read\n");

        while(readTTYACMPacket(watcher->fd));

        return;
    }
}

#define COMMAND_TYPE_TRANSLATE 0
#define DISTANCE_UNIT_IN_METERS_FACTOR 0.0001

void RobotServer_sendTranslateCommand(struct Command_Translate command_translate)
{
    float x = command_translate.x * DISTANCE_UNIT_IN_METERS_FACTOR;
    float y = command_translate.y * DISTANCE_UNIT_IN_METERS_FACTOR;

    uint8_t data[sizeof(float) * 2];

    memcpy(data, &x, sizeof(float));
    memcpy(data + sizeof(float), &y, sizeof(float));

    writeTTYACMPacket(COMMAND_TYPE_TRANSLATE, data, sizeof(data));
}

void RobotServer_sendRotateCommand(struct Command_Rotate command_rotate) {}
void RobotServer_sendLightRedLEDCommand(
    struct Command_LightRedLED command_light_red_led) {}
void RobotServer_sendLightGreenLEDCommand(
    struct Command_LightGreenLED command_light_green_led) {}
void RobotServer_sendRisePenCommand(struct Command_RisePen command_rise_pen) {}
void RobotServer_sendLowerPenCommand(struct Command_LowerPen command_lower_pen) {}
