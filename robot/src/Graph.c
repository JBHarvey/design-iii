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
