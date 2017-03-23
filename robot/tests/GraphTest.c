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

    Coordinates_delete(coordinates_zero);
    Graph_delete(graph);
}

struct Map *base_map;
struct Map *graph_map;
struct Graph *graph;
int navigable_length;
int navigable_height;
struct Coordinates *south_western_table_corner;
struct Coordinates *south_eastern_table_corner;
struct Coordinates *north_western_table_corner;
struct Coordinates *north_eastern_table_corner;
struct Coordinates *south_western_drawing_zone_corner;
struct Coordinates *south_eastern_drawing_zone_corner;
struct Coordinates *north_western_drawing_zone_corner;
struct Coordinates *north_eastern_drawing_zone_corner;
struct Coordinates *invalid_coordinates;

struct Coordinates *coordinates_center_west;
struct Coordinates *coordinates_center_center;
struct Coordinates *coordinates_center_east;
void setup_Graph(void)
{
    base_map = Map_new();
    south_western_table_corner = Coordinates_zero();
    south_eastern_table_corner = Coordinates_new(THEORICAL_WORLD_LENGTH, 0);
    north_western_table_corner = Coordinates_new(0, THEORICAL_WORLD_HEIGHT);
    north_eastern_table_corner = Coordinates_new(THEORICAL_WORLD_LENGTH, THEORICAL_WORLD_HEIGHT);
    Map_updateTableCorners(base_map, north_eastern_table_corner, south_eastern_table_corner, south_western_table_corner,
                           north_western_table_corner);

    south_western_drawing_zone_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X -
                                        THEORICAL_DRAWING_ZONE_SIDE, THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y);
    south_eastern_drawing_zone_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
                                        THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y);
    north_western_drawing_zone_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X -
                                        THEORICAL_DRAWING_ZONE_SIDE, THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE);
    north_eastern_drawing_zone_corner = Coordinates_new(THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_X,
                                        THEORICAL_DRAWING_ZONE_SOUTH_EASTERN_Y + THEORICAL_DRAWING_ZONE_SIDE);
    Map_updateDrawingCorners(base_map, north_eastern_drawing_zone_corner, south_eastern_drawing_zone_corner,
                             south_western_drawing_zone_corner, north_western_drawing_zone_corner);
    invalid_coordinates = Coordinates_new(-1, -1);

    Map_updateObstacle(base_map, invalid_coordinates, CENTER, 0);
    Map_updateObstacle(base_map, invalid_coordinates, CENTER, 1);
    Map_updateObstacle(base_map, invalid_coordinates, CENTER, 2);
    graph_map = Map_fetchNavigableMap(base_map, THEORICAL_ROBOT_RADIUS);

    navigable_length = graph_map->south_eastern_table_corner->x - south_western_table_corner->x;
    navigable_height = graph_map->south_eastern_drawing_corner->y - south_western_table_corner->y;

    graph = Graph_new();
}

void teardown_Graph(void)
{
    Graph_delete(graph);
    Map_delete(graph_map);
    Coordinates_delete(invalid_coordinates);
    Coordinates_delete(south_western_drawing_zone_corner);
    Coordinates_delete(south_eastern_drawing_zone_corner);
    Coordinates_delete(north_western_drawing_zone_corner);
    Coordinates_delete(north_eastern_drawing_zone_corner);
    Coordinates_delete(south_western_table_corner);
    Coordinates_delete(south_eastern_table_corner);
    Coordinates_delete(north_western_table_corner);
    Coordinates_delete(north_eastern_table_corner);
    Map_delete(base_map);
}

const int GRAPH_WESTERN_OBSTACLE_X = 4125;
const int GRAPH_WEST_OBSTACLE_X = 6500;
const int GRAPH_CENTER_OBSTACLE_X = 8875;
const int GRAPH_EAST_OBSTACLE_X = 11250;
const int GRAPH_EASTERN_OBSTACLE_X = 13625;

const int GRAPH_SOUTHERN_OBSTACLE_Y = 625;
const int GRAPH_SOUTH_OBSTACLE_Y = 1875;
const int GRAPH_CENTER_OBSTACLE_Y = 5650;
const int GRAPH_NORTH_OBSTACLE_Y = 9425;
const int GRAPH_NORTHERN_OBSTACLE_Y = 10675;

