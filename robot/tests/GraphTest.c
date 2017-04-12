#include <criterion/criterion.h>
#include <stdio.h>
#include "Pathfinder.h"

Test(Graph, given_aMapWithNoObstacles_when_createsGraph_then_theGraphTypeIsZero)
{
    struct Graph *graph = Graph_new();

    struct Coordinates *coordinates_zero = Coordinates_zero();

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

const int GRAPH_WESTERN_OBSTACLE_X = 4150;
const int GRAPH_WEST_OBSTACLE_X = 6500;
const int GRAPH_CENTER_OBSTACLE_X = 8875;
const int GRAPH_EAST_OBSTACLE_X = 11250;
const int GRAPH_EASTERN_OBSTACLE_X = 13600;

const int GRAPH_SOUTHERN_OBSTACLE_Y = 650;
const int GRAPH_SOUTH_OBSTACLE_Y = 1875;
const int GRAPH_CENTER_SOUTH_OBSTACLE_Y = 4500;
const int GRAPH_CENTER_OBSTACLE_Y = 5650;
const int GRAPH_CENTER_NORTH_OBSTACLE_Y = 6800;
const int GRAPH_NORTH_OBSTACLE_Y = 9425;
const int GRAPH_NORTHERN_OBSTACLE_Y = 10650;

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

Test(Graph,
     given_aSoloGraph_when_generatesGraph_then_itsEasternNodeIsBetweenTheTheEasternPointOfTheObstacleAndTheEasternNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);

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
     given_aSoloGraphWithNorthObstacle_when_generatesGraph_then_itsEasternNodeIsBetweenTheTheNorthernPointOfTheObstacleAndTheNorthernNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);

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
     given_aSoloGraphWithSouthObstacle_when_generatesGraph_then_itsEasternNodeIsBetweenTheTheSouthernPointOfTheObstacleAndTheSouthernNavigableCoordinates
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);

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
     given_aSoloGraphWithCenterObstacleWithItsNorthernPointNorthOfTheNorthernNavigablePoint_when_generatesGraph_then_itsEasternNodeYValueIsTheSameAsIfTheObstacleWasSouth
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterNorthMap();
    graph = Graph_generateForMap(graph_map);

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
     given_aSoloGraphWithCenterObstacleWithItsSouthernPointSouthOfTheSouthernNavigablePoint_when_generatesGraph_then_itsEasternNodeYValueIsTheSameAsIfTheObstacleWasNorth
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterSouthMap();
    graph = Graph_generateForMap(graph_map);

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
     given_aSoloGraphWithCenterObstacle_when_generatesGraph_then_itsEasternNodeYValueIsTheMeanOfTheNorthAndSouthNavigablePossibilities
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);
    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *middle = graph->eastern_node->coordinates;

    int is_south_of_the_north_wall = Coordinates_isToTheSouthOf(middle, graph_map->north_eastern_table_corner);
    int is_north_of_the_south_wall = Coordinates_isToTheNorthOf(middle, graph_map->south_eastern_table_corner);
    cr_assert(is_south_of_the_north_wall);
    cr_assert(is_north_of_the_south_wall);

}

Test(Graph,
     given_aSoloGraph_when_generatesGraph_then_itsWesternNodeIsBetweenTheWesternWallAndTheWesternPointOfTheObstacle
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveLastObstacle(graph_map);
    struct Coordinates *node_coordinates = graph->western_node->coordinates;
    struct Coordinates *western_point_of_the_obstacle = Obstacle_retrieveWesternPointOf(obstacle);
    struct Coordinates *western_navigable_point = graph_map->south_western_table_corner;
    int to_the_west_of_the_obstacle = Coordinates_isToTheWestOf(node_coordinates, western_point_of_the_obstacle);
    int to_the_east_of_the_west_wall = Coordinates_isToTheEastOf(node_coordinates, western_navigable_point);
    cr_assert(to_the_east_of_the_west_wall);
    cr_assert(to_the_west_of_the_obstacle);

    Coordinates_delete(western_point_of_the_obstacle);
}

