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

    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();

    struct DataReceiver_Callbacks from_logger = Logger_startLoggingDataReceiverAndReturnCallbacks(logger,
            data_receiver_callbacks);

    cr_assert_eq(from_logger.updateWorld, updateWorld);
    cr_assert_eq(from_logger.updateWheelsTranslation, updateWheelsTranslation);
    cr_assert_eq(from_logger.updateWheelsRotation, updateWheelsRotation);
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
}

/*
Test(Logger, given_aDefaultLogger_when_changesItsAction_then_itsActionIsChanged
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct Logger *logger = LoggerBuilder_default();
    void (*action)(struct Robot *) = &Robot_takePicture;
    Logger_changeAction(logger, action);

    cr_assert_eq(logger->action, action);

    Logger_delete(logger);
}

void LoggerActionTest(struct Robot *robot)
{
    Robot_takePicture(robot);
}

Test(Logger, given_aSpecificFunction_when_acts_then_theActionTakesPlace
     , .init = setup_logger
     , .fini = teardown_logger)
{
    struct Robot *robot = Robot_new();
    cr_assert(robot->current_state->flags->picture_taken == FALSE);

    struct Logger *logger = LoggerBuilder_build(
                                LoggerBuilder_withAction(&LoggerActionTest,
                                        LoggerBuilder_end()));

    Logger_act(logger, robot);

    cr_assert(robot->current_state->flags->picture_taken == TRUE);

    Logger_delete(logger);
    Robot_delete(robot);
}
*/
