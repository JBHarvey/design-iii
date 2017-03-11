#ifndef ROBOTSERVER_H_
#define ROBOTSERVER_H_

#include "network.h"
//#include "RobotReceiver.h"

struct RobotServer {
    struct Object *object;
    struct ev_loop *loop;
    struct Robot *robot;
    int port;
};
/*
struct RobotServer *RobotServer_initClient(char *ip, unsigned short port);
struct RobotServer *RobotServer_initServer(unsigned short port);
*/
struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port);
void RobotServer_delete(struct RobotServer *robot_server);

/*
void RobotServer_do(struct RobotServer *robot_server, unsigned int milliseconds);
void RobotServer_close(struct RobotServer *robot_server);
*/
/*
FROM Protocol:

void sendStartPacket();
void sendContinuePacket();
void sendWorldToRobot(struct Communication_World communication_world);


   */
#endif // ROBOTSERVER_H_
