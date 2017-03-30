#include <stdio.h>
#include <unistd.h>
#include "Logger.h"

static const char *LOGGER_EVENT = "Logger event ---> ";
static const char *TAB = "    ";
static const char *ITEM = " -- ";
static const char *SUB = " |- ";
static const char *SUBITEM = " |---|- ";

struct Logger {
    struct Object *object;
    struct DataReceiver_Callbacks original_data_receiver_callbacks;
    struct CommandSender_Callbacks original_command_sender_callbacks;
    struct DataSender_Callbacks original_data_sender_callbacks;
    FILE *log_file;
};

struct Logger *Logger_new(void)
{
    struct Object *new_object = Object_new();
    struct DataReceiver_Callbacks new_data_receiver_callbacks = DataReceiver_fetchCallbacks();
    struct CommandSender_Callbacks new_command_sender_callbacks = CommandSender_fetchCallbacksForRobot();
    struct DataSender_Callbacks new_data_sender_callbacks = DataSender_fetchCallbacksForRobot();

    FILE *new_log_file = fopen("RobotLogs.log", "a+"); // a+ (create + append)
    fprintf(new_log_file, "\n\n\n%s%s------------ NEW EXECUTION ------------\n", TAB, TAB);
    setbuf(new_log_file, NULL);

    struct Logger *pointer = malloc(sizeof(struct Logger));

    pointer->object = new_object;
    pointer->original_data_receiver_callbacks = new_data_receiver_callbacks;
    pointer->original_command_sender_callbacks = new_command_sender_callbacks;
    pointer->original_data_sender_callbacks = new_data_sender_callbacks;
    pointer->log_file = new_log_file;

    return pointer;
}

void Logger_delete(struct Logger *logger)
{
    Object_removeOneReference(logger->object);

    if(Object_canBeDeleted(logger->object)) {
        Object_delete(logger->object);
        fprintf(logger->log_file, "\n%s%s------------ END OF EXECUTION ------------\n\n", TAB, TAB);
        fclose(logger->log_file);
        free(logger);
    }
}

struct DataReceiver_Callbacks Logger_startLoggingRobot(struct Robot *robot)
{
    struct DataReceiver_Callbacks data_receiver_callbacks = DataReceiver_fetchCallbacks();
    struct CommandSender_Callbacks command_sender_callbacks = CommandSender_fetchCallbacksForRobot();
    struct DataSender_Callbacks data_sender_callbacks = DataSender_fetchCallbacksForRobot();
    data_receiver_callbacks = Logger_startLoggingDataReceiverAndReturnCallbacks(robot->logger, data_receiver_callbacks);
    command_sender_callbacks = Logger_startLoggingCommandSenderAndReturnCallbacks(robot->logger, command_sender_callbacks);
    data_sender_callbacks = Logger_startLoggingDataSenderAndReturnCallbacks(robot->logger, data_sender_callbacks);

    RobotServer_updateDataReceiverCallbacks(data_receiver_callbacks);
    CommandSender_changeTarget(robot->command_sender, command_sender_callbacks);
    DataSender_changeTarget(robot->data_sender, data_sender_callbacks);
    return data_receiver_callbacks;
}

static struct Logger *file_logger;

static void robotLog(const char *text)
{
    fprintf(file_logger->log_file, "\n%s%s%s %s", ITEM, ITEM, LOGGER_EVENT, text);
}

struct DataReceiver_Callbacks Logger_startLoggingDataReceiverAndReturnCallbacks(struct Logger *logger,
        struct DataReceiver_Callbacks callbacks_to_log)
{
    file_logger = logger;

    robotLog("Start of DataReceiver callbacks logging.\n");

    logger->original_data_receiver_callbacks = callbacks_to_log;

