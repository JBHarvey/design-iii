#ifndef STATION_INTERFACE_H_
#define STATION_INTERFACE_H_

/* Flag definitions */

enum ThreadStatus {TERMINATED, RUNNING};

enum ConnectionStatus {DISCONNECTED, CONNECTED};

void StationInterface_launch(int argc, char *argv[]);

int StationInterface_isRunning(void);

int StationInterface_isConnectedToRobot(void);

void StationInterface_setRobotConnectionStatusOn(void);

#endif // STATION_INTERFACE_H_
