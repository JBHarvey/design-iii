#ifndef ROBOTSERVER_H_
#define ROBOTSERVER_H_

#include "network.h"
#include "DataReceiver.h"
#include "CoordinatesSequence.h"

struct RobotServer {
    struct Object *object;
    struct ev_loop *loop;
    struct Robot *robot;
};
struct RobotServer *RobotServer_new(struct Robot *new_robot, int new_port, char *ttyacm_path);
void RobotServer_delete(struct RobotServer *robot_server);

void RobotServer_updateDataReceiverCallbacks(struct DataReceiver_Callbacks data_receiver_callbacks);
void RobotServer_communicate(struct RobotServer *robot_server);

_Bool writeTTYACMPacket(uint8_t type, uint8_t *data, unsigned int length);

void RobotServer_sendTranslateCommand(struct Command_Translate command_translate);

// THESE FUNCTIONS ARE NEITHER TESTED NOR WORKING YET.
// THIS WILL COME WITH INTEGRATION TESTS
void RobotServer_sendRotateCommand(struct Command_Rotate command_rotate);
void RobotServer_sendLightRedLEDCommand(void);
void RobotServer_sendLightGreenLEDCommand(void);
void RobotServer_sendRisePenCommand(void);
void RobotServer_sendLowerPenCommand(void);
void RobotServer_fetchManchesterCodeCommand(void);
void RobotServer_sendStopSendingManchesterSignalCommand(void);

void RobotServer_sendImageToStation(IplImage *image);
void RobotServer_sendPlannedTrajectoryToStation(struct CoordinatesSequence *coordinates_sequence);

#endif // ROBOTSERVER_H_
