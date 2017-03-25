#ifndef PATHFINDER_H_
#define PATHFINDER_H_

#include "CoordinatesSequence.h"
#include "Graph.h"

struct CoordinatesSequence *Pathfinder_generatePathWithDijkstra(struct Graph *graph, struct Node *start, struct Node *end);
int Pathfinder_pathExists(struct Graph *graph, struct Node *start, struct Node *end);

#endif //PATHFINDER_H
