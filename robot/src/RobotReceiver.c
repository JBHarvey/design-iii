#include "RobotReceiver.h"

static void updateTableCorners(struct Map *map, struct Communication_Environment environment)
{
    int northEastX = environment.northEasternTableCorner.x;
    int northEastY = environment.northEasternTableCorner.y;
    int southEastX = environment.southEasternTableCorner.x;
    int southEastY = environment.southEasternTableCorner.y;
    int southWestX = environment.southWesternTableCorner.x;
    int southWestY = environment.southWesternTableCorner.y;
    int northWestX = environment.northWesternTableCorner.x;
    int northWestY = environment.northWesternTableCorner.y;

    struct Coordinates *northEasternTableCorner = Coordinates_new(northEastX, northEastY);
    struct Coordinates *southEasternTableCorner = Coordinates_new(southEastX, southEastY);
    struct Coordinates *southWesternTableCorner = Coordinates_new(southWestX, southWestY);
    struct Coordinates *northWesternTableCorner = Coordinates_new(northWestX, northWestY);

    Map_updateTableCorners(map, northEasternTableCorner,
                           southEasternTableCorner, southWesternTableCorner,
                           northWesternTableCorner);

    Coordinates_delete(northEasternTableCorner);
    Coordinates_delete(southEasternTableCorner);
    Coordinates_delete(southWesternTableCorner);
    Coordinates_delete(northWesternTableCorner);
}

static void updateDrawingCorners(struct Map *map, struct Communication_Environment environment)
{
    int northEastX = environment.northEasternDrawingCorner.x;
    int northEastY = environment.northEasternDrawingCorner.y;
    int southEastX = environment.southEasternDrawingCorner.x;
    int southEastY = environment.southEasternDrawingCorner.y;
    int southWestX = environment.southWesternDrawingCorner.x;
    int southWestY = environment.southWesternDrawingCorner.y;
    int northWestX = environment.northWesternDrawingCorner.x;
    int northWestY = environment.northWesternDrawingCorner.y;

    struct Coordinates *northEasternDrawingCorner = Coordinates_new(northEastX, northEastY);
    struct Coordinates *southEasternDrawingCorner = Coordinates_new(southEastX, southEastY);
    struct Coordinates *southWesternDrawingCorner = Coordinates_new(southWestX, southWestY);
    struct Coordinates *northWesternDrawingCorner = Coordinates_new(northWestX, northWestY);


    Map_updateDrawingCorners(map, northEasternDrawingCorner,
                             southEasternDrawingCorner, southWesternDrawingCorner,
                             northWesternDrawingCorner);

    Coordinates_delete(northEasternDrawingCorner);
    Coordinates_delete(southEasternDrawingCorner);
    Coordinates_delete(southWesternDrawingCorner);
    Coordinates_delete(northWesternDrawingCorner);
}

static void updateAntennaZone(struct Map *map, struct Communication_Environment environment)
{
    int startX = environment.antennaZoneStart.x;
    int startY = environment.antennaZoneStart.y;
    int stopX = environment.antennaZoneStop.x;
    int stopY = environment.antennaZoneStop.y;

    struct Coordinates *start = Coordinates_new(startX, startY);
    struct Coordinates *stop = Coordinates_new(stopX, stopY);

    Map_updateAntennaZone(map, start, stop);

    Coordinates_delete(start);
    Coordinates_delete(stop);
}

enum CardinalDirection orientAngle(int theta)
{
    enum CardinalDirection orientation;


    return orientation;
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
    int i = environment.paintingZone[0].index;

    while(i < 8) {
        x = environment.paintingZone[i].pose.coordinates.x;
        y = environment.paintingZone[i].pose.coordinates.y;
        theta = environment.paintingZone[i].pose.theta;

        struct Pose *pose = Pose_new(x, y, theta);

        Map_updatePaintingZone(map, pose, i);

        Pose_delete(pose);

        ++i;
    }

}

static void updateWorldCameraRobot(struct WorldCamera *worldCamera, struct Communication_Object robot)
{
    Sensor_receivesData(worldCamera->robotSensor);

    int x = robot.zone.pose.coordinates.x;
    int y = robot.zone.pose.coordinates.y;
    int theta = robot.zone.pose.theta;
    int radius = robot.radius;
    struct Pose *pose = Pose_new(x, y, theta);

    Pose_copyValuesFrom(worldCamera->robotPose, pose);
    worldCamera->robotRadius = radius;

    Pose_delete(pose);
}

// Add the handle_recv_packet somewhere here and make it call these
void RobotReceiver_updateMesurements(struct Robot *robot)
{
    // TODO: Integrate the handle_recv_packet function to call these callbacks:
    struct Communication_World world;
    RobotReceiver_updateWorld(robot->worldCamera, world);

    // THESE DON'T EXIST YET
    //updateWheels(robot->wheels);
    //updateOtherSensor??
    //updatePen(robot->pen);
}

void RobotReceiver_updateWorld(struct WorldCamera *worldCamera, struct Communication_World world)
{
    if(world.environmentHasChanged) {
        Sensor_receivesData(worldCamera->mapSensor);
        struct Map *map = worldCamera->map;
        updateTableCorners(map, world.environment);
        updateDrawingCorners(map, world.environment);
        updateAntennaZone(map, world.environment);
        updateObstacles(map, world.environment);
        updatePaintingZone(map, world.environment);
    }

    updateWorldCameraRobot(worldCamera, world.robot);


}

struct Mesurements RobotReceiver_fetchInputs(struct Mesurements(*communicationCallback)(void))
{
    struct Mesurements mesurements = (*communicationCallback)();
    return mesurements;
}
