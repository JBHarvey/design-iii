#include <stdlib.h>
#include "Graph.h"

struct Graph *Graph_new(void)
{
    struct Object *new_object = Object_new();
    struct Node *new_eastern_node = Node_new();
    struct Node *new_western_node = Node_new();
    int new_actual_number_of_nodes = 0;
    struct Graph *pointer =  malloc(sizeof(struct Graph));

    pointer->object = new_object;
    pointer->eastern_node = new_eastern_node;
    pointer->western_node = new_western_node;
    pointer->nodes[0] = new_eastern_node;
    pointer->nodes[1] = new_western_node;
    pointer->actual_number_of_nodes = new_actual_number_of_nodes;

    return pointer;
}

void Graph_delete(struct Graph *graph)
{
    Object_removeOneReference(graph->object);

    if(Object_canBeDeleted(graph->object)) {
        Object_delete(graph->object);
        Node_delete(graph->eastern_node);
        Node_delete(graph->western_node);

        for(int i = 2; i < graph->actual_number_of_nodes; ++i) {
            Node_delete(graph->nodes[i]);
        }

        free(graph);
    }
}

static int computeYNorthBetweenObstacleAndWall(struct Map *map, struct Obstacle *obstacle)
{
    struct Coordinates *northern_obstacle_coordinates = Obstacle_retrieveNorthernPointOf(obstacle);
    int y = Coordinates_computeMeanY(northern_obstacle_coordinates, map->north_eastern_table_corner);
    Coordinates_delete(northern_obstacle_coordinates);
    return y;
}

static int computeYSouthBetweenObstacleAndWall(struct Map *map, struct Obstacle *obstacle)
{
    struct Coordinates *southern_obstacle_coordinates = Obstacle_retrieveSouthernPointOf(obstacle);
    int y = Coordinates_computeMeanY(southern_obstacle_coordinates, map->south_eastern_table_corner);
    Coordinates_delete(southern_obstacle_coordinates);
    return y;
}

static int isNorthOfTheObstacleNavigable(struct Map *map, struct Obstacle *obstacle)
{
    struct Coordinates *northern_coordinates = Obstacle_retrieveNorthernPointOf(obstacle);
    // This ensure the robot has enough space next to the obstacle
    northern_coordinates->y += MINIMAL_GAP;
    int navigable = Map_isCoordinateFree(map, northern_coordinates);
    Coordinates_delete(northern_coordinates);
    return navigable;
}

static int isSouthOfTheObstacleNavigable(struct Map *map, struct Obstacle *obstacle)
{
    struct Coordinates *southern_coordinates = Obstacle_retrieveSouthernPointOf(obstacle);
    // This ensure the robot has enough space next to the obstacle
    southern_coordinates->y -= MINIMAL_GAP;
    int navigable = Map_isCoordinateFree(map, southern_coordinates);
    Coordinates_delete(southern_coordinates);
    return navigable;
}

static int computeOptimalXValueForObstacleEastNodeWithBorder(struct Obstacle *east_obstacle,
        struct Coordinates *eastern_point_x)
{
    struct Coordinates *eastern_obstacle_point = Obstacle_retrieveEasternPointOf(east_obstacle);
    int eastern_node_x = Coordinates_computeMeanX(eastern_obstacle_point, eastern_point_x);
    Coordinates_delete(eastern_obstacle_point);
    return eastern_node_x;
}

static int computeOptimalXValueForObstacleWestNodeWithBorder(struct Obstacle *west_obstacle,
        struct Coordinates *western_point_x)
{
    struct Coordinates *western_obstacle_point = Obstacle_retrieveWesternPointOf(west_obstacle);
    int western_node_x = Coordinates_computeMeanX(western_obstacle_point, western_point_x);
    Coordinates_delete(western_obstacle_point);
    return western_node_x;
}

static int computeOptimalXValueForSoloObstacleEasternNode(struct Obstacle *eastern_obstacle, struct Map *map)
{
    return computeOptimalXValueForObstacleEastNodeWithBorder(eastern_obstacle, map->south_eastern_table_corner);
}

static int computeOptimalXValueForSoloObstacleWesternNode(struct Obstacle *western_obstacle, struct Map *map)
{
    return computeOptimalXValueForObstacleWestNodeWithBorder(western_obstacle, map->south_western_table_corner);
}

static int computeOptimalYValueForSideSoloObstacleNode(struct Obstacle *obstacle, struct Map *map)
{
    int y = 0;
    enum CardinalDirection orientation = obstacle->orientation;

    if(orientation == NORTH) {
        y = computeYNorthBetweenObstacleAndWall(map, obstacle);
    } else if(orientation == SOUTH) {
        y = computeYSouthBetweenObstacleAndWall(map, obstacle);
    } else if(orientation == CENTER) {
        int north_is_navigable = isNorthOfTheObstacleNavigable(map, obstacle);
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, obstacle);

        if(!north_is_navigable && south_is_navigable) {
            y = computeYSouthBetweenObstacleAndWall(map, obstacle);
        } else if(north_is_navigable && !south_is_navigable) {
            y = computeYNorthBetweenObstacleAndWall(map, obstacle);
        } else if(north_is_navigable && south_is_navigable) {
            int y_north = computeYNorthBetweenObstacleAndWall(map, obstacle);
            int y_south = computeYSouthBetweenObstacleAndWall(map, obstacle);
            y = (y_north + y_south) / 2;
        }
    }

    return y;
}

