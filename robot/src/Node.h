#ifndef NODE_H_
#define NODE_H_

#include "Coordinates.h"

struct Node {
    struct Object *object;
    struct Coordinates *coordinates;
    int actual_number_of_neighbours;
};

struct Node *Node_new();
void Node_delete(struct Node *node);

#endif // NODE_H_