Test(Graph, given_aSoloGraph_when_generatesGraph_then_itsWesternNodeYValueIsTheSameAsItsEasternNodeXValue
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);
    struct Coordinates *eastern_node_coordinates = graph->eastern_node->coordinates;
    struct Coordinates *western_node_coordinates = graph->western_node->coordinates;
    cr_assert_eq(eastern_node_coordinates->y, western_node_coordinates->y);
}

Test(Graph, given_aSoloGraphWithNorthernCenterObstacle_when_generatesGraph_then_theTotalNumberOfNodeIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterNorthMap();
    graph = Graph_generateForMap(graph_map);
    int total_number_of_nodes = graph->actual_number_of_nodes;
    int expected_number = 4;
    cr_assert_eq(total_number_of_nodes, expected_number);
}

Test(Graph, given_aSoloGraphWithSouthernCenterObstacle_when_generatesGraph_then_theTotalNumberOfNodeIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloCenterSouthMap();
    graph = Graph_generateForMap(graph_map);
    int total_number_of_nodes = graph->actual_number_of_nodes;
    int expected_number = 4;
    cr_assert_eq(total_number_of_nodes, expected_number);
}

Test(Graph, given_aSoloGraphWithNorthObstacle_when_generatesGraph_then_theTotalNumberOfNodeIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);
    int total_number_of_nodes = graph->actual_number_of_nodes;
    int expected_number = 4;
    cr_assert_eq(total_number_of_nodes, expected_number);
}

Test(Graph,
     given_aNorthSoloGraphWithFourNodes_when_generatesGraph_then_theFirstNewNodeIsConnectedToTheEasternNode
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Node *eastern_node = graph->eastern_node;
    struct Node *first_new_node = graph->nodes[2];

    cr_assert(Node_areNeighbours(eastern_node, first_new_node));

}

Test(Graph,
     given_aNorthSoloGraphWithFourNodes_when_generatesGraph_then_theSecondNewNodeIsConnectedToTheWesternNode
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Node *western_node = graph->western_node;
    struct Node *second_new_node = graph->nodes[3];

    cr_assert(Node_areNeighbours(western_node, second_new_node));

}

Test(Graph,
     given_aNorthSoloGraphWithFourNodes_when_generatesGraph_then_theCoordinatesOfTheTwoNewNodesAreTheEasternAndWesternPointBetweenTheObstacleAndTheNorthWall
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle);
    struct Coordinates *northern_point = Obstacle_retrieveNorthernPointOf(obstacle);
    int y = Coordinates_computeMeanY(northern_point, graph_map->north_eastern_table_corner);
    eastern_point->y = y;
    western_point->y = y;

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Coordinates_haveTheSameValues(first_new_node->coordinates, eastern_point));
    cr_assert(Coordinates_haveTheSameValues(second_new_node->coordinates, western_point));

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(northern_point);
}

Test(Graph,
     given_aNorthSoloGraphWithFourNodes_when_generatesGraph_then_theNewNodesAreNeighbours
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Node_areNeighbours(first_new_node, second_new_node));
}

Test(Graph, given_aSouthSoloGraphWithSouthObstacle_when_generatesGraph_then_theTotalNumberOfNodeIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);
    int total_number_of_nodes = graph->actual_number_of_nodes;
    int expected_number = 4;
    cr_assert_eq(total_number_of_nodes, expected_number);
}

Test(Graph,
     given_aSouthSoloGraphWithFourNodes_when_generatesGraph_then_theFirstNewNodeIsConnectedToTheEasternNode
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Node *eastern_node = graph->eastern_node;
    struct Node *first_new_node = graph->nodes[2];

    cr_assert(Node_areNeighbours(eastern_node, first_new_node));

}

Test(Graph,
     given_aSouthSoloGraphWithFourNodes_when_generatesGraph_then_theSecondNewNodeIsConnectedToTheWesternNode
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Node *western_node = graph->western_node;
    struct Node *second_new_node = graph->nodes[3];

    cr_assert(Node_areNeighbours(western_node, second_new_node));

}