static void addNodesForXBordersAndYValue(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        int east_border, int west_border, int y)
{
    if(east_border < west_border) {
        int temporary = east_border;
        east_border = west_border;
        west_border = temporary;
    }

    struct Coordinates *new_east_node_coordinates = Coordinates_new(east_border, y);

    struct Coordinates *new_west_node_coordinates = Coordinates_new(west_border, y);

    struct Node *new_east_node = Node_new();

    struct Node *new_west_node = Node_new();

    Coordinates_copyValuesFrom(new_east_node->coordinates, new_east_node_coordinates);

    Coordinates_copyValuesFrom(new_west_node->coordinates, new_west_node_coordinates);

    Coordinates_delete(new_east_node_coordinates);

    Coordinates_delete(new_west_node_coordinates);

    Node_attemptToConnectAsNeighbours(east_node, new_east_node);

    Node_attemptToConnectAsNeighbours(new_east_node, new_west_node);

    Node_attemptToConnectAsNeighbours(west_node, new_west_node);

    int next_node_index = graph->actual_number_of_nodes;

    graph->nodes[next_node_index++] = new_east_node;

    graph->nodes[next_node_index++] = new_west_node;

    graph->actual_number_of_nodes = next_node_index;
}

static void addNodesForObstacleAndYValue(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        struct Obstacle *obstacle, int y)
{
    struct Coordinates *east_point = Obstacle_retrieveEasternPointOf(obstacle);
    struct Coordinates *west_point = Obstacle_retrieveWesternPointOf(obstacle);
    int east_border = east_point->x;
    int west_border = west_point->x;
    Coordinates_delete(east_point);
    Coordinates_delete(west_point);
    addNodesForXBordersAndYValue(graph, east_node, west_node, east_border, west_border, y);
}

static void addNodesNorthOfSoloObstacle(struct Graph *graph, struct Node *east_node, struct Node *west_node,
                                        struct Obstacle *obstacle, struct Map *map)
{
    int new_nodes_y = computeYNorthBetweenObstacleAndWall(map, obstacle);
    addNodesForObstacleAndYValue(graph, east_node, west_node, obstacle, new_nodes_y);
}


static void addNodesSouthOfSoloObstacle(struct Graph *graph, struct Node *east_node, struct Node *west_node,
                                        struct Obstacle *obstacle, struct Map *map)
{
    int new_nodes_y = computeYSouthBetweenObstacleAndWall(map, obstacle);
    addNodesForObstacleAndYValue(graph, east_node, west_node, obstacle, new_nodes_y);
}

static void addNodesNorthOfSoloObstacleIfNavigable(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        struct Obstacle *obstacle, struct Map *map)
{
    int north_is_navigable = isNorthOfTheObstacleNavigable(map, obstacle);

    if(obstacle->orientation == CENTER && north_is_navigable) {
        addNodesNorthOfSoloObstacle(graph, east_node, west_node, obstacle, map);
    }
}

static void addNodesSouthOfSoloObstacleIfNavigable(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        struct Obstacle *obstacle, struct Map *map)
{
    int south_is_navigable = isSouthOfTheObstacleNavigable(map, obstacle);

    if(obstacle->orientation == CENTER && south_is_navigable) {
        addNodesSouthOfSoloObstacle(graph, east_node, west_node, obstacle, map);
    }
}

static void linkNodesForSoloObstacle(struct Graph *graph, struct Node *east_node, struct Node *west_node,
                                     struct Obstacle *obstacle, struct Map *map)
{

    enum CardinalDirection orientation = obstacle->orientation;

    if(orientation == NORTH) {
        addNodesNorthOfSoloObstacle(graph, east_node, west_node, obstacle, map);
    } else if(orientation == SOUTH) {
        addNodesSouthOfSoloObstacle(graph, east_node, west_node, obstacle, map);
    } else if(orientation == CENTER) {
        addNodesNorthOfSoloObstacleIfNavigable(graph, east_node, west_node, obstacle, map);
        addNodesSouthOfSoloObstacleIfNavigable(graph, east_node, west_node, obstacle, map);
    }
}

static void establishEasternNodeSolo(struct Graph *graph, struct Obstacle *obstacle, struct Map *map)
{

    int x = computeOptimalXValueForSoloObstacleEasternNode(obstacle, map);
    int y = computeOptimalYValueForSideSoloObstacleNode(obstacle, map);
    struct Coordinates *new_eastern_node_coordinates = Coordinates_new(x, y);

    Coordinates_copyValuesFrom(graph->eastern_node->coordinates, new_eastern_node_coordinates);
    Coordinates_delete(new_eastern_node_coordinates);
}

static void establishWesternNodeSolo(struct Graph *graph, struct Obstacle *obstacle, struct Map *map)
{

    int x = computeOptimalXValueForSoloObstacleWesternNode(obstacle, map);
    int y = computeOptimalYValueForSideSoloObstacleNode(obstacle, map);
    struct Coordinates *new_western_node_coordinates = Coordinates_new(x, y);

    Coordinates_copyValuesFrom(graph->western_node->coordinates, new_western_node_coordinates);
    Coordinates_delete(new_western_node_coordinates);
}

static struct Node *createMiddleNode(struct Obstacle *first_obstacle, struct Obstacle *second_obstacle, struct Map *map)
{
    struct Obstacle *east_obstacle = Obstacle_retrieveEastern(first_obstacle, second_obstacle);
    struct Obstacle *west_obstacle = Obstacle_retrieveWestern(first_obstacle, second_obstacle);

    struct Node *middle_node = Node_new();
    int x = Coordinates_computeMeanX(east_obstacle->coordinates, west_obstacle->coordinates);
    int east_y = computeOptimalYValueForSideSoloObstacleNode(east_obstacle, map);
    int west_y = computeOptimalYValueForSideSoloObstacleNode(west_obstacle, map);
    int y = (int)((east_y + west_y) / 2);

    struct Coordinates *middle_coordinates = Coordinates_new(x, y);
    Coordinates_copyValuesFrom(middle_node->coordinates, middle_coordinates);
    Coordinates_delete(middle_coordinates);

    return middle_node;
}