    struct DataReceiver_Callbacks data_receiver_callbacks_with_logging = {
        .updateWorld = &Logger_updateWorld,
        .updateWheelsTranslation = &Logger_updateWheelsTranslation,
        .updateWheelsRotation = &Logger_updateWheelsRotation,
        .updateManchesterCode = &Logger_updateManchesterCode,
        .updateFlagsStartCycle = &Logger_updateFlagsStartCycle,
        .updateFlagsImageReceivedByStation = &Logger_updateFlagsImageReceivedByStation,
        .updateFlagsPlannedTrajectoryReceivedByStation = &Logger_updateFlagsPlannedTrajectoryReceivedByStation,
        .updateFlagsReadyToStartSignalReceivedByStation = &Logger_updateFlagsReadyToStartSignalReceivedByStation,
        .updateFlagsReadyToDrawSignalReceivedByStation = &Logger_updateFlagsReadyToDrawSignalReceivedByStation,
        .updateFlagsEndOfCycleSignalReceivedByStation = &Logger_updateFlagsEndOfCycleSignalReceivedByStation
    };
    return data_receiver_callbacks_with_logging;
}

struct DataReceiver_Callbacks Logger_stopLoggingDataReceiverAndReturnCallbacks(struct Logger *logger)
{
    robotLog("End of DataReceiver callbacks logging.\n");
    return logger->original_data_receiver_callbacks;
}

struct CommandSender_Callbacks Logger_startLoggingCommandSenderAndReturnCallbacks(struct Logger *logger,
        struct CommandSender_Callbacks callbacks_to_log)
{
    file_logger = logger;

    robotLog("Start of CommandSender callbacks logging.\n");

    logger->original_command_sender_callbacks = callbacks_to_log;

    struct CommandSender_Callbacks command_sender_callbacks_with_logging = {
        .sendTranslateCommand = &Logger_sendTranslateCommand,
        .sendSpeedsCommand = &Logger_sendSpeedsCommand,
        .sendRotateCommand = &Logger_sendRotateCommand,
        .sendLightRedLEDCommand = &Logger_sendLightRedLEDCommand,
        .sendLightGreenLEDCommand = &Logger_sendLightGreenLEDCommand,
        .sendRisePenCommand = &Logger_sendRisePenCommand,
        .sendLowerPenCommand = &Logger_sendLowerPenCommand,
        .sendFetchManchesterCodeCommand = &Logger_sendFetchManchesterCode,
        .sendStopSendingManchesterSignalCommand = &Logger_sendStopSendingManchesterSignal
    };
    return command_sender_callbacks_with_logging;
}

struct CommandSender_Callbacks Logger_stopLoggingCommandSenderAndReturnCallbacks(struct Logger *logger)
{
    robotLog("End of CommandSender callbacks logging.\n");
    return logger->original_command_sender_callbacks;
}

struct DataSender_Callbacks Logger_startLoggingDataSenderAndReturnCallbacks(struct Logger *logger,
        struct DataSender_Callbacks callbacks_to_log)
{
    file_logger = logger;

    robotLog("Start of DataSender callbacks logging.\n");

    logger->original_data_sender_callbacks = callbacks_to_log;

    struct DataSender_Callbacks data_sender_callbacks_with_logging = {
        .sendRobotPoseEstimate = &Logger_sendRobotPoseEstimate,
        .sendImage = &Logger_sendImage,
        .sendPlannedTrajectory = &Logger_sendPlannedTrajectory,
        .sendSignalReadyToStart = &Logger_sendSignalReadyToStart,
        .sendSignalReadyToDraw = &Logger_sendSignalReadyToDraw,
        .sendSignalEndOfCycle = &Logger_sendSignalEndOfCycle,
    };
    return data_sender_callbacks_with_logging;
}

struct DataSender_Callbacks Logger_stopLoggingDataSenderAndReturnCallbacks(struct Logger *logger)
{
    robotLog("End of DataSender callbacks logging.\n");
    return logger->original_data_sender_callbacks;
}

static const char *ROBOT_UPDATE = "Robot Update: ";
static void logRobot(struct Communication_Object robot)
{
    int radius = robot.radius;
    int x = robot.zone.pose.coordinates.x;
    int y = robot.zone.pose.coordinates.y;
    int theta = robot.zone.pose.theta;
    fprintf(file_logger->log_file, "\n%s%s \n%sradius:  %d\n%sx:  %d\n%sy:  %d\n%stheta:  %d",
            ITEM, ROBOT_UPDATE,
            SUB, radius,
            SUB, x,
            SUB, y,
            SUB, theta);
}