Test(Graph,
     given_aSouthSoloGraphWithFourNodes_when_generatesGraph_then_theCoordinatesOfTheTwoNewNodesAreTheEasternAndWesternPointBetweenTheObstacleAndTheSouthWall
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);

    struct Obstacle *obstacle = Map_retrieveFirstObstacle(graph_map);
    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle);
    struct Coordinates *southern_point = Obstacle_retrieveSouthernPointOf(obstacle);
    int y = Coordinates_computeMeanY(southern_point, graph_map->south_eastern_table_corner);
    eastern_point->y = y;
    western_point->y = y;

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Coordinates_haveTheSameValues(first_new_node->coordinates, eastern_point));
    cr_assert(Coordinates_haveTheSameValues(second_new_node->coordinates, western_point));

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(southern_point);
}

Test(Graph,
     given_aSouthSoloGraphWithFourNodes_when_generatesGraph_then_theNewNodesAreNeighbours
     , .init = setup_Graph
     , .fini = teardown_Graph)
{

    generateSoloMap(SOUTH);
    graph = Graph_generateForMap(graph_map);

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Node_areNeighbours(first_new_node, second_new_node));
}

Test(Graph,
     given_anySoloGraphWithFourNodes_when_generatesGraph_then_aPathExistsBetweenTheEasternAndWesternNode
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);
    struct CoordinatesSequence *sequence = Pathfinder_generatePathWithDijkstra(graph, graph->eastern_node,
                                           graph->western_node);
    int size = CoordinatesSequence_size(sequence);
    int expected = 4;
    cr_assert_eq(size, expected);

    CoordinatesSequence_delete(sequence);
}

Test(Graph, given_thePathOfASoloGraphWithFourNodes_when_generatesGraph_then_allTheCoordinatesAreFree
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(NORTH);
    graph = Graph_generateForMap(graph_map);
    struct CoordinatesSequence *sequence_head = Pathfinder_generatePathWithDijkstra(graph, graph->eastern_node,
            graph->western_node);
    struct CoordinatesSequence *sequence = sequence_head;

    struct Coordinates *coordinates = sequence->coordinates;
    cr_assert(Map_isCoordinateFree(graph_map, coordinates));

    sequence = sequence->next_element;
    coordinates = sequence->coordinates;
    cr_assert(Map_isCoordinateFree(graph_map, coordinates));

    sequence = sequence->next_element;
    coordinates = sequence->coordinates;
    cr_assert(Map_isCoordinateFree(graph_map, coordinates));

    sequence = sequence->next_element;
    coordinates = sequence->coordinates;
    cr_assert(Map_isCoordinateFree(graph_map, coordinates));


    CoordinatesSequence_delete(sequence_head);
}

Test(Graph, given_aSoloGraphWithCenterObstacle_when_generatesGraph_then_theTotalNumberOfNodeIsSix
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);
    int total_number_of_nodes = graph->actual_number_of_nodes;
    int expected_number = 6;
    cr_assert_eq(total_number_of_nodes, expected_number);
}

Test(Graph,
     given_aSoloGraphWithCenterObstacleAndSixNodes_when_generatesGraph_then_thePathGeneratedFromTheGraphHasFourElements
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    generateSoloMap(CENTER);
    graph = Graph_generateForMap(graph_map);
    struct CoordinatesSequence *sequence = Pathfinder_generatePathWithDijkstra(graph, graph->eastern_node,
                                           graph->western_node);
    int size = CoordinatesSequence_size(sequence);
    int expected = 4;
    cr_assert_eq(size, expected);

    CoordinatesSequence_delete(sequence);
}

/* -- END OF SOLO GRAPH --  */
/* -- START OF TWO OBSTACLE GRAPH -- */
struct Obstacle *obstacle_a;
struct Obstacle *obstacle_b;

