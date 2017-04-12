#ifndef COORDINATESSEQUENCE_H_
#define COORDINATESSEQUENCE_H_

#include "Coordinates.h"

struct CoordinatesSequence {
    struct Object *object;
    struct CoordinatesSequence *next_element;
    struct Coordinates *coordinates;
};

struct CoordinatesSequence *CoordinatesSequence_new(struct Coordinates *new_coordinates);
void CoordinatesSequence_delete(struct CoordinatesSequence *coordinates_sequence);

int CoordinatesSequence_isLast(struct CoordinatesSequence *coordinates_sequence);
void CoordinatesSequence_append(struct CoordinatesSequence *coordinates_sequence, struct Coordinates *new_coordinates);
int CoordinatesSequence_size(struct CoordinatesSequence *coordinates_sequence);
struct CoordinatesSequence *CoordinatesSequence_shortenSegments(struct CoordinatesSequence *coordinates_sequence, int max_distance);

#endif // COORDINATESSEQUENCE_H_
