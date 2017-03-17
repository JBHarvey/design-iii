#ifndef STATION_INTERFACE_H_
#define STATION_INTERFACE_H_

/* Flag definitions */

enum ThreadStatus {TERMINATED, RUNNING};

enum ConnectionStatus {DISCONNECTED, CONNECTED};

void StationInterface_launch(int argc, char *argv[]);

#endif // STATION_INTERFACE_H_
