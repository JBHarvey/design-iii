#ifndef MANCHESTERCODE_H_
#define MANCHESTERCODE_H_

#include "Pose.h"

struct ManchesterCode {
    struct Object *object;
    int painting_number;
    int scale_factor;
    enum CardinalDirection orientation;
};

struct ManchesterCode *ManchesterCode_new(void);
void ManchesterCode_delete(struct ManchesterCode *Manchester_code);

void ManchesterCode_updateCodeValues(struct ManchesterCode *manchester_code, int new_painting_number, int new_scale_factor, enum CardinalDirection new_orientation);
struct Angle *ManchesterCode_retrieveOrientationAngle(struct ManchesterCode *manchester_code);

#endif // MANCHESTERCODE_H_
