#include <stdlib.h>
#include "Map.h"

struct Map *Map_new()
{
    struct Object *new_object = Object_new();
    struct Coordinates *new_north_eastern_table_corner = Coordinates_zero();
    struct Coordinates *new_north_western_table_corner = Coordinates_zero();
    struct Coordinates *new_south_eastern_table_corner = Coordinates_zero();
    struct Coordinates *new_south_western_table_corner = Coordinates_zero();
    struct Coordinates *new_north_eastern_drawing_corner = Coordinates_zero();
    struct Coordinates *new_north_western_drawing_corner = Coordinates_zero();
    struct Coordinates *new_south_eastern_drawing_corner = Coordinates_zero();
    struct Coordinates *new_south_western_drawing_corner = Coordinates_zero();
    struct Coordinates *new_antenna_zone_start = Coordinates_zero();
    struct Coordinates *new_antenna_zone_stop = Coordinates_zero();

    int i;
    struct Obstacle *new_obstacles[3];

    for(i = 0; i < 3; ++i) {
        new_obstacles[i] = Obstacle_new(0, 0, THEORICAL_OBSTACLE_RADIUS, CENTER);
    }

    struct Pose *new_painting_zones[8];

    for(i = 0; i < 8; ++i) {
        new_painting_zones[i] = Pose_zero();
    }

    struct Map *pointer = (struct Map *) malloc(sizeof(struct Map));

    pointer->object = new_object;

    pointer->north_eastern_table_corner = new_north_eastern_table_corner;

    pointer->north_western_table_corner = new_north_western_table_corner;

    pointer->south_eastern_table_corner = new_south_eastern_table_corner;

    pointer->south_western_table_corner = new_south_western_table_corner;

    pointer->north_eastern_drawing_corner = new_north_eastern_drawing_corner;

    pointer->north_western_drawing_corner = new_north_western_drawing_corner;

    pointer->south_eastern_drawing_corner = new_south_eastern_drawing_corner;

    pointer->south_western_drawing_corner = new_south_western_drawing_corner;

    pointer->antenna_zone_start = new_antenna_zone_start;

    pointer->antenna_zone_stop = new_antenna_zone_stop;

    for(i = 0; i < 3; ++i) {
        pointer->obstacles[i] = new_obstacles[i];
    }

    for(i = 0; i < 8; ++i) {
        pointer->painting_zones[i] = new_painting_zones[i];
    }

    return pointer;
}

void Map_delete(struct Map *map)
{
    Object_removeOneReference(map->object);

    if(Object_canBeDeleted(map->object)) {
        Object_delete(map->object);
        Coordinates_delete(map->north_eastern_table_corner);
        Coordinates_delete(map->north_western_table_corner);
        Coordinates_delete(map->south_eastern_table_corner);
        Coordinates_delete(map->south_western_table_corner);
        Coordinates_delete(map->north_eastern_drawing_corner);
        Coordinates_delete(map->north_western_drawing_corner);
        Coordinates_delete(map->south_eastern_drawing_corner);
        Coordinates_delete(map->south_western_drawing_corner);
        Coordinates_delete(map->antenna_zone_start);
        Coordinates_delete(map->antenna_zone_stop);
        int i;

        for(i = 0; i < 3; ++i) {
            Obstacle_delete(map->obstacles[i]);
        }

        for(i = 0; i < 8; ++i) {
            Pose_delete(map->painting_zones[i]);
        }

        free(map);
    }
}

void Map_updateTableCorners(struct Map *map, struct Coordinates *north_eastern,
                            struct Coordinates *south_eastern, struct Coordinates *south_western,
                            struct Coordinates *north_western)
{
    Coordinates_copyValuesFrom(map->north_eastern_table_corner, north_eastern);
    Coordinates_copyValuesFrom(map->south_eastern_table_corner, south_eastern);
    Coordinates_copyValuesFrom(map->south_western_table_corner, south_western);
    Coordinates_copyValuesFrom(map->north_western_table_corner, north_western);
}

void Map_updateDrawingCorners(struct Map *map, struct Coordinates *north_eastern,
                              struct Coordinates *south_eastern, struct Coordinates *south_western,
                              struct Coordinates *north_western)
{
    Coordinates_copyValuesFrom(map->north_eastern_drawing_corner, north_eastern);
    Coordinates_copyValuesFrom(map->south_eastern_drawing_corner, south_eastern);
    Coordinates_copyValuesFrom(map->south_western_drawing_corner, south_western);
    Coordinates_copyValuesFrom(map->north_western_drawing_corner, north_western);
}

void Map_updateAntennaZone(struct Map *map, struct Coordinates *start, struct Coordinates *stop)
{
    Coordinates_copyValuesFrom(map->antenna_zone_start, start);
    Coordinates_copyValuesFrom(map->antenna_zone_stop, stop);
}

void Map_updateObstacle(struct Map *map, struct Coordinates *new_coordinates, enum CardinalDirection new_orientation,
                        int index)
{
    Obstacle_changeCoordinates(map->obstacles[index], new_coordinates);
    Obstacle_changeOrientation(map->obstacles[index], new_orientation);
}
void Map_updatePaintingZone(struct Map *map, struct Pose *new_pose, int index)
{
    Pose_copyValuesFrom(map->painting_zones[index], new_pose);
}