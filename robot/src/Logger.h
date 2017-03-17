#ifndef LOGGER_H_
#define LOGGER_H_

#include "RobotServer.h"

struct Logger;

struct Logger *Logger_new(void);
void Logger_delete(struct Logger *logger);

struct DataReceiver_Callbacks Logger_startLoggingDataReceiverAndReturnCallbacks(struct Logger *logger, struct DataReceiver_Callbacks callbacks_to_log);
struct DataReceiver_Callbacks Logger_stopLoggingDataReceiverAndReturnCallbacks(struct Logger *logger);

void Logger_updateWorld(struct WorldCamera *world_camera, struct Communication_World world);
void Logger_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation);
void Logger_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation);

#endif // LOGGER_H_
