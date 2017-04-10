#include <stdlib.h>
#include "math.h"
#include "Navigator.h"
#include "Pathfinder.h"

int last_speed = 0;

struct Navigator *Navigator_new(void)
{
    struct Object *new_object = Object_new();
    struct Map *new_navigable_map = NULL;
    struct Graph *new_graph = Graph_new();
    int new_oriented_before_last_command_value = 0;
    struct CoordinatesSequence *new_planned_trajectory = NULL;
    struct Navigator *pointer =  malloc(sizeof(struct Navigator));

    pointer->object = new_object;
    pointer->navigable_map = new_navigable_map;
    pointer->graph = new_graph;
    pointer->was_oriented_before_last_command = new_oriented_before_last_command_value;
    pointer->planned_trajectory = new_planned_trajectory;

    return pointer;
}

static void deletePlannedTrajectoryIfExistant(struct Navigator *navigator)
{
    if(navigator->planned_trajectory != NULL) {
        CoordinatesSequence_delete(navigator->planned_trajectory);
    }
}

void Navigator_delete(struct Navigator *navigator)
{
    Object_removeOneReference(navigator->object);

    if(Object_canBeDeleted(navigator->object)) {
        Object_delete(navigator->object);

        if(navigator->navigable_map != NULL) {
            Map_delete(navigator->navigable_map);
        }

        deletePlannedTrajectoryIfExistant(navigator);

        Graph_delete(navigator->graph);

        free(navigator);
    }
}

void Navigator_updateNavigableMap(struct Robot *robot)
{
    int map_has_been_updated = robot->world_camera->map_sensor->has_received_new_data;

    if(map_has_been_updated) {
        struct Map *base_map = robot->world_camera->map;
        int robot_radius = robot->world_camera->robot_radius;
        robot->navigator->navigable_map = Map_fetchNavigableMap(base_map, robot_radius);
        Graph_updateForMap(robot->navigator->graph, robot->navigator->navigable_map);
        Sensor_readsData(robot->world_camera->map_sensor);
        // TODO: add validation that map is navigable && robot can go through the obstacles
        Flags_setNavigableMapIsReady(robot->current_state->flags, 1);
    }
}

int Navigator_isAngleWithinRotationTolerance(int angle)
{
    if(angle > HALF_PI) {
        angle -= HALF_PI;
    }

    while(angle < 0) {
        angle += HALF_PI;
    }

    int is_within = 1;
    int lower_border = THETA_TOLERANCE_DEFAULT;
    int upper_border = HALF_PI - THETA_TOLERANCE_DEFAULT;

    if((angle >= lower_border)
       && (angle <= upper_border)) {
        is_within = 0;
    }

    return is_within;
}

static int isAngleWithinHalfRotationTolerance(int angle)
{
    if(angle > HALF_PI) {
        angle -= HALF_PI;
    }

    while(angle < 0) {
        angle += HALF_PI;
    }

    int is_within = 1;
    int lower_border = THETA_TOLERANCE_DEFAULT / 2;
    int upper_border = HALF_PI - (THETA_TOLERANCE_DEFAULT / 2);

    if((angle >= lower_border)
       && (angle <= upper_border)) {
        is_within = 0;
    }

    return is_within;
}


static int convertDistanceToSpeed(int distance)
{
    double x = (double) distance;
    int top_speed = (int)(sqrt(x) * sqrt(1600));
    int speed;

    if(top_speed > 1600) {
        top_speed = 1600;
    } else if(distance < 1000 && distance > 5) {
        top_speed /= 3.5;
    } else if(distance <= 5) {
        top_speed = 0;
    }

    if(last_speed < top_speed) {
        if(last_speed == 0) {
            speed = 10;
        } else {
            speed = (last_speed * 1.3 < top_speed) ? last_speed * 1.3 : top_speed;
        }
    }

    last_speed = speed;
    return speed;
}

static int convertAngleToSpeed(int theta)
{
    int speed = theta;

    if(theta < THETA_TOLERANCE_DEFAULT && theta > -THETA_TOLERANCE_DEFAULT) {
        speed /= 2;
    }

    if(theta < THETA_TOLERANCE_DEFAULT / 2 && theta > -THETA_TOLERANCE_DEFAULT / 2) {
        speed = 0;
    }

    return speed;
}

