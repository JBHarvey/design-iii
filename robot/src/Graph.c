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

    switch(number_of_obstacle) {
        case 0:
            graph->type = NONE;
            break;

        case 1:
            graph->type = SOLO;
            break;

        case 2:
            graph->type = SOLO_SOLO;
            break;

    }
