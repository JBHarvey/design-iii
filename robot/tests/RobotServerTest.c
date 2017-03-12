#include <criterion/criterion.h>
#include <stdio.h>
#include "RobotServer.h"

struct RobotServer *robot_server;
struct Robot *robot;
const int SERVER_PORT = 31653;

Test(RobotServer, creation_destruction)
{
    struct Robot *robot = Robot_new();
    robot_server = RobotServer_new(robot, SERVER_PORT);

    cr_assert(robot_server->robot == robot);
    cr_assert(robot_server->port == SERVER_PORT);

    Robot_delete(robot);
    RobotServer_delete(robot_server);
}

