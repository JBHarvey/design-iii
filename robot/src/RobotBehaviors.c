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
                BehaviorBuilder_withAction(sendReadyToStart,
                                           BehaviorBuilder_withFlags(map_is_ready_flags,
                                                   BehaviorBuilder_withFreePoseEntry(
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

    void (*planTowardsAntennaMiddle)(struct Robot *) = &Navigator_planTowardsAntennaMiddle;
    struct Flags *start_cycle_signal_received = Flags_irrelevant();
    Flags_setStartCycleSignalReceived(start_cycle_signal_received, 1);
    struct Behavior *behavior_plan_towards_antenna_when_middle_cycle_signal_is_received;
    behavior_plan_towards_antenna_when_middle_cycle_signal_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(start_cycle_signal_received,
                                          BehaviorBuilder_withFreeEntry(
                                                  BehaviorBuilder_withAction(planTowardsAntennaMiddle,
                                                          BehaviorBuilder_end()))));

    Behavior_addChild(behavior_idle_when_ready_to_start_is_received,
                      behavior_plan_towards_antenna_when_middle_cycle_signal_is_received);

    Flags_delete(start_cycle_signal_received);

    Behavior_delete(behavior_plan_towards_antenna_when_middle_cycle_signal_is_received);
    Behavior_delete(behavior_idle_when_ready_to_start_is_received);
    Behavior_delete(behavior_send_ready_to_start_when_map_is_navigable);
}

static struct Behavior *fetchLastBehavior(struct Robot *robot)
{
    struct Behavior *current = robot->current_behavior;

    while(current != current->first_child) {
        current = current->first_child;
    }

    return current;
}

void RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(struct Robot *robot)
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*sendPlannedTrajectory)(struct Robot *) = &Robot_sendPlannedTrajectory;
    struct Behavior *new_behavior = BehaviorBuilder_build(
                                        BehaviorBuilder_withAction(sendPlannedTrajectory,
                                                BehaviorBuilder_withFreeEntry(
                                                        BehaviorBuilder_end())));

    Behavior_addChild(last_behavior, new_behavior);
    Behavior_delete(new_behavior);
}

void RobotBehaviors_appendComputeTrajectoryStartAngle(struct Robot *robot)
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*computeTrajectoryStartAngle)(struct Robot *) = &Navigator_computeTrajectoryStartAngle;
    struct Behavior *new_behavior = BehaviorBuilder_build(
                                        BehaviorBuilder_withAction(computeTrajectoryStartAngle,
                                                BehaviorBuilder_withFreeEntry(
                                                        BehaviorBuilder_end())));

    Behavior_addChild(last_behavior, new_behavior);
    Behavior_delete(new_behavior);
}

static void appendFreeEntryNavigationBehavior(struct Behavior *behavior)
{
    void (*navigationAction)(struct Robot *) = &Navigator_navigateRobotTowardsGoal;
    struct Behavior *free_entry_navigation_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withFreeEntry(
                    BehaviorBuilder_withAction(navigationAction,
                            BehaviorBuilder_end())));
    Behavior_addChild(behavior, free_entry_navigation_behavior);
    Behavior_delete(free_entry_navigation_behavior);
}