static void sendSpeedsCommand(struct Robot *robot, int angular_distance_to_target, int angle_to_target)
{
    int x = 0;
    int y = 0;
    int tolerance = THETA_TOLERANCE_DEFAULT;
    int angular_distance_to_east = abs(angle_to_target);
    int angular_distance_to_north = abs(HALF_PI - angle_to_target);
    int angular_distance_to_south = abs(MINUS_HALF_PI - angle_to_target);
    int speed = convertDistanceToSpeed(angular_distance_to_target);

    if(angular_distance_to_east < tolerance) {
        x = speed;
    } else if(angular_distance_to_north < tolerance) {
        y = speed;
    } else if(angular_distance_to_south < tolerance) {
        y = -1 * speed;
    } else {
        x = -1 * speed;
    }

    struct Command_Speeds speeds_command = {
        .x = x,
        .y = y
    };

    CommandSender_sendSpeedsCommand(robot->command_sender, speeds_command, robot->wheels);
}

static void sendSlowDownCommand(struct Robot *robot, int angular_distance_to_target, int angle_to_target)
{
    int x = 0;
    int y = 0;
    int tolerance = THETA_TOLERANCE_DEFAULT;
    int angular_distance_to_east = abs(angle_to_target);
    int angular_distance_to_north = abs(HALF_PI - angle_to_target);
    int angular_distance_to_south = abs(MINUS_HALF_PI - angle_to_target);
    int speed = last_speed / 2;

    if(speed < 100) {
        speed = 100;
    }

    last_speed = speed;

    if(angular_distance_to_east < tolerance) {
        x = speed;
    } else if(angular_distance_to_north < tolerance) {
        y = speed;
    } else if(angular_distance_to_south < tolerance) {
        y = -1 * speed;
    } else {
        x = -1 * speed;
    }

    struct Command_Speeds speeds_command = {
        .x = x,
        .y = y
    };

    CommandSender_sendSpeedsCommand(robot->command_sender, speeds_command, robot->wheels);

}

static void sendRotationCommand(struct Robot *robot, int value)
{
    struct Command_Rotate rotate_command = {
        .theta = convertAngleToSpeed(value)
    };

    CommandSender_sendRotateCommand(robot->command_sender, rotate_command, robot->wheels);
}

static void sendRotationCommandForNavigation(struct Robot *robot, int angle_to_target)
{

    int theta;
    int tolerance = THETA_TOLERANCE_DEFAULT;

    while(angle_to_target > QUARTER_PI) {
        angle_to_target -= HALF_PI;
    }

    while(angle_to_target < MINUS_QUARTER_PI) {
        angle_to_target += HALF_PI;
    }

    if(angle_to_target < tolerance && angle_to_target > (-1 * tolerance)) {
        theta = 0;
    } else {
        theta = angle_to_target;
    }

    sendRotationCommand(robot, theta);
}

static void resetPlannedTrajectoryFlagsIfNecessary(struct Robot *robot)
{
    int flag_value = robot->current_state->flags->planned_trajectory_received_by_station;

    if(flag_value == TRUE) {
        Flags_setPlannedTrajectoryReceivedByStation(robot->current_state->flags, FALSE);
    }
}

void Navigator_stopMovement(struct Robot *robot)
{
    sendRotationCommand(robot, 0);
    struct Command_Speeds speeds_command = {
        .x = 0,
        .y = 0
    };
    CommandSender_sendSpeedsCommand(robot->command_sender, speeds_command, robot->wheels);
}

void Navigator_navigateRobotTowardsGoal(struct Robot *robot)
{
    struct Coordinates *goal_coordinates =
            robot->current_behavior->first_child->entry_conditions->goal_state->pose->coordinates;
    struct Pose *current_pose = robot->current_state->pose;
    int angle_between_robot_and_target = Pose_computeAngleBetween(current_pose, goal_coordinates);
    int was_oriented = robot->navigator->was_oriented_before_last_command;
    int is_oriented = Navigator_isAngleWithinRotationTolerance(angle_between_robot_and_target);
    int is_very_oriented = isAngleWithinHalfRotationTolerance(angle_between_robot_and_target);
    resetPlannedTrajectoryFlagsIfNecessary(robot);

    if(!is_oriented) {
        robot->navigator->was_oriented_before_last_command = 0;
        sendRotationCommandForNavigation(robot, angle_between_robot_and_target);
    } else {
        robot->navigator->was_oriented_before_last_command = 1;

        if(was_oriented && is_very_oriented) {
            int distance_to_target = Coordinates_distanceBetween(current_pose->coordinates, goal_coordinates);
            sendSpeedsCommand(robot, distance_to_target, angle_between_robot_and_target);
        } else if(was_oriented && !is_very_oriented) {
            int distance_to_target = Coordinates_distanceBetween(current_pose->coordinates, goal_coordinates);
            sendSlowDownCommand(robot, distance_to_target, angle_between_robot_and_target);
        } else {
            sendRotationCommandForNavigation(robot, angle_between_robot_and_target);
        }
    }

}

