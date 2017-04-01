#include "DataReceiver.h"

struct DataReceiver_Callbacks DataReceiver_fetchCallbacks(void)
{
    struct DataReceiver_Callbacks callbacks = {
        .updateWorld = &DataReceiver_updateWorld,
        .updateWheelsTranslation = &DataReceiver_updateWheelsTranslation,
        .updateWheelsRotation = &DataReceiver_updateWheelsRotation,
        .updateManchesterCode = &DataReceiver_updateManchesterCode,
        .updateFlagsStartCycle = &DataReceiver_updateFlagsStartCycle,
        .updateFlagsImageReceivedByStation = &DataReceiver_updateFlagsImageReceivedByStation,
        .updateFlagsPlannedTrajectoryReceivedByStation = &DataReceiver_updateFlagsPlannedTrajectoryReceivedByStation,
        .updateFlagsReadyToStartSignalReceivedByStation = &DataReceiver_updateFlagsReadyToStartSignalReceivedByStation,
        .updateFlagsReadyToDrawSignalReceivedByStation = &DataReceiver_updateFlagsReadyToDrawSignalReceivedByStation,
        .updateFlagsEndOfCycleSignalReceivedByStation = &DataReceiver_updateFlagsEndOfCycleSignalReceivedByStation
    };

    return callbacks;
}

static void updateTableCorners(struct Map *map, struct Communication_Environment environment)
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

static void updateDrawingCorners(struct Map *map, struct Communication_Environment environment)
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

static void updateAntennaZone(struct Map *map, struct Communication_Environment environment)
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

static void updateObstacles(struct Map *map, struct Communication_Environment environment)
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

static void updatePaintingZone(struct Map *map, struct Communication_Environment environment)
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

static void updateWorldCameraRobot(struct WorldCamera *world_camera, struct Communication_Object robot)
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
        struct Map *map = world_camera->map;
        updateTableCorners(map, world.environment);
        updateDrawingCorners(map, world.environment);
        updateAntennaZone(map, world.environment);
        updateObstacles(map, world.environment);
        updatePaintingZone(map, world.environment);
        Sensor_receivesData(world_camera->map_sensor);
    }

    updateWorldCameraRobot(world_camera, world.robot);
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

void DataReceiver_updateManchesterCode(struct ManchesterCode *manchester_code,
                                       struct Communication_ManchesterCode new_manchester_code)
{
    int new_painting_number = new_manchester_code.painting_number;
    int new_scaling_factor = new_manchester_code.scale_factor;
    enum CardinalDirection new_orientation;

    switch(new_manchester_code.orientation) {
        case 'N':
            new_orientation = NORTH;
            break;

        case 'E':
            new_orientation = EAST;
            break;

        case 'S':
            new_orientation = SOUTH;
            break;

        case 'W':
            new_orientation = WEST;
            break;
    };

    ManchesterCode_updateCodeValues(manchester_code, new_painting_number, new_scaling_factor, new_orientation);
}

void DataReceiver_updateFlagsStartCycle(struct Flags *flags)
{
    Flags_setStartCycleSignalReceived(flags, 1);
}

void DataReceiver_updateFlagsImageReceivedByStation(struct Flags *flags)
{
    Flags_setImageReceivedByStation(flags, 1);
}

void DataReceiver_updateFlagsPlannedTrajectoryReceivedByStation(struct Flags *flags)
{
    Flags_setPlannedTrajectoryReceivedByStation(flags, 1);
}

void DataReceiver_updateFlagsReadyToStartSignalReceivedByStation(struct Flags *flags)
{
    Flags_setReadyToStartReceivedByStation(flags, 1);
}

void DataReceiver_updateFlagsReadyToDrawSignalReceivedByStation(struct Flags *flags)
{
    Flags_setReadyToDrawReceivedByStation(flags, 1);
}

void DataReceiver_updateFlagsEndOfCycleSignalReceivedByStation(struct Flags *flags)
{
    Flags_setEndOfCycleReceivedByStation(flags, 1);
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