void RobotBehaviors_appendTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory,
        void (*last_action)(struct Robot *))
{
    struct Behavior *planning_behavior = fetchLastBehavior(robot);

    void (*computeTrajectoryStartAngle)(struct Robot *) = &Navigator_computeTrajectoryStartAngle;
    struct Flags *planned_trajectory_received_by_station = Flags_irrelevant();
    Flags_setPlannedTrajectoryReceivedByStation(planned_trajectory_received_by_station, 1);

    struct Behavior *first_trajectory_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withAction(computeTrajectoryStartAngle,
                                           BehaviorBuilder_withFlags(planned_trajectory_received_by_station,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(planning_behavior, first_trajectory_behavior);

    appendFreeEntryNavigationBehavior(first_trajectory_behavior);

    struct Behavior *trajectory_behavior = first_trajectory_behavior->first_child;
    int goal_coordinates_x;
    int goal_coordinates_y;

    while(!CoordinatesSequence_isLast(trajectory)) {

        trajectory = trajectory->next_element;
        goal_coordinates_x = trajectory->coordinates->x;
        goal_coordinates_y = trajectory->coordinates->y;

        struct Behavior *start_angle_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withAction(computeTrajectoryStartAngle,
                                           BehaviorBuilder_withGoalX(goal_coordinates_x,
                                                   BehaviorBuilder_withGoalY(goal_coordinates_y,
                                                           BehaviorBuilder_withFreeEntryForTrajectory(
                                                                   BehaviorBuilder_end())))));
        Behavior_addChild(trajectory_behavior, start_angle_behavior);
        appendFreeEntryNavigationBehavior(start_angle_behavior);

        trajectory_behavior = start_angle_behavior->first_child;

        Behavior_delete(start_angle_behavior);
    }

    trajectory_behavior->first_child->action = last_action;

    Flags_delete(planned_trajectory_received_by_station);
    Behavior_delete(first_trajectory_behavior);
}

void RobotBehaviors_appendDrawingTrajectoryBehaviors(struct Robot *robot, struct CoordinatesSequence *trajectory,
        void (*last_action)(struct Robot *))
{
    struct Behavior *planning_behavior = fetchLastBehavior(robot);

    void (*computeTrajectoryStartAngle)(struct Robot *) = &Navigator_computeTrajectoryStartAngle;
    struct Flags *planned_trajectory_received_by_station = Flags_irrelevant();
    Flags_setPlannedTrajectoryReceivedByStation(planned_trajectory_received_by_station, 1);

    struct Behavior *first_trajectory_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withFlags(planned_trajectory_received_by_station,
                                          BehaviorBuilder_withFreePoseEntry(
                                                  BehaviorBuilder_withAction(computeTrajectoryStartAngle,
                                                          BehaviorBuilder_end()))));
    Behavior_addChild(planning_behavior, first_trajectory_behavior);

    appendFreeEntryNavigationBehavior(first_trajectory_behavior);

    struct Behavior *trajectory_behavior = first_trajectory_behavior->first_child;

    int goal_coordinates_x;
    int goal_coordinates_y;

    while(!CoordinatesSequence_isLast(trajectory)) {

        trajectory = trajectory->next_element;
        goal_coordinates_x = trajectory->coordinates->x;
        goal_coordinates_y = trajectory->coordinates->y;

        struct Behavior *start_angle_behavior = BehaviorBuilder_build(
                BehaviorBuilder_withAction(computeTrajectoryStartAngle,
                                           BehaviorBuilder_withGoalX(goal_coordinates_x,
                                                   BehaviorBuilder_withGoalY(goal_coordinates_y,
                                                           BehaviorBuilder_withFreeEntryForDrawingTrajectory(
                                                                   BehaviorBuilder_end())))));
        Behavior_addChild(trajectory_behavior, start_angle_behavior);

        appendFreeEntryNavigationBehavior(start_angle_behavior);

        trajectory_behavior = start_angle_behavior->first_child;

        Behavior_delete(start_angle_behavior);
    }

    trajectory_behavior->first_child->action = last_action;

    Flags_delete(planned_trajectory_received_by_station);
    Behavior_delete(first_trajectory_behavior);
}

