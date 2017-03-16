#include <stdio.h>
#include "RobotServer.h"

struct RobotServer *robot_server;
const int port = 35794;

int main(int argc, char *argv[])
{
    struct Robot *robot = Robot_new();
    robot_server = RobotServer_new(robot, port);

    //    while(1) {
    //      RobotServer_communicate(robot_server, 75);
    printf("IT LIVES\n");
    //   }

    RobotServer_delete(robot_server);
    Robot_delete(robot);

    return 0;
}
