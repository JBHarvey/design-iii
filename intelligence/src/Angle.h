#ifndef ANGLE_H_
#define ANGLE_H_

#define PI 314159
#define MINUS_PI -314159

enum RotationDirection {ANTICLOCKWISE, CLOCKWISE};
/*
 * This module exists to facilitate the manipulation of angle in this program
 * It is important to note that the default used unit is:
 *      0.00001 rad
 */
struct Angle {
    int theta;
};

struct Angle* Angle_new(int new_theta);
void Angle_delete(struct Angle* angle);

#endif // ANGLE_H_
