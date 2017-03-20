#ifndef NODE_H_
#define NODE_H_

#include "Coordinates.h"

#define MAX_NUMBER_OF_NEIGHBOURS 4

struct Node {
    struct Object *object;
    struct Coordinates *coordinates;
    struct Node *neighbours[MAX_NUMBER_OF_NEIGHBOURS];
    int distance_to_neighbours[MAX_NUMBER_OF_NEIGHBOURS];
    int actual_number_of_neighbours;
};

struct Node *Node_new();
void Node_delete(struct Node *node);

void Node_attemptToConnectAsNeighbours(struct Node *a, struct Node *b);
#endif // NODE_H_