static const char *WORLD_CAMERA_UPDATE = "WorldCamera Update: ";
static void logWorld(struct Communication_World world)
{
    struct Communication_Object robot = world.robot;
    fprintf(file_logger->log_file, "%s", WORLD_CAMERA_UPDATE);
    logRobot(world.robot);
}

void Logger_updateWorld(struct WorldCamera *world_camera, struct Communication_World world)
{
    logWorld(world);
    (*(file_logger->original_data_receiver_callbacks.updateWorld))(world_camera, world);
}

static const char *TRANSLATION_UPDATE = "Translation Update from physical wheels:";
static void logWheelsTranslationUpdate(struct Communication_Translation translation)
{
    int x_movement = translation.movement.x;
    int y_movement = translation.movement.y;
    int x_speed = translation.speeds.x;
    int y_speed = translation.speeds.y;
    fprintf(file_logger->log_file,
            "\n%s%s \n%sMovement: \n%s%sx:  %d\n%s%sy:  %d\n%sSpeed: \n%s%sx:  %d\n%s%sy:  %d\n",
            ITEM, TRANSLATION_UPDATE,
            SUBITEM,
            TAB, SUB, x_movement,
            TAB, SUB, y_movement,
            SUBITEM,
            TAB, SUB, x_speed,
            TAB, SUB, y_speed);
}

static const char *ROTATION_UPDATE = "Rotation Update from physical wheels:";
static void logWheelsRotationUpdate(struct Communication_Rotation rotation)
{
    int theta = rotation.theta;
    int gamma = rotation.gamma;
    fprintf(file_logger->log_file,
            "\n%s%s \n%sTheta:  %d\n%sGamma:  %d\n",
            ITEM, ROTATION_UPDATE,
            SUB, theta,
            SUB, gamma);
}

void Logger_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation)
{
    logWheelsTranslationUpdate(translation);
    (*(file_logger->original_data_receiver_callbacks.updateWheelsTranslation))(wheels, translation);
}

void Logger_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation)
{
    logWheelsRotationUpdate(rotation);
    (*(file_logger->original_data_receiver_callbacks.updateWheelsRotation))(wheels, rotation);
}

static const char *MANCHESTER_CODE_UPDATE = "ManchesterCode Update";
static void logManchesterCode(struct Communication_ManchesterCode manchester_code)
{
    int new_painting_number = manchester_code.painting_number;
    int new_scaling_factor = manchester_code.scale_factor;
    char new_orientation = manchester_code.orientation;

    fprintf(file_logger->log_file,
            "\n%s%s \n%sPainting Number: %d\n%sScale Factor:    %d\n%sOrientation:     %c\n",
            ITEM, MANCHESTER_CODE_UPDATE,
            SUB, new_painting_number,
            SUB, new_scaling_factor,
            SUB, new_orientation);
}

void Logger_updateManchesterCode(struct ManchesterCode *manchester_code,
                                 struct Communication_ManchesterCode code_informations)
{
    logManchesterCode(code_informations);
    (*(file_logger->original_data_receiver_callbacks.updateManchesterCode))(manchester_code, code_informations);
}

static const char *FLAG_UPDATE = "New Flag Value:";
static void logFlags(const char *flag_name)
{
    fprintf(file_logger->log_file,
            "\n%s%s \n%sName:  %s\n%sNew value:  %d\n",
            ITEM, FLAG_UPDATE,
            SUB, flag_name,
            SUB, 1);
}

static const char *FLAG_START_CYCLE = "Start Cycle";
void Logger_updateFlagsStartCycle(struct Flags *flags)
{
    logFlags(FLAG_START_CYCLE);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsStartCycle))(flags);
}

static const char *FLAG_IMAGE_RECEIVED_BY_STATION = "Image Received By Station - (Ack)";
void Logger_updateFlagsImageReceivedByStation(struct Flags *flags)
{
    logFlags(FLAG_IMAGE_RECEIVED_BY_STATION);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsImageReceivedByStation))(flags);
}

static const char *FLAG_PLANNED_TRAJECTORY_RECEIVED_BY_STATION = "Planned Trajectory Received By Station - (Ack)";
void Logger_updateFlagsPlannedTrajectoryReceivedByStation(struct Flags *flags)
{
    logFlags(FLAG_PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsPlannedTrajectoryReceivedByStation))(flags);
}