static void linkNodesForSoloSoloObstacleNoOverlap(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        struct Obstacle *east_obstacle, struct Obstacle *west_obstacle, struct Map *map)
{
    struct Node *middle_node = createMiddleNode(east_obstacle, west_obstacle, map);

    int next_node_index = graph->actual_number_of_nodes;
    graph->nodes[next_node_index++] = middle_node;
    graph->actual_number_of_nodes = next_node_index;

    linkNodesForSoloObstacle(graph, east_node, middle_node, east_obstacle, map);
    linkNodesForSoloObstacle(graph, middle_node, west_node, west_obstacle, map);
}

static void linkNodesBetweenObstacles(struct Graph *graph, struct Node *east_node, struct Node *west_node,
                                      struct Obstacle *first_obstacle, struct Obstacle *second_obstacle)
{

    struct Obstacle *north_obstacle = Obstacle_retrieveNorthern(first_obstacle, second_obstacle);
    struct Obstacle *south_obstacle = Obstacle_retrieveSouthern(first_obstacle, second_obstacle);

    int new_node_y = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
    struct Obstacle *eastern_obstacle = Obstacle_retrieveEastern(north_obstacle, south_obstacle);
    struct Obstacle *western_obstacle = Obstacle_retrieveWestern(north_obstacle, south_obstacle);
    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(eastern_obstacle);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(western_obstacle);
    int east_border = eastern_point->x;
    int west_border = western_point->x;
    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);
    addNodesForXBordersAndYValue(graph, east_node, west_node, east_border, west_border, new_node_y);
}

static void linkNodesForDuoObstacleOverlappingInX(struct Graph *graph, struct Node *east_node, struct Node *west_node,
        struct Obstacle *first_obstacle, struct Obstacle *second_obstacle, struct Map *map)
{
    struct Obstacle *north_obstacle = Obstacle_retrieveNorthern(first_obstacle, second_obstacle);
    struct Obstacle *south_obstacle = Obstacle_retrieveSouthern(first_obstacle, second_obstacle);

    enum CardinalDirection northern_obstacle_orientation = north_obstacle->orientation;
    enum CardinalDirection southern_obstacle_orientation = south_obstacle->orientation;

    if(northern_obstacle_orientation == NORTH) {
        addNodesNorthOfSoloObstacle(graph, east_node, west_node, north_obstacle, map);
    } else if(southern_obstacle_orientation == SOUTH) {
        addNodesSouthOfSoloObstacle(graph, east_node, west_node, south_obstacle, map);
    } else if(northern_obstacle_orientation == SOUTH) {
        linkNodesBetweenObstacles(graph, east_node, west_node, north_obstacle, south_obstacle);
        addNodesSouthOfSoloObstacleIfNavigable(graph, east_node, west_node, south_obstacle, map);

    } else if(southern_obstacle_orientation == NORTH) {
        linkNodesBetweenObstacles(graph, east_node, west_node, north_obstacle, south_obstacle);
        addNodesNorthOfSoloObstacleIfNavigable(graph, east_node, west_node, north_obstacle, map);

    } else if(northern_obstacle_orientation == CENTER && southern_obstacle_orientation == CENTER) {
        addNodesNorthOfSoloObstacleIfNavigable(graph, east_node, west_node, north_obstacle, map);
        addNodesSouthOfSoloObstacleIfNavigable(graph, east_node, west_node, south_obstacle, map);
    }
}

static int computeOptimalYValueForDuoOverlappingXObstacles(struct Graph *graph, struct Obstacle *first_obstacle,
        struct Obstacle *second_obstacle, struct Map *map)
{
    struct Obstacle *north_obstacle = Obstacle_retrieveNorthern(first_obstacle, second_obstacle);
    struct Obstacle *south_obstacle = Obstacle_retrieveSouthern(first_obstacle, second_obstacle);

    int y;
    enum CardinalDirection northern_obstacle_orientation = north_obstacle->orientation;
    enum CardinalDirection southern_obstacle_orientation = south_obstacle->orientation;

    if(northern_obstacle_orientation == NORTH) {
        y = computeOptimalYValueForSideSoloObstacleNode(north_obstacle, map);
    } else if(southern_obstacle_orientation == SOUTH) {
        y = computeOptimalYValueForSideSoloObstacleNode(south_obstacle, map);
    } else if(northern_obstacle_orientation == SOUTH && southern_obstacle_orientation == CENTER) {
        int y_between = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, south_obstacle);

        if(south_is_navigable) {
            int y_south = computeYSouthBetweenObstacleAndWall(map, south_obstacle);
            y = ((y_between + y_south) / 2);
        } else {
            y = y_between;
        }

    } else if(southern_obstacle_orientation == NORTH && northern_obstacle_orientation == CENTER) {
        int y_between = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
        int north_is_navigable = isNorthOfTheObstacleNavigable(map, north_obstacle);

        if(north_is_navigable) {
            int y_north = computeYNorthBetweenObstacleAndWall(map, north_obstacle);
            y = ((y_between + y_north) / 2);
        } else {
            y = y_between;
        }
    } else if(northern_obstacle_orientation == SOUTH && southern_obstacle_orientation == NORTH) {
        y = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
    } else { // CENTER CENTER
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, south_obstacle);
        int north_is_navigable = isNorthOfTheObstacleNavigable(map, north_obstacle);

        if(!north_is_navigable && south_is_navigable) {
            y = computeYSouthBetweenObstacleAndWall(map, south_obstacle);
        } else if(north_is_navigable && !south_is_navigable) {
            y = computeYNorthBetweenObstacleAndWall(map, north_obstacle);
        } else if(north_is_navigable && south_is_navigable) {
            y = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
        }
    }

    return y;
}

