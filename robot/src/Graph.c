#include <stdlib.h>
#include "Graph.h"

struct Graph *Graph_new(void)
{
    struct Object *new_object = Object_new();
    struct Node *new_eastern_node = Node_new();
    struct Node *new_western_node = Node_new();
    enum ObstaclePlacementType new_type = NONE;
    int new_actual_number_of_nodes = 0;
    struct Graph *pointer =  malloc(sizeof(struct Graph));

    pointer->object = new_object;
    pointer->eastern_node = new_eastern_node;
    pointer->western_node = new_western_node;
    pointer->nodes[0] = new_eastern_node;
    pointer->nodes[1] = new_western_node;
    pointer->type = new_type;
    pointer->actual_number_of_nodes = new_actual_number_of_nodes;

    return pointer;
}

void Graph_delete(struct Graph *graph)
{
    Object_removeOneReference(graph->object);

    if(Object_canBeDeleted(graph->object)) {
        Object_delete(graph->object);
        Node_delete(graph->eastern_node);
        Node_delete(graph->western_node);

        free(graph);
    }
}

static void establishEasternNode(struct Graph *graph, struct Obstacle *eastern_obstacle, struct Map *map)
{
    struct Coordinates *eastern_obstacle_coordinates = Obstacle_retrieveEasternPointOf(eastern_obstacle);
    int obstacle_eastern_x = eastern_obstacle_coordinates->x;
    int eastern_navigable_point = map->south_eastern_table_corner->x;
    int middle_x = (obstacle_eastern_x + eastern_navigable_point) / 2;
    int new_y = 0;


    struct Coordinates *new_eastern_node_coordinates = Coordinates_new(middle_x, new_y);
    Coordinates_copyValuesFrom(graph->eastern_node->coordinates, new_eastern_node_coordinates);
    Coordinates_delete(eastern_obstacle_coordinates);
    Coordinates_delete(new_eastern_node_coordinates);
}

void Graph_updateForMap(struct Graph *graph, struct Map* map)
{
    int number_of_obstacle = Map_fetchNumberOfObstacles(map);

    struct Obstacle *first;
    struct Obstacle *last;

    switch(number_of_obstacle) {
        case 0:
            graph->type = NONE;
            break;

        case 1:
            graph->type = SOLO;
            first = Map_retrieveFirstObstacle(map);
            establishEasternNode(graph, first, map);
            break;

        case 2:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            int are_overlapping = Obstacle_areOverlapping(first, last);

            if(are_overlapping) {
                graph->type = DUO;
            } else {
                graph->type = SOLO_SOLO;
            }

            break;

        case 3:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            struct Obstacle *middle = Map_retrieveMiddleObstacle(map, first, last);
            int eastern_are_overlapping = Obstacle_areOverlapping(first, middle);
            int western_are_overlapping = Obstacle_areOverlapping(last, middle);

            if(eastern_are_overlapping && ! western_are_overlapping) {
                graph->type = DUO_SOLO;
            } else if(!eastern_are_overlapping && western_are_overlapping) {
                graph->type = SOLO_DUO;
            } else if(eastern_are_overlapping && western_are_overlapping) {
                graph->type = TRIO;
            } else {
                graph->type = SOLO_SOLO_SOLO;
            }

            break;

    };
}
