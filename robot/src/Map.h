#ifndef MAP_H_
#define MAP_H_

#include "Obstacle.h"
#include "State.h"
#include "ManchesterCode.h"
#include "CoordinatesSequence.h"

struct Map {
    struct Object *object;
    struct Coordinates *north_eastern_table_corner;
    struct Coordinates *north_western_table_corner;
    struct Coordinates *south_eastern_table_corner;
    struct Coordinates *south_western_table_corner;
    struct Coordinates *north_eastern_drawing_corner;
    struct Coordinates *north_western_drawing_corner;
    struct Coordinates *south_eastern_drawing_corner;
    struct Coordinates *south_western_drawing_corner;
    struct Coordinates *antenna_zone_start;
    struct Coordinates *antenna_zone_stop;
    struct Obstacle *obstacles[MAXIMUM_OBSTACLE_NUMBER];
    struct Pose *painting_zones[NUMBER_OF_PAINTING];
};

struct Map *Map_new(void);
void Map_delete(struct Map *map);

void Map_updateTableCorners(struct Map *map, struct Coordinates *north_eastern, 
        struct Coordinates *south_eastern, struct Coordinates *south_western, 
        struct Coordinates *north_western);
void Map_updateDrawingCorners(struct Map *map, struct Coordinates *north_eastern, 
        struct Coordinates *south_eastern, struct Coordinates *south_western, 
        struct Coordinates *north_western);
void Map_updateAntennaZone(struct Map *map, struct Coordinates *start, struct Coordinates *stop);
void Map_updateObstacle(struct Map *map, struct Coordinates *new_coordinates, enum CardinalDirection new_orientation, int index);
void Map_updatePaintingZone(struct Map *map, struct Pose *new_pose, int index);

int Map_fetchNumberOfObstacles(struct Map *map);
struct Map *Map_fetchNavigableMap(struct Map *original_map, int robot_radius);

struct Obstacle *Map_retrieveFirstObstacle(struct Map *map);
struct Obstacle *Map_retrieveLastObstacle(struct Map *map);
struct Obstacle *Map_retrieveMiddleObstacle(struct Map *map, struct Obstacle *first, struct Obstacle *last);

struct Obstacle *Map_retrieveFirstOverlappingObstacle(struct Map *map);
struct Obstacle *Map_retrieveLastOverlappingObstacle(struct Map *map);

int Map_isCoordinateFree(struct Map *map, struct Coordinates *coordinates);
void Map_createDrawingTrajectory(struct Map *map, struct ManchesterCode *manchester_code, struct CoordinatesSequence *coordinates_sequence);
#endif // MAP_H_
