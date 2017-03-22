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
struct Coordinates *south_western_table_corner;
struct Coordinates *south_eastern_table_corner;
struct Coordinates *north_western_table_corner;
struct Coordinates *north_eastern_table_corner;
struct Coordinates *south_western_drawing_zone_corner;
struct Coordinates *south_eastern_drawing_zone_corner;
struct Coordinates *north_western_drawing_zone_corner;
struct Coordinates *north_eastern_drawing_zone_corner;
struct Coordinates *invalid_coordinates;

void setup_baseMapAndComponents(void)
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
}

void teardown_baseMapAndComponents(void)
{
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

struct Map *graph_map;
struct Graph *graph;
struct Coordinates *coordinates_center_center;
void setup_GraphOne(void)
{
    setup_baseMapAndComponents();
    graph = Graph_new();

    coordinates_center_center = Coordinates_new(THEORICAL_WORLD_LENGTH / 2, THEORICAL_WORLD_HEIGHT / 2);
    Map_updateObstacle(base_map, coordinates_center_center, CENTER, 0);
    Map_updateObstacle(base_map, invalid_coordinates, CENTER, 1);
    Map_updateObstacle(base_map, invalid_coordinates, CENTER, 2);

    graph_map = Map_fetchNavigableMap(base_map, THEORICAL_ROBOT_RADIUS);

}
void teardown_GraphOne(void)
{
    Coordinates_delete(coordinates_center_center);
    Graph_delete(graph);
    teardown_baseMapAndComponents();
}

Test(Graph, given_aNavigableMapWithOneObstacle_when_updatesGraph_then_theGraphTypeIsSOLO
     , .init = setup_GraphOne
     , .fini = teardown_GraphOne)
{
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO;

    cr_assert_eq(expected_type, actual_type);
}