void Navigator_orientRobotTowardsGoal(struct Robot *robot)
{
    int tolerance = THETA_TOLERANCE_DEFAULT;
    int rotation_value = 0;
    struct Angle *orientation_goal = Angle_new(
                                         robot->current_behavior->first_child->entry_conditions->goal_state->pose->angle->theta);
    struct Angle *current_angle = robot->current_state->pose->angle;
    int angular_distance_to_goal = abs(Angle_smallestAngleBetween(orientation_goal, current_angle));

    if(angular_distance_to_goal > tolerance) {
        enum RotationDirection direction = Angle_fetchOptimalRotationDirection(orientation_goal, current_angle);

        if(direction == ANTICLOCKWISE) {
            rotation_value = angular_distance_to_goal;
        } else if(direction == CLOCKWISE) {
            rotation_value = -1 * angular_distance_to_goal;
        }
    }

    sendRotationCommand(robot, rotation_value);
    Angle_delete(orientation_goal);
}

void Navigator_planTowardsAntennaStart(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *antenna_start_coordinates = robot->navigator->navigable_map->antenna_zone_start;
    struct CoordinatesSequence *trajectory_to_antenna_start = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(trajectory_to_antenna_start, antenna_start_coordinates);

    robot->navigator->planned_trajectory = trajectory_to_antenna_start;
    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*orientationAction)(struct Robot *) = &Navigator_planOrientationTowardsAntenna;
    RobotBehaviors_appendTrajectoryBehaviors(robot, trajectory_to_antenna_start, orientationAction);
}

void Navigator_planOrientationTowardsAntenna(struct Robot *robot)
{
    int angle = 0;
    void (*action)(struct Robot *) = &Navigator_planTowardsAntennaMiddle;
    RobotBehavior_appendOrientationBehaviorWithChildAction(robot, angle, action);
}

void Navigator_planTowardsAntennaMiddle(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *start = robot->navigator->navigable_map->antenna_zone_start;
    struct Coordinates *stop = robot->navigator->navigable_map->antenna_zone_stop;
    int middle_x = Coordinates_computeMeanX(start, stop);
    int middle_y = Coordinates_computeMeanY(start, stop);
    struct Coordinates *antenna_middle_coordinates = Coordinates_new(middle_x, middle_y);
    struct CoordinatesSequence *trajectory_to_antenna_middle = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(trajectory_to_antenna_middle, antenna_middle_coordinates);
    Coordinates_delete(antenna_middle_coordinates);

    robot->navigator->planned_trajectory = trajectory_to_antenna_middle;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*stopMotionBeforeManchester)(struct Robot *) = &Navigator_planStopMotionBeforeFetchingManchester;
    RobotBehaviors_appendTrajectoryBehaviors(robot, trajectory_to_antenna_middle, stopMotionBeforeManchester);
}

void Navigator_planStopMotionBeforeFetchingManchester(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planFetchingManchesterCode;
    RobotBehavior_appendStopMovementBehaviorWithChildAction(robot, action);
}


void Navigator_planFetchingManchesterCode(struct Robot *robot)
{

    void (*action)(struct Robot *);

    if(robot->current_state->flags->has_completed_a_cycle) {
        action = &Navigator_planTowardsObstacleZoneEastSide;
    } else {
        action = &Navigator_planLowerPenForAntennaMark;
    }

    RobotBehavior_appendFetchManchesterCodeBehaviorWithChildAction(robot, action);
}

void Navigator_planLowerPenForAntennaMark(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTowardsAntennaMarkEnd;
    RobotBehavior_appendLowerPenBehaviorWithChildAction(robot, action);
}

void Navigator_planTowardsAntennaMarkEnd(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    int mark_end_x = current_coordinates->x;
    int mark_end_y = current_coordinates->y - ANTENNA_MARK_DISTANCE;
    struct Coordinates *mark_end = Coordinates_new(mark_end_x, mark_end_y);
    struct CoordinatesSequence *mark_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(mark_trajectory, mark_end);
    Coordinates_delete(mark_end);

    robot->navigator->planned_trajectory = mark_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*risePenBeforeCrossing)(struct Robot *) = &Navigator_planRisePenForObstacleCrossing;
    RobotBehaviors_appendTrajectoryBehaviors(robot, mark_trajectory, risePenBeforeCrossing);
}

