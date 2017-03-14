#include <stdio.h>
#include "RobotServer.h"

struct RobotServer *robot_server;
const int port = 35794;
//const char *ttyACM = "test";
const char *ttyACM = "/dev/ttyACM0";

int main(int argc, char *argv[])
{
    struct Robot *robot = Robot_new();
    robot_server = RobotServer_new(robot, port, ttyACM);

    uint64_t data = 0;
    printf("writing packet\n");
    if(writeTTYACMPacket(robot_server, 1, &data, sizeof(data))) {
        printf("wrote packet\n");
    }

    while(1) {
        RobotServer_communicate(robot_server);
        usleep(1000000);
        printf("ok\n");
    }

    RobotServer_delete(robot_server);
    Robot_delete(robot);

    return 0;
}