static const char *FLAG_READY_TO_START_RECEIVED_BY_STATION = "Ready To Start Signal Received By Station - (Ack)";
void Logger_updateFlagsReadyToStartSignalReceivedByStation(struct Flags *flags)
{
    logFlags(FLAG_READY_TO_START_RECEIVED_BY_STATION);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsReadyToStartSignalReceivedByStation))(flags);
}

static const char *FLAG_READY_TO_DRAW_RECEIVED_BY_STATION = "Ready To Draw Signal Received By Station - (Ack)";
void Logger_updateFlagsReadyToDrawSignalReceivedByStation(struct Flags *flags)
{
    logFlags(FLAG_READY_TO_DRAW_RECEIVED_BY_STATION);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsReadyToDrawSignalReceivedByStation))(flags);
}

static const char *FLAG_END_OF_CYCLE_RECEIVED_BY_STATION = "End Of Cycle Signal Received By Station - (Ack)";
void Logger_updateFlagsEndOfCycleSignalReceivedByStation(struct Flags *flags)
{
    logFlags(FLAG_END_OF_CYCLE_RECEIVED_BY_STATION);
    (*(file_logger->original_data_receiver_callbacks.updateFlagsEndOfCycleSignalReceivedByStation))(flags);
}


// START OF COMMAND SENDER CALLBACK
static const char *TRANSLATION_COMMAND = "Sending Translation Command:";
static void logTranslationCommand(struct Command_Translate translation)
{
    int x_movement = translation.x;
    int y_movement = translation.y;
    fprintf(file_logger->log_file,
            "\n%s%s \n%s%sx:  %d\n%s%sy:  %d\n",
            ITEM, TRANSLATION_COMMAND,
            TAB, SUB, x_movement,
            TAB, SUB, y_movement);
}

static const char *SPEEDS_COMMAND = "Sending Speeds Command:";
static void logSpeedsCommand(struct Command_Speeds speeds)
{
    int x_speeds = speeds.x;
    int y_speeds = speeds.y;
    fprintf(file_logger->log_file,
            "\n%s%s \n%s%sx:  %d\n%s%sy:  %d\n",
            ITEM, SPEEDS_COMMAND,
            TAB, SUB, x_speeds,
            TAB, SUB, y_speeds);
}

static const char *ROTATION_COMMAND = "Sending Rotation Command:";
static void logRotationCommand(struct Command_Rotate rotation)
{
    int theta = rotation.theta;
    fprintf(file_logger->log_file,
            "\n%s%s \n%sTheta:  %d\n",
            ITEM, ROTATION_COMMAND,
            SUB, theta);
}

void Logger_sendTranslateCommand(struct Command_Translate translate_command)
{
    logTranslationCommand(translate_command);
    (*(file_logger->original_command_sender_callbacks.sendTranslateCommand))(translate_command);
}

void Logger_sendSpeedsCommand(struct Command_Speeds speeds_command)
{
    logSpeedsCommand(speeds_command);
    (*(file_logger->original_command_sender_callbacks.sendSpeedsCommand))(speeds_command);
}

void Logger_sendRotateCommand(struct Command_Rotate rotate_command)
{
    logRotationCommand(rotate_command);
    (*(file_logger->original_command_sender_callbacks.sendRotateCommand))(rotate_command);
}

static const char *PARAMETERLESS_COMMAND = "Sending ";
static const char *LIGHT_RED_LED_COMMAND = "Light Red LED";
static const char *LIGHT_GREEN_LED_COMMAND = "Light Green LED";
static const char *RISE_PEN_COMMAND = "Rise Pen";
static const char *LOWER_PEN_COMMAND = "Lower Pen";
static const char *FETCH_MANCHESTER_CODE_COMMAND = "Fetch Manchester Code";
static const char *STOP_SENDING_MANCHESTER_SIGNAL_COMMAND = "Stop Sending Manchester Signal";

static void logCommand(const char *command_type)
{
    fprintf(file_logger->log_file,
            "\n%s%s%s \n",
            ITEM, PARAMETERLESS_COMMAND, command_type);
}

void Logger_sendLightRedLEDCommand(void)
{
    logCommand(LIGHT_RED_LED_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendLightRedLEDCommand))();
}

