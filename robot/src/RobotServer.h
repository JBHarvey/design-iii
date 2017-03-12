#ifndef ROBOTSERVER_H_
#define ROBOTSERVER_H_

#include "network.h"
#include "DataReceiver.h"

struct RobotServer {
    struct Object *object;
    struct ev_loop *loop;
    struct Robot *robot;
    int port;
};
struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port);
void RobotServer_delete(struct RobotServer *robot_server);

void RobotServer_communicate(struct RobotServer *robot_server, unsigned int milliseconds);

#endif // ROBOTSERVER_H_