static void establishEasternNodeDuoOverlappingX(struct Graph *graph, struct Obstacle *first_obstacle,
        struct Obstacle *second_obstacle, struct Map *map)
{
    struct Obstacle *north_obstacle = Obstacle_retrieveNorthern(first_obstacle, second_obstacle);
    struct Obstacle *south_obstacle = Obstacle_retrieveSouthern(first_obstacle, second_obstacle);
    struct Obstacle *eastern_obstacle = Obstacle_retrieveEastern(first_obstacle, second_obstacle);

    int east_x = computeOptimalXValueForSoloObstacleEasternNode(eastern_obstacle, map);

    int y = computeOptimalYValueForDuoOverlappingXObstacles(graph, north_obstacle, south_obstacle, map);

    struct Coordinates *new_eastern_node_coordinates = Coordinates_new(east_x, y);
    Coordinates_copyValuesFrom(graph->eastern_node->coordinates, new_eastern_node_coordinates);
    Coordinates_delete(new_eastern_node_coordinates);
}

static void establishWesternNodeDuoOverlappingX(struct Graph *graph, struct Obstacle *first_obstacle,
        struct Obstacle *second_obstacle, struct Map *map)
{
    struct Obstacle *north_obstacle = Obstacle_retrieveNorthern(first_obstacle, second_obstacle);
    struct Obstacle *south_obstacle = Obstacle_retrieveSouthern(first_obstacle, second_obstacle);
    struct Obstacle *western_obstacle = Obstacle_retrieveWestern(first_obstacle, second_obstacle);

    int west_x = computeOptimalXValueForSoloObstacleWesternNode(western_obstacle, map);

    int y = computeOptimalYValueForDuoOverlappingXObstacles(graph, north_obstacle, south_obstacle, map);

    struct Coordinates *new_western_node_coordinates = Coordinates_new(west_x, y);
    Coordinates_copyValuesFrom(graph->western_node->coordinates, new_western_node_coordinates);
    Coordinates_delete(new_western_node_coordinates);
}

static void establishEasternAndWesternNodeDuoOverlappingX(struct Graph *graph, struct Obstacle *first_obstacle,
        struct Obstacle *second_obstacle, struct Map *map)
{
    establishEasternNodeDuoOverlappingX(graph, first_obstacle, second_obstacle, map);
    establishWesternNodeDuoOverlappingX(graph, first_obstacle, second_obstacle, map);
}

static int computeOptimalYValueForDuoOverlappingXAndYObstacles(struct Obstacle *east_obstacle,
        struct Obstacle *west_obstacle, struct Map *map)
{
    int y;
    struct Obstacle *northern_obstacle = Obstacle_retrieveNorthern(east_obstacle, west_obstacle);
    struct Obstacle *southern_obstacle = Obstacle_retrieveSouthern(east_obstacle, west_obstacle);

    if(east_obstacle->orientation == NORTH || west_obstacle->orientation == NORTH) {
        y = computeYNorthBetweenObstacleAndWall(map, northern_obstacle);
    } else if(east_obstacle->orientation == SOUTH || west_obstacle->orientation == SOUTH) {
        y = computeYSouthBetweenObstacleAndWall(map, southern_obstacle);
    } else {
        int y_north;
        int y_south;
        int north_is_navigable = isNorthOfTheObstacleNavigable(map, northern_obstacle);
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, southern_obstacle);

        if(!north_is_navigable && south_is_navigable) {
            y_south = computeYSouthBetweenObstacleAndWall(map, southern_obstacle);
            y = y_south;
        } else if(north_is_navigable && !south_is_navigable) {
            y_north = computeYNorthBetweenObstacleAndWall(map, northern_obstacle);
            y = y_north;
        } else if(north_is_navigable && south_is_navigable) {
            y_south = computeYSouthBetweenObstacleAndWall(map, southern_obstacle);
            y_north = computeYNorthBetweenObstacleAndWall(map, northern_obstacle);
            y = ((y_south + y_north) / 2);
        }
    }

    return y;
}

static void establishEastNodesCoordinatesDuoOverlappingXAndYWithEasternXBorders(struct Node *target_node,
        struct Graph *graph, struct Obstacle *first_obstacle, struct Obstacle *second_obstacle,
        struct Coordinates *eastern_point, struct Map *map)
{
    struct Obstacle *east_obstacle = Obstacle_retrieveEastern(first_obstacle, second_obstacle);
    struct Obstacle *west_obstacle = Obstacle_retrieveWestern(first_obstacle, second_obstacle);

    int y = computeOptimalYValueForDuoOverlappingXAndYObstacles(east_obstacle, west_obstacle, map);
    int east_node_x = computeOptimalXValueForObstacleEastNodeWithBorder(east_obstacle, eastern_point);
    struct Coordinates *new_east_node_coordinates = Coordinates_new(east_node_x, y);
    Coordinates_copyValuesFrom(target_node->coordinates, new_east_node_coordinates);
    Coordinates_delete(new_east_node_coordinates);
}

static void establishWestNodesCoordinatesDuoOverlappingXAndYWithWesternXBorders(struct Node *target_node,
        struct Graph *graph, struct Obstacle *first_obstacle, struct Obstacle *second_obstacle,
        struct Coordinates *western_point,
        struct Map *map)
{
    struct Obstacle *east_obstacle = Obstacle_retrieveEastern(first_obstacle, second_obstacle);
    struct Obstacle *west_obstacle = Obstacle_retrieveWestern(first_obstacle, second_obstacle);

    int y = computeOptimalYValueForDuoOverlappingXAndYObstacles(east_obstacle, west_obstacle, map);
    int west_node_x = computeOptimalXValueForObstacleWestNodeWithBorder(west_obstacle, western_point);
    struct Coordinates *new_west_node_coordinates = Coordinates_new(west_node_x, y);
    Coordinates_copyValuesFrom(target_node->coordinates, new_west_node_coordinates);
    Coordinates_delete(new_west_node_coordinates);
}

