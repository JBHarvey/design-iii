#ifndef STATION_CLIENT_H_
#define STATION_CLIENT_H_

#include "network.h"
#include "CommunicationStructures.h"
//#include "RobotReceiver.h"

struct StationClient {
    struct ev_loop *loop;
    unsigned short port;
    char *server_ip;
};

/*
struct RobotServer *RobotServer_initClient(char *ip, unsigned short port);
struct RobotServer *RobotServer_initServer(unsigned short port);
*/
struct StationClient *StationClient_new(int new_port, const char *server_ip);
void StationClient_delete(struct StationClient *station_client);
/*
void RobotServer_do(struct RobotServer *robot_server, unsigned int milliseconds);
void RobotServer_close(struct RobotServer *robot_server);
*/
#endif // STATION_CLIENT_H_