void setup_TwoObstaclesNoOverlap(enum CardinalDirection orientation_a, enum CardinalDirection orientation_b)
{
    struct Coordinates *coordinates_a = Coordinates_new(GRAPH_EAST_OBSTACLE_X, GRAPH_NORTH_OBSTACLE_Y);
    struct Coordinates *coordinates_b = Coordinates_new(GRAPH_WEST_OBSTACLE_X, GRAPH_SOUTH_OBSTACLE_Y);
    Map_updateObstacle(graph_map, coordinates_a, orientation_a, 0);
    Map_updateObstacle(graph_map, coordinates_b, orientation_b, 1);
    obstacle_a = graph_map->obstacles[0];
    obstacle_b = graph_map->obstacles[1];

    Coordinates_delete(coordinates_a);
    Coordinates_delete(coordinates_b);
    graph = Graph_generateForMap(graph_map);
}

void setup_TwoObstaclesOverlappingInY(enum CardinalDirection orientation_a, enum CardinalDirection orientation_b)
{
    struct Coordinates *coordinates_a = Coordinates_new(GRAPH_EAST_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *coordinates_b = Coordinates_new(GRAPH_WEST_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, coordinates_a, orientation_a, 0);
    Map_updateObstacle(graph_map, coordinates_b, orientation_b, 1);
    obstacle_a = graph_map->obstacles[0];
    obstacle_b = graph_map->obstacles[1];

    Coordinates_delete(coordinates_a);
    Coordinates_delete(coordinates_b);
    graph = Graph_generateForMap(graph_map);
}

void setup_TwoObstaclesOverlappingInXWithYValues(enum CardinalDirection orientation_a,
        enum CardinalDirection orientation_b, int y_a, int y_b)
{
    struct Coordinates *coordinates_a = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, y_a);
    struct Coordinates *coordinates_b = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, y_b);
    Map_updateObstacle(graph_map, coordinates_a, orientation_a, 0);
    Map_updateObstacle(graph_map, coordinates_b, orientation_b, 1);
    obstacle_a = graph_map->obstacles[0];
    obstacle_b = graph_map->obstacles[1];

    Coordinates_delete(coordinates_a);
    Coordinates_delete(coordinates_b);
    graph = Graph_generateForMap(graph_map);
}

void setup_TwoObstaclesOverlappingInXSouthCenter(void)
{
    setup_TwoObstaclesOverlappingInXWithYValues(SOUTH, CENTER, GRAPH_NORTHERN_OBSTACLE_Y, GRAPH_CENTER_SOUTH_OBSTACLE_Y);
}

void setup_TwoObstaclesOverlappingInXCenterNorth(void)
{
    setup_TwoObstaclesOverlappingInXWithYValues(CENTER, NORTH, GRAPH_CENTER_NORTH_OBSTACLE_Y, GRAPH_SOUTHERN_OBSTACLE_Y);
}

void setup_TwoObstaclesOverlappingInX(enum CardinalDirection orientation_a, enum CardinalDirection orientation_b)
{
    setup_TwoObstaclesOverlappingInXWithYValues(orientation_a, orientation_b, GRAPH_NORTH_OBSTACLE_Y,
            GRAPH_SOUTH_OBSTACLE_Y);
}

