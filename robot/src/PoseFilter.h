#ifndef POSEFILTER_H_
#define POSEFILTER_H_

#include "gsl/gsl_randist.h"
#include "Robot.h"
#include "Timer.h"

struct PoseFilter {
    struct Object *object;
    struct Robot *robot;
    struct Pose **particles;
    int is_all_particles_dead;
    double *particles_weight;
    struct Timer *command_timer;
    struct Timer *data_timer;
    gsl_rng *random_number_generator;
};

struct PoseFilter_Callbacks {
    void (*updateFromCameraOnly)(struct PoseFilter *);
    void (*particlesFilterUsingWorldCameraAndWheels)(struct PoseFilter *);
};

struct PoseFilter *PoseFilter_new(struct Robot *new_robot);
void PoseFilter_delete(struct PoseFilter *pose_filter);

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void);
void PoseFilter_executeFilter(struct PoseFilter *pose_filter, void (*filter)(struct PoseFilter *));

void PoseFilter_updateFromCameraOnly(struct PoseFilter *pose_filter);
void PoseFilter_particlesFilterUsingWorldCameraAndWheels(struct PoseFilter * pose_filter);

#endif // POSEFILTER_H_
