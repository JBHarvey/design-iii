#include <stdlib.h>
#include "Navigator.h"

struct Navigator *Navigator_new(void)
{
    struct Object *new_object = Object_new();
    struct Map *new_navigable_map = NULL;
    struct Graph *new_graph = Graph_new();
    int new_oriented_before_last_command_value = 0;
    struct Navigator *pointer =  malloc(sizeof(struct Navigator));

    pointer->object = new_object;
    pointer->navigable_map = new_navigable_map;
    pointer->graph = new_graph;
    pointer->was_oriented_before_last_command = new_oriented_before_last_command_value;

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
    }
}

void Navigator_navigateRobotTowardsGoal(struct Robot *robot)
{
    struct Command_Speeds speeds_command = {
        .x = 0,
        .y = 0
    };
    CommandSender_sendSpeedsCommand(robot->command_sender, speeds_command);
}

int Navigator_isAngleWithinRotationTolerance(int angle)
{
    return 1;
}

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance)
{
    int value = (int)(planned_distance * THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO) +
                THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE;
    return value;
}