void Navigator_planRisePenForObstacleCrossing(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTowardsObstacleZoneEastSide;
    RobotBehavior_appendRisePenBehaviorWithChildAction(robot, action);
}

void Navigator_planTowardsObstacleZoneEastSide(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    Graph_updateForMap(robot->navigator->graph, robot->navigator->navigable_map);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *obstacles_east_zone_coordinates = robot->navigator->graph->eastern_node->coordinates;
    struct CoordinatesSequence *obstacles_east_zone_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(obstacles_east_zone_trajectory, obstacles_east_zone_coordinates);

    robot->navigator->planned_trajectory = obstacles_east_zone_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planTowardsPaintingZone)(struct Robot *) = &Navigator_planTowardsPaintingZone;
    RobotBehaviors_appendTrajectoryBehaviors(robot, obstacles_east_zone_trajectory, planTowardsPaintingZone);

}

void Navigator_planTowardsPaintingZone(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Graph *graph = robot->navigator->graph;
    struct CoordinatesSequence *obstacle_crossing_trajectory = Pathfinder_generatePathWithDijkstra(graph,
            graph->eastern_node, graph->western_node);

    robot->navigator->planned_trajectory = obstacle_crossing_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planTowardsPainting)(struct Robot *) = &Navigator_planTowardsPainting;
    RobotBehaviors_appendTrajectoryBehaviors(robot, obstacle_crossing_trajectory, planTowardsPainting);
}

// TODO: TEST THIS FUNCTION
void Navigator_planTowardsPainting(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    int target_painting = robot->manchester_code->painting_number;
    struct Pose *painting_pose = robot->navigator->navigable_map->painting_zones[target_painting];
    struct Coordinates *painting_coordinates = painting_pose->coordinates;
    struct CoordinatesSequence *target_painting_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(target_painting_trajectory, painting_coordinates);

    robot->navigator->planned_trajectory = target_painting_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planOrientationTowardsPainting)(struct Robot *) = &Navigator_planOrientationTowardsPainting;
    RobotBehaviors_appendTrajectoryBehaviors(robot, target_painting_trajectory, planOrientationTowardsPainting);
}

void Navigator_planOrientationTowardsPainting(struct Robot *robot)
{
    int target_painting = robot->manchester_code->painting_number;
    struct Pose *painting_pose = robot->navigator->navigable_map->painting_zones[target_painting];
    int angle = painting_pose->angle->theta;
    void (*action)(struct Robot *) = &Navigator_planStopMotionBeforePicture;
    RobotBehavior_appendOrientationBehaviorWithChildAction(robot, angle, action);
}

void Navigator_planStopMotionBeforePicture(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planLightingGreenLedBeforePicture;
    RobotBehavior_appendStopMovementBehaviorWithChildAction(robot, action);
}

void Navigator_planLightingGreenLedBeforePicture(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTakingPicture;
    RobotBehavior_appendLightGreenLedBehaviorWithChildAction(robot, action);
}

void Navigator_planTakingPicture(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTowardsObstacleZoneWestSide;
    RobotBehavior_appendTakePictureBehaviorWithChildAction(robot, action);
}

void Navigator_planTowardsObstacleZoneWestSide(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    Graph_updateForMap(robot->navigator->graph, robot->navigator->navigable_map);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *obstacles_west_zone_coordinates = robot->navigator->graph->western_node->coordinates;
    struct CoordinatesSequence *obstacles_west_zone_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(obstacles_west_zone_trajectory, obstacles_west_zone_coordinates);

    robot->navigator->planned_trajectory = obstacles_west_zone_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planTowardsDrawingZone)(struct Robot *) = &Navigator_planTowardsDrawingZone;
    RobotBehaviors_appendTrajectoryBehaviors(robot, obstacles_west_zone_trajectory, planTowardsDrawingZone);
}

// TODO: TEST THIS FUNCTION
void Navigator_planTowardsDrawingZone(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Graph *graph = robot->navigator->graph;
    struct CoordinatesSequence *obstacle_crossing_trajectory = Pathfinder_generatePathWithDijkstra(graph,
            graph->western_node, graph->eastern_node);

    robot->navigator->planned_trajectory = obstacle_crossing_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planTowardsPainting)(struct Robot *) = &Navigator_planTowardsCenterOfDrawingZone;
    RobotBehaviors_appendTrajectoryBehaviors(robot, obstacle_crossing_trajectory, planTowardsPainting);
}