void setup_TwoObstaclesOverlappingInXAndY(enum CardinalDirection orientation_a, enum CardinalDirection orientation_b)
{
    struct Coordinates *coordinates_a = Coordinates_new(GRAPH_CENTER_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    struct Coordinates *coordinates_b = Coordinates_new(GRAPH_WEST_OBSTACLE_X, GRAPH_CENTER_OBSTACLE_Y);
    Map_updateObstacle(graph_map, coordinates_a, orientation_a, 0);
    Map_updateObstacle(graph_map, coordinates_b, orientation_b, 1);
    obstacle_a = graph_map->obstacles[0];
    obstacle_b = graph_map->obstacles[1];

    Coordinates_delete(coordinates_a);
    Coordinates_delete(coordinates_b);
    graph = Graph_generateForMap(graph_map);
}

void assertActualNumberOfNodeIs(int expected_number)
{
    int total_number_of_nodes = graph->actual_number_of_nodes;
    cr_assert_eq(total_number_of_nodes, expected_number, "\nNumber of Nodes do not match.\nExpected: %d - Got: %d",
                 expected_number, total_number_of_nodes);
}

Test(Graph, given_twoObstacleWithNoOverlapNorthSouth_when_generatesGraph_then_theActualNumberOfNodesIsSeven
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesNoOverlap(NORTH, SOUTH);
    assertActualNumberOfNodeIs(7);
}

void assertNodeCoordinatesAreAbout(struct Node *node, int expected_x, int expected_y)
{
    struct Coordinates *expected_coordinates = Coordinates_new(expected_x, expected_y);
    cr_assert(Coordinates_distanceBetween(node->coordinates, expected_coordinates) <= 2,
              "\nNode coordinates are wrong.\nExpected: (%d,%d) - Got: (%d,%d)",
              expected_x, expected_y, node->coordinates->x, node->coordinates->y);
    Coordinates_delete(expected_coordinates);
}

Test(Graph,
     given_twoObstacleWithNoOverlapSouthNorth_when_generatesGraph_then_theMiddleNodeIsInTheCenterOfTheObstaclesPassingNodes
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesNoOverlap(SOUTH, NORTH);

    int center_x = Coordinates_computeMeanX(obstacle_a->coordinates, obstacle_b->coordinates);

    struct Coordinates *northern_point_of_a = Obstacle_retrieveNorthernPointOf(obstacle_a);
    struct Coordinates *southern_point_of_b = Obstacle_retrieveSouthernPointOf(obstacle_b);
    struct Coordinates *northern_navigable_point = graph_map->north_eastern_table_corner;
    struct Coordinates *southern_navigable_point = graph_map->south_eastern_table_corner;
    int north_y = Coordinates_computeMeanY(northern_point_of_a, northern_navigable_point);
    int south_y = Coordinates_computeMeanY(southern_point_of_b, southern_navigable_point);
    int center_y = (int)((north_y + south_y) / 2);

    struct Node *center_node = graph->nodes[2];
    assertNodeCoordinatesAreAbout(center_node, center_x, center_y);

    Coordinates_delete(northern_point_of_a);
    Coordinates_delete(southern_point_of_b);
}

Test(Graph, given_twoObstacleWithOverlappingInYCenterCenter_when_generatesGraph_then_theActualNumberOfNodesIsEleven
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInY(CENTER, CENTER);
    assertActualNumberOfNodeIs(11);
}

Test(Graph,
     given_twoObstacleWithOverlappingInYCenterNorth_when_generatesGraph_then_theMiddleNodeIsInTheCenterOfTheObstaclesPassingNodes
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInY(CENTER, NORTH);

    int center_x = Coordinates_computeMeanX(obstacle_a->coordinates, obstacle_b->coordinates);

    struct Coordinates *northern_point_of_a = Obstacle_retrieveNorthernPointOf(obstacle_a);
    struct Coordinates *southern_point_of_a = Obstacle_retrieveSouthernPointOf(obstacle_a);
    struct Coordinates *northern_point_of_b = Obstacle_retrieveNorthernPointOf(obstacle_b);
    struct Coordinates *northern_navigable_point = graph_map->north_eastern_table_corner;
    struct Coordinates *southern_navigable_point = graph_map->south_eastern_table_corner;
    int north_a_y = Coordinates_computeMeanY(northern_point_of_a, northern_navigable_point);
    int south_a_y = Coordinates_computeMeanY(southern_point_of_a, southern_navigable_point);
    int north_b_y = Coordinates_computeMeanY(northern_point_of_b, northern_navigable_point);
    int center_a = (int)((north_a_y + south_a_y) / 2);
    int center_y = (int)((center_a + north_b_y) / 2);

    struct Node *center_node = graph->nodes[2];
    assertNodeCoordinatesAreAbout(center_node, center_x, center_y);

    Coordinates_delete(northern_point_of_a);
    Coordinates_delete(southern_point_of_a);
    Coordinates_delete(northern_point_of_b);
}

Test(Graph, given_twoObstacleWithOverlappingInXNorthIrrelevant_when_generatesGraph_then_theActualNumberOfNodesIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXWithYValues(NORTH, CENTER, GRAPH_CENTER_OBSTACLE_Y, GRAPH_SOUTHERN_OBSTACLE_Y);
    assertActualNumberOfNodeIs(4);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXNorthIrrelevant_when_generatesGraph_then_theNodesArePlacedIgnoringTheSouthObstacle
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXWithYValues(NORTH, CENTER, GRAPH_CENTER_OBSTACLE_Y, GRAPH_SOUTHERN_OBSTACLE_Y);
    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle_a);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle_a);
    struct Coordinates *northern_point = Obstacle_retrieveNorthernPointOf(obstacle_a);
    int y = Coordinates_computeMeanY(northern_point, graph_map->north_eastern_table_corner);
    eastern_point->y = y;
    western_point->y = y;

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Coordinates_haveTheSameValues(first_new_node->coordinates, eastern_point));
    cr_assert(Coordinates_haveTheSameValues(second_new_node->coordinates, western_point));

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(northern_point);
}

