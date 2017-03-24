#include <stdlib.h>
#include "Pathfinder.h"

static int findIndexOf(struct Node *node, struct Graph *graph, int number_of_nodes)
{
    int i;

    for(i = 0; i < number_of_nodes; ++i) {
        if(graph->nodes[i] == node) {
            return i;
        }
    }

    return -1;
}

// TODO: rename to Pathfinder_findPathDijksta
// TODO: add Pathfinder_pathExists return ^ path not null;
int Pathfinder_pathExists(struct Graph *graph, struct Node *start, struct Node *end)
{
    int number_of_nodes = graph->actual_number_of_nodes;
    struct Node *neighbour;
    struct Node *precedent_node[number_of_nodes];
    float distance_from_start_to_node[number_of_nodes];
    int unvisited_nodes[number_of_nodes];

    int i;
    int index_of_current;
    int index_of_neighbour;
    int current_distance_to_start;
    int remains_unvisited_nodes;

    float distance;
    int unvisited;

    for(i = 0; i < number_of_nodes; ++i) {
        if(graph->nodes[i] != start) {
            distance = MAX_VALUE_OF_FLOAT;
            unvisited = 1;
        } else {
            distance = 0;
            unvisited = 0;
        }

        distance_from_start_to_node[i] = distance;
        unvisited_nodes[i] = unvisited;
        precedent_node[i] = NULL;
    }

    struct Node *current = start;

    while(remains_unvisited_nodes) {

        // Establishes distance from start to neighbours of current node
        index_of_current = findIndexOf(current, graph, number_of_nodes);
        current_distance_to_start = distance_from_start_to_node[index_of_current];

        for(i = 0; i < current->actual_number_of_neighbours; ++i) {
            neighbour = current->neighbours[i];
            index_of_neighbour = findIndexOf(neighbour, graph, number_of_nodes);

            if(unvisited_nodes[index_of_neighbour]) {
                distance = current_distance_to_start + Node_distanceToNeighbour(current, neighbour);

                if(distance < distance_from_start_to_node[index_of_neighbour]) {
                    distance_from_start_to_node[index_of_neighbour] = distance;
                    precedent_node[index_of_neighbour] = current;
                }
            }
        }

        unvisited_nodes[index_of_current] = 0;

        // Checks if there is any node to visit left and if so
        // sets the closest one to be the next visited
        remains_unvisited_nodes = 0;
        distance = MAX_VALUE_OF_FLOAT;

        for(i = 0; i < number_of_nodes; ++i) {
            if(unvisited_nodes[i]) {
                remains_unvisited_nodes = 1;

                if(distance_from_start_to_node[i] < distance) {
                    distance = distance_from_start_to_node[i];
                    current = graph->nodes[i];
                }
            }
        }
    }

    return 0;

}
