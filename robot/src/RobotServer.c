#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <termios.h>
#include "opencv2/imgcodecs/imgcodecs_c.h"

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

void RobotServer_updateDataReceiverCallbacks(struct DataReceiver_Callbacks data_receiver_callbacks)
{
    reception_callbacks = data_receiver_callbacks;
}

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

#define BASE_UNIT_TO_MILLIMETERS 10
void RobotServer_sendRobotPoseEstimate(struct Pose *pose)
{
    struct Communication_Pose communication_pose = {.coordinates = {
            .x = (pose->coordinates->x) / BASE_UNIT_TO_MILLIMETERS,
            .y = (pose->coordinates->y) / BASE_UNIT_TO_MILLIMETERS
        },
        .theta = (pose->angle->theta) * ANGLE_BASE_UNIT
    };
    uint8_t data[1 + sizeof(communication_pose)];
    data[0] = DATA_ESTIMATED_ROBOT_POSITION;
    memcpy(data + 1, &communication_pose, sizeof(communication_pose));
    addPacket(data, sizeof(data));
}

void RobotServer_sendImageToStation(IplImage *image)
{
    CvMat *image_data = cvEncodeImage(".jpg", image, 0);
    uint8_t data[1 + image_data->cols];
    data[0] = DATA_IMAGE;
    memcpy(data + 1, image_data->data.ptr, image_data->cols);
    addPacket(data, sizeof(data));
    cvReleaseMat(&image_data);
}


void RobotServer_sendPlannedTrajectoryToStation(struct CoordinatesSequence *coordinates_sequence)
{
    unsigned int size = CoordinatesSequence_size(coordinates_sequence);

    struct Communication_Coordinates coordinates[size];

    struct CoordinatesSequence *coordinates_sequence_current = coordinates_sequence;

    unsigned int i;

    for(i = 0; i < size; ++i) {
        coordinates[i].x = (int)(coordinates_sequence_current->coordinates->x / BASE_UNIT_TO_MILLIMETERS);
        coordinates[i].y = (int)(coordinates_sequence_current->coordinates->y / BASE_UNIT_TO_MILLIMETERS);
        coordinates_sequence_current = coordinates_sequence_current->next_element;
    }

    uint8_t data[1 + sizeof(coordinates)];
    data[0] = DATA_PLANNED_TRAJECTORY;
    memcpy(data + 1, coordinates, sizeof(coordinates));
    addPacket(data, sizeof(data));
}

void RobotServer_sendSignalReadyToStart(void)
{
    uint8_t data[1];
    data[0] = SIGNAL_READY_TO_START;
    addPacket(data, sizeof(data));
}

void RobotServer_sendSignalReadyToDraw(void)
{
    uint8_t data[1];
    data[0] = SIGNAL_READY_TO_DRAW;
    addPacket(data, sizeof(data));
}

void RobotServer_sendSignalEndOfCycle(void)
{
    uint8_t data[1];
    data[0] = SIGNAL_END_OF_CYCLE;
    addPacket(data, sizeof(data));
}

