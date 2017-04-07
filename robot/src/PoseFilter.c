#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "gsl_sort.h"
#include "PoseFilter.h"

const int NUMBER_OF_PARTICLES = 50;
const int DEPLETION_THRESHOLD = 20;
const double ROTATION_SPEED_NOISE_VARIANCE = 0.0349066 / ANGLE_BASE_UNIT;
const double TRANSLATION_SPEED_NOISE_VARIANCE = 0.002 / SPEEDS_BASE_UNIT;
const double ABSOLUTE_POSITION_NOISE_VARIANCE = 0.002 / SPEEDS_BASE_UNIT;
const double ABSOLUTE_ANGLE_NOISE_VARIANCE = 0.0349066 / ANGLE_BASE_UNIT;

const gsl_rng_type * RANDOM_NUMBER_GENERATOR_TYPE;

static void initializeParticlesWeight(double *particles_weight)
{
    memset(particles_weight, 1.0 / NUMBER_OF_PARTICLES, NUMBER_OF_PARTICLES * sizeof(double));
}

struct PoseFilter *PoseFilter_new(struct Robot *new_robot)
{
    struct Object *new_object = Object_new();
    struct Pose **new_particles = malloc(NUMBER_OF_PARTICLES * sizeof(struct Pose *));
    int *new_particles_status = calloc(NUMBER_OF_PARTICLES, sizeof(int));
    double *new_particles_weight = malloc(NUMBER_OF_PARTICLES * sizeof(double));
    initializeParticlesWeight(new_particles_weight);
    struct Timer *new_command_timer = Timer_new();
    struct Timer *new_data_timer = Timer_new();
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
    pointer->particles_weight = new_particles_weight;
    pointer->command_timer = new_command_timer;
    pointer->data_timer = new_data_timer;
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
        Timer_delete(pose_filter->data_timer);
        free(pose_filter->particles_status);
        free(pose_filter->particles_weight);
        free(pose_filter->particles);
        gsl_rng_free(pose_filter->random_number_generator);
        free(pose_filter);
    }
}

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void)
{
    struct PoseFilter_Callbacks callbacks = {
        .updateFromCameraOnly = &PoseFilter_updateFromCameraOnly,
        .particlesFilterUsingWorldCameraAndWheels = &PoseFilter_particlesFilterUsingWorldCameraAndWheels
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

static void predictParticlesPoseFromSentCommands(struct Pose **particles, struct Timer *command_timer,
        gsl_rng *random_number_generator, struct Wheels *wheels)
{
    int new_translation_command_has_been_sent = wheels->speed_actuator->has_prepared_new_command;
    int new_rotation_command_has_been_sent = wheels->rotation_actuator->has_prepared_new_command;
    struct Pose *new_command = Pose_zero();

    if(new_translation_command_has_been_sent || new_rotation_command_has_been_sent) {

        double time_delta = Timer_elapsedTime(command_timer) / NUMBER_OF_NANOSECONDS_IN_A_SECOND;
        Timer_reset(command_timer);

        if(new_translation_command_has_been_sent) {
            Coordinates_copyValuesFrom(new_command->coordinates, wheels->speed_command);
        } else if(new_rotation_command_has_been_sent) {
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
    } else {
        Timer_reset(command_timer); // In case of idle state.
    }

    Pose_delete(new_command);
}

static void updateParticlesWeightFromNewSensorData(struct Pose **particles, struct Pose *current_robot_pose,
        struct Timer *data_timer, double *particles_weight, struct Wheels *wheels, struct WorldCamera *world_camera)
{
    int new_translation_speed_data_has_been_received = wheels->translation_sensor->has_received_new_data;
    int new_rotation_speed_data_has_been_received = wheels->rotation_sensor->has_received_new_data;
    int new_absolute_position_data_has_been_received = world_camera->robot_sensor->has_received_new_data;
    struct Pose *new_data_from_wheels = Pose_zero();
    struct Pose *new_data_from_world_camera = Pose_zero();

    if(new_translation_speed_data_has_been_received || new_rotation_speed_data_has_been_received ||
       new_absolute_position_data_has_been_received) {

        double time_delta = Timer_elapsedTime(data_timer) / NUMBER_OF_NANOSECONDS_IN_A_SECOND;
        Timer_reset(data_timer);

        if(new_translation_speed_data_has_been_received) {
            Wheels_readTranslationSpeedData(wheels, new_data_from_wheels->coordinates);
        } else if(new_rotation_speed_data_has_been_received) {
            Wheels_readRotationSpeedData(wheels, new_data_from_wheels->angle);
        }

        if(new_absolute_position_data_has_been_received) {
            WorldCamera_readPoseData(world_camera, new_data_from_world_camera);
        }

        for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {

            if(new_absolute_position_data_has_been_received) {

                double x_translation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->coordinates->x - new_data_from_world_camera->coordinates->x,
                                         sqrt(ABSOLUTE_POSITION_NOISE_VARIANCE));
                double y_translation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->coordinates->y - new_data_from_world_camera->coordinates->y,
                                         sqrt(ABSOLUTE_POSITION_NOISE_VARIANCE));
                double theta_rotation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->angle->theta - new_data_from_world_camera->angle->theta,
                                         sqrt(ABSOLUTE_ANGLE_NOISE_VARIANCE));
                particles_weight[i] = fmin(fmin(x_translation_induced_weight, y_translation_induced_weight),
                                           theta_rotation_induced_weight);

            } else if(new_translation_speed_data_has_been_received) {

                struct Pose *estimated_robot_pose_from_measurements = Pose_zero();
                Pose_copyValuesFrom(estimated_robot_pose_from_measurements, current_robot_pose);
                Pose_translate(estimated_robot_pose_from_measurements, new_data_from_wheels->coordinates->x * time_delta, 0);
                Pose_translate(estimated_robot_pose_from_measurements, 0, new_data_from_wheels->coordinates->y * time_delta);

                double x_translation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->coordinates->x - estimated_robot_pose_from_measurements->coordinates->x,
                                         sqrt(TRANSLATION_SPEED_NOISE_VARIANCE * time_delta));
                double y_translation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->coordinates->y - estimated_robot_pose_from_measurements->coordinates->y,
                                         sqrt(TRANSLATION_SPEED_NOISE_VARIANCE * time_delta));
                particles_weight[i] = fmin(x_translation_induced_weight, y_translation_induced_weight);

                Pose_delete(estimated_robot_pose_from_measurements);

            } else if(new_rotation_speed_data_has_been_received) {

                struct Pose *estimated_robot_pose_from_measurements = Pose_zero();
                Pose_copyValuesFrom(estimated_robot_pose_from_measurements, current_robot_pose);
                Pose_rotate(estimated_robot_pose_from_measurements, new_data_from_wheels->angle->theta * time_delta);

                double theta_rotation_induced_weight =
                    gsl_ran_gaussian_pdf(particles[i]->angle->theta - estimated_robot_pose_from_measurements->angle->theta,
                                         sqrt(ROTATION_SPEED_NOISE_VARIANCE * time_delta));
                particles_weight[i] = theta_rotation_induced_weight;

                Pose_delete(estimated_robot_pose_from_measurements);
            }
        }
    }

    Pose_delete(new_data_from_wheels);
    Pose_delete(new_data_from_world_camera);
}

