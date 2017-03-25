#ifndef POSEFILTER_H_
#define POSEFILTER_H_

#include "Robot.h"


struct PoseFilter {
    struct Object *object;
    struct Robot *robot;
};

struct PoseFilter_Callbacks {
    void (*updateFromCameraOnly)(struct PoseFilter *);
};

struct PoseFilter *PoseFilter_new(struct Robot *new_robot);
void PoseFilter_delete(struct PoseFilter *pose_filter);

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void);
void PoseFilter_executeFilter(struct PoseFilter *pose_filter, void (*filter)(struct PoseFilter *));

void PoseFilter_updateFromCameraOnly(struct PoseFilter *pose_filter);

#endif // POSEFILTER_H_