void handleReceivedPacket(uint8_t *data, uint32_t length)
{
    if(length == 0) {
        return;
    }

    printf("received packet %u of length %u\n", data[0], length);

    struct Flags *flags = robot_server->robot->current_state->flags;
    struct WorldCamera *world_camera = robot_server->robot->world_camera;

    switch(data[0]) {

        case COMMAND_START_CYCLE:
            reception_callbacks.updateFlagsStartCycle(flags);
            break;

        case COMMAND_STOP_EXECUTION:
            reception_callbacks.updateFlagsStopExecution(flags);
            break;

        case ACK_IMAGE_RECEIVED:
            reception_callbacks.updateFlagsImageReceivedByStation(flags);
            break;

        case ACK_PLANNED_TRAJECTORY_RECEIVED:
            reception_callbacks.updateFlagsPlannedTrajectoryReceivedByStation(flags);
            break;

        case ACK_READY_TO_START_RECEIVED:
            reception_callbacks.updateFlagsReadyToStartSignalReceivedByStation(flags);
            break;

        case ACK_READY_TO_DRAW_RECEIVED:
            reception_callbacks.updateFlagsReadyToDrawSignalReceivedByStation(flags);
            break;

        case ACK_END_OF_CYCLE_RECEIVED:
            reception_callbacks.updateFlagsEndOfCycleSignalReceivedByStation(flags);
            break;

        case DATA_WORLD:
            if(length != (sizeof(struct Communication_World) + 1)) {
                printf("wrong struct Communication_World length\n");
                break;
            }

            struct Communication_World communication_world;

            memcpy(&communication_world, data + 1, sizeof(struct Communication_World));

            reception_callbacks.updateWorld(world_camera, communication_world);

            break;

        case DEBUG_TTY_ACM_SEND: {
                if(length >= 2) {
                    printf("send tty packet %u: %u\n", data[1], length - 2);
                    writeTTYACMPacket(data[1], data + 2, length - 2);
                }

                break;
            }

        case DEBUG_CAMERA_GET_FIGURE: {
                IplImage *image;
                struct CoordinatesSequence *coordinates_sequence;
                coordinates_sequence = OnboardCamera_extractTrajectoryFromImage(&image);

                if(coordinates_sequence) {
                    RobotServer_sendImageToStation(image);
                    OnboardCamera_deleteImage(&image);
                    CoordinatesSequence_delete(coordinates_sequence);
                }

                break;
            }

        case DEBUG_SEND_MANCHESTER: {
                if(length == sizeof(struct Communication_ManchesterCode) + 1) {
                    struct Communication_ManchesterCode communication_manchester_code;
                    memcpy(&communication_manchester_code, data + 1, length - 1);
                    reception_callbacks.updateManchesterCode(robot_server->robot->manchester_code,
                            robot_server->robot->current_state->flags,
                            communication_manchester_code);
                }

                break;
            }
    }
}


#define PHYSICAL_FEEDBACK_TRANSLATION 100
#define PHYSICAL_FEEDBACK_ROTATION 101
#define PHYSICAL_ACK_RED_LED 102
#define PHYSICAL_ACK_GREEN_LED 103
#define PHYSICAL_ACK_RISE_PEN 104
#define PHYSICAL_ACK_LOWER_PEN 105
#define MANCHESTER_CODE_DECODED 106
#define PHYSICAL_ACK_STOP_SENDING_SIGNAL 107
#define INFRARED_DATA 108
#define INFRARED_VOLTAGE_BASE_UNIT 0.00001

struct __attribute__((__packed__)) TransitionManchester {
    uint8_t painting_number;
    uint8_t scale_factor;
    char orientation;
};

struct __attribute__((__packed__)) TransitionTranslation {
    float travelled_x;
    float travelled_y;
    float speed_x;
    float speed_y;
};

struct __attribute__((__packed__)) TransitionRotation {
    float travelled_radiants;
    float speed_radiants_second;
};


