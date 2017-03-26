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
void Logger_updateManchesterCode(struct ManchesterCode *manchester_code, struct Communication_ManchesterCode code_informations);
void Logger_updateFlagsStartCycle(struct Flags *flags);
void Logger_updateFlagsImageReceivedByStation(struct Flags *flags);
void Logger_updateFlagsPlannedTrajectoryReceivedByStation(struct Flags *flags);

void Logger_debugLog(char *text);
#endif // LOGGER_H_
