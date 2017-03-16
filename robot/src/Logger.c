#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include "Logger.h"

struct Logger {
    struct Object *object;
    struct DataReceiver_Callbacks original_data_receiver_callbacks;
};

struct Logger *Logger_new(void)
{
    struct Object *new_object = Object_new();
    struct DataReceiver_Callbacks new_data_receiver_callbacks = DataReceiver_fetchCallbacks();
    struct Logger *pointer = (struct Logger *) malloc(sizeof(struct Logger));

    pointer->object = new_object;
    pointer->original_data_receiver_callbacks = new_data_receiver_callbacks;

    return pointer;
}

void Logger_delete(struct Logger *logger)
{
    Object_removeOneReference(logger->object);

    if(Object_canBeDeleted(logger->object)) {
        Object_delete(logger->object);
        free(logger);
    }
}

void Logger_dummyAction(struct Robot *robot) {}

struct DataReceiver_Callbacks Logger_startLoggingDataReceiverAndReturnCallbacks(struct Logger *logger)
{
    struct DataReceiver_Callbacks data_receiver_callbacks_with_logging = {
        .updateWorld = &Logger_updateWorld,
        .updateWheelsTranslation = &Logger_updateWheelsTranslation,
        .updateWheelsRotation = &Logger_updateWheelsRotation
    };
    return data_receiver_callbacks_with_logging;
}
struct DataReceiver_Callbacks Logger_stopLoggingDataReceiverAndReturnCallbacks(struct Logger *logger)
{
    return logger->original_data_receiver_callbacks;
}

void Logger_updateWorld(struct WorldCamera *world_camera, struct Communication_World world) {}
void Logger_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation) {}
void Logger_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation) {}

/*
  openlog("slog", LOG_PID|LOG_CONS, LOG_USER);
 syslog(LOG_DEBUG, "A different kind of Hello world ... ");
 closelog();
*/

/* This here is present for safety purposes.
 * It prevents action of doing anything
 * if it isn't initialised.

void Logger_changeAction(struct Logger *logger, void (*new_action)(struct Robot *))
{
    logger->action = new_action;
}

void Logger_act(struct Logger *logger, struct Robot *robot)
{
    (*logger->action)(robot);
}
 * */
