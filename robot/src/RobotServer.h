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

bool writeTTYACMPacket(struct RobotServer *robot_server, uint8_t type, uint8_t *data, unsigned int length);

// THESE FUNCTIONS ARE NEITHER TESTED NOR WORKING YET.
// THIS WILL COME WITH INTEGRATION TESTS
void RobotServer_sendTranslateCommand(struct Command_Translate command_translate);
void RobotServer_sendRotateCommand(struct Command_Rotate command_rotate);
void RobotServer_sendLightRedLEDCommand(struct Command_LightRedLED command_light_red_led);
void RobotServer_sendLightGreenLEDCommand(struct Command_LightGreenLED command_light_green_led);
void RobotServer_sendRisePenCommand(struct Command_RisePen command_rise_pen);
void RobotServer_sendLowerPenCommand(struct Command_LowerPen command_lower_pen);

#endif // ROBOTSERVER_H_
