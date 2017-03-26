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
}
