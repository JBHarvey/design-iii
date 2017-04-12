#ifndef GRAPH_H_
#define GRAPH_H_

#include "Node.h"
#include "Map.h"


struct Graph {
    struct Object *object;
    struct Node *eastern_node;
    struct Node *western_node;
    struct Node *nodes[MAX_NUMBER_OF_NODE];
    int actual_number_of_nodes;
};

struct Graph *Graph_new(void);
void Graph_delete(struct Graph *graph);

struct Graph *Graph_generateForMap(struct Map* map);
#endif // GRAPH_H_