static void establishEasternAndWesternNodesDuoOverlappingXAndYWithXBorders(struct Graph *graph,
        struct Obstacle *first_obstacle, struct Obstacle *second_obstacle, struct Coordinates *eastern_point,
        struct Coordinates *western_point, struct Map *map)
{
    struct Obstacle *east_obstacle = Obstacle_retrieveEastern(first_obstacle, second_obstacle);
    struct Obstacle *west_obstacle = Obstacle_retrieveWestern(first_obstacle, second_obstacle);

    establishEastNodesCoordinatesDuoOverlappingXAndYWithEasternXBorders(graph->eastern_node, graph, east_obstacle,
            west_obstacle, eastern_point, map);
    establishWestNodesCoordinatesDuoOverlappingXAndYWithWesternXBorders(graph->western_node, graph, east_obstacle,
            west_obstacle, western_point, map);
}

static void linkNodesForDuoObstacleOverlappingInXAndY(struct Graph *graph, struct Node *east_node,
        struct Node *west_node, struct Obstacle *east_obstacle, struct Obstacle *west_obstacle, struct Map *map)
{
    struct Coordinates *eastern_point = Obstacle_retrieveEasternPointOf(east_obstacle);
    struct Coordinates *western_point = Obstacle_retrieveWesternPointOf(west_obstacle);
    int east_border = eastern_point->x;
    int west_border = western_point->x;
    Coordinates_delete(eastern_point);
    Coordinates_delete(western_point);

    struct Obstacle *northern_obstacle = Obstacle_retrieveNorthern(east_obstacle, west_obstacle);
    struct Obstacle *southern_obstacle = Obstacle_retrieveSouthern(east_obstacle, west_obstacle);

    if(east_obstacle->orientation != CENTER || west_obstacle->orientation != CENTER) {

        int y = computeOptimalYValueForDuoOverlappingXAndYObstacles(east_obstacle, west_obstacle, map);;
        addNodesForXBordersAndYValue(graph, east_node, west_node, east_border, west_border, y);
    } else {

        int north_is_navigable = isNorthOfTheObstacleNavigable(map, northern_obstacle);
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, southern_obstacle);

        if(north_is_navigable) {
            int y_north = computeYNorthBetweenObstacleAndWall(map, northern_obstacle);
            addNodesForXBordersAndYValue(graph, east_node, west_node, east_border, west_border, y_north);
        }

        if(south_is_navigable) {
            int y_south = computeYSouthBetweenObstacleAndWall(map, southern_obstacle);
            addNodesForXBordersAndYValue(graph, east_node, west_node, east_border, west_border, y_south);
        }
    }
}
static void linkSoloSoloSoloObstaclesNoOverlap(struct Graph *graph, struct Obstacle *first, struct Obstacle *middle,
        struct Obstacle *last, struct Map *map)
{
    establishEasternNodeSolo(graph, first, map);
    establishWesternNodeSolo(graph, last, map);
    struct Node *middle_east_node = createMiddleNode(first, middle, map);
    struct Node *middle_west_node = createMiddleNode(middle, last, map);
    int next_node_index = graph->actual_number_of_nodes;
    graph->nodes[next_node_index++] = middle_east_node;
    graph->nodes[next_node_index++] = middle_west_node;
    graph->actual_number_of_nodes = next_node_index;
    linkNodesForSoloObstacle(graph, graph->eastern_node, middle_east_node, first, map);
    linkNodesForSoloObstacle(graph, middle_east_node, middle_west_node, middle, map);
    linkNodesForSoloObstacle(graph, middle_west_node, graph->western_node, last, map);

}

static struct Node *createNodeAndAddToGraph(struct Graph *graph, int x, int y)
{
    struct Node *new_node = Node_new();
    struct Coordinates *node_coordinates = Coordinates_new(x, y);
    Coordinates_copyValuesFrom(new_node->coordinates, node_coordinates);
    Coordinates_delete(node_coordinates);
    int next_node_index = graph->actual_number_of_nodes;
    graph->nodes[next_node_index++] = new_node;
    graph->actual_number_of_nodes = next_node_index;
    return new_node;
}

static struct Node *createMiddleNodeSoloDuoOverlapX(struct Graph *graph, struct Obstacle *first,
        struct Obstacle *middle, struct Obstacle *last, struct Map *map)
{
    struct Coordinates *west_border_x;

    if(Coordinates_isToTheEastOf(middle->coordinates, last->coordinates)) {
        west_border_x = Obstacle_retrieveEasternPointOf(middle);
    } else {
        west_border_x = Obstacle_retrieveEasternPointOf(last);
    }

    int middle_node_x = computeOptimalXValueForObstacleWestNodeWithBorder(first, west_border_x);
    Coordinates_delete(west_border_x);

    int west_y = computeOptimalYValueForDuoOverlappingXObstacles(graph, middle, last, map);
    int east_y = computeOptimalYValueForSideSoloObstacleNode(first, map);
    int middle_node_y = ((east_y + west_y) / 2);

    struct Node *middle_node = createNodeAndAddToGraph(graph, middle_node_x, middle_node_y);

    return middle_node;
}

static struct Node *createMiddleNodeDuoOverlapXSolo(struct Graph *graph, struct Obstacle *first,
        struct Obstacle *middle, struct Obstacle *last, struct Map *map)
{
    struct Coordinates *east_border_x;

    struct Obstacle *middle_obstacle = Obstacle_retrieveWestern(first, middle);
    east_border_x = Obstacle_retrieveWesternPointOf(middle_obstacle);

    int middle_node_x = computeOptimalXValueForObstacleEastNodeWithBorder(last, east_border_x);
    Coordinates_delete(east_border_x);

