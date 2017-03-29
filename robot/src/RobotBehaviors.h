#ifndef ROBOTBEHAVIORS_H_
#define ROBOTBEHAVIORS_H_

#include "CommandSender.h"
#include "DataSender.h"
#include "Navigator.h"

void RobotBehaviors_prepareInitialBehaviors(struct Robot *robot);
void RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(struct Robot *robot);
void RobotBehaviors_appendTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory);

#endif // ROBOTBEHAVIORS_H_