Test(Graph, given_twoObstacleWithOverlappingInXIrrelevantSouth_when_generatesGraph_then_theActualNumberOfNodesIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXWithYValues(CENTER, SOUTH, GRAPH_NORTHERN_OBSTACLE_Y, GRAPH_CENTER_OBSTACLE_Y);
    assertActualNumberOfNodeIs(4);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXIrrelevantSouth_when_generatesGraph_then_theNodesArePlacedIgnoringTheNorthObstacle
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXWithYValues(CENTER, SOUTH, GRAPH_NORTHERN_OBSTACLE_Y, GRAPH_CENTER_OBSTACLE_Y);

    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle_b);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle_b);
    struct Coordinates *southern_point = Obstacle_retrieveSouthernPointOf(obstacle_b);
    int y = Coordinates_computeMeanY(southern_point, graph_map->south_eastern_table_corner);
    eastern_point->y = y;
    western_point->y = y;

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Coordinates_haveTheSameValues(first_new_node->coordinates, eastern_point));
    cr_assert(Coordinates_haveTheSameValues(second_new_node->coordinates, western_point));

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(southern_point);
}

Test(Graph, given_twoObstacleWithOverlappingInXSouthCenter_when_generatesGraph_then_theActualNumberOfNodesIsSix
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXSouthCenter();
    assertActualNumberOfNodeIs(6);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXSouthCenter_when_generatesGraph_then_theNodesAreAroundTheCenterObstacleWithTheSouthernPointOfTheNorthObstacleAsLimit
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXSouthCenter();

    struct Coordinates *southern_point_a = Obstacle_retrieveSouthernPointOf(obstacle_a);
    struct Coordinates *northern_point_b = Obstacle_retrieveNorthernPointOf(obstacle_b);
    struct Coordinates *southern_point_b = Obstacle_retrieveSouthernPointOf(obstacle_b);
    int north_y = Coordinates_computeMeanY(northern_point_b, southern_point_a);
    int south_y = Coordinates_computeMeanY(southern_point_b, graph_map->south_eastern_table_corner);
    int mean_y = ((north_y + south_y) / 2);

    struct Node *first_new_node = graph->eastern_node;
    struct Node *second_new_node = graph->western_node;
    cr_assert_eq(mean_y, first_new_node->coordinates->y);
    cr_assert_eq(mean_y, second_new_node->coordinates->y);

    Coordinates_delete(southern_point_a);
    Coordinates_delete(northern_point_b);
    Coordinates_delete(southern_point_b);

}

