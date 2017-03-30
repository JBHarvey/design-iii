#ifndef LOGGER_H_
#define LOGGER_H_

#include "DataSender.h"

struct Logger;

struct Logger *Logger_new(void);
void Logger_delete(struct Logger *logger);
struct DataReceiver_Callbacks Logger_startLoggingRobot(struct Robot *robot);

struct DataReceiver_Callbacks Logger_startLoggingDataReceiverAndReturnCallbacks(struct Logger *logger, struct DataReceiver_Callbacks callbacks_to_log);
struct DataReceiver_Callbacks Logger_stopLoggingDataReceiverAndReturnCallbacks(struct Logger *logger);
struct CommandSender_Callbacks Logger_startLoggingCommandSenderAndReturnCallbacks(struct Logger *logger, struct CommandSender_Callbacks callbacks_to_log);
struct CommandSender_Callbacks Logger_stopLoggingCommandSenderAndReturnCallbacks(struct Logger *logger);
struct DataSender_Callbacks Logger_startLoggingDataSenderAndReturnCallbacks(struct Logger *logger, struct DataSender_Callbacks callbacks_to_log);
struct DataSender_Callbacks Logger_stopLoggingDataSenderAndReturnCallbacks(struct Logger *logger);

// DataSender Calls
void Logger_updateWorld(struct WorldCamera *world_camera, struct Communication_World world);
void Logger_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation);
void Logger_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation);
void Logger_updateManchesterCode(struct ManchesterCode *manchester_code, struct Communication_ManchesterCode code_informations);
void Logger_updateFlagsStartCycle(struct Flags *flags);
void Logger_updateFlagsImageReceivedByStation(struct Flags *flags);
void Logger_updateFlagsPlannedTrajectoryReceivedByStation(struct Flags *flags);
void Logger_updateFlagsReadyToStartSignalReceivedByStation(struct Flags *flags);
void Logger_updateFlagsReadyToDrawSignalReceivedByStation(struct Flags *flags);
void Logger_updateFlagsEndOfCycleSignalReceivedByStation(struct Flags *flags);

// CommandSender Calls
void Logger_sendTranslateCommand(struct Command_Translate translate_command);
void Logger_sendSpeedsCommand(struct Command_Speeds speeds_command);
void Logger_sendRotateCommand(struct Command_Rotate rotate_command);
void Logger_sendLightRedLEDCommand(void);
void Logger_sendLightGreenLEDCommand(void);
void Logger_sendRisePenCommand(void);
void Logger_sendLowerPenCommand(void);
void Logger_sendFetchManchesterCode(void);
void Logger_sendStopSendingManchesterSignal(void);

// DataSender Calls
void Logger_sendRobotPoseEstimate(struct Pose *pose);
void Logger_sendImage(IplImage *image);
void Logger_sendPlannedTrajectory(struct CoordinatesSequence *coordinates_sequence);
void Logger_sendSignalReadyToStart(void);
void Logger_sendSignalReadyToDraw(void);
void Logger_sendSignalEndOfCycle(void);

#endif // LOGGER_H_