void Navigator_planTowardsCenterOfDrawingZone(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Map *map = robot->navigator->navigable_map;
    struct Coordinates *south_west_corner = map->south_western_drawing_corner;
    struct Coordinates *north_east_corner = map->north_eastern_drawing_corner;
    int center_x = Coordinates_computeMeanX(south_west_corner, north_east_corner);
    int center_y = Coordinates_computeMeanY(south_west_corner, north_east_corner);
    struct Coordinates *center_of_drawing_zone = Coordinates_new(center_x, center_y);
    struct CoordinatesSequence *center_of_drawing_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(center_of_drawing_trajectory, center_of_drawing_zone);

    robot->navigator->planned_trajectory = center_of_drawing_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planToTellReadyToDraw)(struct Robot *) = &Navigator_planToTellReadyToDraw;
    RobotBehaviors_appendTrajectoryBehaviors(robot, center_of_drawing_trajectory, planToTellReadyToDraw);
    Coordinates_delete(center_of_drawing_zone);
}

void Navigator_planToTellReadyToDraw(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTowardsDrawingStart;
    RobotBehavior_appendSendReadyToDrawBehaviorWithChildAction(robot, action);
}

void Navigator_planTowardsDrawingStart(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Map *map = robot->navigator->navigable_map;
    struct ManchesterCode *manchester_code = robot->manchester_code;
    struct CoordinatesSequence *drawing_trajectory = robot->drawing_trajectory;
    Map_createDrawingTrajectory(map, manchester_code, drawing_trajectory);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *drawing_start_coordinates = robot->drawing_trajectory->coordinates;
    struct CoordinatesSequence *start_of_drawing_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(start_of_drawing_trajectory, drawing_start_coordinates);

    robot->navigator->planned_trajectory = start_of_drawing_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planLowerPenBeforeDrawing)(struct Robot *) = &Navigator_planLowerPenBeforeDrawing;
    RobotBehaviors_appendTrajectoryBehaviors(robot, start_of_drawing_trajectory, planLowerPenBeforeDrawing);
}

void Navigator_planLowerPenBeforeDrawing(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planDrawing;
    RobotBehavior_appendLowerPenBehaviorWithChildAction(robot, action);
}

//TODO: test this function
void Navigator_planDrawing(struct Robot *robot)
{
    struct CoordinatesSequence *drawing_trajectory = robot->drawing_trajectory;

    robot->navigator->planned_trajectory = drawing_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planRisePenBeforeGoingToAntennaStop)(struct Robot *) = &Navigator_planRisePenBeforeGoingToAntennaStop;
    RobotBehaviors_appendTrajectoryBehaviors(robot, drawing_trajectory, planRisePenBeforeGoingToAntennaStop);
}

void Navigator_planRisePenBeforeGoingToAntennaStop(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planTowardsAntennaStop;
    RobotBehavior_appendRisePenBehaviorWithChildAction(robot, action);
}

void Navigator_planTowardsAntennaStop(struct Robot *robot)
{
    deletePlannedTrajectoryIfExistant(robot->navigator);
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *antenna_stop_coordinates = robot->navigator->navigable_map->antenna_zone_stop;
    struct CoordinatesSequence *antenna_stop_trajectory = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(antenna_stop_trajectory, antenna_stop_coordinates);

    robot->navigator->planned_trajectory = antenna_stop_trajectory;

    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    void (*planStopMotionForEndOfCycle)(struct Robot *) = &Navigator_planStopMotionForEndOfCycle;
    RobotBehaviors_appendTrajectoryBehaviors(robot, antenna_stop_trajectory, planStopMotionForEndOfCycle);
}

void Navigator_planStopMotionForEndOfCycle(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planEndOfCycleAndSendSignal;
    RobotBehavior_appendStopMovementBehaviorWithChildAction(robot, action);
}

void Navigator_planEndOfCycleAndSendSignal(struct Robot *robot)
{
    void (*action)(struct Robot *) = &Navigator_planLightingRedLedUntilNewCycle;
    RobotBehavior_appendCloseCycleAndSendSignalBehaviorWithChildAction(robot, action);
}

void Navigator_planLightingRedLedUntilNewCycle(struct Robot *robot)
{
    void(*action)(struct Robot *) = &Navigator_planUpdateMapForNewCycle;
    RobotBehavior_appendLightRedLedBehaviorWithChildAction(robot, action);
}

void Navigator_planUpdateMapForNewCycle(struct Robot *robot)
{
    void(*action)(struct Robot *) = &Navigator_planOrientationTowardsAntenna;
    RobotBehavior_appendUpdateNavgableMapBehaviorWithChildAction(robot, action);
}

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance)
{
    int value = (int)(planned_distance * THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO) +
                THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE;
    return value;
}