Test(Graph, given_twoObstacleWithOverlappingInXSouthNorth_when_generatesGraph_then_theActualNumberOfNodesIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInX(SOUTH, NORTH);
    assertActualNumberOfNodeIs(4);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXSouthNorth_when_generatesGraph_then_theNodesArePlacedBetweenTheObstacles
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInX(SOUTH, NORTH);

    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle_a);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle_a);
    struct Coordinates *southern_point_a = Obstacle_retrieveSouthernPointOf(obstacle_a);
    struct Coordinates *northern_point_b = Obstacle_retrieveNorthernPointOf(obstacle_b);
    int y = Coordinates_computeMeanY(northern_point_b, southern_point_a);
    eastern_point->y = y;
    western_point->y = y;

    struct Node *first_new_node = graph->nodes[2];
    struct Node *second_new_node = graph->nodes[3];
    cr_assert(Coordinates_haveTheSameValues(first_new_node->coordinates, eastern_point)
              , "Expected : (%d,%d) - Got: (%d,%d)\n",
              eastern_point->x, eastern_point->y, first_new_node->coordinates->x, first_new_node->coordinates->y);
    cr_assert(Coordinates_haveTheSameValues(second_new_node->coordinates, western_point)
              , "Expected : (%d,%d) - Got: (%d,%d)\n",
              eastern_point->x, eastern_point->y, second_new_node->coordinates->x, second_new_node->coordinates->y);

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(southern_point_a);
    Coordinates_delete(northern_point_b);
}

Test(Graph, given_twoObstacleWithOverlappingInXCenterNorth_when_generatesGraph_then_theActualNumberOfNodesIsSix
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXCenterNorth();
    assertActualNumberOfNodeIs(6);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXCenterNorth_when_generatesGraph_then_theNodesAreAroundTheCenterObstacleWithTheNorthernPointOfTheSouthObstacleAsLimit
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXCenterNorth();

    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle_a);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle_a);
    struct Coordinates *northern_point_a = Obstacle_retrieveNorthernPointOf(obstacle_a);
    struct Coordinates *southern_point_a = Obstacle_retrieveSouthernPointOf(obstacle_a);
    struct Coordinates *northern_point_b = Obstacle_retrieveNorthernPointOf(obstacle_b);
    int north_y = Coordinates_computeMeanY(northern_point_a, graph_map->north_eastern_table_corner);
    int south_y = Coordinates_computeMeanY(southern_point_a, northern_point_b);
    int mean_y = ((north_y + south_y) / 2);

    struct Node *first_new_node = graph->eastern_node;
    struct Node *second_new_node = graph->western_node;
    cr_assert_eq(mean_y, first_new_node->coordinates->y);
    cr_assert_eq(mean_y, second_new_node->coordinates->y);

    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    Coordinates_delete(northern_point_a);
    Coordinates_delete(southern_point_a);
    Coordinates_delete(northern_point_b);
}

Test(Graph, given_twoObstacleWithOverlappingInXAndYAndBothAreCenter_when_generatesGraph_then_theActualNumberOfNodesIsSix
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(CENTER, CENTER);
    assertActualNumberOfNodeIs(6);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXAndYAndBothAreCenter_when_generatesGraph_then_theNodesAreAroundTheObstacleWithTheSouthernPointOfTheSouthernObstacleAndNorthernPointOfTheNorthernObstacleAsLimit
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(CENTER, CENTER);

    struct Coordinates *northern_point_a = Obstacle_retrieveNorthernPointOf(obstacle_a);
    struct Coordinates *southern_point_b = Obstacle_retrieveSouthernPointOf(obstacle_b);
    int y = Coordinates_computeMeanY(northern_point_a, southern_point_b);

    struct Node *first_new_node = graph->eastern_node;
    struct Node *second_new_node = graph->western_node;
    cr_assert_eq(y, first_new_node->coordinates->y);
    cr_assert_eq(y, second_new_node->coordinates->y);

    Coordinates_delete(northern_point_a);
    Coordinates_delete(southern_point_b);

}

Test(Graph,
     given_twoObstacleWithOverlappingInXAndY_when_generatesGraph_then_theEasternNodeIsBetweenTheEasternObstacleAndTheEasternWall
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(CENTER, CENTER);

    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(obstacle_a);
    int x = Coordinates_computeMeanX(eastern_point, graph_map->south_eastern_table_corner);

    cr_assert_eq(x, graph->eastern_node->coordinates->x);

    Coordinates_delete(eastern_point);

}