void Logger_sendLightGreenLEDCommand(void)
{
    logCommand(LIGHT_GREEN_LED_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendLightGreenLEDCommand))();
}

void Logger_sendRisePenCommand(void)
{
    logCommand(RISE_PEN_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendRisePenCommand))();
}

void Logger_sendLowerPenCommand(void)
{
    logCommand(LOWER_PEN_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendLowerPenCommand))();
}

void Logger_sendFetchManchesterCode(void)
{
    logCommand(FETCH_MANCHESTER_CODE_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendFetchManchesterCodeCommand))();
}

void Logger_sendStopSendingManchesterSignal(void)
{
    logCommand(STOP_SENDING_MANCHESTER_SIGNAL_COMMAND);
    (*(file_logger->original_command_sender_callbacks.sendStopSendingManchesterSignalCommand))();
}


// START OF DATA SENDER CALLBACKS
static const char *SENDING_ROBOT_ESTIMATE = "Sending Robot Estimated Pose: ";
static void logRobotPoseEstimate(struct Pose *pose)
{
    int x = pose->coordinates->x;
    int y = pose->coordinates->y;
    int theta = pose->angle->theta;
    fprintf(file_logger->log_file, "\n%s%s \n%sx:  %d\n%sy:  %d\n%stheta:  %d",
            ITEM, SENDING_ROBOT_ESTIMATE,
            SUB, x,
            SUB, y,
            SUB, theta);
}

static const char *SENDING_PLANNED_TRAJECTORY = "Sending Planned Trajectory: ";
static void logCoordinatesSequence(const char *sequence_type, struct CoordinatesSequence *sequence)
{
    int x;
    int y;
    struct CoordinatesSequence *current_element = sequence;
    x = current_element->coordinates->x;
    y = current_element->coordinates->y;
    fprintf(file_logger->log_file,
            "\n%s%s \n%s (%d,%d)\n",
            ITEM, sequence_type,
            SUBITEM, x, y);

    do {
        current_element = current_element->next_element;
        x = current_element->coordinates->x;
        y = current_element->coordinates->y;

        fprintf(file_logger->log_file,
                "%s (%d,%d)\n",
                SUBITEM, x, y);
    } while(!CoordinatesSequence_isLast(current_element));
}

static const char *SIGNAL_DATA = "Sending ";
static const char *IMAGE_SIGNAL = "Image with Detected Borders";
static const char *READY_TO_START_SIGNAL = "Ready To Start";
static const char *READY_TO_DRAW_SIGNAL = "Ready To Draw";
static const char *END_OF_CYCLE_SIGNAL = "End of Cycle";

static void logSignal(const char *signal_type)
{
    fprintf(file_logger->log_file,
            "\n%s%s%s \n",
            ITEM, SIGNAL_DATA, signal_type);
}
void Logger_sendRobotPoseEstimate(struct Pose *pose)
{
    logRobotPoseEstimate(pose);
    (*(file_logger->original_data_sender_callbacks.sendRobotPoseEstimate))(pose);
}

void Logger_sendImage(IplImage *image)
{
    logSignal(IMAGE_SIGNAL);
    (*(file_logger->original_data_sender_callbacks.sendImage))(image);
}

void Logger_sendPlannedTrajectory(struct CoordinatesSequence *coordinates_sequence)
{
    logCoordinatesSequence(SENDING_PLANNED_TRAJECTORY, coordinates_sequence);
    (*(file_logger->original_data_sender_callbacks.sendPlannedTrajectory))(coordinates_sequence);
}

void Logger_sendSignalReadyToStart(void)
{
    logSignal(READY_TO_START_SIGNAL);
    (*(file_logger->original_data_sender_callbacks.sendSignalReadyToStart))();
}

void Logger_sendSignalReadyToDraw(void)
{
    logSignal(READY_TO_DRAW_SIGNAL);
    (*(file_logger->original_data_sender_callbacks.sendSignalReadyToDraw))();
}

void Logger_sendSignalEndOfCycle(void)
{
    logSignal(END_OF_CYCLE_SIGNAL);
    (*(file_logger->original_data_sender_callbacks.sendSignalEndOfCycle))();
}

