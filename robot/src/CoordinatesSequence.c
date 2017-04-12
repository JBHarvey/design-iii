#include <stdlib.h>
#include "CoordinatesSequence.h"

struct CoordinatesSequence *CoordinatesSequence_new(struct Coordinates *new_coordinates)
{
    struct Object *new_object = Object_new();
    struct CoordinatesSequence *pointer =  malloc(sizeof(struct CoordinatesSequence));

    pointer->object = new_object;
    pointer->coordinates = new_coordinates;
    pointer->next_element = pointer;

    Object_addOneReference(new_coordinates->object);
    return pointer;
}

void CoordinatesSequence_delete(struct CoordinatesSequence *coordinates_sequence)
{
    Object_removeOneReference(coordinates_sequence->object);

    if(Object_canBeDeleted(coordinates_sequence->object)) {
        Object_delete(coordinates_sequence->object);
        Coordinates_delete(coordinates_sequence->coordinates);

        if(CoordinatesSequence_isLast(coordinates_sequence)) {
            coordinates_sequence->next_element = NULL;
        } else {
            CoordinatesSequence_delete(coordinates_sequence->next_element);
        }

        free(coordinates_sequence);
    }
}

int CoordinatesSequence_isLast(struct CoordinatesSequence *coordinates_sequence)
{
    return (coordinates_sequence->next_element == coordinates_sequence);
}

void CoordinatesSequence_append(struct CoordinatesSequence *coordinates_sequence, struct Coordinates *new_coordinates)
{
    if(CoordinatesSequence_isLast(coordinates_sequence)) {
        coordinates_sequence->next_element = CoordinatesSequence_new(new_coordinates);
    } else {
        CoordinatesSequence_append(coordinates_sequence->next_element, new_coordinates);
    }
}

static int countSizeOfSequenceFrom(struct CoordinatesSequence *coordinates_sequence, int current_size)
{
    if(CoordinatesSequence_isLast(coordinates_sequence)) {
        return current_size;
    }

    int size = countSizeOfSequenceFrom(coordinates_sequence->next_element, current_size + 1);
    return size;
}

int CoordinatesSequence_size(struct CoordinatesSequence *coordinates_sequence)
{
    return countSizeOfSequenceFrom(coordinates_sequence, 1);
}

static struct Coordinates *fetchNextValidCoordinatesForDistance(struct Coordinates *current, struct Coordinates *goal,
        int max_distance)
{
    struct Coordinates *next_valid_coordinates;
    int distance_between = Coordinates_distanceBetween(current, goal);

    if(distance_between > max_distance) {
        next_valid_coordinates = Coordinates_new(goal->x + max_distance, goal->y);
        struct Angle *angle = Coordinates_angleBetween(current, goal);
        angle->theta *= -1;
        Coordinates_rotateOfAngleAround(next_valid_coordinates, angle, current);
        Angle_delete(angle);
    } else {
        next_valid_coordinates = Coordinates_new(goal->x, goal->y);
    }

    return next_valid_coordinates;
}

struct CoordinatesSequence *CoordinatesSequence_shortenSegments(struct CoordinatesSequence *coordinates_sequence,
        int max_distance)
{
    struct CoordinatesSequence *old_sequence_head = coordinates_sequence;
    struct Coordinates *last_valid_coordinates = coordinates_sequence->coordinates;
    struct Coordinates *goal_coordinates = coordinates_sequence->coordinates;

    struct CoordinatesSequence *new_sequence = CoordinatesSequence_new(last_valid_coordinates);

    while(!(CoordinatesSequence_isLast(coordinates_sequence)
            && Coordinates_haveTheSameValues(last_valid_coordinates, goal_coordinates))) {
        if(Coordinates_haveTheSameValues(last_valid_coordinates, goal_coordinates)) {
            coordinates_sequence = coordinates_sequence->next_element;
            goal_coordinates = coordinates_sequence->coordinates;
        }

        struct Coordinates *next_valid_coordinates = fetchNextValidCoordinatesForDistance(last_valid_coordinates,
                goal_coordinates, max_distance);

        CoordinatesSequence_append(new_sequence, next_valid_coordinates);

        Coordinates_copyValuesFrom(last_valid_coordinates, next_valid_coordinates);

        Coordinates_delete(next_valid_coordinates);
    };

    CoordinatesSequence_delete(old_sequence_head);

    return new_sequence;
}
