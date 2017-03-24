#include <stdlib.h>
#include "Graph.h"

struct Graph *Graph_new(void)
{
    struct Object *new_object = Object_new();
    struct Node *new_eastern_node = Node_new();
    struct Node *new_western_node = Node_new();
    enum ObstaclePlacementType new_type = NONE;
    int new_actual_number_of_nodes = 0;
    struct Graph *pointer =  malloc(sizeof(struct Graph));

    pointer->object = new_object;
    pointer->eastern_node = new_eastern_node;
    pointer->western_node = new_western_node;
    pointer->nodes[0] = new_eastern_node;
    pointer->nodes[1] = new_western_node;
    pointer->type = new_type;
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

void Graph_updateForMap(struct Graph *graph, struct Map* map)
{
    int number_of_obstacle = Map_fetchNumberOfObstacles(map);

    struct Obstacle *first;
    struct Obstacle *last;

    switch(number_of_obstacle) {
        case 0:
            graph->type = NONE;
            break;

        case 1:
            graph->type = SOLO;
            first = Map_retrieveFirstObstacle(map);
            establishEasternNodeSolo(graph, first, map);
            establishWesternNodeSolo(graph, first, map);
            break;

        case 2:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            int are_overlapping = Obstacle_areOverlapping(first, last);

            if(are_overlapping) {
                graph->type = DUO;
            } else {
                graph->type = SOLO_SOLO;
            }

            break;

        case 3:
            first = Map_retrieveFirstObstacle(map);
            last = Map_retrieveLastObstacle(map);
            struct Obstacle *middle = Map_retrieveMiddleObstacle(map, first, last);
            int eastern_are_overlapping = Obstacle_areOverlapping(first, middle);
            int western_are_overlapping = Obstacle_areOverlapping(last, middle);

            if(eastern_are_overlapping && ! western_are_overlapping) {
                graph->type = DUO_SOLO;
            } else if(!eastern_are_overlapping && western_are_overlapping) {
                graph->type = SOLO_DUO;
            } else if(eastern_are_overlapping && western_are_overlapping) {
                graph->type = TRIO;
            } else {
                graph->type = SOLO_SOLO_SOLO;
            }

            break;

    };
}