void normalizeParticlesWeight(double *particles_weight)
{
    double cumulative_weight = 0;

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        cumulative_weight += particles_weight[i];
    }

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        particles_weight[i] /= cumulative_weight;
    }
}

int calculateNumberOfEffectiveParticles(double *normalized_particles_weight)
{
    double number_of_effective_particles;
    double cumulative_square_of_normalized_weight = 0;

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        cumulative_square_of_normalized_weight += pow(normalized_particles_weight[i], 2);
    }

    number_of_effective_particles = 1 / cumulative_square_of_normalized_weight;

    return (int) number_of_effective_particles;
}

void resampleParticles(double *normalized_particles_weight, int *particles_status, gsl_rng *random_number_generator)
{
    double cumulative_weight_sums[NUMBER_OF_PARTICLES];
    double uniform_random_numbers_array[NUMBER_OF_PARTICLES + 1];
    memset(cumulative_weight_sums, 0.0, NUMBER_OF_PARTICLES * sizeof(double));

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        for(int j = 0; j <= i; j++) {
            cumulative_weight_sums[i] += normalized_particles_weight[j];
        }
    }

    for(int i = 0; i < NUMBER_OF_PARTICLES + 1; i++) {
        uniform_random_numbers_array[i] = gsl_rng_uniform(random_number_generator);
    }

    gsl_sort(uniform_random_numbers_array, 1, NUMBER_OF_PARTICLES + 1);
    uniform_random_numbers_array[NUMBER_OF_PARTICLES] = 1;
    int i = 0, j = 0;

    while(i <= NUMBER_OF_PARTICLES) {
        if(uniform_random_numbers_array[i] < cumulative_weight_sums[j]) {
            particles_status[j] = 0;
            i++;
        } else {
            j++;
        }
    }

    for(int k = 0; k < NUMBER_OF_PARTICLES; k++) {
        particles_status[k] = !particles_status[k];
    }

    initializeParticlesWeight(normalized_particles_weight);
}

static struct Pose *calculateNewRobotPoseForParticlesFilter(struct Pose **particles, double *particles_weight)
{
    double new_theta = 0;
    double new_x = 0;
    double new_y = 0;

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        new_x += particles_weight[i] * (double) particles[i]->coordinates->x;
        new_y += particles_weight[i] * (double) particles[i]->coordinates->y;
        new_theta += particles_weight[i] * (double) particles[i]->angle->theta;
    }

    struct Pose *new_robot_pose = Pose_new((int) new_x, (int) new_y, (int) new_theta);

    return new_robot_pose;
}

void PoseFilter_particlesFilterUsingWorldCameraAndWheels(struct PoseFilter *pose_filter)
{
    struct Robot *robot = pose_filter->robot;
    int number_of_effective_particles;

    populateParticles(robot->world_camera->map, pose_filter->particles, pose_filter->particles_status,
                      pose_filter->random_number_generator);
    predictParticlesPoseFromSentCommands(pose_filter->particles, pose_filter->command_timer,
                                         pose_filter->random_number_generator,
                                         robot->wheels);
    updateParticlesWeightFromNewSensorData(pose_filter->particles, robot->current_state->pose, pose_filter->data_timer,
                                           pose_filter->particles_weight, robot->wheels, robot->world_camera);
    normalizeParticlesWeight(pose_filter->particles_weight);
    number_of_effective_particles = calculateNumberOfEffectiveParticles(pose_filter->particles_weight);

    if(number_of_effective_particles < DEPLETION_THRESHOLD) {
        resampleParticles(pose_filter->particles_weight, pose_filter->particles_status,
                          pose_filter->random_number_generator);
    } else {
        struct Pose *new_robot_pose = calculateNewRobotPoseForParticlesFilter(pose_filter->particles,
                                      pose_filter->particles_weight);
        Pose_copyValuesFrom(robot->current_state->pose, new_robot_pose);
        Pose_delete(new_robot_pose);
    }
}