Test(Graph, given_aNavigableMapWithOneObstacle_when_updatesGraph_then_theGraphTypeIsSOLO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *center = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, center, CENTER, 0);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(center);
}

Test(Graph, given_aNavigableMapWithTwoNonOverlappingObstacles_when_updatesGrah_then_theGraphTypeIsSOLO_SOLO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *center = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *west = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, center, CENTER, 0);
    Map_updateObstacle(graph_map, west, CENTER, 1);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO_SOLO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(center);
    Coordinates_delete(west);
}

Test(Graph, given_aNavigableMapWithTwoOverlappingObstacles_when_updatesGrah_then_theGraphTypeIsDUO
     , .init = setup_Graph, .fini = teardown_Graph)
{
    struct Coordinates *south = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_SOUTHERN_OBSTACLE_Y);
    struct Coordinates *north = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_NORTHERN_OBSTACLE_Y);
    Map_updateObstacle(graph_map, south, CENTER, 0);
    Map_updateObstacle(graph_map, north, CENTER, 1);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = DUO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(south);
    Coordinates_delete(north);
}

Test(Graph, given_aNavigableMapWithThreeNonOverlappingObstacles_when_updatesGrah_then_theGraphTypeIsSOLO_SOLO_SOLO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *east = Coordinates_new(GRAPH_EASTERN_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *center = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *west = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, east, CENTER, 0);
    Map_updateObstacle(graph_map, center, CENTER, 1);
    Map_updateObstacle(graph_map, west, CENTER, 2);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO_SOLO_SOLO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(east);
    Coordinates_delete(center);
    Coordinates_delete(west);
}

Test(Graph,
     given_aNavigableMapWithTwoEasternOverlappingObstaclesAndAWesternOneNotOverlapping_when_updatesGrah_then_theGraphTypeIsDUO_SOLO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *north_eastern = Coordinates_new(GRAPH_EASTERN_OBSTACLE_X, GRAPH_SOUTH_OBSTACLE_Y);
    struct Coordinates *western = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *south_east = Coordinates_new(GRAPH_EAST_OBSTACLE_X, GRAPH_NORTH_OBSTACLE_Y);
    Map_updateObstacle(graph_map, north_eastern, CENTER, 0);
    Map_updateObstacle(graph_map, western, CENTER, 1);
    Map_updateObstacle(graph_map, south_east, CENTER, 2);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = DUO_SOLO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(north_eastern);
    Coordinates_delete(western);
    Coordinates_delete(south_east);
}

Test(Graph,
     given_aNavigableMapWithTwoWesternOverlappingObstaclesAndAnEasternOneNotOverlapping_when_updatesGrah_then_theGraphTypeIsSOLO_DUO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *north_western = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_SOUTH_OBSTACLE_Y);
    struct Coordinates *eastern = Coordinates_new(GRAPH_EASTERN_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *south_west = Coordinates_new(GRAPH_WEST_OBSTACLE_X, GRAPH_NORTH_OBSTACLE_Y);
    Map_updateObstacle(graph_map, north_western, CENTER, 0);
    Map_updateObstacle(graph_map, eastern, CENTER, 1);
    Map_updateObstacle(graph_map, south_west, CENTER, 2);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO_DUO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(north_western);
    Coordinates_delete(eastern);
    Coordinates_delete(south_west);
}

Test(Graph,
     given_aNavigableMapWithThreeOverlappingObstacles_when_updatesGrah_then_theGraphTypeIsTRIO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    struct Coordinates *east = Coordinates_new(GRAPH_EAST_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *north = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_NORTH_OBSTACLE_Y);
    struct Coordinates *south_west = Coordinates_new(GRAPH_WEST_OBSTACLE_X, GRAPH_SOUTH_OBSTACLE_Y);
    Map_updateObstacle(graph_map, east, CENTER, 0);
    Map_updateObstacle(graph_map, north, CENTER, 1);
    Map_updateObstacle(graph_map, south_west, CENTER, 2);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = TRIO;

    cr_assert_eq(expected_type, actual_type);

    Coordinates_delete(east);
    Coordinates_delete(north);
    Coordinates_delete(south_west);
}
