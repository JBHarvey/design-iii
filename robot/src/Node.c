#include <stdlib.h>
#include "Node.h"

struct Node *Node_new()
{
    struct Object *new_object = Object_new();
    struct Coordinates *new_coordinates = Coordinates_zero();
    int new_actual_number_of_neighbours = 0;
    struct Node *pointer =  malloc(sizeof(struct Node));

    pointer->object = new_object;
    pointer->coordinates = new_coordinates;
    pointer->actual_number_of_neighbours = new_actual_number_of_neighbours;
    int i;

    for(i = 0; i < MAX_NUMBER_OF_NEIGHBOURS; ++i) {
        pointer->neighbours[i] = pointer;
        pointer->distance_to_neighbours[i] = 0;
    }

    return pointer;
}

void Node_delete(struct Node *node)
{
    Object_removeOneReference(node->object);

    if(Object_canBeDeleted(node->object)) {
        Object_delete(node->object);
        Coordinates_delete(node->coordinates);

        free(node);
    }
}

int canBecomeNeighbours(struct Node *a, struct Node *b)
{
    int are_already_neighbours = 0;
    int i;

    for(i = 0; i < MAX_NUMBER_OF_NEIGHBOURS; ++i) {
        if(Node_areNeighbours(a, b)) {
            are_already_neighbours = 1;
        }
    }

    return (!are_already_neighbours &&
            a->actual_number_of_neighbours < MAX_NUMBER_OF_NEIGHBOURS &&
            b->actual_number_of_neighbours < MAX_NUMBER_OF_NEIGHBOURS &&
            Coordinates_distanceBetween(a->coordinates, b->coordinates) != 0);
}

void Node_attemptToConnectAsNeighbours(struct Node *a, struct Node *b)
{
    int can_become_neighbours = canBecomeNeighbours(a, b);

    if(can_become_neighbours) {
        a->neighbours[a->actual_number_of_neighbours] = b;
        b->neighbours[b->actual_number_of_neighbours] = a;

        float distance = Coordinates_distanceBetween(a->coordinates, b->coordinates);
        a->distance_to_neighbours[a->actual_number_of_neighbours] = distance;
        b->distance_to_neighbours[b->actual_number_of_neighbours] = distance;
        ++a->actual_number_of_neighbours;
        ++b->actual_number_of_neighbours;
    }
}

int Node_areNeighbours(struct Node *a, struct Node *b)
{
    int neighbours_of_a = a->actual_number_of_neighbours;

    for(int i = 0; i < neighbours_of_a; ++i) {
        if(a->neighbours[i] == b) {
            return 1;
        }
    }

    return 0;
}

float Node_distanceToNeighbour(struct Node *node, struct Node *neighbour)
{
    for(int i = 0; i < node->actual_number_of_neighbours; ++i) {
        if(node->neighbours[i] == neighbour) {
            return node->distance_to_neighbours[i];
        }
    }

    return MAX_VALUE_OF_FLOAT;
}
