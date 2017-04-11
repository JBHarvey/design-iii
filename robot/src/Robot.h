#ifndef ROBOT_H_
#define ROBOT_H_

#include "RobotBehaviors.h"

struct Robot {
    struct Object *object;
    struct DefaultValues *default_values;
    struct State *current_state;
    struct ManchesterCode *manchester_code;
    struct Wheels *wheels;
    struct WorldCamera *world_camera;
    struct Behavior *first_behavior;
    struct Behavior *current_behavior;
    struct Navigator *navigator;
    struct DataSender *data_sender;
    struct CommandSender *command_sender;
    struct Logger *logger;
    struct Timer *timer;
    struct CoordinatesSequence *drawing_trajectory;
};

struct Robot *Robot_new();
void Robot_delete(struct Robot *robot);

void Robot_resetAllActuators(struct Robot *robot);
void Robot_updateBehaviorIfNeeded(struct Robot *robot);
void Robot_act(struct Robot *robot);
void Robot_sendReadyToStartSignal(struct Robot *robot);
void Robot_sendReadyToDrawSignal(struct Robot *robot);
void Robot_sendPlannedTrajectory(struct Robot *robot);
void Robot_sendPoseEstimate(struct Robot *robot);
void Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(struct Robot *robot);
void Robot_lightGreenLedAndWaitASecond(struct Robot *robot);
void Robot_lightRedLed(struct Robot *robot);
void Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond(struct Robot *robot);
void Robot_stopWaitTwoSecondsRisePenWaitTwoSecond(struct Robot *robot);
void Robot_closeCycleAndSendEndOfCycleSignal(struct Robot *robot);


#endif // ROBOT_H_
