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

Test(Logger, given_aLogger_when_startsLoggingDataReceiver_then_decoratedDataReceiverCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    void (*updateWorld)(struct WorldCamera *, struct Communication_World) = &Logger_updateWorld;
    void (*updateWheelsTranslation)(struct Wheels *, struct Communication_Translation) = &Logger_updateWheelsTranslation;
    void (*updateWheelsRotation)(struct Wheels *, struct Communication_Rotation) = &Logger_updateWheelsRotation;
    void (*updateManchesterCode)(struct ManchesterCode *,
                                 struct Communication_ManchesterCode) = &Logger_updateManchesterCode;
    void (*updateFlagsStartCycle)(struct Flags *) = &Logger_updateFlagsStartCycle;
    void (*updateFlagsImageReceivedByStation)(struct Flags *) = &Logger_updateFlagsImageReceivedByStation;
    void (*updateFlagsPlannedTrajectoryReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsPlannedTrajectoryReceivedByStation;
    void (*updateFlagsReadyToStartSignalReceivedByStation)(struct Flags *) =
        &Logger_updateFlagsReadyToStartSignalReceivedByStation;

    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();

    struct DataReceiver_Callbacks from_logger = Logger_startLoggingDataReceiverAndReturnCallbacks(logger,
            data_receiver_callbacks);

    cr_assert_eq(from_logger.updateWorld, updateWorld);
    cr_assert_eq(from_logger.updateWheelsTranslation, updateWheelsTranslation);
    cr_assert_eq(from_logger.updateWheelsRotation, updateWheelsRotation);
    cr_assert_eq(from_logger.updateManchesterCode, updateManchesterCode);
    cr_assert_eq(from_logger.updateFlagsStartCycle, updateFlagsStartCycle);
    cr_assert_eq(from_logger.updateFlagsImageReceivedByStation, updateFlagsImageReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsPlannedTrajectoryReceivedByStation, updateFlagsPlannedTrajectoryReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsReadyToStartSignalReceivedByStation,
                 updateFlagsReadyToStartSignalReceivedByStation);
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
    cr_assert_eq(from_logger.updateFlagsImageReceivedByStation, data_receiver_callbacks.updateFlagsImageReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsPlannedTrajectoryReceivedByStation,
                 data_receiver_callbacks.updateFlagsPlannedTrajectoryReceivedByStation);
    cr_assert_eq(from_logger.updateFlagsReadyToStartSignalReceivedByStation,
                 data_receiver_callbacks.updateFlagsReadyToStartSignalReceivedByStation);

}

Test(Logger, given_aLogger_when_startsLoggingCommandSender_then_decoratedCommandSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    void (*sendTranslateCommand)(struct Command_Translate) = &Logger_sendTranslateCommand;
    void (*sendRotateCommand)(struct Command_Rotate) = &Logger_sendRotateCommand;
    void (*sendLightRedLEDCommand)(void) = &Logger_sendLightRedLEDCommand;
    void (*sendLightGreenLEDCommand)(void) = &Logger_sendLightGreenLEDCommand;
    void (*sendRisePenCommand)(void) = &Logger_sendRisePenCommand;
    void (*sendLowerPenCommand)(void) = &Logger_sendLowerPenCommand;
    void (*sendFetchManchesterCodeCommand)(void) = &Logger_sendFetchManchesterCode;
    void (*sendStopSendingManchesterSignalCommand)(void) = &Logger_sendStopSendingManchesterSignal;

    struct CommandSender_Callbacks callbacks = CommandSender_fetchCallbacksForRobot();
    struct CommandSender_Callbacks from_logger = Logger_startLoggingCommandSenderAndReturnCallbacks(logger, callbacks);

    cr_assert_eq(from_logger.sendTranslateCommand, sendTranslateCommand);
    cr_assert_eq(from_logger.sendRotateCommand, sendRotateCommand);
    cr_assert_eq(from_logger.sendLightRedLEDCommand, sendLightRedLEDCommand);
    cr_assert_eq(from_logger.sendLightGreenLEDCommand, sendLightGreenLEDCommand);
    cr_assert_eq(from_logger.sendRisePenCommand, sendRisePenCommand);
    cr_assert_eq(from_logger.sendLowerPenCommand, sendLowerPenCommand);
    cr_assert_eq(from_logger.sendFetchManchesterCodeCommand, sendFetchManchesterCodeCommand);
    cr_assert_eq(from_logger.sendStopSendingManchesterSignalCommand, sendStopSendingManchesterSignalCommand);

}

Test(Logger, given_aLogger_when_stopsLoggingCommandSender_then_originalCommandSenderCallbacksAreReturned
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct CommandSender_Callbacks command_sender_callbacks = CommandSender_fetchCallbacksForRobot();
    Logger_startLoggingCommandSenderAndReturnCallbacks(logger, command_sender_callbacks);

    struct CommandSender_Callbacks from_logger = Logger_stopLoggingCommandSenderAndReturnCallbacks(logger);

    cr_assert_eq(from_logger.sendTranslateCommand, command_sender_callbacks.sendTranslateCommand);
    cr_assert_eq(from_logger.sendRotateCommand, command_sender_callbacks.sendRotateCommand);
    cr_assert_eq(from_logger.sendLightRedLEDCommand, command_sender_callbacks.sendLightRedLEDCommand);
    cr_assert_eq(from_logger.sendLightGreenLEDCommand, command_sender_callbacks.sendLightGreenLEDCommand);
    cr_assert_eq(from_logger.sendRisePenCommand, command_sender_callbacks.sendRisePenCommand);
    cr_assert_eq(from_logger.sendLowerPenCommand, command_sender_callbacks.sendLowerPenCommand);
    cr_assert_eq(from_logger.sendFetchManchesterCodeCommand, command_sender_callbacks.sendFetchManchesterCodeCommand);
    cr_assert_eq(from_logger.sendStopSendingManchesterSignalCommand,
                 command_sender_callbacks.sendStopSendingManchesterSignalCommand);
}
