#ifndef ROBOTSERVER_H_
#define ROBOTSERVER_H_

#include "network.h"
#include "DataReceiver.h"

struct RobotServer {
    struct Object *object;
    struct ev_loop *loop;
    struct Robot *robot;
    int fd_ttyACM;
    int port;
};
struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port, char *ttyacm_path);
void RobotServer_delete(struct RobotServer *robot_server);

void RobotServer_communicate(struct RobotServer *robot_server);

_Bool writeTTYACMPacket(struct RobotServer *robot_server, uint8_t type, uint8_t *data, unsigned int length);

/*
void RobotServer_do(struct RobotServer *robot_server, unsigned int milliseconds);

void sendWorldToRobot(struct Communication_World communication_world);

   */
#endif // ROBOTSERVER_H_