    int west_y = computeOptimalYValueForDuoOverlappingXObstacles(graph, first, middle, map);
    int east_y = computeOptimalYValueForSideSoloObstacleNode(last, map);
    int middle_node_y = ((east_y + west_y) / 2);

    struct Node *middle_node = createNodeAndAddToGraph(graph, middle_node_x, middle_node_y);

    return middle_node;
}

static struct Node *createMiddleNodeSoloDuoOverlapXAndY(struct Graph *graph, struct Obstacle *first,
        struct Obstacle *middle, struct Obstacle *last, struct Map *map)
{
    struct Coordinates *west_border_x;

    struct Obstacle *middle_obstacle = Obstacle_retrieveEastern(middle, last);
    west_border_x = Obstacle_retrieveEasternPointOf(middle_obstacle);

    int middle_node_x = computeOptimalXValueForObstacleWestNodeWithBorder(first, west_border_x);
    Coordinates_delete(west_border_x);

    int west_y = computeOptimalYValueForDuoOverlappingXAndYObstacles(middle, last, map);
    int east_y = computeOptimalYValueForSideSoloObstacleNode(first, map);
    int middle_node_y = ((east_y + west_y) / 2);

    struct Node *middle_node = createNodeAndAddToGraph(graph, middle_node_x, middle_node_y);

    return middle_node;
}

static struct Node *createMiddleNodeDuoOverlapXAndYSolo(struct Graph *graph, struct Obstacle *first,
        struct Obstacle *middle, struct Obstacle *last, struct Map *map)
{
    struct Coordinates *east_border_x;

    if(Coordinates_isToTheEastOf(first->coordinates, middle->coordinates)) {
        east_border_x = Obstacle_retrieveWesternPointOf(first);
    } else {
        east_border_x = Obstacle_retrieveWesternPointOf(middle);
    }

    int middle_node_x = computeOptimalXValueForObstacleEastNodeWithBorder(last, east_border_x);
    Coordinates_delete(east_border_x);

    int west_y = computeOptimalYValueForDuoOverlappingXAndYObstacles(first, middle, map);
    int east_y = computeOptimalYValueForSideSoloObstacleNode(last, map);
    int middle_node_y = ((east_y + west_y) / 2);

    struct Node *middle_node = createNodeAndAddToGraph(graph, middle_node_x, middle_node_y);

    return middle_node;
}

static void linkSoloDuoOverlapX(struct Graph *graph, struct Obstacle *first, struct Obstacle *middle,
                                struct Obstacle *last, struct Map *map)
{
    struct Node *middle_node = createMiddleNodeSoloDuoOverlapX(graph, first, middle, last, map);
    establishWesternNodeDuoOverlappingX(graph, middle, last, map);
    linkNodesForSoloObstacle(graph, graph->eastern_node, middle_node, first, map);
    linkNodesForDuoObstacleOverlappingInX(graph, middle_node, graph->western_node, middle, last, map);
}

static void linkSoloDuoOverlapXAndY(struct Graph *graph, struct Obstacle *first, struct Obstacle *middle,
                                    struct Obstacle *last, struct Map *map)
{
    struct Node *middle_node = createMiddleNodeSoloDuoOverlapXAndY(graph, first, middle, last, map);
    establishWestNodesCoordinatesDuoOverlappingXAndYWithWesternXBorders(graph->western_node, graph, middle, last,
            map->south_western_table_corner, map);
    linkNodesForSoloObstacle(graph, graph->eastern_node, middle_node, first, map);
    linkNodesForDuoObstacleOverlappingInXAndY(graph, middle_node, graph->western_node, middle, last, map);
}

static void linkDuoOverlapXSolo(struct Graph *graph, struct Obstacle *first, struct Obstacle *middle,
                                struct Obstacle *last, struct Map *map)
{
    struct Node *middle_node = createMiddleNodeDuoOverlapXSolo(graph, first, middle, last, map);
    establishEasternNodeDuoOverlappingX(graph, first, middle, map);
    linkNodesForSoloObstacle(graph, middle_node, graph->western_node, last, map);
    linkNodesForDuoObstacleOverlappingInX(graph, graph->eastern_node, middle_node, first, middle, map);
}

static void linkDuoOverlapXAndYSolo(struct Graph *graph, struct Obstacle *first, struct Obstacle *middle,
                                    struct Obstacle *last, struct Map *map)
{
    struct Node *middle_node = createMiddleNodeDuoOverlapXAndYSolo(graph, first, middle, last, map);
    establishEastNodesCoordinatesDuoOverlappingXAndYWithEasternXBorders(graph->eastern_node, graph, first, middle,
            map->south_eastern_table_corner, map);
    linkNodesForSoloObstacle(graph, middle_node, graph->western_node, last, map);
    linkNodesForDuoObstacleOverlappingInXAndY(graph, graph->eastern_node, middle_node, first, middle, map);
}

static void establishEasternAndWesternNodeForTrioObstacles(struct Graph *graph, int east_node_x, int west_node_x, int y)
{
    struct Coordinates *eastern_node_coordinates = Coordinates_new(east_node_x, y);
    struct Coordinates *western_node_coordinates = Coordinates_new(west_node_x, y);
    Coordinates_copyValuesFrom(graph->eastern_node->coordinates, eastern_node_coordinates);
    Coordinates_copyValuesFrom(graph->western_node->coordinates, western_node_coordinates);
    Coordinates_delete(eastern_node_coordinates);
    Coordinates_delete(western_node_coordinates);
}

static void createGraphTrioOneY(struct Graph *graph, int eastern_node_x, int western_node_x, int east_node_x,
                                int west_node_x, int y)
{
    establishEasternAndWesternNodeForTrioObstacles(graph, eastern_node_x, western_node_x, y);
    addNodesForXBordersAndYValue(graph, graph->eastern_node, graph->western_node, east_node_x, west_node_x, y);
}

