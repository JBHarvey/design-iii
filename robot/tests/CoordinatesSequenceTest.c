#include <criterion/criterion.h>
#include <stdio.h>
#include "CoordinatesSequence.h"


const int SEQUENCE_X = 2500;
const int SEQUENCE_Y = 4200;

Test(CoordinatesSequence, creation_destruction)
{
    struct Coordinates *coordinates = Coordinates_new(SEQUENCE_X, SEQUENCE_Y);
    struct CoordinatesSequence *coordinates_sequence = CoordinatesSequence_new(coordinates);

    cr_assert_eq(coordinates_sequence->next_element, coordinates_sequence);
    cr_assert(Coordinates_haveTheSameValues(coordinates_sequence->coordinates, coordinates));

    CoordinatesSequence_delete(coordinates_sequence);
    Coordinates_delete(coordinates);
}

struct CoordinatesSequence *sequence;
struct Coordinates *coordinates;
struct Coordinates *other_coordinates;
void setup_CoordinatesSequence(void)
{
    coordinates = Coordinates_new(SEQUENCE_X, SEQUENCE_Y);
    other_coordinates = Coordinates_new(SEQUENCE_X * 3, SEQUENCE_Y);
    sequence = CoordinatesSequence_new(coordinates);
}

void teardown_CoordinatesSequence(void)
{
    CoordinatesSequence_delete(sequence);
    Coordinates_delete(coordinates);
    Coordinates_delete(other_coordinates);
}

Test(CoordinatesSequence, given_aSingleCoordinatesSequence_when_askedIfIsLast_then_returnsOne
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    int is_last = CoordinatesSequence_isLast(sequence);
    cr_assert(is_last);
}

Test(CoordinatesSequence,
     given_newCoordinates_when_appendCoordinates_then_aNewElementContainingTheCoordinatesBecomesTheNextElementOfTheSequence
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    CoordinatesSequence_append(sequence, other_coordinates);
    struct CoordinatesSequence *new_element = sequence->next_element;

    cr_assert(sequence->next_element = new_element);
}

Test(CoordinatesSequence,
     given_newCoordinates_when_appendCoordinates_then_theSequenceHeadIsNoLongerLastButTheNewElementIs
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    CoordinatesSequence_append(sequence, other_coordinates);
    struct CoordinatesSequence *new_element = sequence->next_element;

    int new_is_last = CoordinatesSequence_isLast(new_element);
    int first_is_not_last = !CoordinatesSequence_isLast(sequence);
    cr_assert(new_is_last);
    cr_assert(first_is_not_last);
}

Test(CoordinatesSequence,
     given_aSequenceWithTwoElements_when_appendsACoordinates_then_theCoordinatesIsHeldInTheThirdElement
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    CoordinatesSequence_append(sequence, other_coordinates);
    CoordinatesSequence_append(sequence, coordinates);
    struct CoordinatesSequence *last = sequence->next_element->next_element;

    cr_assert(Coordinates_haveTheSameValues(last->coordinates, coordinates));
}

Test(CoordinateSequence, given_aSequence_when_fetchSize_then_returnsTheTotalNumberOfCoordinatesHeldInTheSequence
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    int number_of_elements = CoordinatesSequence_size(sequence);
    cr_assert_eq(number_of_elements, 1);

    CoordinatesSequence_append(sequence, other_coordinates);
    number_of_elements = CoordinatesSequence_size(sequence);
    cr_assert_eq(number_of_elements, 2);

    CoordinatesSequence_append(sequence, coordinates);
    number_of_elements = CoordinatesSequence_size(sequence);
    cr_assert_eq(number_of_elements, 3);

}

Test(CoordinatesSequence,
     given_aSequenceOfSizeTwoBiggerThanADistance_when_askedToShortenTheSegments_then_aSequenceOfBiggerSizeIsReturned
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    int distance = MAX_SEGMENT_LENGHT_DRAWING;
    other_coordinates->x = SEQUENCE_X + 2 * distance;
    CoordinatesSequence_append(sequence, other_coordinates);
    int original_size = CoordinatesSequence_size(sequence);

    sequence = CoordinatesSequence_shortenSegments(sequence, distance);
    int new_size = CoordinatesSequence_size(sequence);
    cr_assert(original_size < new_size);

}

Test(CoordinatesSequence,
     given_aSequenceOfSizeTwoBiggerThanADistance_when_askedToShortenTheSegments_then_aSequenceOfSegmentsOfTheDistanceLenghtAtMostIsReturned
     , .init = setup_CoordinatesSequence
     , .fini = teardown_CoordinatesSequence)
{
    int distance = MAX_SEGMENT_LENGHT_DRAWING;
    other_coordinates->x = SEQUENCE_X + 3 * MAX_SEGMENT_LENGHT_DRAWING;
    CoordinatesSequence_append(sequence, other_coordinates);

    sequence = CoordinatesSequence_shortenSegments(sequence, distance);
    struct CoordinatesSequence *new_element = sequence;
    int segment_distance;

    do {
        segment_distance = Coordinates_distanceBetween(new_element->coordinates, new_element->next_element->coordinates);
        cr_assert(distance >= segment_distance, "Distance max: %d, got : %d", distance, segment_distance);
        new_element = new_element->next_element;

    } while(!CoordinatesSequence_isLast(new_element));

}
