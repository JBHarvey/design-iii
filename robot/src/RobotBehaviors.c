
#include "RobotBehaviors.h"

void prepareInitialBehavior(struct Robot *robot)
{
    void (*initial_action)(struct Robot*) = &Navigator_updateNavigableMap;
    robot->behavior = BehaviorBuilder_build(
                          BehaviorBuilder_withAction(initial_action,
                                  BehaviorBuilder_end()));

    struct Flags *map_is_ready_flags = Flags_new();
    Flags_setNavigableMapIsReady(map_is_ready_flags, 1);
    void (*sendReadyToStart)(struct Robot *) = &Robot_sendReadyToStartSignal;
    struct Behavior *behavior_send_ready_to_start_when_map_is_navigable;
    behavior_send_ready_to_start_when_map_is_navigable = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(map_is_ready_flags,
                                          BehaviorBuilder_withAction(sendReadyToStart,
                                                  BehaviorBuilder_end())));
    Behavior_addChild(robot->behavior, behavior_send_ready_to_start_when_map_is_navigable);

    Flags_delete(map_is_ready_flags);
    Behavior_delete(behavior_send_ready_to_start_when_map_is_navigable);
}

