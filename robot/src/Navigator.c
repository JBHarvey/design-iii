#include <stdlib.h>
#include "Navigator.h"

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

void Navigator_delete(struct Navigator *navigator)
{
    Object_removeOneReference(navigator->object);

    if(Object_canBeDeleted(navigator->object)) {
        Object_delete(navigator->object);

        if(navigator->navigable_map != NULL) {
            Map_delete(navigator->navigable_map);
        }

        if(navigator->planned_trajectory != NULL) {
            CoordinatesSequence_delete(navigator->planned_trajectory);
        }

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
        /*
        robot->navigator->navigable_map = Map_fetchNavigableMap(base_map, robot_radius);
        Graph_updateForMap(robot->navigator->graph, robot->navigator->navigable_map);
        Sensor_readsData(robot->world_camera->map_sensor);
        */
        // TODO: add validation that map is navigable && robot can go through the obstacles
        // Add to tests
    }

    Flags_setNavigableMapIsReady(robot->current_state->flags, 1);

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

static int convertDistanceToSpeed(int distance)
{
    int speed;

    if(distance > LOW_SPEED_DISTANCE) {
        speed = MEDIUM_SPEED_VALUE;
    } else if(distance > STOP_DISTANCE * 2) {
        speed = LOW_SPEED_VALUE;
    } else {
        speed = STOP_VALUE;
    }

    return speed;
}

static void sendSpeedsCommand(struct Robot *robot, int distance_to_target, int angle_to_target)
{
    int x = 0;
    int y = 0;
    int tolerance = THETA_TOLERANCE_DEFAULT;
    int distance_to_east = abs(angle_to_target);
    int distance_to_north = abs(HALF_PI - angle_to_target);
    int distance_to_south = abs(MINUS_HALF_PI - angle_to_target);
    int speed = convertDistanceToSpeed(distance_to_target);

    if(distance_to_east < tolerance) {
        x = speed;
    } else if(distance_to_north < tolerance) {
        y = speed;
    } else if(distance_to_south < tolerance) {
        y = -1 * speed;
    } else {
        x = -1 * speed;
    }

    struct Command_Speeds speeds_command = {
        .x = x,
        .y = y
    };

    CommandSender_sendSpeedsCommand(robot->command_sender, speeds_command);
}

static void sendRotationCommand(struct Robot *robot, int angle_to_target)
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

    struct Command_Rotate rotate_command = {
        .theta = theta
    };

    CommandSender_sendRotateCommand(robot->command_sender, rotate_command);
}

void Navigator_navigateRobotTowardsGoal(struct Robot *robot)
{
    struct Coordinates *goal_coordinates =
            robot->current_behavior->first_child->entry_conditions->goal_state->pose->coordinates;
    struct Pose *current_pose = robot->current_state->pose;
    int angle_between_robot_and_target = Pose_computeAngleBetween(current_pose, goal_coordinates);
    int was_oriented = robot->navigator->was_oriented_before_last_command;
    int is_oriented = Navigator_isAngleWithinRotationTolerance(angle_between_robot_and_target);

    if(!is_oriented) {
        robot->navigator->was_oriented_before_last_command = 0;
        sendRotationCommand(robot, angle_between_robot_and_target);
    } else {
        robot->navigator->was_oriented_before_last_command = 1;

        if(was_oriented) {
            int distance_to_target = Coordinates_distanceBetween(current_pose->coordinates, goal_coordinates);
            sendSpeedsCommand(robot, distance_to_target, angle_between_robot_and_target);
        } else {
            sendRotationCommand(robot, angle_between_robot_and_target);
        }
    }

}

void Navigator_planTowardsAntennaStart(struct Robot *robot)
{
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *antenna_start_coordinates = robot->navigator->navigable_map->antenna_zone_start;
    struct CoordinatesSequence *trajectory_to_antenna_start = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(trajectory_to_antenna_start, antenna_start_coordinates);

    robot->navigator->planned_trajectory = trajectory_to_antenna_start;
    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    RobotBehaviors_appendTrajectoryBehaviors(robot, trajectory_to_antenna_start);
}

void Navigator_planTowardsAntennaStop(struct Robot *robot)
{
    struct Coordinates *current_coordinates = robot->current_state->pose->coordinates;
    struct Coordinates *antenna_stop_coordinates = robot->navigator->navigable_map->antenna_zone_stop;
    struct CoordinatesSequence *trajectory_to_antenna_stop = CoordinatesSequence_new(current_coordinates);
    CoordinatesSequence_append(trajectory_to_antenna_stop, antenna_stop_coordinates);

    robot->navigator->planned_trajectory = trajectory_to_antenna_stop;
    RobotBehaviors_appendSendPlannedTrajectoryWithFreeEntry(robot);
    RobotBehaviors_appendTrajectoryBehaviors(robot, trajectory_to_antenna_stop);
}

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance)
{
    int value = (int)(planned_distance * THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO) +
                THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE;
    return value;
}
