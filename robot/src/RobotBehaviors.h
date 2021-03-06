#ifndef ROBOTBEHAVIORS_H_
#define ROBOTBEHAVIORS_H_

#include "CommandSender.h"
#include "DataSender.h"
#include "Navigator.h"

void RobotBehaviors_prepareInitialBehaviors(struct Robot *robot);
void RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(struct Robot *robot);
void RobotBehaviors_appendTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory,void (*last_action)(struct Robot *));
void RobotBehaviors_appendDrawingTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory, void (*last_action)(struct Robot *));
void RobotBehavior_appendOrientationBehaviorWithChildAction(struct Robot *robot, int orientation, void (*action)(struct Robot *));
void RobotBehavior_appendFetchManchesterCodeBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendLowerPenBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendRisePenBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendTakePictureBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendStopMovementBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendLightGreenLedBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendSendReadyToDrawBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendCloseCycleAndSendSignalBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendLightRedLedBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));
void RobotBehavior_appendUpdateNavigableMapBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *));

#endif // ROBOTBEHAVIORS_H_
