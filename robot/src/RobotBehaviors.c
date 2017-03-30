
#include "RobotBehaviors.h"

void RobotBehaviors_prepareInitialBehaviors(struct Robot *robot)
{
    void (*initial_action)(struct Robot*) = &Navigator_updateNavigableMap;
    robot->current_behavior = BehaviorBuilder_build(
                                  BehaviorBuilder_withAction(initial_action,
                                          BehaviorBuilder_end()));


    void (*sendReadyToStart)(struct Robot *) = &Robot_sendReadyToStartSignal;
    struct Flags *map_is_ready_flags = Flags_irrelevant();
    Flags_setNavigableMapIsReady(map_is_ready_flags, 1);
    struct Behavior *behavior_send_ready_to_start_when_map_is_navigable;
    behavior_send_ready_to_start_when_map_is_navigable = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(map_is_ready_flags,
                                          BehaviorBuilder_withFreePoseEntry(
                                                  BehaviorBuilder_withAction(sendReadyToStart,
                                                          BehaviorBuilder_end()))));

    Behavior_addChild(robot->current_behavior, behavior_send_ready_to_start_when_map_is_navigable);

    Flags_delete(map_is_ready_flags);

    void (*beIdle)(struct Robot*) = &Behavior_idle;
    struct Flags *ready_to_start_signal_received = Flags_irrelevant();
    Flags_setReadyToStartReceivedByStation(ready_to_start_signal_received, 1);
    struct Behavior *behavior_idle_when_ready_to_start_is_received;
    behavior_idle_when_ready_to_start_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(ready_to_start_signal_received,
                                          BehaviorBuilder_withFreePoseEntry(
                                                  BehaviorBuilder_withAction(beIdle,
                                                          BehaviorBuilder_end()))));

    Behavior_addChild(behavior_send_ready_to_start_when_map_is_navigable, behavior_idle_when_ready_to_start_is_received);

    Flags_delete(ready_to_start_signal_received);

    void (*planTowardsAntennaStart)(struct Robot *) = &Navigator_planTowardsAntennaStart;
    struct Flags *start_cycle_signal_received = Flags_irrelevant();
    Flags_setStartCycleSignalReceived(start_cycle_signal_received, 1);
    struct Behavior *behavior_plan_towards_antenna_when_start_cycle_signal_is_received;
    behavior_plan_towards_antenna_when_start_cycle_signal_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(start_cycle_signal_received,
                                          BehaviorBuilder_withFreeEntry(
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

void RobotBehaviors_appendTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory)
{
    struct Behavior *planning_behavior = robot->current_behavior;

    while(planning_behavior->first_child != planning_behavior) {
        planning_behavior = planning_behavior->first_child;
    }

    void (*navigationAction)(struct Robot *) = &Navigator_navigateRobotTowardsGoal;
    struct Flags *planned_trajectory_received_by_station = Flags_irrelevant();
    Flags_setPlannedTrajectoryReceivedByStation(planned_trajectory_received_by_station, 1);

    struct Behavior *first_trajectory_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(planned_trajectory_received_by_station,
                                          BehaviorBuilder_withFreePoseEntry(
                                                  BehaviorBuilder_withAction(navigationAction,
                                                          BehaviorBuilder_end()))));
    Behavior_addChild(planning_behavior, first_trajectory_behavior);

    struct Behavior *trajectory_behavior = planning_behavior->first_child;

    int goal_coordinates_x;
    int goal_coordinates_y;

    while(!CoordinatesSequence_isLast(trajectory)) {

        trajectory = trajectory->next_element;
        goal_coordinates_x = trajectory->coordinates->x;
        goal_coordinates_y = trajectory->coordinates->y;
        struct Behavior *trajectory_behavior_child = BehaviorBuilder_build(
                    BehaviorBuilder_withGoalX(goal_coordinates_x,
                                              BehaviorBuilder_withGoalY(goal_coordinates_y,
                                                      BehaviorBuilder_withFreeTrajectoryEntry(
                                                              BehaviorBuilder_withAction(navigationAction,
                                                                      BehaviorBuilder_end())))));
        Behavior_addChild(trajectory_behavior, trajectory_behavior_child);
        trajectory_behavior = trajectory_behavior->first_child;
        Behavior_delete(trajectory_behavior_child);
    }

    Flags_delete(planned_trajectory_received_by_station);
    Behavior_delete(first_trajectory_behavior);
}