void RobotBehavior_appendOrientationBehaviorWithChildAction(struct Robot *robot, int angle,
        void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*orientTowardsGoal)(struct Robot*) = &Navigator_orientRobotTowardsGoal;
    struct Behavior *behavior_orient_robot_with_free_entry;
    behavior_orient_robot_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(orientTowardsGoal,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_orient_robot_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_after_orientation;
    behavior_do_action_after_orientation = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withGoalTheta(angle,
                                               BehaviorBuilder_withFreeEntryForOrientation(
                                                       BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_after_orientation);

    Behavior_delete(behavior_orient_robot_with_free_entry);
    Behavior_delete(behavior_do_action_after_orientation);
}

void RobotBehavior_appendFetchManchesterCodeBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*fetchManchesterGoal)(struct Robot*) =
        &Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset;
    struct Behavior *behavior_fetch_manchester_code_with_free_entry;
    behavior_fetch_manchester_code_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(fetchManchesterGoal,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_fetch_manchester_code_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Flags *manchester_code_received_flags = Flags_irrelevant();
    Flags_setManchesterCodeReceived(manchester_code_received_flags, 1);
    struct Behavior *behavior_do_action_after_manchester_code_reception;
    behavior_do_action_after_manchester_code_reception = BehaviorBuilder_build(
                BehaviorBuilder_withAction(action,
                                           BehaviorBuilder_withFlags(manchester_code_received_flags,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_after_manchester_code_reception);

    Flags_delete(manchester_code_received_flags);

    Behavior_delete(behavior_fetch_manchester_code_with_free_entry);
    Behavior_delete(behavior_do_action_after_manchester_code_reception);
}

void RobotBehavior_appendLowerPenBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*lowerPen)(struct Robot*) = &Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond;
    struct Behavior *behavior_lower_pen_with_free_entry;
    behavior_lower_pen_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(lowerPen,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_lower_pen_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_with_free_entry;
    behavior_do_action_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_do_action_with_free_entry);


    Behavior_delete(behavior_do_action_with_free_entry);
    Behavior_delete(behavior_lower_pen_with_free_entry);
}

void RobotBehavior_appendRisePenBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*risePen)(struct Robot*) = &Robot_stopWaitTwoSecondsRisePenWaitTwoSecond;
    struct Behavior *behavior_rise_pen_with_free_entry;
    behavior_rise_pen_with_free_entry = BehaviorBuilder_build(
                                            BehaviorBuilder_withAction(risePen,
                                                    BehaviorBuilder_withFreeEntry(
                                                            BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_rise_pen_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_with_free_entry;
    behavior_do_action_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_do_action_with_free_entry);


    Behavior_delete(behavior_do_action_with_free_entry);
    Behavior_delete(behavior_rise_pen_with_free_entry);
}

void RobotBehavior_appendStopMovementBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*stopMotion)(struct Robot*) = &Navigator_stopMovement;
    struct Behavior *behavior_stop_motion_with_free_entry;
    behavior_stop_motion_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(stopMotion,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_stop_motion_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_with_free_entry;
    behavior_do_action_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_do_action_with_free_entry);


    Behavior_delete(behavior_do_action_with_free_entry);
    Behavior_delete(behavior_stop_motion_with_free_entry);
}

void RobotBehavior_appendLightGreenLedBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*lightGreenLed)(struct Robot*) = &Robot_lightGreenLedAndWaitASecond;
    struct Behavior *behavior_light_green_led_with_free_entry;
    behavior_light_green_led_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(lightGreenLed,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_light_green_led_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_with_free_entry;
    behavior_do_action_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_do_action_with_free_entry);


    Behavior_delete(behavior_do_action_with_free_entry);
    Behavior_delete(behavior_light_green_led_with_free_entry);
}