static void createGraphTrioTwoYs(struct Graph *graph, int eastern_node_x, int western_node_x, int east_node_x,
                                 int west_node_x, int first_y, int second_y)
{
    int y = ((first_y + second_y) / 2);
    establishEasternAndWesternNodeForTrioObstacles(graph, eastern_node_x, western_node_x, y);
    addNodesForXBordersAndYValue(graph, graph->eastern_node, graph->western_node, east_node_x, west_node_x, first_y);
    addNodesForXBordersAndYValue(graph, graph->eastern_node, graph->western_node, east_node_x, west_node_x, second_y);
}

static void createGraphTrioThreeYs(struct Graph *graph, int eastern_node_x, int western_node_x, int east_node_x,
                                   int west_node_x, int first_y, int second_y, int third_y)
{
    int y = ((first_y + second_y + third_y) / 3);
    establishEasternAndWesternNodeForTrioObstacles(graph, eastern_node_x, western_node_x, y);
    addNodesForXBordersAndYValue(graph, graph->eastern_node, graph->western_node, east_node_x, west_node_x, first_y);
    addNodesForXBordersAndYValue(graph, graph->eastern_node, graph->western_node, east_node_x, west_node_x, second_y);
}

static void establishGraphForTrioObstacles(struct Graph *graph, struct Obstacle *eastern_obstacle,
        struct Obstacle *middle_x_obstacle, struct Obstacle *western_obstacle, struct Map *map)
{
    struct Coordinates *eastern_point_of_eastern_obstacle = Obstacle_retrieveEasternPointOf(eastern_obstacle);
    struct Coordinates *western_point_of_western_obstacle = Obstacle_retrieveWesternPointOf(western_obstacle);
    int east_node_x = eastern_point_of_eastern_obstacle->x;
    int west_node_x = western_point_of_western_obstacle->x;
    int eastern_node_x = Coordinates_computeMeanX(map->south_eastern_table_corner, eastern_point_of_eastern_obstacle);
    int western_node_x = Coordinates_computeMeanX(map->south_western_table_corner, western_point_of_western_obstacle);
    Coordinates_delete(eastern_point_of_eastern_obstacle);
    Coordinates_delete(western_point_of_western_obstacle);

    struct Obstacle *northern_obstacle = Obstacle_retrieveNorthern(eastern_obstacle, middle_x_obstacle);
    northern_obstacle = Obstacle_retrieveNorthern(northern_obstacle, western_obstacle);
    struct Obstacle *southern_obstacle = Obstacle_retrieveSouthern(eastern_obstacle, middle_x_obstacle);
    southern_obstacle = Obstacle_retrieveSouthern(southern_obstacle, western_obstacle);
    struct Obstacle *middle_y_obstacle = Map_retrieveMiddleObstacle(map, northern_obstacle, southern_obstacle);

    int northern_overlaps_middle = Obstacle_areOverlappingInY(northern_obstacle, middle_y_obstacle);
    int southern_overlaps_middle = Obstacle_areOverlappingInY(southern_obstacle, middle_y_obstacle);
    int navigable_north_y = THEORICAL_WORLD_HEIGHT;
    int navigable_south_y = 0;
    int y_between_northern_middle = Coordinates_computeMeanY(northern_obstacle->coordinates,
                                    middle_y_obstacle->coordinates);
    int y_between_middle_southern = Coordinates_computeMeanY(middle_y_obstacle->coordinates,
                                    southern_obstacle->coordinates);
    int north_is_navigable = isNorthOfTheObstacleNavigable(map, northern_obstacle);
    int south_is_navigable = isSouthOfTheObstacleNavigable(map, southern_obstacle);

    if(north_is_navigable) {
        navigable_north_y = computeYNorthBetweenObstacleAndWall(map, northern_obstacle);
    }

    if(south_is_navigable) {
        navigable_south_y = computeYSouthBetweenObstacleAndWall(map, southern_obstacle);
    }


    enum CardinalDirection northern_obstacle_orientation = northern_obstacle->orientation;
    enum CardinalDirection middle_obstacle_orientation = middle_y_obstacle->orientation;
    enum CardinalDirection southern_obstacle_orientation = southern_obstacle->orientation;

    int middle_y;

