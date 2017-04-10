#include <criterion/criterion.h>
#include <stdio.h>
#include "Logger.h"

struct Logger *logger;

void setup_logger(void)
{
    logger = Logger_new();
}

void teardown_logger(void)
{
    Logger_delete(logger);
}

void assertDataReceiverCallbacksAreDecoratedByLogger(struct DataReceiver_Callbacks data_receiver_callbacks)
{
    void (*updateWorld)(struct WorldCamera *, struct Communication_World) = &Logger_updateWorld;
    void (*updateWheelsTranslation)(struct Wheels *, struct Communication_Translation) = &Logger_updateWheelsTranslation;
    void (*updateWheelsRotation)(struct Wheels *, struct Communication_Rotation) = &Logger_updateWheelsRotation;
    void (*updateManchesterCode)(struct ManchesterCode *, struct Flags *,
                                 struct Communication_ManchesterCode) = &Logger_updateManchesterCode;
    void (*updateFlagsStartCycle)(struct Flags *) = &Logger_updateFlagsStartCycle;
    void (*updateFlagsStopExecution)(struct Flags *) = &Logger_updateFlagsStopExecution;
    void (*updateFlagsImageReceivedByStation)(struct Flags *) = &Logger_updateFlagsImageReceivedByStation;
    void (*updateFlagsPlannedTrajectoryReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsPlannedTrajectoryReceivedByStation;
    void (*updateFlagsReadyToStartSignalReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsReadyToStartSignalReceivedByStation;
    void (*updateFlagsReadyToDrawSignalReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsReadyToDrawSignalReceivedByStation;
    void (*updateFlagsEndOfCycleSignalReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsEndOfCycleSignalReceivedByStation;

    cr_assert_eq(data_receiver_callbacks.updateWorld, updateWorld);
    cr_assert_eq(data_receiver_callbacks.updateWheelsTranslation, updateWheelsTranslation);
    cr_assert_eq(data_receiver_callbacks.updateWheelsRotation, updateWheelsRotation);
    cr_assert_eq(data_receiver_callbacks.updateManchesterCode, updateManchesterCode);
    cr_assert_eq(data_receiver_callbacks.updateFlagsStartCycle, updateFlagsStartCycle);
    cr_assert_eq(data_receiver_callbacks.updateFlagsStopExecution, updateFlagsStopExecution);
    cr_assert_eq(data_receiver_callbacks.updateFlagsImageReceivedByStation, updateFlagsImageReceivedByStation);
    cr_assert_eq(data_receiver_callbacks.updateFlagsPlannedTrajectoryReceivedByStation,
                 updateFlagsPlannedTrajectoryReceivedByStation);
    cr_assert_eq(data_receiver_callbacks.updateFlagsReadyToStartSignalReceivedByStation,
                 updateFlagsReadyToStartSignalReceivedByStation);
    cr_assert_eq(data_receiver_callbacks.updateFlagsReadyToDrawSignalReceivedByStation,
                 updateFlagsReadyToDrawSignalReceivedByStation);
    cr_assert_eq(data_receiver_callbacks.updateFlagsEndOfCycleSignalReceivedByStation,
                 updateFlagsEndOfCycleSignalReceivedByStation);
}

Test(Logger, given_aLogger_when_startsLoggingDataReceiver_then_decoratedDataReceiverCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{

    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();

    struct DataReceiver_Callbacks from_logger = Logger_startLoggingDataReceiverAndReturnCallbacks(logger,
            data_receiver_callbacks);
    assertDataReceiverCallbacksAreDecoratedByLogger(from_logger);

}

Test(Logger, given_aLogger_when_stopsLoggingDataReceiver_then_originalDataReceiverCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();
    Logger_startLoggingDataReceiverAndReturnCallbacks(logger, data_receiver_callbacks);

    struct DataReceiver_Callbacks from_logger = Logger_stopLoggingDataReceiverAndReturnCallbacks(logger);

    cr_assert_eq(from_logger.updateWorld, data_receiver_callbacks.updateWorld);
    cr_assert_eq(from_logger.updateWheelsTranslation, data_receiver_callbacks.updateWheelsTranslation);
    cr_assert_eq(from_logger.updateWheelsRotation, data_receiver_callbacks.updateWheelsRotation);
    cr_assert_eq(from_logger.updateManchesterCode, data_receiver_callbacks.updateManchesterCode);
    cr_assert_eq(from_logger.updateFlagsStartCycle, data_receiver_callbacks.updateFlagsStartCycle);
    cr_assert_eq(from_logger.updateFlagsStopExecution, data_receiver_callbacks.updateFlagsStopExecution);
    cr_assert_eq(from_logger.updateFlagsImageReceivedByStation, data_receiver_callbacks.updateFlagsImageReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsPlannedTrajectoryReceivedByStation,
                 data_receiver_callbacks.updateFlagsPlannedTrajectoryReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsReadyToStartSignalReceivedByStation,
                 data_receiver_callbacks.updateFlagsReadyToStartSignalReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsReadyToDrawSignalReceivedByStation,
                 data_receiver_callbacks.updateFlagsReadyToDrawSignalReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsEndOfCycleSignalReceivedByStation,
                 data_receiver_callbacks.updateFlagsEndOfCycleSignalReceivedByStation);
}

void assertCommandSenderCallbacksAreDecoratedByLogger(struct CommandSender_Callbacks command_sender_callbacks)
{
    void (*sendTranslateCommand)(struct Command_Translate) = &Logger_sendTranslateCommand;
    void (*sendSpeedsCommand)(struct Command_Speeds) = &Logger_sendSpeedsCommand;
    void (*sendRotateCommand)(struct Command_Rotate) = &Logger_sendRotateCommand;
    void (*sendLightRedLEDCommand)(void) = &Logger_sendLightRedLEDCommand;
    void (*sendLightGreenLEDCommand)(void) = &Logger_sendLightGreenLEDCommand;
    void (*sendRisePenCommand)(void) = &Logger_sendRisePenCommand;
    void (*sendLowerPenCommand)(void) = &Logger_sendLowerPenCommand;
    void (*sendFetchManchesterCodeCommand)(void) = &Logger_sendFetchManchesterCode;
    void (*sendStopSendingManchesterSignalCommand)(void) = &Logger_sendStopSendingManchesterSignal;

    cr_assert_eq(command_sender_callbacks.sendTranslateCommand, sendTranslateCommand);
    cr_assert_eq(command_sender_callbacks.sendSpeedsCommand, sendSpeedsCommand);
    cr_assert_eq(command_sender_callbacks.sendRotateCommand, sendRotateCommand);
    cr_assert_eq(command_sender_callbacks.sendLightRedLEDCommand, sendLightRedLEDCommand);
    cr_assert_eq(command_sender_callbacks.sendLightGreenLEDCommand, sendLightGreenLEDCommand);
    cr_assert_eq(command_sender_callbacks.sendRisePenCommand, sendRisePenCommand);
    cr_assert_eq(command_sender_callbacks.sendLowerPenCommand, sendLowerPenCommand);
    cr_assert_eq(command_sender_callbacks.sendFetchManchesterCodeCommand, sendFetchManchesterCodeCommand);
    cr_assert_eq(command_sender_callbacks.sendStopSendingManchesterSignalCommand, sendStopSendingManchesterSignalCommand);
}

Test(Logger, given_aLogger_when_startsLoggingCommandSender_then_decoratedCommandSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{

    struct CommandSender_Callbacks callbacks = CommandSender_fetchCallbacksForRobot();
    struct CommandSender_Callbacks from_logger = Logger_startLoggingCommandSenderAndReturnCallbacks(logger, callbacks);
    assertCommandSenderCallbacksAreDecoratedByLogger(from_logger);

}

Test(Logger, given_aLogger_when_stopsLoggingCommandSender_then_originalCommandSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct CommandSender_Callbacks command_sender_callbacks = CommandSender_fetchCallbacksForRobot();
    Logger_startLoggingCommandSenderAndReturnCallbacks(logger, command_sender_callbacks);

    struct CommandSender_Callbacks from_logger = Logger_stopLoggingCommandSenderAndReturnCallbacks(logger);

    cr_assert_eq(from_logger.sendTranslateCommand, command_sender_callbacks.sendTranslateCommand);
    cr_assert_eq(from_logger.sendSpeedsCommand, command_sender_callbacks.sendSpeedsCommand);
    cr_assert_eq(from_logger.sendRotateCommand, command_sender_callbacks.sendRotateCommand);
    cr_assert_eq(from_logger.sendLightRedLEDCommand, command_sender_callbacks.sendLightRedLEDCommand);
    cr_assert_eq(from_logger.sendLightGreenLEDCommand, command_sender_callbacks.sendLightGreenLEDCommand);
    cr_assert_eq(from_logger.sendRisePenCommand, command_sender_callbacks.sendRisePenCommand);
    cr_assert_eq(from_logger.sendLowerPenCommand, command_sender_callbacks.sendLowerPenCommand);
    cr_assert_eq(from_logger.sendFetchManchesterCodeCommand, command_sender_callbacks.sendFetchManchesterCodeCommand);
    cr_assert_eq(from_logger.sendStopSendingManchesterSignalCommand,
                 command_sender_callbacks.sendStopSendingManchesterSignalCommand);
}

void assertDataSenderCallbacksAreDecoratedByLogger(struct DataSender_Callbacks data_sender_callbacks)
{
    void (*sendRobotPoseEstimate)(struct Pose *) = &Logger_sendRobotPoseEstimate;
    void (*sendImage)(IplImage *) = &Logger_sendImage;
    void (*sendPlannedTrajectory)(struct CoordinatesSequence *) = &Logger_sendPlannedTrajectory;
    void (*sendSignalReadyToStart)(void) = &Logger_sendSignalReadyToStart;
    void (*sendSignalReadyToDraw)(void) = &Logger_sendSignalReadyToDraw;
    void (*sendSignalEndOfCycle)(void) = &Logger_sendSignalEndOfCycle;

    cr_assert_eq(data_sender_callbacks.sendRobotPoseEstimate, sendRobotPoseEstimate);
    cr_assert_eq(data_sender_callbacks.sendImage, sendImage);
    cr_assert_eq(data_sender_callbacks.sendPlannedTrajectory, sendPlannedTrajectory);
    cr_assert_eq(data_sender_callbacks.sendSignalReadyToStart, sendSignalReadyToStart);
    cr_assert_eq(data_sender_callbacks.sendSignalReadyToDraw, sendSignalReadyToDraw);
    cr_assert_eq(data_sender_callbacks.sendSignalEndOfCycle, sendSignalEndOfCycle);
}

Test(Logger, given_aLogger_when_startsLoggingDataSender_then_decoratedDataSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{

    struct DataSender_Callbacks callbacks = DataSender_fetchCallbacksForRobot();
    struct DataSender_Callbacks from_logger = Logger_startLoggingDataSenderAndReturnCallbacks(logger, callbacks);
    assertDataSenderCallbacksAreDecoratedByLogger(from_logger);

}

Test(Logger, given_aLogger_when_stopsLoggingDataSender_then_originalDataSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct DataSender_Callbacks data_sender_callbacks = DataSender_fetchCallbacksForRobot();
    Logger_startLoggingDataSenderAndReturnCallbacks(logger, data_sender_callbacks);

    struct DataSender_Callbacks from_logger = Logger_stopLoggingDataSenderAndReturnCallbacks(logger);

    cr_assert_eq(from_logger.sendRobotPoseEstimate, data_sender_callbacks.sendRobotPoseEstimate);
    cr_assert_eq(from_logger.sendImage, data_sender_callbacks.sendImage);
    cr_assert_eq(from_logger.sendPlannedTrajectory, data_sender_callbacks.sendPlannedTrajectory);
    cr_assert_eq(from_logger.sendSignalReadyToStart, data_sender_callbacks.sendSignalReadyToStart);
    cr_assert_eq(from_logger.sendSignalEndOfCycle, data_sender_callbacks.sendSignalEndOfCycle);
    cr_assert_eq(from_logger.sendSignalReadyToDraw, data_sender_callbacks.sendSignalReadyToDraw);
}

Test(Logger, given_aRobot_when_askedToStartLoggingRobot_then_allTheCallbacksAreLogged
     , .init = setup_logger
     , .fini = teardown_logger)
{
    char *ttyACM = "/dev/null";
    const int port = 25895;
    struct Robot *robot = Robot_new();
    struct RobotServer *robot_server = RobotServer_new(robot, port, ttyACM);

    struct DataReceiver_Callbacks data_receiver_callbacks = Logger_startLoggingRobot(robot);
    struct DataSender_Callbacks data_sender_callbacks = robot->data_sender->data_callbacks;
    struct CommandSender_Callbacks command_sender_callbacks = robot->command_sender->command_callbacks;
    assertDataSenderCallbacksAreDecoratedByLogger(data_sender_callbacks);
    assertDataReceiverCallbacksAreDecoratedByLogger(data_receiver_callbacks);
    assertCommandSenderCallbacksAreDecoratedByLogger(command_sender_callbacks);
    RobotServer_delete(robot_server);
    Robot_delete(robot);
}
