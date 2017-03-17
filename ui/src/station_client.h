#ifndef STATION_CLIENT_H_
#define STATION_CLIENT_H_

#include <gtk/gtk.h>
#include "network.h"
#include "CommunicationStructures.h"

struct StationClient {
    struct ev_loop *loop;
    unsigned short port;
    char *server_ip;
};

struct StationClient *StationClient_new(int new_port, const char *server_ip);

gpointer StationClient_init(struct StationClient *station_client);

void StationClient_delete(struct StationClient *station_client);

void sendStartPacket();

void sendContinuePacket();

#endif // STATION_CLIENT_H_
