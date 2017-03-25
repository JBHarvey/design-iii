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

struct CoordinatesSequence *Pathfinder_generatePathWithDijkstra(struct Graph *graph, struct Node *start,
        struct Node *end)
{
    int number_of_nodes = graph->actual_number_of_nodes;
    struct Node *neighbour;
    float distance_from_start_to_node[number_of_nodes];
    int precedent_node_indexes[number_of_nodes];
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
        precedent_node_indexes[i] = -1;
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
                    precedent_node_indexes[index_of_neighbour] = index_of_current;
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

    // Creates a reversed array of the node indexes forming the shortest path
    int reversed_path_node_indexes[number_of_nodes];
    int start_index = findIndexOf(start, graph, number_of_nodes);
    int end_index = findIndexOf(end, graph, number_of_nodes);

    for(i = 0; i < number_of_nodes; ++i) {
        // Fills the array with "empty" node indexes
        reversed_path_node_indexes[i] = -1;
    }

    i = 0;
    int precedence_index = end_index;

    do {
        reversed_path_node_indexes[i] = precedent_node_indexes[precedence_index];
        precedence_index = precedent_node_indexes[precedence_index];
        ++i;
    } while(precedence_index != start_index);

    // Creates the CoordinatesSequence
    i = number_of_nodes - 1;
    struct Coordinates *node_coordinates;
    node_coordinates = graph->nodes[start_index]->coordinates;
    struct CoordinatesSequence *sequence = CoordinatesSequence_new(node_coordinates);

    while(i >= 0) {

        // skips empty indexes and the first node, as they already have been added
        index_of_current = reversed_path_node_indexes[i];

        if(index_of_current != -1 && index_of_current != start_index) {
            node_coordinates = graph->nodes[index_of_current]->coordinates;
            CoordinatesSequence_append(sequence, node_coordinates);
        }

        --i;
    }

    node_coordinates = graph->nodes[end_index]->coordinates;
    CoordinatesSequence_append(sequence, node_coordinates);
    return sequence;
}
