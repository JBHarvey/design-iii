#ifndef STATION_CLIENT_H_
#define STATION_CLIENT_H_

#include <gtk/gtk.h>
#include "network.h"
#include "CommunicationStructures.h"
//#include "RobotReceiver.h"

struct StationClient {
    struct ev_loop *loop;
    unsigned short port;
    char *server_ip;
};

struct StationClient *StationClient_new(int new_port, const char *server_ip);
gboolean StationClient_init(struct StationClient *station_client);
void StationClient_delete(struct StationClient *station_client);
/*
void RobotServer_do(struct RobotServer *robot_server, unsigned int milliseconds);
*/
#endif // STATION_CLIENT_H_
