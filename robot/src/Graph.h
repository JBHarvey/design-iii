#ifndef GRAPH_H_
#define GRAPH_H_

#include "Node.h"
#include "Map.h"


enum ObstaclePlacementType {
    NONE,
    SOLO,
    SOLO_SOLO,
    DUO,
    SOLO_SOLO_SOLO,
    SOLO_DUO,
    DUO_SOLO,
    TRIO
};

struct Graph {
    struct Object *object;
    struct Node *eastern_node;
    struct Node *western_node;
    struct Node *nodes[MAX_NUMBER_OF_NODE];
    enum ObstaclePlacementType type;
    int actual_number_of_nodes;
};

struct Graph *Graph_new(void);
void Graph_delete(struct Graph *graph);

void Graph_updateForMap(struct Graph *graph, struct Map* map);
#endif // GRAPH_H_
