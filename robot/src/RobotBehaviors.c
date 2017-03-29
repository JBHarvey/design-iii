
#include "RobotBehaviors.h"

void RobotBehaviors_prepareInitialBehaviors(struct Robot *robot)
{
    void (*initial_action)(struct Robot*) = &Navigator_updateNavigableMap;
    robot->current_behavior = BehaviorBuilder_build(
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

    Behavior_addChild(robot->current_behavior, behavior_send_ready_to_start_when_map_is_navigable);

    Flags_delete(map_is_ready_flags);

    void (*beIdle)(struct Robot*) = &Behavior_idle;
    struct Flags *ready_to_start_signal_received = Flags_new();
    Flags_setReadyToStartReceivedByStation(ready_to_start_signal_received, 1);
    struct Behavior *behavior_idle_when_ready_to_start_is_received;
    behavior_idle_when_ready_to_start_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(ready_to_start_signal_received,
                                          BehaviorBuilder_withAction(beIdle,
                                                  BehaviorBuilder_end())));

    Behavior_addChild(behavior_send_ready_to_start_when_map_is_navigable, behavior_idle_when_ready_to_start_is_received);

    Flags_delete(ready_to_start_signal_received);

    void (*planTowardsAntennaStart)(struct Robot *) = &Navigator_planTowardsAntennaStart;
    struct Flags *start_cycle_signal_received = Flags_new();
    Flags_setStartCycleSignalReceived(start_cycle_signal_received, 1);
    struct Behavior *behavior_plan_towards_antenna_when_start_cycle_signal_is_received;
    behavior_plan_towards_antenna_when_start_cycle_signal_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(start_cycle_signal_received,
                                          BehaviorBuilder_withAction(planTowardsAntennaStart,
                                                  BehaviorBuilder_withAction(planTowardsAntennaStart,
                                                          BehaviorBuilder_end()))));

    Behavior_addChild(behavior_idle_when_ready_to_start_is_received,
                      behavior_plan_towards_antenna_when_start_cycle_signal_is_received);

    Flags_delete(start_cycle_signal_received);

    Behavior_delete(behavior_plan_towards_antenna_when_start_cycle_signal_is_received);
    Behavior_delete(behavior_idle_when_ready_to_start_is_received);
    Behavior_delete(behavior_send_ready_to_start_when_map_is_navigable);
}

void RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(struct Robot *robot)
{
    struct Behavior *last_behavior = robot->current_behavior;

    while(last_behavior->first_child != last_behavior) {
        last_behavior = last_behavior->first_child;
    }

    void (*sendPlannedTrajectory)(struct Robot *) = &Robot_sendPlannedTrajectory;
    struct Behavior *new_behavior = BehaviorBuilder_build(
                                        BehaviorBuilder_withAction(sendPlannedTrajectory,
                                                BehaviorBuilder_withFreeEntry(
                                                        BehaviorBuilder_end())));

    Behavior_addChild(last_behavior, new_behavior);
    Behavior_delete(new_behavior);
}