    if(!northern_overlaps_middle && !southern_overlaps_middle) {

        if(middle_obstacle_orientation == NORTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, y_between_northern_middle);
        } else if(middle_obstacle_orientation == SOUTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, y_between_middle_southern);
        } else if(middle_obstacle_orientation == CENTER) {
            createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, y_between_northern_middle,
                                 y_between_middle_southern);
        }
    } else if(northern_overlaps_middle && !southern_overlaps_middle) {
        middle_y = computeOptimalYValueForDuoOverlappingXObstacles(graph, middle_y_obstacle, southern_obstacle, map);

        if(southern_obstacle_orientation == SOUTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_south_y);
        } else if(northern_obstacle_orientation == NORTH || middle_obstacle_orientation == NORTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y);
        } else if(northern_obstacle_orientation == SOUTH || middle_obstacle_orientation == SOUTH) {
            if(southern_obstacle_orientation == CENTER && south_is_navigable) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_south_y, middle_y);
            } else {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y);
            }
        } else if(northern_obstacle_orientation == CENTER && middle_obstacle_orientation == CENTER) {
            if(north_is_navigable && (south_is_navigable && southern_obstacle_orientation == CENTER)) {
                createGraphTrioThreeYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y, middle_y,
                                       navigable_north_y);
            } else if(!north_is_navigable && (south_is_navigable && southern_obstacle_orientation == CENTER)) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y, navigable_south_y);
            } else if(north_is_navigable && !(south_is_navigable && southern_obstacle_orientation == CENTER)) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y, middle_y);
            } else if(!north_is_navigable && !(south_is_navigable && southern_obstacle_orientation == CENTER)) {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y);
            }
        }
    } else if(!northern_overlaps_middle && southern_overlaps_middle) {
        middle_y = computeOptimalYValueForDuoOverlappingXObstacles(graph, northern_obstacle, middle_y_obstacle, map);

        if(northern_obstacle_orientation == NORTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y);
        } else if(middle_obstacle_orientation == SOUTH || southern_obstacle_orientation == SOUTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_south_y);
        } else if(middle_obstacle_orientation == NORTH || southern_obstacle_orientation == NORTH) {
            if(northern_obstacle_orientation == CENTER && north_is_navigable) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y, middle_y);
            } else {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y);
            }
        } else if(middle_obstacle_orientation == CENTER && southern_obstacle_orientation == CENTER) {
            if(south_is_navigable && (north_is_navigable && northern_obstacle_orientation == CENTER)) {
                createGraphTrioThreeYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y, middle_y,
                                       navigable_south_y);
            } else if(!south_is_navigable && (north_is_navigable && northern_obstacle_orientation == CENTER)) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y, middle_y);
            } else if(south_is_navigable && !(north_is_navigable && northern_obstacle_orientation == CENTER)) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y, navigable_south_y);
            } else if(!south_is_navigable && !(north_is_navigable && northern_obstacle_orientation == CENTER)) {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, middle_y);
            }
        }
    } else if(northern_overlaps_middle && southern_overlaps_middle) {
        if(northern_obstacle_orientation == NORTH || middle_obstacle_orientation == NORTH
           || southern_obstacle_orientation == NORTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y);
        } else if(northern_obstacle_orientation == SOUTH || middle_obstacle_orientation == SOUTH
                  || southern_obstacle_orientation == SOUTH) {
            createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_south_y);
        } else if(northern_obstacle_orientation == CENTER && middle_obstacle_orientation == CENTER
                  && southern_obstacle_orientation == CENTER) {
            if(north_is_navigable && south_is_navigable) {
                createGraphTrioTwoYs(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y,
                                     navigable_south_y);
            } else if(north_is_navigable && !south_is_navigable) {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_north_y);
            } else if(!north_is_navigable && south_is_navigable) {
                createGraphTrioOneY(graph, eastern_node_x, western_node_x, east_node_x, west_node_x, navigable_south_y);
            }
        }
    }
}

struct Graph *Graph_generateForMap(struct Map* map)
{
    struct Graph *graph = Graph_new();
    int number_of_obstacle = Map_fetchNumberOfObstacles(map);

    struct Obstacle *first;
    struct Obstacle *middle;
    struct Obstacle *last;

    switch(number_of_obstacle) {
        case 0:
            break;

        case 1:
            first = Map_retrieveFirstObstacle(map);
            establishEasternNodeSolo(graph, first, map);
            establishWesternNodeSolo(graph, first, map);
            graph->actual_number_of_nodes = 2;

            linkNodesForSoloObstacle(graph, graph->eastern_node, graph->western_node, first, map);
            break;

        case 2:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            int are_overlapping_in_x = Obstacle_areOverlappingInX(first, last);
            int are_overlapping_in_y = Obstacle_areOverlappingInY(first, last);
            graph->actual_number_of_nodes = 2;

            if(!are_overlapping_in_x) {
                establishEasternNodeSolo(graph, first, map);
                establishWesternNodeSolo(graph, last, map);
                linkNodesForSoloSoloObstacleNoOverlap(graph, graph->eastern_node, graph->western_node, first, last, map);
            } else {
                if(!are_overlapping_in_y) {
                    first = Map_retrieveFirstOverlappingObstacle(map);
                    last = Map_retrieveLastOverlappingObstacle(map);
                    establishEasternAndWesternNodeDuoOverlappingX(graph, first, last, map);
                    linkNodesForDuoObstacleOverlappingInX(graph, graph->eastern_node, graph->western_node, first, last, map);
                } else {
                    establishEasternAndWesternNodesDuoOverlappingXAndYWithXBorders(graph, first, last, map->south_eastern_table_corner,
                            map->south_western_table_corner, map);
                    linkNodesForDuoObstacleOverlappingInXAndY(graph, graph->eastern_node, graph->western_node, first, last, map);
                }
            }

            break;

        case 3:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            middle = Map_retrieveMiddleObstacle(map, first, last);
            int first_overlaps_middle_x = Obstacle_areOverlappingInX(first, middle);
            int first_overlaps_middle_y = Obstacle_areOverlappingInY(first, middle);
            int last_overlaps_middle_x = Obstacle_areOverlappingInX(last, middle);
            int last_overlaps_middle_y = Obstacle_areOverlappingInY(last, middle);

            graph->actual_number_of_nodes = 2;

            if(!first_overlaps_middle_x && !last_overlaps_middle_x) {
            } else if(!first_overlaps_middle_x && last_overlaps_middle_x) {
                establishEasternNodeSolo(graph, first, map);

                if(!last_overlaps_middle_y) {
                    linkSoloDuoOverlapX(graph, first, middle, last, map);
                } else {
                    linkSoloDuoOverlapXAndY(graph, first, middle, last, map);
                }
            } else if(first_overlaps_middle_x && !last_overlaps_middle_x) {
                establishWesternNodeSolo(graph, last, map);

                if(!first_overlaps_middle_y) {
                    linkDuoOverlapXSolo(graph, first, middle, last, map);
                } else {
                    linkDuoOverlapXAndYSolo(graph, first, middle, last, map);
                }
            } else if(first_overlaps_middle_x && last_overlaps_middle_x) {
                establishGraphForTrioObstacles(graph, first, middle, last, map);
            }

            break;

    };

    return graph;
}
