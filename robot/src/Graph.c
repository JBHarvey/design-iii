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

static int computeOptimalXValueForSoloObstacleEasternNode(struct Obstacle *eastern_obstacle, struct Map *map)
{
    struct Coordinates *eastern_obstacle_coordinates = Obstacle_retrieveEasternPointOf(eastern_obstacle);
    int x = Coordinates_computeMeanX(eastern_obstacle_coordinates, map->south_eastern_table_corner);
    Coordinates_delete(eastern_obstacle_coordinates);
    return x;
}

static int computeOptimalXValueForSoloObstacleWesternNode(struct Obstacle *western_obstacle, struct Map *map)
{
    struct Coordinates *western_obstacle_coordinates = Obstacle_retrieveWesternPointOf(western_obstacle);
    int x = Coordinates_computeMeanX(western_obstacle_coordinates, map->south_western_table_corner);
    Coordinates_delete(western_obstacle_coordinates);
    return x;
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

static struct Node *createMiddleNode(struct Obstacle *east_obstacle, struct Obstacle *west_obstacle, struct Map *map)
{
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

static void linkNodesForDuoObstacleNoOverlap(struct Graph *graph, struct Node *east_node, struct Node *west_node,
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
                                      struct Obstacle *north_obstacle, struct Obstacle *south_obstacle)
{
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
        struct Obstacle *north_obstacle, struct Obstacle *south_obstacle, struct Map *map)
{
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

static void establishEasternAndWesternNodeDuo(struct Graph *graph, struct Obstacle *north_obstacle,
        struct Obstacle *south_obstacle, struct Map *map)
{
    int east_x;
    int west_x;

    if(Coordinates_isToTheEastOf(north_obstacle->coordinates, south_obstacle->coordinates)) {
        east_x = computeOptimalXValueForSoloObstacleEasternNode(north_obstacle, map);
        west_x = computeOptimalXValueForSoloObstacleWesternNode(south_obstacle, map);
    } else {
        east_x = computeOptimalXValueForSoloObstacleEasternNode(south_obstacle, map);
        west_x = computeOptimalXValueForSoloObstacleWesternNode(north_obstacle, map);
    }

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
        int y_north;
        int y_south;
        int y_between = Coordinates_computeMeanY(north_obstacle->coordinates, south_obstacle->coordinates);
        int south_is_navigable = isSouthOfTheObstacleNavigable(map, south_obstacle);
        int north_is_navigable = isNorthOfTheObstacleNavigable(map, north_obstacle);

        if(south_is_navigable) {
            y_south = computeYSouthBetweenObstacleAndWall(map, south_obstacle);
        } else {
            y_south = y_between;
        }

        if(north_is_navigable) {
            y_north = computeYNorthBetweenObstacleAndWall(map, north_obstacle);
        } else {
            y_north = y_between;
        }

        y = ((y_north + y_between + y_south) / 3);
    }

    struct Coordinates *new_eastern_node_coordinates = Coordinates_new(east_x, y);

    struct Coordinates *new_western_node_coordinates = Coordinates_new(west_x, y);

    Coordinates_copyValuesFrom(graph->eastern_node->coordinates, new_eastern_node_coordinates);

    Coordinates_copyValuesFrom(graph->western_node->coordinates, new_western_node_coordinates);

    Coordinates_delete(new_eastern_node_coordinates);

    Coordinates_delete(new_western_node_coordinates);
}

void Graph_updateForMap(struct Graph *graph, struct Map* map)
{
    int number_of_obstacle = Map_fetchNumberOfObstacles(map);

    struct Obstacle *first;
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

            if(!are_overlapping_in_x) {
                establishEasternNodeSolo(graph, first, map);
                establishWesternNodeSolo(graph, last, map);
                graph->actual_number_of_nodes = 2;
                linkNodesForDuoObstacleNoOverlap(graph, graph->eastern_node, graph->western_node, first, last, map);
            } else {
                if(!are_overlapping_in_y) {
                    first = Map_retrieveFirstOverlappingObstacle(map);
                    last = Map_retrieveLastOverlappingObstacle(map);
                    establishEasternAndWesternNodeDuo(graph, first, last, map);
                    graph->actual_number_of_nodes = 2;
                    linkNodesForDuoObstacleOverlappingInX(graph, graph->eastern_node, graph->western_node, first, last, map);
                }
            }

            break;

        case 3:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            struct Obstacle *middle = Map_retrieveMiddleObstacle(map, first, last);
            int eastern_are_overlapping = Obstacle_areOverlappingInX(first, middle);
            int western_are_overlapping = Obstacle_areOverlappingInX(last, middle);

            break;

    };
}
