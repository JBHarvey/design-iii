
#include "RobotBehaviors.h"

void prepareInitialBehavior(struct Robot *robot)
{
    struct Flags *initialFlags = Flags_new();
    robot->behavior = BehaviorBuilder_build(
                          BehaviorBuilder_withFlags(initialFlags,
                                  BehaviorBuilder_end()));
    Flags_delete(initialFlags);
}

