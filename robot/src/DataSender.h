#ifndef DATASENDER_H_
#define DATASENDER_H_

#include "RobotServer.h"

struct DataSender_Callbacks {
    void (*sendRobotPoseEstimate)(struct Pose *);
    void (*sendImage)(IplImage *);
    void (*sendPlannedTrajectory)(struct CoordinatesSequence *);
    void (*sendSignalReadyToStart)(void);
    void (*sendSignalReadyToDraw)(void);
    void (*sendSignalEndOfCycle)(void);
};

struct DataSender {
    struct Object *object;
    struct DataSender_Callbacks data_callbacks;
};

struct DataSender *DataSender_new(void);
void DataSender_delete(struct DataSender *data_sender);

struct DataSender_Callbacks DataSender_fetchCallbacksForRobot(void);

void DataSender_changeTarget(struct DataSender *data_sender, struct DataSender_Callbacks new_callbacks);

void DataSender_sendRobotPoseEstimate(struct DataSender *data_sender, struct Pose *pose);
void DataSender_sendImage(struct DataSender *data_sender, IplImage *image);
void DataSender_sendPlannedTrajectory(struct DataSender *data_sender, struct CoordinatesSequence *coordinates_sequence);
void DataSender_sendSignalReadyToStart(struct DataSender *data_sender);
void DataSender_sendSignalReadyToDraw(struct DataSender *data_sender);
void DataSender_sendSignalEndOfCycle(struct DataSender *data_sender);
#endif // DATASENDER_H_
