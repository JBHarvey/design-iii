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