Test(Graph,
     given_twoObstacleWithOverlappingInXAndY_when_generatesGraph_then_theWesternNodeIsBetweenTheWesternObstacleAndTheWesternWall
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(NORTH, CENTER);

    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(obstacle_b);
    int x = Coordinates_computeMeanX(western_point, graph_map->south_western_table_corner);

    cr_assert_eq(x, graph->western_node->coordinates->x);

    Coordinates_delete(western_point);

}

Test(Graph, given_twoObstacleWithOverlappingInXAndYAndOneIsNorth_when_generatesGraph_then_theActualNumberOfNodesIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(NORTH, CENTER);
    assertActualNumberOfNodeIs(4);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXAndYAndOneIsNorth_when_generatesGraph_then_theNodesAreOnlyToTheNorthOfTheObstacles
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(CENTER, NORTH);

    struct Coordinates *northern_point = Obstacle_retrieveNorthernPointOf(obstacle_a);
    int y = Coordinates_computeMeanY(northern_point, graph_map->north_eastern_table_corner);

    cr_assert_eq(y, graph->eastern_node->coordinates->y);
    cr_assert_eq(y, graph->western_node->coordinates->y);

    Coordinates_delete(northern_point);

}

Test(Graph, given_twoObstacleWithOverlappingInXAndYAndOneIsSouth_when_generatesGraph_then_theActualNumberOfNodesIsFour
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(SOUTH, CENTER);
    assertActualNumberOfNodeIs(4);
}

Test(Graph,
     given_twoObstacleWithOverlappingInXAndYAndOneIsSouth_when_generatesGraph_then_theNodesAreOnlyToTheSouthOfTheObstacles
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_TwoObstaclesOverlappingInXAndY(CENTER, SOUTH);

    struct Coordinates *southern_point = Obstacle_retrieveSouthernPointOf(obstacle_a);
    int y = Coordinates_computeMeanY(southern_point, graph_map->south_eastern_table_corner);

    cr_assert_eq(y, graph->eastern_node->coordinates->y);
    cr_assert_eq(y, graph->western_node->coordinates->y);

    Coordinates_delete(southern_point);

}

struct Obstacle *obstacle_c;
void setup_ThreeObstaclesSoloDuoOverlappingInXAndY(enum CardinalDirection orientation_a,
        enum CardinalDirection orientation_b, enum CardinalDirection orientation_c)
{
    struct Coordinates *coordinates_a = Coordinates_new(GRAPH_EAST_OBSTACLE_X, GRAPH_NORTH_OBSTACLE_Y);
    struct Coordinates *coordinates_b = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_SOUTH_OBSTACLE_Y);
    struct Coordinates *coordinates_c = Coordinates_new(GRAPH_WESTERN_OBSTACLE_X, GRAPH_SOUTHERN_OBSTACLE_Y);
    Map_updateObstacle(graph_map, coordinates_a, orientation_a, 0);
    Map_updateObstacle(graph_map, coordinates_b, orientation_b, 1);
    Map_updateObstacle(graph_map, coordinates_c, orientation_c, 2);
    obstacle_a = graph_map->obstacles[0];
    obstacle_b = graph_map->obstacles[1];
    obstacle_c = graph_map->obstacles[2];

    Coordinates_delete(coordinates_a);
    Coordinates_delete(coordinates_b);
    Coordinates_delete(coordinates_c);
    graph = Graph_generateForMap(graph_map);
}

Test(Graph,
     given_aSoloDuoOverlappingInXAndYObstacleConfiguration_when_generatesGraph_then_theWesternNodeIsToTheWestOfTheWesternObstacle
     , .init = setup_Graph
     , .fini = teardown_Graph)
{
    setup_ThreeObstaclesSoloDuoOverlappingInXAndY(SOUTH, CENTER, NORTH);
    cr_assert(Coordinates_isToTheWestOf(graph->western_node->coordinates, obstacle_b->coordinates));
}


/* -- END OF TWO OBSTACLE GRAPH -- */
