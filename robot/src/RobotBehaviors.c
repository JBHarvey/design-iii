
#include "RobotBehaviors.h"

void prepareInitialBehaviors(struct Robot *robot)
{
    void (*initial_action)(struct Robot*) = &Navigator_updateNavigableMap;
    robot->behavior = BehaviorBuilder_build(
                          BehaviorBuilder_withAction(initial_action,
                                  BehaviorBuilder_end()));


    void (*sendReadyToStart)(struct Robot *) = &Robot_sendReadyToStartSignal;
    struct Flags *map_is_ready_flags = Flags_new();
    Flags_setNavigableMapIsReady(map_is_ready_flags, 1);
    struct Behavior *behavior_send_ready_to_start_when_map_is_navigable;
    behavior_send_ready_to_start_when_map_is_navigable = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(map_is_ready_flags,
                                          BehaviorBuilder_withAction(sendReadyToStart,
                                                  BehaviorBuilder_end())));

    Behavior_addChild(robot->behavior, behavior_send_ready_to_start_when_map_is_navigable);

    Flags_delete(map_is_ready_flags);


    struct Flags *ready_to_start_signal_received = Flags_new();
    Flags_setReadyToStartReceivedByStation(ready_to_start_signal_received, 1);
    struct Behavior *behavior_idle_when_ready_to_start_is_received;
    behavior_send_ready_to_start_when_map_is_navigable = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(ready_to_start_signal_received,
                                          BehaviorBuilder_end()));

    /*
    Behavior_addChild(behavior_send_ready_to_start_when_map_is_navigable, behavior_idle_when_ready_to_start_is_received);
    */

    Flags_delete(ready_to_start_signal_received);

    Behavior_delete(behavior_send_ready_to_start_when_map_is_navigable);
    Behavior_delete(behavior_idle_when_ready_to_start_is_received);
}