void RobotBehavior_appendTakePictureBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*takePicture)(struct Robot*) = &OnboardCamera_takePictureAndIfValidSendAndUpdateDrawingBaseTrajectory;
    struct Behavior *behavior_take_picture_with_free_entry;
    behavior_take_picture_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(takePicture,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_take_picture_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Flags *image_received_by_station = Flags_irrelevant();
    Flags_setImageReceivedByStation(image_received_by_station, 1);
    struct Behavior *behavior_do_action_when_image_received_by_station;
    behavior_do_action_when_image_received_by_station = BehaviorBuilder_build(
                BehaviorBuilder_withAction(action,
                                           BehaviorBuilder_withFlags(image_received_by_station,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_when_image_received_by_station);
    Flags_delete(image_received_by_station);

    Behavior_delete(behavior_do_action_when_image_received_by_station);
    Behavior_delete(behavior_take_picture_with_free_entry);
}

void RobotBehavior_appendSendReadyToDrawBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*sendReadyToDraw)(struct Robot*) = &Robot_sendReadyToDrawSignal;
    struct Behavior *behavior_send_ready_to_draw_with_free_entry;
    behavior_send_ready_to_draw_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(sendReadyToDraw,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_send_ready_to_draw_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Flags *ready_to_draw_received_flags = Flags_irrelevant();
    Flags_setReadyToDrawReceivedByStation(ready_to_draw_received_flags, 1);
    struct Behavior *behavior_do_action_after_ready_to_draw_received;
    behavior_do_action_after_ready_to_draw_received = BehaviorBuilder_build(
                BehaviorBuilder_withAction(action,
                                           BehaviorBuilder_withFlags(ready_to_draw_received_flags,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_after_ready_to_draw_received);

    Flags_delete(ready_to_draw_received_flags);

    Behavior_delete(behavior_do_action_after_ready_to_draw_received);
    Behavior_delete(behavior_send_ready_to_draw_with_free_entry);
}

void RobotBehavior_appendCloseCycleAndSendSignalBehaviorWithChildAction(struct Robot *robot,
        void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*closeCycleAndSendSignal)(struct Robot*) = &Robot_closeCycleAndSendEndOfCycleSignal;
    struct Behavior *behavior_close_cycle_and_send_signal_with_free_entry;
    behavior_close_cycle_and_send_signal_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(closeCycleAndSendSignal,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_close_cycle_and_send_signal_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Flags *end_of_cycle_received_flags = Flags_irrelevant();
    Flags_setEndOfCycleReceivedByStation(end_of_cycle_received_flags, 1);
    struct Behavior *behavior_do_action_after_end_of_cycle_received;
    behavior_do_action_after_end_of_cycle_received = BehaviorBuilder_build(
                BehaviorBuilder_withAction(action,
                                           BehaviorBuilder_withFlags(end_of_cycle_received_flags,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_after_end_of_cycle_received);

    Flags_delete(end_of_cycle_received_flags);

    Behavior_delete(behavior_do_action_after_end_of_cycle_received);
    Behavior_delete(behavior_close_cycle_and_send_signal_with_free_entry);
}

void RobotBehavior_appendLightRedLedBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*lightRedLed)(struct Robot*) = &Robot_lightRedLed;
    struct Behavior *behavior_light_red_led_with_free_entry;
    behavior_light_red_led_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(lightRedLed,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_light_red_led_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Flags *start_cycle_signal_received_flags = Flags_irrelevant();
    Flags_setStartCycleSignalReceived(start_cycle_signal_received_flags, 1);
    struct Behavior *behavior_do_action_when_start_cycle_signal_is_received;
    behavior_do_action_when_start_cycle_signal_is_received = BehaviorBuilder_build(
                BehaviorBuilder_withAction(action,
                                           BehaviorBuilder_withFlags(start_cycle_signal_received_flags,
                                                   BehaviorBuilder_withFreePoseEntry(
                                                           BehaviorBuilder_end()))));
    Behavior_addChild(last_behavior, behavior_do_action_when_start_cycle_signal_is_received);

    Flags_delete(start_cycle_signal_received_flags);


    Behavior_delete(behavior_do_action_when_start_cycle_signal_is_received);
    Behavior_delete(behavior_light_red_led_with_free_entry);
}

void RobotBehavior_appendUpdateNavigableMapBehaviorWithChildAction(struct Robot *robot, void (*action)(struct Robot *))
{
    struct Behavior *last_behavior = fetchLastBehavior(robot);

    void (*updateNavigableMap)(struct Robot*) = &Navigator_updateNavigableMap;
    struct Behavior *behavior_update_navigable_map_with_free_entry;
    behavior_update_navigable_map_with_free_entry = BehaviorBuilder_build(
                BehaviorBuilder_withAction(updateNavigableMap,
                                           BehaviorBuilder_withFreeEntry(
                                                   BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_update_navigable_map_with_free_entry);

    last_behavior = last_behavior->first_child;

    struct Behavior *behavior_do_action_with_free_entry;
    behavior_do_action_with_free_entry = BehaviorBuilder_build(
            BehaviorBuilder_withAction(action,
                                       BehaviorBuilder_withFreeEntry(
                                           BehaviorBuilder_end())));
    Behavior_addChild(last_behavior, behavior_do_action_with_free_entry);

    Behavior_delete(behavior_do_action_with_free_entry);
    Behavior_delete(behavior_update_navigable_map_with_free_entry);
}
