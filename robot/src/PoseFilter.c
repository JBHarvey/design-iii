#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "PoseFilter.h"

const int NUMBER_OF_PARTICLES = 50;
const double ROTATION_SPEED_NOISE_VARIANCE = 0.04 / ANGLE_BASE_UNIT;
const double TRANSLATION_SPEED_NOISE_VARIANCE = 0.2 / SPEEDS_BASE_UNIT;

const gsl_rng_type * RANDOM_NUMBER_GENERATOR_TYPE;

struct PoseFilter *PoseFilter_new(struct Robot *new_robot)
{
    struct Object *new_object = Object_new();
    struct Pose **new_particles = malloc(NUMBER_OF_PARTICLES * sizeof(struct Pose *));
    int *new_particles_status = calloc(NUMBER_OF_PARTICLES, sizeof(int));
    struct Timer *new_command_timer = Timer_new();
    struct PoseFilter *pointer =  malloc(sizeof(struct PoseFilter));

    gsl_rng *new_random_number_generator;
    gsl_rng_env_setup();
    RANDOM_NUMBER_GENERATOR_TYPE = gsl_rng_default;
    new_random_number_generator = gsl_rng_alloc(RANDOM_NUMBER_GENERATOR_TYPE);
    unsigned int seed = (unsigned int) time(NULL);
    gsl_rng_set(new_random_number_generator, seed);

    pointer->object = new_object;
    pointer->robot = new_robot;

    pointer->particles = new_particles;
    pointer->particles_status = new_particles_status;
    pointer->command_timer = new_command_timer;
    pointer->random_number_generator = new_random_number_generator;

    Object_addOneReference(new_robot->object);
    return pointer;
}

void PoseFilter_delete(struct PoseFilter *pose_filter)
{
    Object_removeOneReference(pose_filter->object);

    if(Object_canBeDeleted(pose_filter->object)) {
        Object_delete(pose_filter->object);
        Robot_delete(pose_filter->robot);

        for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
            if(pose_filter->particles_status[i]) {
                Pose_delete(pose_filter->particles[i]);
            }
        }

        Timer_delete(pose_filter->command_timer);
        free(pose_filter->particles_status);
        free(pose_filter->particles);
        gsl_rng_free(pose_filter->random_number_generator);
        free(pose_filter);
    }
}

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void)
{
    struct PoseFilter_Callbacks callbacks = {
        .updateFromCameraOnly = &PoseFilter_updateFromCameraOnly,
        .updateFromCameraAndWheels = &PoseFilter_updateFromCameraAndWheels
    };
    return callbacks;
}

void PoseFilter_executeFilter(struct PoseFilter *pose_filter, void (*filter)(struct PoseFilter *))
{
    (*(filter))(pose_filter);
}

void PoseFilter_updateFromCameraOnly(struct PoseFilter *pose_filter)
{
    struct Robot *robot = pose_filter->robot;
    int new_data_from_world = robot->world_camera->robot_sensor->has_received_new_data;

    if(new_data_from_world) {
        struct Pose *current_robot_pose = robot->current_state->pose;
        struct Pose *world_camera_pose = robot->world_camera->robot_pose;
        Pose_copyValuesFrom(current_robot_pose, world_camera_pose);
        Sensor_readsData(robot->world_camera->robot_sensor);
    }
}

static void populateParticles(struct Map *map, struct Pose **particles, int *particles_status,
                              gsl_rng *random_number_generator)
{
    int table_length = map->south_eastern_table_corner->x - map->south_western_table_corner->x;
    int table_width = map->north_western_table_corner->y - map->south_western_table_corner->y;

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if(!particles_status[i]) {
            particles[i] = Pose_new((int)(gsl_rng_uniform(random_number_generator) * table_length),
                                    (int)(gsl_rng_uniform(random_number_generator) * table_width),
                                    (int)(gsl_rng_uniform(random_number_generator) * 2 * PI) - PI);
        }
    }
}

static void predict(struct Pose **particles, struct Timer *command_timer, gsl_rng *random_number_generator,
                    struct Wheels *wheels, struct WorldCamera *world_camera)
{
    int new_speed_command = wheels->speed_actuator->has_prepared_new_command;
    int new_rotation_command = wheels->rotation_actuator->has_prepared_new_command;
    struct Pose *new_command = Pose_zero();

    if(new_speed_command || new_rotation_command) {

        double time_delta = Timer_elapsedTime(command_timer) / NUMBER_OF_NANOSECONDS_IN_A_SECOND;

        if(new_speed_command) {
            Coordinates_copyValuesFrom(new_command->coordinates, wheels->speed_command);
        } else if(new_rotation_command) {
            new_command->angle->theta = wheels->rotation_command->theta;
        }

        for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
            if(new_command->angle->theta) {
                int theta_rotation_with_noise = (int)((double)(new_command->angle->theta +
                                                      (int) gsl_ran_gaussian(random_number_generator, sqrt(ROTATION_SPEED_NOISE_VARIANCE))) *
                                                      time_delta);
                Pose_rotate(particles[i], theta_rotation_with_noise);
            } else if(new_command->coordinates->x) {
                int x_translation_with_noise = (int)((double)(new_command->coordinates->x +
                                                     (int) gsl_ran_gaussian(random_number_generator, sqrt(TRANSLATION_SPEED_NOISE_VARIANCE))) *
                                                     time_delta);
                Pose_translate(particles[i], x_translation_with_noise, 0);
            } else if(new_command->coordinates->y) {
                int y_translation_with_noise = (int)((double)(new_command->coordinates->y +
                                                     (int) gsl_ran_gaussian(random_number_generator, sqrt(TRANSLATION_SPEED_NOISE_VARIANCE))) *
                                                     time_delta);
                Pose_translate(particles[i], 0, y_translation_with_noise);
            }
        }
    }

    Timer_reset(command_timer);

    Pose_delete(new_command);
}

void PoseFilter_updateFromCameraAndWheels(struct PoseFilter * pose_filter)
{
    struct Robot *robot = pose_filter->robot;
    populateParticles(robot->world_camera->map, pose_filter->particles, pose_filter->particles_status,
                      pose_filter->random_number_generator);
    predict(pose_filter->particles, pose_filter->command_timer, pose_filter->random_number_generator,
            robot->wheels, robot->world_camera);

}
