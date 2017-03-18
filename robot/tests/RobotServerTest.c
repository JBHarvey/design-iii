#include <criterion/criterion.h>
#include <stdio.h>
#include "RobotServer.h"

struct RobotServer *robot_server;
struct Robot *robot;
const int SERVER_PORT = 35794;

void setup_RobotServer(void)
{
    char *TEST_TTY_ACM = "/dev/null";
    robot = Robot_new();
    robot_server = RobotServer_new(robot, SERVER_PORT, TEST_TTY_ACM);
}

void teardown_Robot_Server(void)
{
    Robot_delete(robot);
    RobotServer_delete(robot_server);
}

Test(RobotServer, creation_destruction
     , .init = setup_RobotServer
     , .fini = teardown_Robot_Server)
{
    cr_assert(robot_server->robot == robot);
}
