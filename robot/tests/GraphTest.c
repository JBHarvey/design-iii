#include <criterion/criterion.h>
#include <stdio.h>
#include "Graph.h"


Test(Graph, given_aMapWithNoObstacles_when_createsGraph_then_theGraphTypeIsZero)
{
    struct Graph *graph = Graph_new();

    struct Coordinates *coordinates_zero = Coordinates_zero();
    enum ObstaclePlacementType expected_type = NONE;

    cr_assert(graph->type == expected_type);
    cr_assert(graph->actual_number_of_nodes == 0);
    cr_assert(graph->eastern_node->actual_number_of_neighbours == 0);
    cr_assert(Coordinates_haveTheSameValues(graph->eastern_node->coordinates, coordinates_zero));
    cr_assert(graph->western_node->actual_number_of_neighbours == 0);
    cr_assert(Coordinates_haveTheSameValues(graph->western_node->coordinates, coordinates_zero));
    cr_assert(graph->nodes[0] == graph->eastern_node);
    cr_assert(graph->nodes[1] == graph->western_node);

    Graph_delete(graph);
}

struct Map *graph_map;
struct Graph *graph;
void setup_GraphOne(void)
{
    graph = Graph_new();
    struct Map *base_map = Map_new();
    //    struct Coordinates coordinates_center_center
    graph_map = Map_fetchNavigableMap(base_map, THEORICAL_ROBOT_RADIUS);

    Map_delete(base_map);
}

void teardown_GraphOne(void)
{
    Map_delete(graph_map);
    Graph_delete(graph);
}

Test(Graph, given_aNavigableMapWithOneObstacle_when_updatesGraph_then_theGraphTypeIsSOLO
     , .init = setup_GraphOne
     , .fini = teardown_GraphOne)
{

}
