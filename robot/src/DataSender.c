#include "DataSender.h"

struct DataSender_Callbacks DataSender_fetchCallbacksForRobot(void)
{
    struct DataSender_Callbacks commands = {
        .sendRobotPoseEstimate = &RobotServer_sendRobotPoseEstimate,
        .sendImage = &RobotServer_sendImageToStation,
        .sendPlannedTrajectory = &RobotServer_sendPlannedTrajectoryToStation,
        .sendSignalReadyToStart = &RobotServer_sendSignalReadyToStart,
        .sendSignalReadyToDraw = &RobotServer_sendSignalReadyToDraw,
        .sendSignalEndOfCycle = &RobotServer_sendSignalEndOfCycle
    };
    return commands;
}

struct DataSender *DataSender_new(void)
{
    struct Object *new_object = Object_new();
    struct DataSender_Callbacks new_data_callbacks = DataSender_fetchCallbacksForRobot();

    struct DataSender *pointer = malloc(sizeof(struct DataSender));

    pointer->object = new_object;
    pointer->data_callbacks = new_data_callbacks;

    return pointer;
}

void DataSender_delete(struct DataSender *data_sender)
{
    Object_removeOneReference(data_sender->object);

    if(Object_canBeDeleted(data_sender->object)) {
        Object_delete(data_sender->object);

        free(data_sender);
    }
}

void DataSender_changeTarget(struct DataSender *data_sender, struct DataSender_Callbacks new_callbacks)
{
    data_sender->data_callbacks = new_callbacks;
}

void DataSender_sendRobotPoseEstimate(struct DataSender *data_sender, struct Pose *pose)
{
    (*(data_sender->data_callbacks.sendRobotPoseEstimate))(pose);
}

void DataSender_sendImage(struct DataSender *data_sender, IplImage *image)
{
    (*(data_sender->data_callbacks.sendImage))(image);
}

void DataSender_sendPlannedTrajectory(struct DataSender *data_sender, struct CoordinatesSequence *coordinates_sequence)
{
    (*(data_sender->data_callbacks.sendPlannedTrajectory))(coordinates_sequence);
}

void DataSender_sendSignalReadyToStart(struct DataSender *data_sender)
{
    (*(data_sender->data_callbacks.sendSignalReadyToStart))();
}

void DataSender_sendSignalReadyToDraw(struct DataSender *data_sender)
{
    (*(data_sender->data_callbacks.sendSignalReadyToDraw))();
}

void DataSender_sendSignalEndOfCycle(struct DataSender *data_sender)
{
    (*(data_sender->data_callbacks.sendSignalEndOfCycle))();
}
