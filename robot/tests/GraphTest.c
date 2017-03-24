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

void generateSoloMap(enum CardinalDirection orientation)
{
    struct Coordinates *center = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, center, orientation, 0);

    Coordinates_delete(center);
}

void generateSoloCenterNorthMap(void)
{
    struct Coordinates *north = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_NORTHERN_OBSTACLE_Y);
    Map_updateObstacle(graph_map, north, CENTER, 0);

    Coordinates_delete(north);
}

void generateSoloCenterSouthMap(void)
{
    struct Coordinates *south = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_SOUTHERN_OBSTACLE_Y);
    Map_updateObstacle(graph_map, south, CENTER, 0);

    Coordinates_delete(south);
}

Test(Graph, given_aNavigableMapWithOneObstacle_when_updatesGraph_then_theGraphTypeIsSOLO
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    Graph_updateForMap(graph, graph_map);

    enum ObstaclePlacementType actual_type = graph->type;
    enum ObstaclePlacementType expected_type = SOLO;

    cr_assert_eq(expected_type, actual_type);

}

Test(Graph,
     given_aSoloGraph_when_updatesGraph_then_itsEasternNodeIsBetweenTheTheEasternPointOfTheObstacleAndTheEasternNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    Graph_updateForMap(graph, graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *eastern_point_of_the_obstacle = Obstacle_retrieveEasternPointOf(obstacle);
    struct Coordinates *eastern_navigable_point = graph_map->south_eastern_table_corner;
    int to_the_east_of_the_obstacle = Coordinates_isToTheEastOf(node_coordinates, eastern_point_of_the_obstacle);
    int to_the_west_of_the_east_wall = Coordinates_isToTheWestOf(node_coordinates, eastern_navigable_point);
    cr_assert(to_the_west_of_the_east_wall);
    cr_assert(to_the_east_of_the_obstacle);

    Coordinates_delete(eastern_point_of_the_obstacle);
}

Test(Graph,
     given_aSoloGraphWithNorthObstacle_when_updatesGraph_then_itsEasternNodeIsBetweenTheTheNorthernPointOfTheObstacleAndTheNorthernNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(NORTH);
    Graph_updateForMap(graph, graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *northern_point_of_the_obstacle = Obstacle_retrieveNorthernPointOf(obstacle);
    struct Coordinates *northern_navigable_point = graph_map->north_eastern_table_corner;
    int to_the_north_of_the_obstacle = Coordinates_isToTheNorthOf(node_coordinates, northern_point_of_the_obstacle);
    int to_the_south_of_the_north_wall = Coordinates_isToTheSouthOf(node_coordinates, northern_navigable_point);
    cr_assert(to_the_south_of_the_north_wall);
    cr_assert(to_the_north_of_the_obstacle);

    Coordinates_delete(northern_point_of_the_obstacle);
}

Test(Graph,
     given_aSoloGraphWithSouthObstacle_when_updatesGraph_then_itsEasternNodeIsBetweenTheTheSouthernPointOfTheObstacleAndTheSouthernNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(SOUTH);
    Graph_updateForMap(graph, graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *southern_point_of_the_obstacle = Obstacle_retrieveSouthernPointOf(obstacle);
    struct Coordinates *southern_navigable_point = graph_map->south_eastern_table_corner;
    int to_the_south_of_the_obstacle = Coordinates_isToTheSouthOf(node_coordinates, southern_point_of_the_obstacle);
    int to_the_north_of_the_south_wall = Coordinates_isToTheNorthOf(node_coordinates, southern_navigable_point);
    cr_assert(to_the_north_of_the_south_wall);
    cr_assert(to_the_south_of_the_obstacle);

    Coordinates_delete(southern_point_of_the_obstacle);
}

Test(Graph,
     given_aSoloGraphWithCenterObstacleWithItsNorthernPointNorthOfTheNorthernNavigablePoint_when_updatesGraph_then_itsEasternNodeYValueIsTheSameAsIfTheObstacleWasSouth
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterNorthMap();
    Graph_updateForMap(graph, graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *southern_point_of_the_obstacle = Obstacle_retrieveSouthernPointOf(obstacle);
    struct Coordinates *southern_navigable_point = graph_map->south_eastern_table_corner;
    int to_the_south_of_the_obstacle = Coordinates_isToTheSouthOf(node_coordinates, southern_point_of_the_obstacle);
    int to_the_north_of_the_south_wall = Coordinates_isToTheNorthOf(node_coordinates, southern_navigable_point);
    cr_assert(to_the_north_of_the_south_wall,
              "Not to the north of the south Wall :(\n Obstacle Radius: %d\n X limit: %d, X of node: %d",
              obstacle->radius, southern_navigable_point->x, node_coordinates->x);
    cr_assert(to_the_south_of_the_obstacle);

    Coordinates_delete(southern_point_of_the_obstacle);
}

Test(Graph,
     given_aSoloGraphWithCenterObstacleWithItsSouthernPointSouthOfTheSouthernNavigablePoint_when_updatesGraph_then_itsEasternNodeYValueIsTheSameAsIfTheObstacleWasNorth
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterSouthMap();
    Graph_updateForMap(graph, graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *northern_point_of_the_obstacle = Obstacle_retrieveNorthernPointOf(obstacle);
    struct Coordinates *northern_navigable_point = graph_map->north_eastern_table_corner;
    int to_the_north_of_the_obstacle = Coordinates_isToTheNorthOf(node_coordinates, northern_point_of_the_obstacle);
    int to_the_south_of_the_north_wall = Coordinates_isToTheSouthOf(node_coordinates, northern_navigable_point);
    cr_assert(to_the_south_of_the_north_wall);
    cr_assert(to_the_north_of_the_obstacle, "Node Y: %d\nNorthern Y:%d\nNorthern y of the obstacle:%d\nSouthernWall:%d",
              node_coordinates->y, northern_navigable_point->y, northern_point_of_the_obstacle->y,
              graph_map->south_eastern_table_corner->y);

    Coordinates_delete(northern_point_of_the_obstacle);
}

Test(Graph,
     given_aSoloGraphWithCenterObstacle_when_updatesGraph_then_itsEasternNodeYValueIsTheMeanOfTheNorthAndSouthNavigablePossibilities
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterNorthMap();
    Graph_updateForMap(graph, graph_map);
    struct Coordinates *south = graph->eastern_node->coordinates;
    generateSoloCenterSouthMap();
    Graph_updateForMap(graph, graph_map);
    struct Coordinates *north = graph->eastern_node->coordinates;
    generateSoloMap(CENTER);
    Graph_updateForMap(graph, graph_map);
    struct Coordinates *middle = graph->eastern_node->coordinates;

    int distance_to_north = Coordinates_distanceBetween(middle, north);
    int distance_to_south = Coordinates_distanceBetween(middle, south);
    int is_south_of_the_north_wall = Coordinates_isToTheSouthOf(middle, graph_map->north_eastern_table_corner);
    int is_north_of_the_south_wall = Coordinates_isToTheNorthOf(middle, graph_map->south_eastern_table_corner);
    cr_assert_eq(distance_to_north, distance_to_south);
    cr_assert(is_south_of_the_north_wall);
    cr_assert(is_north_of_the_south_wall);

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