static void handleTTYACMPacket(uint8_t type, uint8_t *data, uint8_t length)
{
    // TODO : ADD:
    // Wheels Translation
    // Wheels Rotation
    printf("packet type: %u length: %u\n", type, length);

    switch(type) {

        case MANCHESTER_CODE_DECODED:

            if(length != (sizeof(struct TransitionManchester))) {
                printf("wrong struct TransitionManchester length\n");
                break;
            }

            struct TransitionManchester transition_manchester;

            memcpy(&transition_manchester, data, sizeof(struct TransitionManchester));

            struct Communication_ManchesterCode communication_manchester_code = {
                .painting_number = (int) transition_manchester.painting_number,
                .scale_factor = (int) transition_manchester.scale_factor,
                .orientation = transition_manchester.orientation
            };

            reception_callbacks.updateManchesterCode(robot_server->robot->manchester_code,
                    robot_server->robot->current_state->flags,
                    communication_manchester_code);

            break;

        case PHYSICAL_FEEDBACK_TRANSLATION: {
                if(length == sizeof(struct TransitionTranslation)) {
                    struct TransitionTranslation transition_translation;
                    memcpy(&transition_translation, data, sizeof(struct TransitionTranslation));
                    struct Communication_Translation communication_translation = {
                        .movement.x = transition_translation.travelled_x / SPEEDS_BASE_UNIT,
                        .movement.y = transition_translation.travelled_y / SPEEDS_BASE_UNIT,
                        .speeds.x = transition_translation.speed_x / SPEEDS_BASE_UNIT,
                        .speeds.y = transition_translation.speed_y / SPEEDS_BASE_UNIT
                    };

                    printf("TRANS_MOVE_X: %f, TRANS_MOVE_Y: %f TRANS_SPEED_X: %f, TRANS_SPEED_Y: %f\n", transition_translation.travelled_x,
                           transition_translation.travelled_y, transition_translation.speed_x, transition_translation.speed_y);
                    reception_callbacks.updateWheelsTranslation(robot_server->robot->wheels, communication_translation);
                } else {
                    printf("wrong struct Communication_Translation length\n");
                }

                break;
            }

        case PHYSICAL_FEEDBACK_ROTATION: {
                if(length == sizeof(struct TransitionRotation)) {
                    struct TransitionRotation transition_rotation;
                    memcpy(&transition_rotation, data, sizeof(struct TransitionRotation));
                    struct Communication_Rotation communication_rotation = {
                        .gamma = transition_rotation.travelled_radiants / ANGLE_BASE_UNIT,
                        .theta = transition_rotation.speed_radiants_second / ANGLE_BASE_UNIT
                    };

                    printf("ROT: travelled: %f speed: %f\n", transition_rotation.travelled_radiants,
                           transition_rotation.speed_radiants_second);

                    reception_callbacks.updateWheelsRotation(robot_server->robot->wheels, communication_rotation);
                } else {
                    printf("wrong struct Communication_Rotation length\n");
                }
            }

        case INFRARED_DATA:
            /*
             * Three floats for a total of 12 octets
             * The data order is the following:
             * North
             * West
             * East
             */
            break;

    };
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

void TTYACMCallback(struct ev_loop * loop, struct ev_io * watcher, int revents)
{
    if(EV_ERROR & revents) {
        perror("got invalid event");
        return;
    }

    if(EV_READ & revents) {
        while(readTTYACMPacket(watcher->fd));

        return;
    }
}

#define COMMAND_TYPE_TRANSLATE 0
#define COMMAND_TYPE_ROTATE 1
#define COMMAND_TYPE_RED_LED 2
#define COMMAND_TYPE_GREEN_LED 3
#define COMMAND_TYPE_RISE_PEN 4
#define COMMAND_TYPE_LOWER_PEN 5
#define COMMAND_TYPE_FETCH_MANCHESTER 6
#define COMMAND_TYPE_STOP_SIGNAL 7
#define COMMAND_TYPE_SPEEDS 8

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

void RobotServer_sendRotateCommand(struct Command_Rotate command_rotate)
{
    float theta = command_rotate.theta * ANGLE_BASE_UNIT;

    uint8_t data[sizeof(float)];

    memcpy(data, &theta, sizeof(float));

    writeTTYACMPacket(COMMAND_TYPE_ROTATE, data, sizeof(data));
}

#define ACTION_ONLY_COMMAND_LENGTH 0

void RobotServer_sendRisePenCommand(void)
{
    writeTTYACMPacket(COMMAND_TYPE_RISE_PEN, 0, ACTION_ONLY_COMMAND_LENGTH);
}

void RobotServer_sendLowerPenCommand(void)
{
    writeTTYACMPacket(COMMAND_TYPE_LOWER_PEN, 0, ACTION_ONLY_COMMAND_LENGTH);
}

void RobotServer_sendLightRedLEDCommand(void)
{
    writeTTYACMPacket(COMMAND_TYPE_RED_LED, 0, ACTION_ONLY_COMMAND_LENGTH);
}

void RobotServer_sendLightGreenLEDCommand(void)
{
    writeTTYACMPacket(COMMAND_TYPE_GREEN_LED, 0, ACTION_ONLY_COMMAND_LENGTH);
}

void RobotServer_fetchManchesterCodeCommand(void)
{
    writeTTYACMPacket(COMMAND_TYPE_FETCH_MANCHESTER, 0, ACTION_ONLY_COMMAND_LENGTH);
}
// all of these have the command type + the ACTION_ONLY_COMMAND_LENGTH
void RobotServer_sendStopSendingManchesterSignalCommand(void) {}

void RobotServer_sendSpeedsCommand(struct Command_Speeds command_speeds)
{
    float x = command_speeds.x * SPEEDS_BASE_UNIT;
    float y = command_speeds.y * SPEEDS_BASE_UNIT;

    uint8_t data[sizeof(float) * 2];

    memcpy(data, &x, sizeof(float));
    memcpy(data + sizeof(float), &y, sizeof(float));

    writeTTYACMPacket(COMMAND_TYPE_SPEEDS, data, sizeof(data));
}
