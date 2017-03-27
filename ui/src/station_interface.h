#ifndef STATION_INTERFACE_H_
#define STATION_INTERFACE_H_

#include <gtk/gtk.h>

/* Flag definitions */

enum ThreadStatus {TERMINATED, RUNNING};

enum ConnectionStatus {DISCONNECTED, CONNECTED};

void StationInterface_launch(int argc, char *argv[]);

int StationInterface_isRunning(void);

int StationInterface_isConnectedToRobot(void);

void StationInterface_setRobotConnectionStatusOn(void);

void StationInterface_initializeStartCycleButton(GtkWidget* widget);

void StationInterface_activateStartCycleButton(void);

#endif // STATION_INTERFACE_H_
