#include "DataReceiver.h"

static void update_table_corners(struct Map *map, struct Communication_Environment environment)
{
    int northEastX = environment.north_eastern_table_corner.x;
    int northEastY = environment.north_eastern_table_corner.y;
    int southEastX = environment.south_eastern_table_corner.x;
    int southEastY = environment.south_eastern_table_corner.y;
    int southWestX = environment.south_western_table_corner.x;
    int southWestY = environment.south_western_table_corner.y;
    int northWestX = environment.north_western_table_corner.x;
    int northWestY = environment.north_western_table_corner.y;

    struct Coordinates *north_eastern_table_corner = Coordinates_new(northEastX, northEastY);
    struct Coordinates *south_eastern_table_corner = Coordinates_new(southEastX, southEastY);
    struct Coordinates *south_western_table_corner = Coordinates_new(southWestX, southWestY);
    struct Coordinates *north_western_table_corner = Coordinates_new(northWestX, northWestY);

    Map_updateTableCorners(map, north_eastern_table_corner,
                           south_eastern_table_corner, south_western_table_corner,
                           north_western_table_corner);

    Coordinates_delete(north_eastern_table_corner);
    Coordinates_delete(south_eastern_table_corner);
    Coordinates_delete(south_western_table_corner);
    Coordinates_delete(north_western_table_corner);
}

static void update_drawing_corners(struct Map *map, struct Communication_Environment environment)
{
    int northEastX = environment.north_eastern_drawing_corner.x;
    int northEastY = environment.north_eastern_drawing_corner.y;
    int southEastX = environment.south_eastern_drawing_corner.x;
    int southEastY = environment.south_eastern_drawing_corner.y;
    int southWestX = environment.south_western_drawing_corner.x;
    int southWestY = environment.south_western_drawing_corner.y;
    int northWestX = environment.north_western_drawing_corner.x;
    int northWestY = environment.north_western_drawing_corner.y;

    struct Coordinates *north_eastern_drawing_corner = Coordinates_new(northEastX, northEastY);
    struct Coordinates *south_eastern_drawing_corner = Coordinates_new(southEastX, southEastY);
    struct Coordinates *south_western_drawing_corner = Coordinates_new(southWestX, southWestY);
    struct Coordinates *north_western_drawing_corner = Coordinates_new(northWestX, northWestY);


    Map_updateDrawingCorners(map, north_eastern_drawing_corner,
                             south_eastern_drawing_corner, south_western_drawing_corner,
                             north_western_drawing_corner);

    Coordinates_delete(north_eastern_drawing_corner);
    Coordinates_delete(south_eastern_drawing_corner);
    Coordinates_delete(south_western_drawing_corner);
    Coordinates_delete(north_western_drawing_corner);
}

static void update_antenna_zone(struct Map *map, struct Communication_Environment environment)
{
    int startX = environment.antenna_zone_start.x;
    int startY = environment.antenna_zone_start.y;
    int stopX = environment.antenna_zone_stop.x;
    int stopY = environment.antenna_zone_stop.y;

    struct Coordinates *start = Coordinates_new(startX, startY);
    struct Coordinates *stop = Coordinates_new(stopX, stopY);

    Map_updateAntennaZone(map, start, stop);

    Coordinates_delete(start);
    Coordinates_delete(stop);
}

enum CardinalDirection orient_angle(int theta)
{
    enum CardinalDirection orientation;


    return orientation;
}

static void update_obstacles(struct Map *map, struct Communication_Environment environment)
{
    int x;
    int y;
    enum CardinalDirection orientation;
    int i = environment.obstacles[0].zone.index;

    while(i < 3) {
        x = environment.obstacles[i].zone.pose.coordinates.x;
        y = environment.obstacles[i].zone.pose.coordinates.y;

        switch(environment.obstacles[i].zone.pose.theta) {

            case HALF_PI:
                orientation = NORTH;
                break;

            case 0:
                orientation = CENTER;
                break;

            case MINUS_HALF_PI:
                orientation = SOUTH;
                break;
        }

        struct Coordinates *coordinates = Coordinates_new(x, y);

        Map_updateObstacle(map, coordinates, orientation, i);

        Coordinates_delete(coordinates);

        ++i;
    }

}

static void update_painting_zone(struct Map *map, struct Communication_Environment environment)
{
    int x;
    int y;
    int theta;
    int i = environment.painting_zone[0].index;

    while(i < 8) {
        x = environment.painting_zone[i].pose.coordinates.x;
        y = environment.painting_zone[i].pose.coordinates.y;
        theta = environment.painting_zone[i].pose.theta;

        struct Pose *pose = Pose_new(x, y, theta);

        Map_updatePaintingZone(map, pose, i);

        Pose_delete(pose);

        ++i;
    }

}

static void update_world_camera_robot(struct WorldCamera *world_camera, struct Communication_Object robot)
{
    Sensor_receivesData(world_camera->robot_sensor);

    int x = robot.zone.pose.coordinates.x;
    int y = robot.zone.pose.coordinates.y;
    int theta = robot.zone.pose.theta;
    int radius = robot.radius;
    struct Pose *pose = Pose_new(x, y, theta);

    Pose_copyValuesFrom(world_camera->robot_pose, pose);
    world_camera->robot_radius = radius;

    Pose_delete(pose);
}


void DataReceiver_updateWorld(struct WorldCamera *world_camera, struct Communication_World world)
{
    if(world.environment_has_changed) {
        Sensor_receivesData(world_camera->map_sensor);
        struct Map *map = world_camera->map;
        update_table_corners(map, world.environment);
        update_drawing_corners(map, world.environment);
        update_antenna_zone(map, world.environment);
        update_obstacles(map, world.environment);
        update_painting_zone(map, world.environment);
    }

    update_world_camera_robot(world_camera, world.robot);


}

void DataReceiver_updateWheelsTranslation(struct Wheels *wheels, struct Communication_Translation translation)
{
    int x_movement = translation.movement.x;
    int y_movement = translation.movement.y;
    int x_speed = translation.speeds.x;
    int y_speed = translation.speeds.y;

    struct Coordinates *translation_movement = Coordinates_new(x_movement, y_movement);
    struct Coordinates *translation_speed = Coordinates_new(x_speed, y_speed);

    Sensor_receivesData(wheels->translation_sensor);
    Wheels_receiveTranslationData(wheels, translation_movement, translation_speed);

    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
}

void DataReceiver_updateWheelsRotation(struct Wheels *wheels, struct Communication_Rotation rotation)
{
    int theta = rotation.theta;
    int gamma = rotation.gamma;
    struct Angle *rotation_movement = Angle_new(theta);
    struct Angle *rotation_speed = Angle_new(gamma);

    Sensor_receivesData(wheels->rotation_sensor);
    Wheels_receiveRotationData(wheels, rotation_movement, rotation_speed);

    Angle_delete(rotation_movement);
    Angle_delete(rotation_speed);
}

struct Mesurements DataReceiver_fetchInputs(struct Mesurements(*communication_callback)(void))
{
    struct Mesurements mesurements = (*communication_callback)();
    return mesurements;
}

/*
 * CURRENTLY UNUSED FUNCTION BUT IS THE BASE OF A DEBUGGING/LOGGING TOOL FOR INPUTS
void DataReceiver_updateMesurements(struct Robot *robot, struct Mesurements mesurements)
{
    // TODO: Integrate the handle_recv_packet function to call these callbacks:
    DataReceiver_updateWorld(robot->world_camera, mesurements.world);

    // THESE DON'T EXIST YET
    //update_wheels(robot->wheels);
    //update_other_sensor??
    //update_pen(robot->pen);
}
*/
